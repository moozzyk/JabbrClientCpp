#pragma once

#include <string>
#include "command.h"

class parser
{
public:
    parser();
    ~parser();

    command parse(const std::wstring& input);

private:
    std::wstring get_command_string(const std::wstring& input);
    std::wstring get_argument(const std::wstring& input);
    command create_2_arg_command(command_type type, const std::wstring& input);
};

