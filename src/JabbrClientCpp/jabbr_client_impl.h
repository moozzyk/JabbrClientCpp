#pragma once

#include <memory>
#include "pplx\pplxtasks.h"
#include "jabbrclient\authentication_provider.h"

namespace pplx = concurrency;

namespace jabbr
{
    class jabbr_client_impl
    {
    public:
        jabbr_client_impl(const utility::string_t& url);

        pplx::task<void> connect(const utility::string_t& user_name, const utility::string_t& password,
            std::unique_ptr<authentication_provider> auth_provider = nullptr);

    private:
        utility::string_t m_url;
        std::shared_ptr<signalr::hub_connection> m_jabbr_connection;
    };
}