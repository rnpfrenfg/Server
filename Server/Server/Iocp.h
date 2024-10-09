#pragma once
#include "include.h"

#include "CSocket.h"
#include "MemoryPool.h"
#include "DataMessage.h"

typedef std::function<void(CSocket*, SOCKADDR_IN*)> IocpAcceptHandler;

typedef std::function<void(int error)> IocpCallback;

class IOInfo
{
public:
	IOInfo()
	{
		memset(&(overlapped), 0, sizeof(overlapped));
	}
	~IOInfo() = default;

	OVERLAPPED overlapped;
	WSABUF wsabuf;
	IocpCallback callback;
	void* _pool;

	static void* operator new(size_t) = delete;
	static void operator delete(void*) = delete;
};

class Iocp
{
	typedef MemoryPool<IOInfo> PoolType;
public:
	inline static IOInfo* CreateNewIoInfo(CSocket&);

	static void async_write(CSocket& sock, DataMessage& msg, IocpCallback func);
	static void async_read_header(CSocket& sock, DataMessage* msg, IocpCallback func);
	static void async_read_body(CSocket& sock, DataMessage* msg, int len, IocpCallback func);
	static void async_read(CSocket& sock, DataMessage* msg, IocpCallback func, IOInfo* ioInfo);

public:
	IocpAcceptHandler acceptHandler;

	bool open(int max_active_threads, int threads, int port);
	void accept_loop();

	bool is_working();

private:
	bool working;

	HANDLE m_iocp;

	SOCKET m_server;
	WSADATA wsa;

	PoolType pool;
};