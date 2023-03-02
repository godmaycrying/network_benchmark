#pragma once
#include <string>
#include <unordered_map>

#include "threadpool.hpp"
#include "tcpsession.h"

class TcpServer
{
public:
	TcpServer(boost::asio::io_context& io, unsigned short port);

private:
	void do_accept();

private:
	boost::asio::io_context& io_context_;
	boost::asio::ip::tcp::acceptor acceptor_;

	std::unordered_map<std::string, std::shared_ptr<TcpSession>> clients_;
};

