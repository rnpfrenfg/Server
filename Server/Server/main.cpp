#include "include.h"

#include "iocp.h"

bool CheckSameProgramExists()
{
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
		if (err == ERROR_ACCESS_DENIED)//관리자 권한 필요
			return false;

		if (err == ERROR_ALREADY_EXISTS)
		{
			privateNamespace = OpenPrivateNamespace(boundary, NAMESPACENAME);
			if (privateNamespace == NULL)
				return false;
			else
			{
				//Namespace Opened
			}
		}
		else//Unexpecteed Error
			return false;
	}

	TCHAR handleName[64];
	StringCchPrintf(handleName, _countof(handleName), TEXT("%s\\%s"), NAMESPACENAME, CHECK_SAME_NAME_EXISTS);
	HANDLE sameExists = CreateEvent(NULL, FALSE, FALSE, handleName);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int main()
{
	if (CheckSameProgramExists())
	{
		std::cout << "server running";
	}

	TCHAR szPort[100];
	int i = GetPrivateProfileString(_T("SERVER"), _T("PORT"), _T(""), szPort, 100, _T(".\\Server.ini"));
	if (0 == i)
	{
		std::cout << "Server Config Error";
		return 0;
	}

	Iocp iocp;

	SYSTEM_INFO sys;
	GetSystemInfo(&sys);

	int processors = sys.dwNumberOfProcessors;
	iocp.open(processors, processors * 2, _wtoi(szPort));

	std::cout << "Server Port : " << szPort << "\nNow Running!" << std::endl;
	iocp.accept_loop();
	printf("end\n");
}