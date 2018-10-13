#include <sstream>
#include "http/http_response.h"

void HttpResponse::reset()
{
    status_code = 202;
    status = "OK";
    setVersion(1, 1);

    header_map.clear();
    setHeader("Server", "HPTEST-GameServer");
    setHeader("Date", "Tue, 31 May 2016 02:09:24 GMT");
    setHeader("Connection", "keep-alive");

    content.clear();
}

void HttpResponse::setContent(const std::string& content)
{
    this->content.assign(content);
    setHeader("Content-Length", std::to_string(content.length()));
    setHeader("Content-Type", "text/html;charset=UTF-8");
}

void HttpResponse::setJsonUtf8Content(const std::string&)
{
    // TODO...
}

void HttpResponse::setHeader(const std::string& header, const std::string& value)
{
    header_map[header] = value;
}

std::shared_ptr<std::string> HttpResponse::unparse()
{
    std::ostringstream oss("HTTP/");
    oss << major_version << "." << sub_version << " " << status_code << " " << status << "\r\n";
    for (auto iter = header_map.begin(); iter != header_map.end(); ++iter)
    {
        oss << iter->first << ": " << iter->second << "\r\n";
    }
    oss << "\r\n";
    oss << content;

    return std::make_shared<std::string>(oss.str());
}
