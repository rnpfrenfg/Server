#include "Server.h"

Server::Server(Iocp& iocp)
	:
	m_iocp(iocp)
{
	iocp.acceptHandler = std::bind(&Server::handle_accept, this, std::placeholders::_1, std::placeholders::_2);
}
void Server::handle_accept(SOCKET* sock, SOCKADDR_IN* addr)
{
	session_ptr new_session(new Session(*sock, m_world));
	new_session->start();

	int namelen = sizeof(addr);
	getpeername(*sock, (sockaddr*)addr, &namelen);

	int port = addr->sin_port;

	std::cout << "Client connected : " << inet_ntoa(addr->sin_addr) << " : " << port << std::endl;
}