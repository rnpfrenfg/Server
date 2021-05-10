#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>

class DataMessage
{
public:
	enum { header_length = sizeof(int) };
	enum { max_body_length = 2048 };

	DataMessage() = default;

	const char* data()  const
	{
		return m_data;
	}

	char* data()
	{
		return m_data;
	}

	size_t length() const
	{
		return header_length + m_body_length;
	}

	const char* body() const
	{
		return m_data + header_length;
	}

	char* body()
	{
		return m_data + header_length;
	}

	size_t body_length() const
	{
		return m_body_length;
	}

	void body_length(size_t length)
	{
		m_body_length = length;
		if (m_body_length > max_body_length)
			m_body_length = max_body_length;
	}

	bool decode_header()
	{
		using namespace std;
		char header[header_length + 1] = "";
		strncat_s(header, m_data, header_length);
		m_body_length = atoi(header);
		if (m_body_length > max_body_length)
		{
			m_body_length = 0;
			return false;
		}
		return true;
	}

	void encode_header()
	{
		using namespace std;
		char header[header_length + 1] = "";
		sprintf_s(header, "%4d", m_body_length);
		memcpy(m_data, header, header_length);
	}

private:
	char m_data[header_length + max_body_length];
	size_t m_body_length;
};