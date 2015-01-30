#include "stdafx.h"
#include "jabbrclient\jabbr_client.h"
#include "jabbr_client_impl.h"

namespace jabbr
{
    jabbr_client::jabbr_client(const utility::string_t& url)
        : m_pImpl(std::make_shared<jabbr_client_impl>(url))
    {}

    jabbr_client::~jabbr_client() = default;

    pplx::task<void> jabbr_client::connect(const utility::string_t& user_name, const utility::string_t& password, std::unique_ptr<authentication_provider> auth_provider)
    {
        return m_pImpl->connect(user_name, password, std::move(auth_provider));
    }
}