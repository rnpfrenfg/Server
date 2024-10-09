#include "iocp.h"

typedef MemoryPool<IOInfo> PoolType;

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
	IOInfo* ioInfo;

	BOOL failed;

	while (true)
	{
		failed = !GetQueuedCompletionStatus(handle, &recvLen, (ULONG_PTR*)&sock, (LPOVERLAPPED*)&ioInfo, INFINITE);

		ioInfo->callback(failed);
		((PoolType*)ioInfo->_pool)->Delete(ioInfo);
		continue;
	}
}

void create_iocp_threads(Iocp* p, HANDLE iocp, int threads)
{
	for (int i = 0; i < threads; i++)
		new std::thread(iocp_thread, iocp, p);
}

void Iocp::accept_loop()
{
	working = true;

	CSocket csock;
	SOCKET socket;
	SOCKADDR_IN clInfo;

	csock.__iocp = &pool;
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

		csock.sock = socket;
		acceptHandler(&csock, &clInfo);
	}
}

bool Iocp::open(int max_active_threads, int threads, int port)
{
	pool.Init(100000, false);

	auto err = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (err)
		return false;

	m_server = socket(AF_INET, SOCK_STREAM, 0);
	if (m_server == INVALID_SOCKET)
		return false;

	SOCKADDR_IN sock_info;
	memset(&sock_info, 0, sizeof(sock_info));
	sock_info.sin_family = AF_INET;
	sock_info.sin_port = htons(port);
	sock_info.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	err = bind(m_server, (SOCKADDR*)&sock_info, sizeof(sock_info));
	if (err == SOCKET_ERROR)
		return false;

	err = listen(m_server, SOMAXCONN);
	if (err == SOCKET_ERROR)
		return false;

	SYSTEM_INFO sys;
	GetSystemInfo(&sys);

	HANDLE iocp = new_iocp(max_active_threads);
	if (iocp == INVALID_HANDLE_VALUE)
		return false;
	create_iocp_threads(this, iocp, threads);

	m_iocp = iocp;
	return true;
}

bool Iocp::is_working()
{
	return working;
}

IOInfo* Iocp::CreateNewIoInfo(CSocket& sock)
{
	IOInfo* info = ((PoolType*)sock.__iocp)->New();
	info->_pool = sock.__iocp;
	return info;
}

void Iocp::async_read_header(CSocket& sock, DataMessage* msg, IocpCallback func)
{
	IOInfo* ioInfo = CreateNewIoInfo(sock);
	ioInfo->wsabuf.buf = msg->data();
	ioInfo->wsabuf.len = DataMessage::header_length;
	ioInfo->callback = func;
	async_read(sock, msg, func, ioInfo);
}

void Iocp::async_read_body(CSocket& sock, DataMessage* msg, IocpCallback func)
{
	IOInfo* ioInfo = CreateNewIoInfo(sock);
	ioInfo->wsabuf.buf = msg->body();
	ioInfo->wsabuf.len = msg->body_length();
	ioInfo->callback = func;
	async_read(sock, msg, func, ioInfo);
}

void Iocp::async_read(CSocket& sock, DataMessage* msg, IocpCallback func, IOInfo* ioInfo)
{
	static DWORD recv_flags = MSG_WAITALL;
	int error = WSARecv(sock.sock, &ioInfo->wsabuf, 1, NULL, &recv_flags, &ioInfo->overlapped, NULL);
	if (error == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			func(1);
			((PoolType*)sock.__iocp)->Delete(ioInfo);
		}
	}
}

void Iocp::async_write(CSocket& sock, DataMessage& msg, IocpCallback func)
{
	IOInfo* ioInfo = CreateNewIoInfo(sock);
	msg.encode_header();
	ioInfo->wsabuf.buf = msg.data();
	ioInfo->wsabuf.len = msg.length();
	ioInfo->callback = func;

	int result = WSASend(sock.sock, &ioInfo->wsabuf, 1, NULL, 0, &ioInfo->overlapped, NULL);
	if (result != 0)
	{
		int err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			func(1);
			((PoolType*)sock.__iocp)->Delete(ioInfo);
		}
	}
}