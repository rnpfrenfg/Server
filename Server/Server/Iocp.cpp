#include "iocp.h"

HANDLE new_iocp(DWORD threads)
{
	return CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, threads);
}

bool connect_to_iocp(HANDLE device, HANDLE iocp, ULONG_PTR key)
{
	HANDLE handle = CreateIoCompletionPort(device, iocp, key, 0);
	return handle == iocp;
}

void iocp_thread(HANDLE handle, Iocp* iocp)
{
	DWORD recvLen;

	SOCKET* sock;
	DataMessage* msg;

	BOOL failed;

	while (true)
	{
		failed = !GetQueuedCompletionStatus(handle, &recvLen, (ULONG_PTR*)&sock, (LPOVERLAPPED*)&msg, INFINITE);

		iocp->end_read(failed, msg);
		continue;
	}
}
void Iocp::end_read(BOOL failed, DataMessage* msg)
{
	msg->callback(failed);
}

void create_iocp_threads(Iocp* p, HANDLE iocp, int threads)
{
	for (int i = 0; i < threads; i++)
		new std::thread(iocp_thread, iocp, p);
}

void Iocp::accept_loop()
{
	working = true;

	//start loop
	SOCKET socket;
	SOCKADDR_IN clInfo;

	int addrLen = sizeof(clInfo);
	while (true)
	{
		socket = accept(m_server, (SOCKADDR*)&clInfo, &addrLen);
		if (socket == INVALID_SOCKET)
		{
			continue;
		}

		if (connect_to_iocp((HANDLE)socket, this->m_iocp, (ULONG_PTR)NULL) == false)
		{
			continue;
		}

		acceptHandler(&socket, &clInfo);
	}
}

void Iocp::open(int max_active_threads, int threads, int port)
{
	WSAStartup(MAKEWORD(2, 2), &wsa);

	m_server = socket(AF_INET, SOCK_STREAM, 0);
	if (m_server == INVALID_SOCKET)
	{
		//TODO
	}

	SOCKADDR_IN sock_info;
	memset(&sock_info, 0, sizeof(sock_info));
	sock_info.sin_family = AF_INET;
	sock_info.sin_port = htons(port);
	sock_info.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	auto err = bind(m_server, (SOCKADDR*)&sock_info, sizeof(sock_info));
	if (err == SOCKET_ERROR)
	{

	}
	err = listen(m_server, SOMAXCONN);
	if (err == SOCKET_ERROR)
	{

	}

	//create iocp
	SYSTEM_INFO sys;
	GetSystemInfo(&sys);

	HANDLE iocp = new_iocp(max_active_threads);
	if (iocp == INVALID_HANDLE_VALUE)
	{

	}

	create_iocp_threads(this, iocp, threads);
	
	m_iocp = iocp;
}

bool Iocp::is_working()
{
	return working;
}

void Iocp::async_read_header(SOCKET sock, DataMessage* msg, IocpCallback func)
{
	msg->wsabuf.buf = msg->data();
	msg->wsabuf.len = DataMessage::header_length;
	msg->callback = func;
	async_read(sock, msg, func);
}

void Iocp::async_read_body(SOCKET sock, DataMessage* msg, int len, IocpCallback func)
{
	msg->wsabuf.buf = msg->body();
	msg->wsabuf.len = len;
	msg->callback = func;
	async_read(sock, msg, func);
}

void Iocp::async_read(SOCKET sock, DataMessage* msg, IocpCallback func)
{
	static DWORD recv_flags = MSG_WAITALL;
	int error = WSARecv(sock, &msg->wsabuf, 1, NULL, &recv_flags, &msg->overlapped, NULL);
	if (error == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			func(1);
			return;
		}
	}
}

void Iocp::async_write(SOCKET sock, DataMessage& msg, int len, IocpCallback func)
{
	msg.wsabuf.buf = msg.data();
	msg.wsabuf.len = len;
	msg.callback = func;

	int result = WSASend(sock, &msg.wsabuf, 1, NULL, 0, &msg.overlapped, NULL);
	if (result != 0)
	{
		int err = WSAGetLastError();
		if (err == WSA_IO_PENDING)
		{
			func(1);
			return;
		}
	}
}