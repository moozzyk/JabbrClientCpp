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
    std::wstring get_argument(const std::wstring& input);
};

