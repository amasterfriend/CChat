#include "CServer.h"
#include "HttpConnection.h"
#include <memory>
#include <iostream>
#include "AsioIOContextPool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) :_ioc(ioc),
_acceptor(ioc, tcp::endpoint(tcp::v4(), port)){

}

//监听新连接
void CServer::Start()
{
    //防止对象被析构，回调函数还没执行结束，通过智能指针延长生命周期
    auto self = shared_from_this();
	auto& io_context = AsioIOContextPool::GetInstance()->GetIOContext();
	std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);
    _acceptor.async_accept(new_con->GetSocket(), [self, new_con](beast::error_code ec) {
        try {
            //出错则放弃这个连接，继续监听新链接
            if (ec) {
                self->Start();
                return;
            }

            //处理新链接，创建HpptConnection类管理新连接
            new_con->Start();
            //继续监听
            self->Start();
        }
        catch (std::exception& exp) {
            std::cout << "exception is " << exp.what() << std::endl;
            self->Start();
        }
        });
}