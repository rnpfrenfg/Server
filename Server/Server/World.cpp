#include "World.h"

World::World()
{
	InitializeCriticalSection(&cs);
}

void World::join(participant_ptr participant)
{
	EnterCriticalSection(&cs);
	m_participants.insert(participant);

	std::for_each(m_recent_msgs.begin(), m_recent_msgs.end(), std::bind(&Participant::deliver, participant, std::placeholders::_1));
	LeaveCriticalSection(&cs);
}

void World::leave(participant_ptr participant)
{
	EnterCriticalSection(&cs);
	m_participants.erase(participant);
	LeaveCriticalSection(&cs);
}

void World::deliver(const DataMessage& msg)
{
	EnterCriticalSection(&cs);
	std::for_each(m_participants.begin(), m_participants.end(), std::bind(&Participant::deliver, std::placeholders::_1, std::ref(msg)));
	LeaveCriticalSection(&cs);
}