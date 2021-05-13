#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "include.h"
#include "resource.h"
#include "Connector.h"
#include "DataMessage.h"

#define DUMMY

class DummyClient
{
public:
	std::thread* threadList[1000];
	void GO(int threads);
};