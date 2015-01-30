#include "stdafx.h"
#include "jabbrclient\default_authentication_provider.h"
#include "cpprest\uri_builder.h"
#include "cpprest\http_client.h"
#include <unordered_map>

#ifdef _WIN32
#include <winhttp.h>
#endif

using namespace signalr;
using namespace web::http;
using namespace web::http::client;

namespace jabbr
{
    default_authentication_provider::default_authentication_provider(const utility::string_t& url)
        : m_url(url)
    {
        // remove trailing 'SignalR(/)?'
        if (m_url.size() > 8)
        {
            auto start_position = url.size() - 1;
            if (m_url.back() == U('/'))
            {
                start_position--;
            }

            // TODO: should be case insensitive
            if ((start_position = m_url.find(U("SignalR"), start_position - 7)) != utility::string_t::npos)
            {
                m_url.erase(start_position);
            }
        }
    }

    pplx::task<void> default_authentication_provider::configure_connection(std::shared_ptr<hub_connection> jabbr_connection,
        const utility::string_t& username, const utility::string_t& password)
    {
        http_client_config config;
#ifdef _WIN32
        config.set_nativehandle_options([](native_handle handle)->void
        {
            auto setting = WINHTTP_DISABLE_REDIRECTS;
            if (!WinHttpSetOption(handle,
                WINHTTP_OPTION_DISABLE_FEATURE,
                &setting,
                sizeof(setting)))
            {
                throw std::runtime_error("could not disable redirects");
            }
        });
#endif
        http_client client(U("https://jabbr-staging.apphb.com/"), config);
        http_request request{ U("POST") };
        request.set_request_uri(U("account/login"));
        request.headers().add(U("Content-Type"), U("application/x-www-form-urlencoded"));
        request.headers().add(U("sec-jabbr-client"), U("1"));

        utility::ostringstream_t body;
        body << U("username=") << username << U("&password=") << password;
        request.set_body(body.str());

        return client.request(request)
            .then([jabbr_connection](http_response response)
            {
                if (response.status_code() != 303)
                {
                    std::ostringstream msg;
                    msg << "authentication failed. http status code: " << response.status_code();
                    throw std::runtime_error(msg.str());
                }

                auto cookie_header = response.headers().find(U("Set-Cookie"));
                if (cookie_header != response.headers().end())
                {
                    jabbr_connection->set_headers(std::unordered_map<utility::string_t, utility::string_t>(
                    {
                        { U("Cookie"), cookie_header->second },
                        { U("sec-jabbr-client"), U("1") },
                    }));
                }
                else
                {
                    throw std::runtime_error("authentication failed - cookie not found");
                }
            });
    }
}