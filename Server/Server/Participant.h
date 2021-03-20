#pragma once

#include "include.h"

class Participant
{
public:
	virtual ~Participant() {}
	virtual void deliver(const DataMessage& msg) = 0;
};

typedef std::shared_ptr<Participant> participant_ptr;