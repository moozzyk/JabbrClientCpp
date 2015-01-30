#include "stdafx.h"
#include "jabbr_client_impl.h"
#include "cpprest\uri_builder.h"
#include "cpprest\http_client.h"
#include "jabbrclient\default_authentication_provider.h"

namespace jabbr
{
    jabbr_client_impl::jabbr_client_impl(const utility::string_t& url)
        : m_url(url), m_jabbr_connection(std::make_shared<signalr::hub_connection>(url))
    { }

    pplx::task<void> jabbr_client_impl::connect(const utility::string_t& user_name, const utility::string_t& password,
        std::unique_ptr<authentication_provider> auth_provider)
    {
        if (!auth_provider)
        {
            auth_provider = std::move(std::make_unique<default_authentication_provider>(m_url));
        }

        auto& jabbr_connection = m_jabbr_connection;
        return auth_provider->configure_connection(m_jabbr_connection, user_name, password)
            .then([jabbr_connection]()
            mutable {
                return jabbr_connection->start();
            });
    }
}