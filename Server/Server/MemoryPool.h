#pragma once

#include "include.h"

class MemoryPool
{
private:
	class _Node
	{
	public:
		void* obj;
		_Node* next;
		_Node* before;
		bool isUsing;
	};
public:

	MemoryPool(int objSize, int len);

	void* GetNew();

	void UseEnd(void* obj);
private:
	BYTE* objList;
	_Node* nodeList;
	int objSize;

	_Node* usingTop = nullptr;
	_Node* dontUsingTop = nullptr;

	SRWLOCK lock;
};