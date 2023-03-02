#pragma once
#include <memory>
#include <vector>
#include <thread>

#include <boost/asio.hpp>

class ThreadPool
{
public:
	static ThreadPool& get_instance() {
		static ThreadPool obj;
		return obj;
	}

	void start() {
		for (unsigned int i = 0; i < thread_size_; ++i)
			threads_.emplace_back([](std::shared_ptr<boost::asio::io_context> contex_ptr) {
				contex_ptr->run();
			}, io_contexts_[i]);

		is_running_ = true;
	}

	void stop() {
		if (is_running_) {
			for (auto&& contex_ptr : io_contexts_)
				contex_ptr->stop();

			for (auto&& thd : threads_)
				thd.join();

			is_running_ = false;
		}
	}

	void wait2stop() {
		if (is_running_) {
			for (auto&& thd : threads_)
				thd.join();

			is_running_ = false;
		}
	}

	boost::asio::io_context& get_io_context() {
		thread_pos_++;
		if (thread_pos_ == thread_size_)
			thread_pos_ = 0;

		return *io_contexts_[thread_pos_];
	}

private:
	ThreadPool() {
		thread_size_ = std::thread::hardware_concurrency();
		for (unsigned int i=0; i<thread_size_; ++i) {
			auto io_context_ptr = std::make_shared<boost::asio::io_context>();
			auto work_ptr = std::make_shared<boost::asio::io_context::work>(*io_context_ptr);
			io_contexts_.emplace_back(io_context_ptr);
			works_.emplace_back(work_ptr);
		}
	}

	~ThreadPool() {
		stop();
	}

	bool is_running_ = false;
	unsigned int thread_size_;
	unsigned int thread_pos_ = 0;

	std::vector<std::thread> threads_;
	std::vector<std::shared_ptr<boost::asio::io_context>> io_contexts_;
	std::vector<std::shared_ptr<boost::asio::io_context::work>> works_;
};