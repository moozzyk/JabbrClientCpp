// JabbrChatCpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "jabbrclient\jabbr_client.h"
#include "jabbrclient\room.h"

#include "jabbr_console.h"

using namespace jabbr;

void show_rooms(const std::vector<room>& rooms)
{
    ucout << U("Rooms:") << std::endl;
    for (const auto& r : rooms)
    {
        ucout << U("Name: ") << r.name << std::endl;
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc != 4)
    {
        std:: cout << "Usage: JabbrChatCpp.exe url username password" << std::endl;
        return 1;
    }

    auto url = argv[1];
    auto user = argv[2];
    auto pwd = argv[3];

    jabbr_client client{ url };
    jabbr_console console{ client };
    console.run(user, pwd);

    /*
    jabbr_client client{ url };

    try
    {
        auto log_on_info = client.connect(user, pwd).get();
        std::cout << "connecting succeeded, logon info:" << std::endl;
        show_rooms(log_on_info.rooms);

        auto rooms = client.get_rooms().get();
        show_rooms(rooms);
    }
    catch (const std::exception& e)
    {
        std::cout << "error connecting to the server: " << e.what() << std::endl;
    }

    return 0;
    */
}
