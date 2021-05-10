#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _AFXDLL

#include <afxwin.h>

#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <sddl.h>
#include <strsafe.h>
#include <thread>
#include <iostream>
#include <list>
#include <set>
#include <algorithm>
#include <deque>
#include <functional>
