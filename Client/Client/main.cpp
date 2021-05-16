#define _CRT_SECURE_NO_WARNINGS

#include "include.h"

#include "resource.h"
#include "Connector.h"
#include "DataMessage.h"

Connector con;

HANDLE connectButton;
HANDLE closeButton;
HANDLE sendButton;

HANDLE dlgEdit;

char ip[50];
char port[50];

wchar_t toSend[DataMessage::max_body_length / sizeof(wchar_t)];

CRITICAL_SECTION cs;
std::wstring talkLog;

std::thread* recvThread;

HWND hwnd;

void UpdateButton()
{
	int connected = con.IsConnected();

	EnableWindow((HWND)connectButton, !connected);
	EnableWindow((HWND)closeButton, connected);
	EnableWindow((HWND)sendButton, connected);
}

void CUpdateDlg()
{
	SetDlgItemText(hwnd, IDC_EDIT6, talkLog.c_str());
	SendMessage((HWND)dlgEdit, EM_LINESCROLL, INT_MAX, INT_MAX);
}

void RecvThread()
{
	con.RecvThread();
}

void Disconnect()
{
	con.Disconnect();
	UpdateButton();
}

INT_PTR CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:

		::hwnd = hwnd;
		dlgEdit = GetDlgItem(hwnd, IDC_EDIT6);

		SetDlgItemText(hwnd, IDC_STATIC1, L"");
		SetDlgItemText(hwnd, IDC_STATIC2, L"Address");
		SetDlgItemText(hwnd, IDC_STATIC3, L"Port");

		SetDlgItemText(hwnd, IDC_EDIT2, L"127.0.0.1");
		SetDlgItemText(hwnd, IDC_EDIT5, L"12341");

		SetDlgItemText(hwnd, IDC_BUTTON1, L"Connect");
		connectButton = GetDlgItem(hwnd, IDC_BUTTON1);
		SetDlgItemText(hwnd, IDC_BUTTON2, L"Close");
		closeButton = GetDlgItem(hwnd, IDC_BUTTON2);
		SetDlgItemText(hwnd, IDC_BUTTON3, L"Send");
		sendButton = GetDlgItem(hwnd, IDC_BUTTON3);
		SetDlgItemText(hwnd, IDC_BUTTON4, L"Clear");

		UpdateButton();

		return true;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
		{
			GetDlgItemTextA(hwnd, IDC_EDIT2, ip, sizeof(ip));
			GetDlgItemTextA(hwnd, IDC_EDIT5, port, sizeof(port));

			int iPort = atoi(port);

			con.Connect(ip, iPort);
			UpdateButton();
			if (con.IsConnected())
			{
				recvThread = new std::thread(RecvThread);
			}
		}
		return true;

		case IDC_BUTTON2:
		{
			Disconnect();
			UpdateButton();
		}
		return true;

		case IDC_BUTTON3:
		{
			int len = GetDlgItemText(hwnd, IDC_EDIT1, toSend, DataMessage::max_body_length / 2);
			con.Send(toSend, _countof(toSend));
		}
		return true;

		case IDC_BUTTON4:
		{
			EnterCriticalSection(&cs);
			talkLog = L"";
			CUpdateDlg();
			LeaveCriticalSection(&cs);
		}
		return true;
		}
		return false;

	case WM_CLOSE:
		EndDialog(hwnd, IDOK);
		return true;
	}
	return false;
}

void OnRecv(int len, char* buffer)
{
	if (len == 0 || len == SOCKET_ERROR)
	{
		Disconnect();
		return;
	}

	wchar_t* buf = (wchar_t*) buffer;
	buf[len] = L'\0';

	EnterCriticalSection(&cs);
	talkLog += buf;
	talkLog += L"\r\n";
	CUpdateDlg();
	LeaveCriticalSection(&cs);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	InitializeCriticalSection(&cs);
	con.on_recv = OnRecv;

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), HWND_DESKTOP, winProc);
	return 0;
}