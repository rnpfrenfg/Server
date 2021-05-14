#pragma once

#include "Participant.h"
#include "World.h"
#include "Iocp.h"

class Session : public Participant, public std::enable_shared_from_this<Session>
{
public:
	Session(SOCKET sock, World& world)
		: m_socket(sock),
		m_world(world)
	{
		InitializeCriticalSection(&cs);
	}

	SOCKET socket()
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
	SOCKET m_socket;
	World& m_world;
	DataMessage m_read_msg;
	DataMessageQueue m_write_msgs;
	CRITICAL_SECTION cs;
};

typedef std::shared_ptr<Session> session_ptr;