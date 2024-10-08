#include "include.h"

#include <conio.h>

#include "Server.h"
#include "iocp.h"

bool CheckSameProgramExists()
{
	const auto CHECK_SAME_NAME_EXISTS = TEXT("asdfasdf");
	const auto BOUNDARYNAME = TEXT("boundary");
	const auto NAMESPACENAME = TEXT("namespace");

	HANDLE boundary = CreateBoundaryDescriptor(BOUNDARYNAME, 0);

	BYTE localAdminSID[SECURITY_MAX_SID_SIZE];
	DWORD sidSize = sizeof(localAdminSID);

	if (!CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &localAdminSID, &sidSize))
		return false;

	if (!AddSIDToBoundaryDescriptor(&boundary, &localAdminSID))
		return false;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = FALSE;
	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(TEXT("D:(A;;GA;;;BA)"), SDDL_REVISION_1, &sa.lpSecurityDescriptor, NULL))
		return false;

	HANDLE privateNamespace = CreatePrivateNamespace(&sa, boundary, NAMESPACENAME);

	LocalFree(sa.lpSecurityDescriptor);

	DWORD err = GetLastError();
	if (privateNamespace == NULL)
	{
		if (err == ERROR_ACCESS_DENIED)
		{
			std::cout << "you must run as administrator...\n";
			return false;
		}

		if (err == ERROR_ALREADY_EXISTS)
		{
			privateNamespace = OpenPrivateNamespace(boundary, NAMESPACENAME);
			if (privateNamespace == NULL)
				return false;
		}
		else//Unexpecteed Error
			return false;
	}

	TCHAR handleName[64];
	StringCchPrintf(handleName, _countof(handleName), TEXT("%s\\%s"), NAMESPACENAME, CHECK_SAME_NAME_EXISTS);
	HANDLE sameExists = CreateEvent(NULL, FALSE, FALSE, handleName);

	return ERROR_ALREADY_EXISTS != GetLastError();
}

int main()
{
	if (false && !(CheckSameProgramExists()))
	{
		std::cout << "already running...\n";
		_getch();
		return 0;
	}

	TCHAR szPort[100];
	int i = GetPrivateProfileString(_T("SERVER"), _T("PORT"), _T(""), szPort, 100, _T(".\\Server.ini"));
	if (0 == i)
	{
		std::cout << "Server Config Error\n";
		_getch();
		return 0;
	}

	SYSTEM_INFO sys;
	GetSystemInfo(&sys);

	Iocp iocp;
	int processors = sys.dwNumberOfProcessors;
	if (!iocp.open(processors, processors * 2, _wtoi(szPort)))
	{
		std::cout << "Cannot open server\n";
		_getch();
		return 0;
	}

	server_list servers;

	server_ptr server(new Server(iocp));
	servers.push_back(server);

	std::cout << "Server Port : " << _wtoi(szPort) << "\nNow Running!" << std::endl;
	iocp.accept_loop();

	std::cout << "end";
}