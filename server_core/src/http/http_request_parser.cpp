/***************************************************************************
 * 
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file http_request_parser.cpp
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/29 21:23:11
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "util/util.h"
#include <tuple>
#include <boost/algorithm/string.hpp>
#include "http/http_request_parser.h"
#include "http/http_request.h"

const static char* HTTP_REQUEST_LINE_END_FLAGS = "\r\n";

static std::tuple<std::size_t, std::size_t> getToken(const char* data, std::size_t begin, std::size_t end, char sep_char = ' ')
{
    std::size_t token_begin = begin;
    for (; token_begin < end && sep_char == data[token_begin]; ++token_begin);
    if (token_begin >= end)
    {
        return std::make_tuple(end, end);
    }

    std::size_t token_end = token_begin + 1;
    for (; token_end < end && sep_char != data[token_end]; ++token_end);
    return std::make_tuple(token_begin, token_end);
}

void HttpRequestParser::pushData(const char* data, std::size_t length)
{
    data_ += std::string(data, length);
}

boost::tribool HttpRequestParser::parse(HttpRequest& req)
{
    boost::tribool ret;

    while (true)
    {
        switch (state_)
        {
        case State::STATE_LINE:
            ret = parseRequestLine(req);
            break;
        case State::STATE_HEADERS:
            ret = parseRequestHeader(req);
            break;
        case State::STATE_BODY:
            ret = parseRequestBody(req);
            break;
        default:
            ERROR_LOG << "no such state: " << static_cast<uint32_t>(state_);
            ret = false;
        }

        if (ret)
        {
            if (State::STATE_OVER != state_)
            {
                continue;
            }
            reset();
            return true;
        }
        else if (!ret)
        {
            return false;
        }

        return boost::indeterminate;
    }
    return boost::indeterminate;
}

boost::tribool HttpRequestParser::parseRequestLine(HttpRequest& req)
{
    auto end = data_.find(HTTP_REQUEST_LINE_END_FLAGS, offset_);
    if (std::string::npos == end)
    {
        return boost::indeterminate;
    }

    // parse method
    std::size_t token_begin, token_end;
    std::tie(token_begin, token_end) = getToken(data_.data(), offset_, end);
    if (token_begin == token_end)
    {
        return false;
    }
    req.method.assign(data_, token_begin, token_end - token_begin);

    // parse uri
    std::tie(token_begin, token_end) = getToken(data_.data(), token_end+1, end);
    if (token_begin == token_end)
    {
        return false;
    }
    req.uri.assign(data_, token_begin, token_end - token_begin);

    // parse http version
    std::tie(token_begin, token_end) = getToken(data_.data(), token_end+1, end);
    if (token_begin == token_end)
    {
        return false;
    }
    req.version.assign(data_, token_begin, token_end - token_begin);

    offset_ = end + strlen(HTTP_REQUEST_LINE_END_FLAGS);
    state_ = State::STATE_HEADERS;

    return req.parseLine();
}

boost::tribool HttpRequestParser::parseRequestHeader(HttpRequest& req)
{
    auto end = data_.find(HTTP_REQUEST_LINE_END_FLAGS, offset_);
    if (std::string::npos == end)
    {
        return boost::indeterminate;
    }
    if (end == offset_)             // empty line
    {
        offset_ += strlen(HTTP_REQUEST_LINE_END_FLAGS);
        state_ = State::STATE_BODY;
        return true;
    }

    // header-key: header-value
    std::size_t token_begin, token_end;
    std::tie(token_begin, token_end) = getToken(data_.data(), offset_, end, ':');
    if (token_begin == token_end)
    {
        return false;
    }
    std::string name(data_.data()+token_begin, token_end-token_begin);
    boost::trim(name);

    std::tie(token_begin, token_end) = getToken(data_.data(), token_end+1, end, ':');
    if (token_begin == token_end)
    {
        return false;
    }
    std::string value(data_.data()+token_begin, token_end-token_begin);
    boost::trim(value);
    auto ret = req.addHeader(std::move(name), std::move(value));
    if (!ret)
    {
        ERROR_LOG << "add header: " << name << " " << value << " failed!";
    }

    offset_ = end + strlen(HTTP_REQUEST_LINE_END_FLAGS);
    return ret;
}

boost::tribool HttpRequestParser::parseRequestBody(HttpRequest& req)
{
    bool ret = req.parseHeader();
    if (!ret)
    {
        return false;
    }

    if (data_.length() - offset_ >= req.getContentLength())
    {
        offset_ += req.getContentLength();
        state_ = State::STATE_OVER;
        return req.parseContent();
    }
    else
    {
        return boost::indeterminate;
    }
}

void HttpRequestParser::reset()
{
    // clear http request memory and remain unparsed data
    data_ = data_.substr(offset_);
    offset_ = 0;
    state_ = State::STATE_LINE;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
