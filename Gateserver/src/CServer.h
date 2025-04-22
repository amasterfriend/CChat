#pragma once
#include "const.h"

// CServer �࣬�̳��� std::enable_shared_from_this<CServer>
class CServer :public std::enable_shared_from_this<CServer>
{
public:
    // ���캯�������� io_context �Ͷ˿ں���Ϊ����
    CServer(boost::asio::io_context& ioc, unsigned short& port);
    
    // ����������
    void Start();
    
private:
    tcp::acceptor  _acceptor;           // �����������ڽ��ܴ�������
    net::io_context& _ioc;              // io_context ���ã����ڹ����첽����
};
