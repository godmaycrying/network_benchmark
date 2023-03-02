#pragma once
#include <memory>
#include <array>
#include <boost/asio.hpp>

class TcpSession : public std::enable_shared_from_this<TcpSession>
{
public:
	TcpSession(boost::asio::io_context& io, boost::asio::ip::tcp::socket socket);

	std::string get_session_info() const { return session_info_; }

	template<typename CloseCallBack>
	void start(CloseCallBack&& cb) {
		boost::system::error_code ec;
		socket_.set_option(boost::asio::ip::tcp::no_delay(true), ec);
		socket_.set_option(boost::asio::socket_base::linger(true, 0), ec);

		is_running_ = true;

		session_info_ = socket_.remote_endpoint().address().to_string() + ":" + std::to_string(socket_.remote_endpoint().port());
		close_cb_ = std::move(cb);

		do_read();
	}

	void do_read();

	void close();

private:
	bool is_running_ = false;
	boost::asio::io_context& io_context_;
	boost::asio::ip::tcp::socket socket_;

	std::string session_info_;
	std::function<void(const std::string&)> close_cb_;

};

