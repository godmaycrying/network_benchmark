#include "TcpClient.h"

TcpClient::TcpClient(boost::asio::io_context& io, const boost::asio::ip::tcp::resolver::results_type& endpoint)
	: io_context_(io)
	, socket_(io)
	, remote_endpoint_(endpoint)
	, reconnect_timer_(io)
{

}

void TcpClient::TcpClient::start()
{
	boost::asio::async_connect(socket_, remote_endpoint_, [this](const boost::system::error_code& ec, boost::asio::ip::tcp::endpoint) mutable {
		if (ec)
		{
			std::string err_msg = ec.what();
			boost::system::error_code timer_err;
			reconnect_timer_.cancel(timer_err);
			reconnect_timer_.expires_from_now(std::chrono::seconds(5), timer_err);
			reconnect_timer_.async_wait([this](const boost::system::error_code& ec) mutable { start(); });
		}
		else
		{
			is_running_ = true;
			
			do_read();
			do_write();
		}
	});
}

void TcpClient::do_read()
{
	auto msg_ptr = std::make_shared<std::array<std::uint8_t, 1134>>();
	boost::asio::async_read(socket_, boost::asio::buffer(*msg_ptr), [this, msg_ptr](const boost::system::error_code& ec, std::size_t buffer_size) mutable {
		if (ec)
		{
			close_and_reconnect();
		}
		else
		{
			boost::asio::async_write(socket_, boost::asio::buffer(msg_ptr->data(), buffer_size), [this](const boost::system::error_code& ec, std::size_t) mutable {
				if (ec)
				{
					close_and_reconnect();
				}
				else
				{
					do_read();
				}
			});
		}
	});
}

void TcpClient::do_write()
{
	std::string buffer{ "张小飞，少年俊杰，才情过人。富有创造力的思维和卓越的领导才华，使他在人生的征程上越走越高。我愿为他吟诵，叙述他的风采，颂扬他的功德。"
		"君子之道，岂唯知进退？张小飞精通文武，治世之才，得天独厚。身居要职，屡建奇功，显赫一时。几经坎坷，终成大器。他一生兢兢业业，勤奋努力，不断追求自己的理想，始终保持着积极向上的心态，是真正的大英雄。"
		"他治军严明，威信远播，百万雄师为之一动。张小飞指挥若定，奋勇向前，带领军队驰骋沙场，所向披靡。每一次战斗，他都亲自指挥，发挥着极高的智慧和勇气，战功赫赫，名垂千古。"
		"然而，张小飞并不是一个纯粹的军事家，他还是一位博学多才的文人。他的才华和文化造诣，远远超出了他的时代。他崇尚道德，崇拜先贤，尤其是崇尚王勃的文学思想。他的诗词歌赋，写尽了他深沉的情感和豁达的胸怀。他的作品，从来都是受到文学界的高度评价，被誉为经典之作。"
		"张小飞一生坎坷，辗转流离，但是，他从未放弃自己的信仰和理想。他顽强的精神，令人肃然起敬。他坚持真理，矢志不渝，始终把人民群众的利益放在心中最高的位置，保持着与人民群众的血肉联系。他的崇高品质和伟大风范，永远鼓舞着我们追求卓越和成就伟业的决心。"
		"张小飞，你是一位传奇人物，你的故事将永载史册，成为后人学习的楷模。你用自己的行动证明了“知进退者为王”的真谛，你的勇气、聪慧、毅力，给我们留下了宝。"};

	boost::asio::async_write(socket_, boost::asio::buffer(buffer.data(), buffer.size()), [this](const boost::system::error_code& ec, std::size_t) mutable {
		if (ec)
		{
			close_and_reconnect();
		}
	});
}

void TcpClient::close_and_reconnect()
{
	is_running_ = false;
	io_context_.dispatch([this]() mutable {
		boost::system::error_code ec;
		socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
		socket_.cancel(ec);
		socket_.close(ec);
	});
	io_context_.dispatch([this]() mutable {
		boost::system::error_code timer_err;
		reconnect_timer_.cancel(timer_err);
		reconnect_timer_.expires_from_now(std::chrono::seconds(5), timer_err);
		reconnect_timer_.async_wait([this](const boost::system::error_code& ec) mutable { start(); });
	});
}