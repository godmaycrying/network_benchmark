#pragma once
#include <boost/asio.hpp>

class TcpClient
{
public:
	explicit TcpClient(boost::asio::io_context& io, const boost::asio::ip::tcp::resolver::results_type& endpoint);

	void start();

private:
	void do_read();
	void do_write();
	void close_and_reconnect();

private:
	bool is_running_ = false;
	boost::asio::ip::tcp::resolver::results_type remote_endpoint_;
	boost::asio::io_context& io_context_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::steady_timer reconnect_timer_;
};

