#include "MemoryPool.h"

MemoryPool::MemoryPool(int objSize, int len)
{
	nodeList = new _Node[len];
	objList = new BYTE[objSize * len];

	this->objSize = objSize;
	dontUsingTop = nodeList;

	const int byteLen = sizeof(BYTE);

	for (int i = 1; i < len; i++)
	{
		int before = byteLen * (i - 1);
		int now = byteLen * i;
		nodeList[before].next = &nodeList[now];
		nodeList[i - 1].next = &nodeList[i];
		nodeList[i].obj = &objList[objSize * i];
		nodeList[i].isUsing = false;
	}
	nodeList[0].obj = &objList[0];
	nodeList[0].isUsing = false;
}

void* MemoryPool::GetNew()
{
	AcquireSRWLockExclusive(&lock);

	if (dontUsingTop == nullptr)
	{
		ReleaseSRWLockExclusive(&lock);
		return nullptr;
	}

	_Node* temp = dontUsingTop;
	dontUsingTop = dontUsingTop->next;
	temp->before = nullptr;
	temp->next = usingTop;
	usingTop = temp;

	ReleaseSRWLockExclusive(&lock);

	return temp->obj;
}

void MemoryPool::UseEnd(void* obj)
{
	AcquireSRWLockExclusive(&lock);

	_Node* node = (_Node*)obj;
	if (node->before != nullptr)
	{
		node->before->next = node->next;
	}
	if (node->next != nullptr)
	{
		node->next->before = node->before;
	}
	if (usingTop == node)
		usingTop = node->next;

	node->next = dontUsingTop;
	dontUsingTop = node;

	ReleaseSRWLockExclusive(&lock);
	return;
}