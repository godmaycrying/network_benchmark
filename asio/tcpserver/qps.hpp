#include <thread>
#include <atomic>
#include <iostream>

class qps_count
{
	qps_count() {
		is_running_ = true;
		count_thd_ = std::thread([this]() mutable {
			while (is_running_) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				std::cout << "QTcpServer - QPS: " << count_.load(std::memory_order_acquire) << std::endl;
				count_.store(0, std::memory_order_release);
			}
		});
	}

	~qps_count() {
		is_running_ = false;
		count_thd_.join();
	}

public:
	static qps_count& get_instance() {
		static qps_count obj;
		return obj;
	}

	void add() {
		count_.fetch_add(1, std::memory_order_release);
	}

private:
	bool is_running_ = false;
	std::atomic_uint64_t count_ = 0;
	std::thread count_thd_;
};