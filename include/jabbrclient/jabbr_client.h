#pragma once

#include "_exports.h"
#include "pplx\pplxtasks.h"
#include "authentication_provider.h"

namespace pplx = concurrency;

namespace jabbr
{
    class jabbr_client_impl;

    class jabbr_client
    {
    public:
        JABBRCLIENTCPP_API jabbr_client(const utility::string_t& url);
        JABBRCLIENTCPP_API ~jabbr_client();

        JABBRCLIENTCPP_API pplx::task<void> connect(const utility::string_t& user_name, const utility::string_t& password,
            std::unique_ptr<authentication_provider> auth_provider = nullptr);

    private:
        std::shared_ptr<jabbr_client_impl> m_pImpl;
    };
}