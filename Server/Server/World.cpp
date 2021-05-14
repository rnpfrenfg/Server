#include "World.h"

World::World()
{
	InitializeSRWLock(&lock);
}

void World::join(participant_ptr participant)
{
	AcquireSRWLockExclusive(&lock);
	m_participants.insert(participant);
	ReleaseSRWLockExclusive(&lock);

	std::for_each(m_recent_msgs.begin(), m_recent_msgs.end(), std::bind(&Participant::deliver, participant, std::placeholders::_1));
}

void World::leave(participant_ptr participant)
{
	AcquireSRWLockExclusive(&lock);
	m_participants.erase(participant);
	ReleaseSRWLockExclusive(&lock);
}

void World::deliver(const DataMessage& msg)
{
	AcquireSRWLockShared(&lock);
	std::for_each(m_participants.begin(), m_participants.end(), std::bind(&Participant::deliver, std::placeholders::_1, std::ref(msg)));
	ReleaseSRWLockShared(&lock);
}