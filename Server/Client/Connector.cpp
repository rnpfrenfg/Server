#include "Connector.h"

void Connector::Connect(const char* ip, u_short port)
{
	if (m_connected)
		return;

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != NO_ERROR)
	{
		m_connected = false;
		return;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		m_connected = false;
		return;
	}

	memset((void*)&addr, 0, sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(addr.sin_addr.s_addr));
	addr.sin_port = htons(port);

	if (connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		m_connected = false;
		return;
	}

	m_connected = true;
	return;
}

bool Connector::IsConnected()
{
	return m_connected;
}

void Connector::Send(void* buf, size_t len)
{
	char header[DataMessage::header_length + 1];

	sprintf_s(header, "%4d", len);

	send(sock, (const char*)header, DataMessage::header_length, 0);
	send(sock, (const char*)buf, len, 0);
}

void Connector::RecvThread()
{
	bool sizeRecved = true;
	int toRecv = 0;

	while (true)
	{
		if (m_connected == false)
			return;

		sizeRecved = !sizeRecved;

		if (sizeRecved)
		{
			int recvLen = recv(sock, m_msg.body(), toRecv, recvFlag);

#ifdef DUMMY
			if (recvLen == SOCKET_ERROR)
			{
				m_connected = false;
				return;
			}
#endif

			on_recv(recvLen, m_msg.body());
		}
		else
		{
			recv(sock, m_msg.data(), DataMessage::header_length, recvFlag);
			char* buf = m_msg.body();
			buf[0] = '\0';
			toRecv = atoi(m_msg.data());
		}
	}
}

void Connector::Disconnect()
{
	m_connected = false;
	shutdown(sock, SD_BOTH);
	closesocket(sock);
}