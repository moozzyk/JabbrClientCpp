#include "stdafx.h"
#include "jabbrclient\jabbr_client.h"
#include "jabbr_client_impl.h"

namespace jabbr
{
    jabbr_client::jabbr_client(const utility::string_t& url)
        : m_pImpl(std::make_shared<jabbr_client_impl>(url))
    {}

    jabbr_client::~jabbr_client() = default;

    pplx::task<log_on_info> jabbr_client::connect(const utility::string_t& user_name, const utility::string_t& password, std::unique_ptr<authentication_provider> auth_provider)
    {
        return m_pImpl->connect(user_name, password, std::move(auth_provider));
    }

    pplx::task<user> jabbr_client::get_user_info()
    {
        return m_pImpl->get_user_info();
    }

    pplx::task<std::vector<room>> jabbr_client::get_rooms()
    {
        return m_pImpl->get_rooms();
    }

    pplx::task<room> jabbr_client::get_room_info(const utility::string_t& room_name)
    {
        return m_pImpl->get_room_info(room_name);
    }

    pplx::task<void> jabbr_client::join_room(const utility::string_t& room_name)
    {
        return m_pImpl->join_room(room_name);
    }

    pplx::task<void> jabbr_client::leave_room(const utility::string_t& room_name)
    {
        return m_pImpl->leave_room(room_name);
    }

    pplx::task<void> jabbr_client::send_message(const utility::string_t& message, const utility::string_t& room_name)
    {
        return m_pImpl->send_message(message, room_name);
    }

    pplx::task<void> jabbr_client::send_private_message(const utility::string_t& user_name, const utility::string_t& message)
    {
        return m_pImpl->send_private_message(user_name, message);
    }

    pplx::task<void> jabbr_client::log_out(const utility::string_t& user_name)
    {
        return m_pImpl->log_out(user_name);
    }

    pplx::task<void> jabbr_client::disconnect()
    {
        return m_pImpl->disconnect();
    }

    void jabbr_client::set_on_message_received(const std::function<void(const message&, const utility::string_t&)>& on_message_received)
    {
        m_pImpl->set_on_message_received(on_message_received);
    }
}