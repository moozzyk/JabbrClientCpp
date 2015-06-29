#include "stdafx.h"
#include "parser.h"
#include "util.h"


parser::parser()
{
}


parser::~parser()
{
}

command parser::parse(const std::wstring& input)
{
    if (input == L":q")
    {
        return command{ command_type::exit };
    }

    if (input[0] == L'/' && input.length() > 1)
    {
        auto command_text = get_command_string(input);
        if (command_text.length() == 2)
        {
            switch (input[1])
            {
            case L'j':
                return command{ command_type::join_room, get_argument(input) };
            case L'l':
                return command{ command_type::leave_room };
            case L'm':
                return create_2_arg_command(command_type::private_message, input);
            }
        }
    }

    return command{ command_type::message, input };
}

std::wstring parser::get_command_string(const std::wstring& input)
{
    auto space_pos = input.find_first_of(L' ');
    return input.substr(0, space_pos);
}

std::wstring parser::get_argument(const std::wstring& input)
{
    auto argument = input.substr(2);
    return util::trim(argument);
}

command parser::create_2_arg_command(command_type type, const std::wstring& input)
{
    auto args = get_argument(input);
    auto space_pos = args.find_first_of(L' ');
    auto arg2 = (space_pos >= args.length() - 1) ? L"" : args.substr(space_pos + 1);

    return command
    {
        type,
        args.substr(0, space_pos),
        util::trim(arg2)
    };
}