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

#define MAX_PLAYER_PER_ROOM 8
#define roomMax 10000
#define roomNameMax 30

#define CLASSNAME TEXT("ProgramCalssName")
#define WINDOWNAME TEXT("ProgramWindowName")
#define CHECK_SAME_NAME_EXISTS TEXT("checkchec311331")
#define BOUNDARYNAME TEXT("BOUNDARYNAME123123")
#define NAMESPACENAME TEXT("NAMESPACENAME123132")

typedef std::deque<DataMessage> DataMessageQueue;