#include "stdafx.h"
#include "cpprest\json.h"
#include "jabbrclient\user.h"
#include "jabbrclient\message.h"
#include "jabbrclient\room.h"

namespace jabbr
{
    namespace json_materializer
    {
        namespace
        {
            utility::string_t get_string(const utility::string_t& name, const web::json::value& value, utility::string_t default_value = U(""));
            int get_int(const utility::string_t& name, const web::json::value& value, int default_value = 0);
            bool get_bool(const utility::string_t& name, const web::json::value& value, bool default_value = false);
            utility::datetime get_datetime(const utility::string_t& name, const web::json::value& value);
        }

        namespace
        {
            user_status translate_status(const utility::string_t status);
        }

        user create_user(const web::json::value& value)
        {
            _ASSERTE(value.is_object());

            return user
            {
                /*name*/            get_string(U("Name"), value),
                /*hash*/            get_string(U("Hash"), value),
                /*is_active*/       get_bool(U("Active"), value),
                /*status*/          translate_status(get_string(U("Status"), value)),
                /*note*/            get_string(U("Note"), value),
                /*afk_note*/        get_string(U("AfkNote"), value),
                /*is_afk*/          get_bool(U("IsAfk"), value),
                /*flag*/            get_string(U("Flag"), value),
                /*country*/         get_string(U("Country"), value),
                /*last_activity*/   get_datetime(U("LastActivity"), value),
                /*is_admin*/        get_bool(U("IsAdmin"), value),

            };
        }

        message create_message(const web::json::value& value)
        {
            _ASSERTE(value.is_object());

            auto m = message
            {
                /*is_html_encoded*/ get_bool(U("HtmlEncoded"), value),
                /*id*/              get_string(U("Id"), value),
                /*content*/         get_string(U("Content"), value),
                /*when*/            get_datetime(U("When"), value)
            };

            if (value.has_field(U("User")) && value.at(U("User")).is_object())
            {
                m.user = create_user(value);
            }

            return m;
        }

        room create_room(const web::json::value& value)
        {
            _ASSERTE(value.is_object());

            auto r = room
            {
                /*name*/        get_string(U("Name"), value),
                /*count*/       get_int(U("Count"), value),
                /*is_private*/  get_bool(U("Private"), value),
                /*topic*/       get_string(U("Topic"), value),
                /*is_closed*/   get_bool(U("Closed"), value),
                /*welcome*/     get_string(U("Welcome"), value),
            };

            if (value.has_field(U("Users")) && value.at(U("Users")).is_array())
            {
                for (auto& user : value.at(U("Users")).as_array())
                {
                    if (user.is_object())
                    {
                        r.users.push_back(create_user(user));
                    }
                }
            }

            if (value.has_field(U("Owners")) && value.at(U("Owners")).is_array())
            {
                for (auto& owner : value.at(U("Owners")).as_array())
                {
                    r.owners.push_back(owner.as_string());
                }
            }

            if (value.has_field(U("RecentMessages")) && value.at(U("RecentMessages")).is_array())
            {
                for (auto& recent_message : value.at(U("RecentMessages")).as_array())
                {
                    r.owners.push_back(recent_message.as_string());
                }
            }

            return r;
        }

        namespace
        {
            user_status translate_status(const utility::string_t status)
            {
                if (status == U("Active"))
                {
                    return user_status::active;
                }

                if (status == U("Inative"))
                {
                    return user_status::inactive;
                }

                return user_status::offline;
            }

        }

        namespace
        {
            utility::string_t get_string(const utility::string_t& name, const web::json::value& value, utility::string_t default_value)
            {
                if (value.has_field(name) && value.at(name).is_string())
                {
                    return value.at(name).as_string();
                }

                return default_value;
            }

            int get_int(const utility::string_t& name, const web::json::value& value, int default_value)
            {
                if (value.has_field(name) && value.at(name).is_number())
                {
                    return value.at(name).as_integer();
                }

                return default_value;
            }

            bool get_bool(const utility::string_t& name, const web::json::value& value, bool default_value)
            {
                if (value.has_field(name) && value.at(name).is_boolean())
                {
                    return value.at(name).as_bool();
                }

                return default_value;
            }

            utility::datetime get_datetime(const utility::string_t& name, const web::json::value& value)
            {
                auto datetime_string = get_string(name, value, U(""));
                if (datetime_string != U(""))
                {
                    return utility::datetime::from_string(datetime_string);
                }

                return utility::datetime();
            }
        }
    }
}