#include "AsioIOContextPool.h"
#include <iostream>

using namespace std;
AsioIOContextPool::AsioIOContextPool(std::size_t size) :_ioContext(size),
_works(size), _nextIOContext(0) {
    for (std::size_t i = 0; i < size; ++i) {
        _works[i] = std::make_unique<Work>(_ioContext[i].get_executor());
    }

    //�������ioservice����������̣߳�ÿ���߳��ڲ�����ioservice
    for (std::size_t i = 0; i < _ioContext.size(); ++i) {
        _threads.emplace_back([this, i]() {
            _ioContext[i].run();
            });
    }
}

AsioIOContextPool::~AsioIOContextPool() {
    std::cout << "AsioIOContextPool destruct" << endl;
}

boost::asio::io_context& AsioIOContextPool::GetIOContext() {
    auto& context = _ioContext[_nextIOContext++];
    if (_nextIOContext == _ioContext.size()) {
        _nextIOContext = 0;
    }
    return context;
}

void AsioIOContextPool::Stop() {
    //��Ϊ����ִ��work.reset��������iocontext��run��״̬���˳�
    //��iocontext�Ѿ����˶���д�ļ����¼��󣬻���Ҫ�ֶ�stop�÷���
    for (auto& context : _ioContext) {
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