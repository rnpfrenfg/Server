#include "Session.h"

void Session::start()
{
	m_world.join(shared_from_this());
	Iocp::async_read_header(m_socket, &m_read_msg, std::bind(&Session::handle_read_header, shared_from_this(), std::placeholders::_1));
}

void Session::close()
{
	m_world.leave(shared_from_this());

	shutdown(m_socket, SD_BOTH);
	closesocket(m_socket);
}

void Session::deliver(const DataMessage& msg)
{
	EnterCriticalSection(&cs);
	bool write_in_progress = !m_write_msgs.empty();
	m_write_msgs.push_back(msg);
	LeaveCriticalSection(&cs);

	if (!write_in_progress)
	{
		Iocp::async_write(m_socket, m_write_msgs.front(),
			m_write_msgs.front().length(),
			std::bind(&Session::handle_write, shared_from_this(), std::placeholders::_1));
	}
}

void Session::handle_read_header(BOOL failed)
{
	if (!failed && m_read_msg.decode_header())
	{
		Iocp::async_read_body(m_socket, 
			&m_read_msg, 
			m_read_msg.body_length(), 
			std::bind(&Session::handle_read_body, shared_from_this(), std::placeholders::_1));
	}
	else
	{
		close();
	}
}
void Session::handle_read_body(BOOL failed)
{
	if (!failed)
	{
		m_world.deliver(m_read_msg);
		Iocp::async_read_header(m_socket, &m_read_msg, std::bind(&Session::handle_read_header, shared_from_this(), std::placeholders::_1));
	}
	else
	{
		close();
	}
}

void Session::handle_write(BOOL failed)
{
	if (failed)
	{
		EnterCriticalSection(&cs);
		m_write_msgs.clear();
		LeaveCriticalSection(&cs);
		return;
	}

	EnterCriticalSection(&cs);
	m_write_msgs.pop_front();
	bool end = m_write_msgs.empty();
	LeaveCriticalSection(&cs);

	if (!end)
	{
		Iocp::async_write(m_socket, m_write_msgs.front(),
			m_write_msgs.front().length(),
			std::bind(&Session::handle_write, shared_from_this(), std::placeholders::_1));
	}
}