#include <string>
#include "command_type.h"

struct command
{
    command_type type;
    std::wstring argument;
    std::wstring argument2;
};