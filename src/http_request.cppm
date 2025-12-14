module;

#include <string>

export module aych:http_request;

namespace aych
{
    export struct HttpRequest
    {
        std::string method;
        std::string path;
        std::string version;
    };
}