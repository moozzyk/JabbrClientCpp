#pragma once
#include "signalrclient\hub_connection.h"

namespace jabbr
{
    class authentication_provider
    {
    public:
        virtual pplx::task<void> configure_connection(std::shared_ptr<signalr::hub_connection> jabbr_connection,
            const utility::string_t& username, const utility::string_t& password) = 0;
        virtual ~authentication_provider() {};
    };
}