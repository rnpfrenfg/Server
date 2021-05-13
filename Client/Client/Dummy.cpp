#include "Dummy.h"
#define DUMMY
#ifdef DUMMY

void CCreateConsole()
{
	AllocConsole();

	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	return;
}

void dummyRecv(int len, char* buffer)
{
}

void dummyThread(wchar_t ch)
{
	wchar_t test[50];

	_snwprintf(test, _countof(test), L"%c", ch);

	Connector c;
	c.Connect("127.0.0.1", 12341);

	if (!c.IsConnected())
		return;

	while (true)
	{
		if (!c.IsConnected())
		{
			char aaa[] = { ch,'\0' };
			MessageBoxA(NULL, "disconnected", aaa, MB_OK);
			return;
		}
		c.Send(test, _countof(test));
	}
}

void DummyClient::GO(int threads)
{
	CCreateConsole();

	wchar_t nowChar = L'a';

	for (int i = 0; i < threads; i++)
	{
		threadList[i] = new std::thread(dummyThread, nowChar++);
	}

	for (int i = 0; i < threads; i++)
	{
		threadList[i]->join();
	}

	exit(0);
}

#endif