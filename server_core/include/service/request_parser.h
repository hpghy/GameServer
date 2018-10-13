/***************************************************************************
 * 
 * Copyright (c) 2018 hpghy, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file request_parser.h
 * @author hpghy(790042744@qq.com)
 * @date 2018/07/29 14:30:47
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

#include <tuple>
#include <sstream>
#include <cstdint>
#include <memory>
#include <string>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <array>


static const std::size_t size_fields_bytes = sizeof(uint32_t);
static const std::size_t REQUEST_LIMIT = 0xFFFFFF;

class request
{
public:
	request();
	void size_consume(std::size_t, const char*, std::size_t);
	void data_consume(const char*, std::size_t);
	std::size_t size();
	void reset();
	std::ostream& data_ostream() { return data_buff_; }
	std::istream& data_istream() { return data_buff_; }

private:
	std::array<char, size_fields_bytes> size_buff_;
	std::stringstream 					data_buff_;
};


class request_parser
{
public:
	enum class parser_state
	{
		state_error = 0,
		state_again,
		state_request,
	};
	using unparse_rst = std::tuple<bool, std::shared_ptr<std::string>>;
	using parse_rst = std::tuple<parser_state, std::size_t>;
	
	request_parser() {}
	~request_parser() {}

	unparse_rst unparse(const google::protobuf::MethodDescriptor*, const google::protobuf::Message*);
	parse_rst parse(const char *data, const std::size_t size);

	void reset()
	{
		state_ = inner_state::state_size;
		need_bytes_ = size_fields_bytes;
	}

	request& get_request()
	{
		return request_;
	}

	void reset_request()
	{
		request_.reset();
	}

private:
	enum class inner_state
	{
		state_size = 0,
		state_data = 1
	};

	inner_state 		state_ = inner_state::state_size;
	std::size_t 		need_bytes_ = size_fields_bytes;
	request 			request_;
};
using request_parser_ptr = std::shared_ptr<request_parser>;


#endif  // REQUEST_PARSER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
