#ifndef HTTP_REQUEST_PARSER_HPP_
#define HTTP_REQUEST_PARSER_HPP_

#include <string>
#include <boost/logic/tribool.hpp>

class HttpRequest;

class HttpRequestParser
{
public:
    enum class State
    {
        STATE_LINE,         // parse http request line
        STATE_HEADERS,      // parse request headers
        STATE_BODY,         // parse request body
        STATE_OVER
    };

public:
    HttpRequestParser(): state_(State::STATE_LINE), offset_(0)
    {}

    void pushData(const char* data, std::size_t length);
    boost::tribool parse(HttpRequest& req);

private:
    boost::tribool parseRequestLine(HttpRequest& req);
    boost::tribool parseRequestHeader(HttpRequest& req);
    boost::tribool parseRequestBody(HttpRequest& req);
    void reset();

private:
    State state_;
    std::string data_;
    std::size_t offset_;
};

#endif
