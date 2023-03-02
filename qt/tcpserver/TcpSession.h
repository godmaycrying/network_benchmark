#pragma once
#include <QtNetwork/QTcpSocket>

class TcpSession : public QTcpSocket
{
	Q_OBJECT
public:
	TcpSession(qintptr handle, QObject* parent = nullptr);
};

