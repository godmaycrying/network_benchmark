#include "TcpSession.h"
#include "qps.hpp"

TcpSession::TcpSession(qintptr handle, QObject* parent)
	:QTcpSocket(parent)
{
	this->setSocketDescriptor(handle);

	connect(this, &QTcpSocket::readyRead, [this]() {
		auto data = this->readAll();
		this->write(data);
		qps_count::get_instance().add();
	});
}