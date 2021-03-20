#include "World.h"

void World::join(participant_ptr participant)
{
	m_participants.insert(participant);

	std::for_each(m_recent_msgs.begin(), m_recent_msgs.end(), std::bind(&Participant::deliver, participant, std::placeholders::_1));
}

void World::leave(participant_ptr participant)
{
	m_participants.erase(participant);
}

void World::deliver(const DataMessage& msg)
{
	m_recent_msgs.push_back(msg);
	while (m_recent_msgs.size() > max_recent_msgs)
		m_recent_msgs.pop_front();

	std::for_each(m_participants.begin(), m_participants.end(), std::bind(&Participant::deliver, std::placeholders::_1, std::ref(msg)));
}