#pragma once
#include <vector>
#include <unordered_map>

#include <QThread>
#include <QtNetWork/QTcpServer>

#include "TcpSession.h"

class TcpServer : public QTcpServer
{
	Q_OBJECT
public:
	TcpServer(QObject* parent = nullptr);

private:
	void incomingConnection(qintptr handle) override;

private:
	unsigned int max_thread_size_;
	unsigned int thread_pos_ = 0;
	std::vector<QThread*> threads_;
	std::unordered_map<QString, TcpSession*> sessions_;
};

