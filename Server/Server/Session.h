#pragma once

#include "Participant.h"
#include "World.h"
#include "Iocp.h"
#include "CSocket.h"
#include "UserUID.h"

class Session : public Participant, public std::enable_shared_from_this<Session>
{
public:
	Session(CSocket sock, std::shared_ptr<UserUID> uuid, World& world)
		: m_socket(sock),
		m_world(world),
		uuid(uuid)
	{
		InitializeCriticalSection(&cs);
	}

	CSocket socket()
	{
		return m_socket;
	}

	void start();
	void close();
	void deliver(const DataMessage& msg) override;
	void handle_read_header(BOOL failed);
	void handle_read_body(BOOL failed);
	void handle_write(BOOL failed);

private:
	CSocket m_socket;
	World& m_world;
	DataMessage m_read_msg;
	DataMessageQueue m_write_msgs;
	CRITICAL_SECTION cs;
	std::shared_ptr<UserUID> uuid;
};

typedef std::shared_ptr<Session> session_ptr;