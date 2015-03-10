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
    void leave_current_room();
    void send_private_message(const std::wstring& user, const std::wstring& message);

private:
    jabbr::jabbr_client m_jabbr_client;
    std::shared_ptr<jabbr_console> m_console;
    jabbr::user m_user;
    std::vector<jabbr::room> m_rooms;
    parser parser;

    jabbr::room m_current_room;
};

