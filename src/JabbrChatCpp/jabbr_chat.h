#pragma once
#include "jabbrclient\jabbr_client.h"
#include "jabbr_console.h"

class jabbr_chat
{
public:
    jabbr_chat(std::wstring url);
    ~jabbr_chat();

    void run(std::wstring user_name, std::wstring password);

private:
    bool on_user_input(const std::wstring& user_input);

private:
    jabbr::jabbr_client m_jabbr_client;
    jabbr_console m_console;
    jabbr_user m_user;
};

