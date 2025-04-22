#pragma once
#include "const.h"

// CServer 类，继承自 std::enable_shared_from_this<CServer>
class CServer :public std::enable_shared_from_this<CServer>
{
public:
    // 构造函数，接受 io_context 和端口号作为参数
    CServer(boost::asio::io_context& ioc, unsigned short& port);
    
    // 启动服务器
    void Start();
    
private:
    tcp::acceptor  _acceptor;           // 接收器，用于接受传入连接
    net::io_context& _ioc;              // io_context 引用，用于管理异步操作
};
