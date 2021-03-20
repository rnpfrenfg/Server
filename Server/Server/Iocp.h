#pragma once
#include "include.h"

#include "DataMessage.h"

typedef std:: function<void(SOCKET*, SOCKADDR_IN*)> IocpAcceptHandler;

class Iocp
{
public:
	static void async_write(SOCKET sock, DataMessage& msg, int len, IocpCallback func);

	static void async_read_header(SOCKET sock, DataMessage* msg, IocpCallback func);

	static void async_read_body(SOCKET sock, DataMessage* msg, int len, IocpCallback func);

	static void async_read(SOCKET sock, DataMessage* msg, IocpCallback func);

public:
	IocpAcceptHandler acceptHandler;

	void open(int max_active_threads, int threads, int port);
	void accept_loop();

	bool is_working();

	//TODO
	void end_read(BOOL failed, DataMessage* msg);
private:
	int recv_flags = 0;

	bool working;

	HANDLE m_iocp;

	SOCKET m_server;
	WSADATA wsa;
};