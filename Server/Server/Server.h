#pragma once

#include "include.h"

#include "Iocp.h"
#include "Session.h"

class Server
{
public:
	Server(Iocp& iocp);
	void handle_accept(CSocket* sock, SOCKADDR_IN* addr);
private:
	Iocp& m_iocp;
	World m_world;
};

typedef std::shared_ptr<Server> server_ptr;
typedef std::list<server_ptr> server_list;