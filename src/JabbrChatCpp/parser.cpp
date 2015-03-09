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

    if (input[0] == L'/')
    {
        if (input[1] == L'j')
        {
            return command{ command_type::join_room, get_argument(input) };
        }
    }

    return command{ command_type::message, input };
}

std::wstring parser::get_argument(const std::wstring& input)
{
    auto argument = input.substr(2);
    return util::trim(argument);
}