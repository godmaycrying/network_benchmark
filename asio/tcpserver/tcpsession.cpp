#include "tcpsession.h"
#include "qps.hpp"

TcpSession::TcpSession(boost::asio::io_context& io, boost::asio::ip::tcp::socket socket)
	: io_context_(io)
	, socket_(std::move(socket))
{

}

void TcpSession::do_read()
{
	auto buffer_ptr = std::make_shared<std::array<std::uint8_t, 1134>>();
	boost::asio::async_read(socket_, boost::asio::buffer(*buffer_ptr), [this, self = shared_from_this(), buffer_ptr](const boost::system::error_code& ec, std::size_t buffer_size) mutable {
		if (!is_running_ || !socket_.is_open())
			return;

		if (!ec)
		{
			qps_count::get_instance().add();
			boost::asio::async_write(socket_, boost::asio::buffer(buffer_ptr->data(), buffer_size), [this, self = shared_from_this()](const boost::system::error_code& ec, std::size_t buffer_size) mutable {
				if (!is_running_ || !socket_.is_open())
					return;

				if (!ec)
				{
					do_read();
				}
				else
				{
					close();
				}
			});
		}
		else
		{
			close();
		}
	});
}

void TcpSession::close()
{
	if (!is_running_)
		return;

	is_running_ = false;
	io_context_.dispatch([this, self = shared_from_this()] {
		boost::system::error_code ec;
		socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
		socket_.cancel(ec);
		socket_.close(ec);

		close_cb_(session_info_);
	});
}