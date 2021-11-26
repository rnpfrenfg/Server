#include "Server.h"

Server::Server(Iocp& iocp)
	:
	m_iocp(iocp)
{
	iocp.acceptHandler = std::bind(&Server::handle_accept, this, std::placeholders::_1, std::placeholders::_2);
}

#include <mutex>
static std::mutex tempUUIDLock;

void Server::handle_accept(CSocket* sock, SOCKADDR_IN* addr)
{
	static int tempUUID = 0;
	tempUUIDLock.lock();
	int id = tempUUID++;
	tempUUIDLock.unlock();
	UserUID uuid = std::to_string(id);

	session_ptr new_session(new Session(*sock, std::make_shared<UserUID>(uuid), m_world));
	new_session->start();

	int namelen = sizeof(addr);
	getpeername(sock->sock, (sockaddr*)addr, &namelen);

	int port = addr->sin_port;

	std::cout << "Client connected : " << inet_ntoa(addr->sin_addr) << " : " << port << std::endl;
}