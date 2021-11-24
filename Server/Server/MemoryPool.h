#pragma once

#include <mutex>
#include <stack>

#define PrintError
#ifdef PrintError
#include <iostream>
#endif

template<class T>
class MemoryPool
{
public:
	MemoryPool() = default;
	~MemoryPool()
	{
		if (mPool != nullptr)
		{
			delete[] mPool;
			delete[] remain;
		}

		if (needEventCall)
			CloseHandle(objectReturnedEvent);
	}

	DWORD Init(int size, bool waitForNewObject)
	{
		mPool = new T[size];
		mSize = size;

		remainSize = size;
		remain = new T * [size];
		for (int i = 0; i < size; i++)
			remain[i] = &mPool[i];

		if (waitNewObject = waitForNewObject)
		{
			needEventCall = 0;
			objectReturnedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

			if (objectReturnedEvent == NULL)
			{
				return GetLastError();
			}
		}
	}

	T* New()
	{
		lock.lock();
		if (remainSize == 0)
		{
			if (!waitNewObject)
			{
				lock.unlock();
				return nullptr;
			}
			else
			{
				needEventCall++;
				lock.unlock();

#ifdef PrintError
				std::cout << "wait new....\n";
#endif
				WaitForSingleObject(objectReturnedEvent, INFINITE);
				
				newObjLock.lock();
				T* returnVal = newObjects.top();
				newObjects.pop();
				newObjLock.unlock();

				return returnVal;
				/*
				if Run under code with default lock,
				other thread can lock and get the remain 1,
				then we will return error pointer;
				*/
			}
		}

		RETURN:
		remainSize--;
		T* returnVal = remain[remainSize];
		lock.unlock();

		return returnVal;
	}
	
	void Delete(T* obj)
	{
		lock.lock();
		if (needEventCall)
		{
			lock.unlock();

			newObjLock.lock();
			newObjects.push(obj);
			newObjLock.unlock();

			SetEvent(objectReturnedEvent);
			needEventCall--;
			return;
		}

		remain[remainSize] = obj;
		remainSize++;

		lock.unlock();
	}

private:
	T* mPool = nullptr;
	T** remain = nullptr;
	int mSize;
	int remainSize;

	std::mutex lock;

	HANDLE objectReturnedEvent;
	int needEventCall = 0;
	bool waitNewObject = false;
	std::stack<T*> newObjects;
	std::mutex newObjLock;
};

