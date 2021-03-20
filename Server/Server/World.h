#pragma once

#include "include.h"

#include "Participant.h"

class World
{
public:
	void join(participant_ptr participant);
	void leave(participant_ptr participant);
	void deliver(const DataMessage& msg);

private:
	std::set<participant_ptr> m_participants;
	enum {max_recent_msgs = 0};
	DataMessageQueue m_recent_msgs;
};

