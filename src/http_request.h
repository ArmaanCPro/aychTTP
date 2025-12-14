#pragma once

#include <string>

namespace aych
{
    struct HttpRequest
    {
        std::string method;
        std::string path;
        std::string version;
    };
}
