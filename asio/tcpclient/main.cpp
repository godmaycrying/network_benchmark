#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "threadpool.hpp"
#include "tcpclient.h"

int main(int argc, char* argv[])
{
	std::vector<std::string> remote_addr;
	boost::split(remote_addr, argv[1], boost::is_any_of(":"));
	int client_size = boost::lexical_cast<int>(argv[2]);

	ThreadPool::get_instance().start();
	auto& io_context = ThreadPool::get_instance().get_io_context();
	auto endpoint = boost::asio::ip::tcp::resolver(io_context).resolve(remote_addr[0], remote_addr[1]);
	{
		boost::system::error_code ec;
		boost::asio::ip::tcp::socket socket(io_context);
		boost::asio::connect(socket, endpoint, ec);
		if (ec)
		{
			auto err_msg = ec.what();
			return -1;
		}

		socket.shutdown(boost::asio::socket_base::shutdown_both);
		socket.cancel(ec);
		socket.close(ec);
	}

	std::vector<std::shared_ptr<TcpClient>> clients;
	clients.resize(client_size);
	for (auto i=0; i<client_size; ++i)
	{
		auto client_ptr = std::make_shared<TcpClient>(ThreadPool::get_instance().get_io_context(), endpoint);
		client_ptr->start();
		clients.emplace_back(client_ptr);
	}

	ThreadPool::get_instance().wait2stop();

	return 0;
}