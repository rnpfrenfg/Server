#pragma once

#include <Windows.h>
#include <mutex>
#include <stack>

template<class T>
class MemoryPool
{
public:
	MemoryPool() = default;
	~MemoryPool()
	{
		if (mPool != nullptr)
		{
			VirtualFree(mPool, 0, MEM_RELEASE);
			delete[] remain;
		}

		if (needEventCall)
			CloseHandle(objectReturnedEvent);
	}

	DWORD Init(__int64 size, bool waitForNewObject)
	{
		mPool = (T*)VirtualAlloc(NULL, sizeof(T) * size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (mPool == nullptr)
			return GetLastError();

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

		return 0;
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
			needEventCall--;
			lock.unlock();

			newObjLock.lock();
			newObjects.push(obj);
			newObjLock.unlock();

			SetEvent(objectReturnedEvent);
			return;
		}

		remain[remainSize] = obj;
		remainSize++;

		lock.unlock();
	}

	inline __int64 size()
	{
		return mSize;
	}

private:
	T* mPool = nullptr;
	T** remain = nullptr;
	__int64 mSize;
	__int64 remainSize;

	std::mutex lock;

	HANDLE objectReturnedEvent;
	int needEventCall = 0;
	bool waitNewObject = false;
	std::stack<T*> newObjects;
	std::mutex newObjLock;
};

