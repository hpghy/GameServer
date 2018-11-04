/***************************************************************************
 *
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @file RequestParser.cpp
* @author hpghy(790042744@qq.com)
* @date 2018/07/29 14:29:34
* @version $Revision$
* @brief
*
**/

#include <cstring>
#include <iostream>
#include "service/request_parser.h"

Request::Request()
{
    reset();
}

void Request::size_consume(std::size_t start, const char* data, std::size_t size)
{
    //不用担心字节序列，只要保证和发送一致
    std::memcpy(&size_buff_[start], data, size);
}

void Request::data_consume(const char* data, std::size_t size)
{
    data_buff_.write(data, size);
}

std::size_t Request::size()
{
    uint32_t s = *reinterpret_cast<uint32_t*>(size_buff_.data());
    return s;
}

void Request::reset()
{
    std::memset(size_buff_.data(), 0, size_buff_.size());
    data_buff_.str("");		//清空数据
    data_buff_.clear();
}

RequestParser::unparse_rst RequestParser::unparse(const google::protobuf::MethodDescriptor* method,
        const google::protobuf::Message* request)
{
    std::ostringstream odata;
    uint32_t len = 0;
    odata.write(reinterpret_cast<const char*>(&len), sizeof(len));
    auto index = method->index();
    odata.write(reinterpret_cast<const char*>(&index), sizeof(uint16_t));
    if (!request->SerializeToOstream(&odata))
    {
        std::cerr << "SerializeToOstream failed....." << std::endl;
        static std::shared_ptr<std::string> failed;
        return std::make_tuple(false, failed);
    }

    len = odata.tellp();
    auto data_len = len - sizeof(len);
    odata.seekp(0, std::ios_base::beg);
    odata.write(reinterpret_cast<const char*>(&data_len), sizeof(len));
    odata.seekp(len);

    //注意要创建堆上数据而不是栈上数据,应该应该有右值拷贝函数
    return std::make_tuple(true, std::make_shared<std::string>(odata.str()));
}

RequestParser::parse_rst RequestParser::parse(const char* data, const std::size_t size)
{
    std::size_t consume = 0;
    switch (state_)
    {
        case inner_state::state_size:
        {
            if (need_bytes_ <= size)
            {
                consume = need_bytes_;
                request_.size_consume(size_fields_bytes - need_bytes_, data, consume);
                state_ = inner_state::state_data;
                need_bytes_ = request_.size();
                return std::make_tuple(parser_state::state_again, consume);
            }
            else
            {
                consume = size;
                request_.size_consume(size_fields_bytes - need_bytes_, data, consume);
                need_bytes_ -= size;
                return std::make_tuple(parser_state::state_again, consume);
            }
            break;
        }

        case inner_state::state_data:
        {
            if (need_bytes_ <= size)
            {
                consume = need_bytes_;
                request_.data_consume(data, consume);
                reset();
                return std::make_tuple(parser_state::state_request, consume);
            }
            else
            {
                consume = size;
                request_.data_consume(data, consume);
                need_bytes_ -= consume;
                return std::make_tuple(parser_state::state_again, consume);
            }
            break;
        }

        default:
        {
            std::cerr << "error ......" << std::endl;
            return std::make_tuple(parser_state::state_error, consume);
        }
    }
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
