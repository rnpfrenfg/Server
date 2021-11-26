#pragma once

#include <iostream>
#include <mutex>

#include <locale>
#include <codecvt>

#include "UserUID.h"
#include "mysql.h"

class ChatLogger
{
private:
	ChatLogger() = default;
	~ChatLogger()
	{
		if (connected)
			mysql_close(&conn);
	}
	
	static ChatLogger* instance;

public:
	static ChatLogger* GetInstance()
	{
		return instance;
	}

	static bool Init(const char* ip, const char* id, const char* pw, const char* db, unsigned int port)
	{
		if (instance != nullptr)
			return false;

		instance = new ChatLogger();
		return instance->_Init(ip, id, pw, db, port);
	}

	inline void Log(UserUID* uuid, const wchar_t* msg, int msgLen)
	{
		MYSQL_BIND bind[2] = { 0, };
		bind[0].buffer_type = MYSQL_TYPE_STRING;
		bind[0].buffer = (void*)uuid->c_str();
		bind[0].buffer_length = uuid->length();
		bind[0].is_null = 0;
		bind[0].length = nullptr;

		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
		std::string converted = conv.to_bytes(msg);
		std::cout << converted<<'\n';
		std::cout << converted.c_str()<<'\n';

		bind[1].buffer_type = MYSQL_TYPE_VARCHAR;
		bind[1].buffer = (void*)converted.c_str();
		bind[1].buffer_length = converted.length();
		bind[1].is_null = 0;
		bind[1].length = nullptr;

		stmtLock.lock();//TODO
		mysql_stmt_bind_param(insertChatLog, bind);
		mysql_stmt_execute(insertChatLog);
		stmtLock.unlock();


		//
		/*
				MYSQL_RES* res;
		MYSQL_ROW row;

		mysql_query(&conn, "select uuid,msg from chat");

		res = mysql_store_result(&conn);        // 데이터베이스로부터 받은 값 변수에 넣기
		while ((row = mysql_fetch_row(res)) != NULL)
		{        // 디비로 부터 받은 데이터 활용
			printf("%s::::::::::::%s\n", row[0], row[1]);                                  //여러 값을 받개되면 배열 뒤에 붙어짐 ex) row[1], row[2]

		}
		*/
	}

private:
	bool _Init(const char* ip, const char* id, const char* pw, const char* db, unsigned int port)
	{
		if (connected)
			return false;

		std::cout << "mysql version : " << mysql_get_client_info() << '\n';

		mysql_init(&conn);

		if (mysql_real_connect(&conn, ip, id, pw, db, port, NULL, NULL) == 0)
			return false;

		if (mysql_set_character_set(&conn, "utf8"))
			return false;

		insertChatLog = mysql_stmt_init(&conn);
		const char* query = "insert into chat(uuid,msg) values(?,?)";
		mysql_stmt_prepare(insertChatLog, query, strlen(query));

		return connected = true;
	}

	bool connected = false;

	MYSQL conn;
	MYSQL_STMT* insertChatLog;

	std::mutex stmtLock;
};