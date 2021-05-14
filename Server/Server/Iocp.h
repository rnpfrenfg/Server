#pragma once
#include "include.h"

#include "DataMessage.h"

typedef std::function<void(SOCKET*, SOCKADDR_IN*)> IocpAcceptHandler;

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
};

class Iocp
{
public:
	static void async_write(SOCKET sock, DataMessage& msg, int len, IocpCallback func);

	static void async_read_header(SOCKET sock, DataMessage* msg, IocpCallback func);

	static void async_read_body(SOCKET sock, DataMessage* msg, int len, IocpCallback func);

	static void async_read(SOCKET sock, DataMessage* msg, IocpCallback func, IOInfo* ioInfo);

public:
	IocpAcceptHandler acceptHandler;

	void open(int max_active_threads, int threads, int port);
	void accept_loop();

	bool is_working();

private:
	bool working;

	HANDLE m_iocp;

	SOCKET m_server;
	WSADATA wsa;
};