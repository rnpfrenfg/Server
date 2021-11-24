#pragma once

#include <WinSock2.h>

class Iocp;

class CSocket
{
	friend class Iocp;
public:
	SOCKET sock;

private:
	void* __iocp;
};