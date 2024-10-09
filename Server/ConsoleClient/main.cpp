#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <thread>

#include "Connector.h"


Connector conn;
char ip[100];
char szport[100];
DataMessage msg;
std::thread* thred = nullptr;

void Disconnect()
{
	printf("\n\n\ndisconnected\n\n\n");
	conn.Disconnect();
}

void RecvThread()
{
	conn.RecvThread();
}

void OnRecv(int len, char* buffer)
{
	if (len == 0 || len == SOCKET_ERROR)
	{
		Disconnect();
		return;
	}

#ifndef SKIP_PRINT
	wchar_t* buf = (wchar_t*)buffer;
	buf[len / sizeof(wchar_t)] = L'\0';

	_putws((wchar_t*) buffer);
#endif
}

void Connect()
{
	u_short port = atoi(szport);

	conn.on_recv = OnRecv;
	conn.Connect(ip, port);

	if (!conn.IsConnected())
		return;

	if (thred != nullptr)
	{
		thred->join();
		delete thred;
		thred = nullptr;
	}
	thred = new std::thread(RecvThread);
}

int main()
{

	printf("ip\t(default:localhost) : ");
	gets_s(ip);

	printf("\nport\t(default:12341) : ");
	gets_s(szport);

	if (ip[0] == 0)
	{
		strcpy(ip, "127.0.0.1");
	}

	if (szport[0] == 0)
	{
		strcpy(szport, "12341");
	}

	int len = 0;

	while (true)
	{
		while (true)
		{
			printf("\n\ntry connect to %s :: %s\n", ip, szport);
			Connect();

			if (!conn.IsConnected())
			{
				printf("cannot connect to server\n");
				puts("press key to retry\n");
				_getch();
				puts("\n");
			}
			else
				break;
		}
		printf("connected\n\n\n");

		while (true)
		{
			if (!conn.IsConnected())
				break;

			wscanf(L"%s", msg.body());
			len = sizeof(wchar_t) * wcslen((const wchar_t*) msg.body());

			if (len == 0)
				continue;
			msg.body_length(len);

			conn.Send(&msg);
		}
	}
}