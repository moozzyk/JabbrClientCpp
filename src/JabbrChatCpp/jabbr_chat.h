#pragma once
#include "jabbrclient\jabbr_client.h"
#include "jabbrclient\room.h"
#include "jabbr_console.h"
#include "parser.h"

class jabbr_chat
{
public:
    jabbr_chat(std::wstring url);
    ~jabbr_chat();

    void run(std::wstring user_name, std::wstring password);

private:
    bool on_user_input(const std::wstring& user_input);
    void join_room(const std::wstring& room);

private:
    jabbr::jabbr_client m_jabbr_client;
    jabbr_console m_console;
    jabbr_user m_user;
    parser parser;

    jabbr::room m_current_room;
};

