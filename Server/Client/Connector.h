#pragma once

#pragma comment (lib,"ws2_32.lib")

#include <WinSock2.h>
#include <Ws2tcpip.h>

#include "DataMessage.h"

typedef std::function<void(int, char*)> callBack;

class Connector
{
public:
	Connector() = default;

	void Send(DataMessage* msg);
	void RecvThread();
	void Connect(const char* ip, u_short port);
	bool IsConnected();

	callBack on_recv;

	SOCKET sock;
	SOCKADDR_IN addr;

	void Disconnect();

private:
	DataMessage m_msg;

	bool m_connected = false;

	int recvFlag = MSG_WAITALL;
};
