#include <QCoreApplication>
#include <iostream>

#include "TcpServer.h"

int main(int argc, char* argv[])
{
	std::cout << "QTcpServer begin" << std::endl;
	QCoreApplication a(argc, argv);

	TcpServer srv;
	auto ret = srv.listen(QHostAddress::Any, 49999);

	if (!ret)
		return -1;

	return a.exec();
}