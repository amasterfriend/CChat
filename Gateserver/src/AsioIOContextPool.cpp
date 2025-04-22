#include "AsioIOContextPool.h"
#include <iostream>

AsioIOContextPool::AsioIOContextPool(std::size_t size) :_ioContexts(size), _works(size), _threads(size), _nextIOContext(0)
{
	for (std::size_t i = 0; i < size; i++)
	{
		//get_executor() ���� io_context ��ִ���������ڹ��� io_context ���������ڣ���ֹ run() ��ǰ�˳���
		_works[i] = std::make_unique<Work>(_ioContexts[i].get_executor());
		//��������̣߳�ÿ���߳�����һ�� io_context
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
	//��Ϊ����ִ��work.reset��������iocontext��run��״̬���˳�
	//��iocontext�Ѿ����˶���д�ļ����¼��󣬻���Ҫ�ֶ�stop�÷���

	for (auto& context : _ioContexts) {
		//�ѷ�����ֹͣ
		context.stop();
	}

	for (auto& work : _works) {
		work.reset();
	}

	for (auto& t : _threads) {
		t.join();
	}
}

