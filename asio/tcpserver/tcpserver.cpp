#include "tcpserver.h"

TcpServer::TcpServer(boost::asio::io_context& io, unsigned short port)
	: io_context_(io)
	, acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port), true)
{
	do_accept();
}

void TcpServer::do_accept()
{
	acceptor_.async_accept( [this](const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket) mutable {
		if (!ec)
		{
			auto session_ptr = std::make_shared<TcpSession>(ThreadPool::get_instance().get_io_context(), std::move(socket));
			session_ptr->start([this](const std::string& session_info) {
				clients_.erase(session_info);
			});

			clients_[session_ptr->get_session_info()] = session_ptr;
		}

		do_accept();
	});
}