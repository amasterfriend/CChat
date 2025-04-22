#pragma once
#include "Singleton.h"
#include <boost/asio.hpp>

class AsioIOContextPool :public Singleton<AsioIOContextPool>
{
	friend class Singleton<AsioIOContextPool>;
public:
	using IOContext = boost::asio::io_context;
	using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
	using WorkPtr = std::unique_ptr<Work>;
	~AsioIOContextPool();
	AsioIOContextPool(const AsioIOContextPool&) = delete;
	AsioIOContextPool& operator=(const AsioIOContextPool&) = delete;
	// 使用 round-robin 的方式返回一个 io_context
	boost::asio::io_context& GetIOContext();
	void Stop();
private:
	AsioIOContextPool(std::size_t size = 2/*std::thread::hardware_concurrency()*/);
	std::vector<IOContext> _ioContexts;
	std::vector<WorkPtr> _works;
	std::vector<std::thread> _threads;
	std::size_t                        _nextIOContext;
};
