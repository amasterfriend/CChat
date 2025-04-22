#include "AsioIOContextPool.h"
#include <iostream>

AsioIOContextPool::AsioIOContextPool(std::size_t size) :_ioContexts(size), _works(size), _threads(size), _nextIOContext(0)
{
	for (std::size_t i = 0; i < size; i++)
	{
		//get_executor() 返回 io_context 的执行器，用于管理 io_context 的生命周期，防止 run() 提前退出。
		_works[i] = std::make_unique<Work>(_ioContexts[i].get_executor());
		//启动多个线程，每个线程运行一个 io_context
		_threads.emplace_back([this, i]() {
			_ioContexts[i].run();
			});
	}
}

AsioIOContextPool::~AsioIOContextPool() {
	Stop();
	std::cout << "AsioIOContextPool destruct" << std::endl;
}

boost::asio::io_context& AsioIOContextPool::GetIOContext()
{
	auto& context = _ioContexts[_nextIOContext++];
	if (_nextIOContext == _ioContexts.size())
	{
		_nextIOContext = 0;
	}
	return context;
}

void AsioIOContextPool::Stop()
{
	//因为仅仅执行work.reset并不能让iocontext从run的状态中退出
	//当iocontext已经绑定了读或写的监听事件后，还需要手动stop该服务。

	for (auto& context : _ioContexts) {
		//把服务先停止
		context.stop();
	}

	for (auto& work : _works) {
		work.reset();
	}

	for (auto& t : _threads) {
		t.join();
	}
}

