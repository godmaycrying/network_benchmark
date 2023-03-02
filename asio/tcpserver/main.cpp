#include <iostream>

#include "tcpserver.h"

int main(int argc, char* argv[])
{
	try
	{
		ThreadPool::get_instance().start();
		boost::asio::io_context io;
		boost::asio::io_context::work work(io);
		TcpServer srv(io, 49999);

		io.run();
	}
	catch (const std::exception& ec)
	{
		std::string err_msg = ec.what();
		std::cout << err_msg << std::endl;
	}
	catch (const boost::system::error_code& ec)
	{
		std::string err_msg = ec.what();
		std::cout << err_msg << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
	
	return 0;
}