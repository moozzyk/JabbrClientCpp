#pragma once

#include "_exports.h"
#include "jabbrclient\authentication_provider.h"
#include "signalrclient\hub_connection.h"

namespace jabbr
{
    class default_authentication_provider : public authentication_provider
    {
    public:
        default_authentication_provider(const utility::string_t& url);

        pplx::task<void> configure_connection(std::shared_ptr<signalr::hub_connection> jabbr_connection,
            const utility::string_t& username, const utility::string_t& password) override;

    private:
        utility::string_t m_url;
    };
}