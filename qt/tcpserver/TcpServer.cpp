#include "TcpServer.h"

TcpServer::TcpServer(QObject* parent)
	: QTcpServer(parent)
{
	max_thread_size_ = std::thread::hardware_concurrency();
	for (auto i=0; i< max_thread_size_; ++i) {
		auto thread_ptr = new QThread;
		thread_ptr->start();
		threads_.push_back(thread_ptr);
	}
}

void TcpServer::incomingConnection(qintptr handle)
{
	auto session_ptr = new TcpSession(handle);
	session_ptr->moveToThread(threads_[thread_pos_]);

	thread_pos_++;
	if (thread_pos_ == max_thread_size_)
		thread_pos_ = 0;

	auto session_info = QString("%1:%2").arg(session_ptr->peerAddress().toString()).arg(session_ptr->peerPort());
	sessions_[session_info] = session_ptr;

	connect(session_ptr, &QTcpSocket::disconnected, [this, session_ptr, session_info = std::move(session_info)]() {
		sessions_.erase(session_info);
		session_ptr->deleteLater();
	});
}