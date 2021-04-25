#pragma once

#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <sddl.h>
#include <strsafe.h>
#include <Windows.h>
#include <thread>
#include <iostream>
#include <list>
#include <set>
#include <algorithm>
#include <deque>
#include <functional>

#include "DataMessage.h"

typedef std::deque<DataMessage> DataMessageQueue;