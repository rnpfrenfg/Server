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

void DummyRecvThread(Connector* c)
{
	c->RecvThread();
}

void dummyThread(wchar_t ch)
{
	wchar_t test[129] = { ch,'\0' };

	_snwprintf(test, sizeof(test), L"%s%s", test, test);
	_snwprintf(test, sizeof(test), L"%s%s", test, test);
	_snwprintf(test, sizeof(test), L"%s%s", test, test);
	_snwprintf(test, sizeof(test), L"%s%s", test, test);
	_snwprintf(test, sizeof(test), L"%s%s", test, test);
	_snwprintf(test, sizeof(test), L"%s%s", test, test);
	
	int wlen = wcslen(test) * sizeof(wchar_t);

	Connector c;
	c.Connect("127.0.0.1", 12341);
	c.on_recv = dummyRecv;

	if (!c.IsConnected())
		return;

	new std::thread(DummyRecvThread, &c);

	while (true)
	{

		if (!c.IsConnected())
		{
			return;
		}
		c.Send(test, wlen);
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