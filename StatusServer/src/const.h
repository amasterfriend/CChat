#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include "Singleton.h"
#include <functional>
#include <map>
#include <unordered_map>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp> 
#include <boost/property_tree/ini_parser.hpp>
#include <hiredis/hiredis.h>
#include <queue>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace beast = boost::beast;         // �� <boost/beast.hpp> ���������ռ�
namespace http = beast::http;           // �� <boost/beast/http.hpp> ���������ռ�
namespace net = boost::asio;            // �� <boost/asio.hpp> ���������ռ�
using tcp = boost::asio::ip::tcp;       // �� <boost/asio/ip/tcp.hpp> ���������ռ�

enum ErrorCodes {
    Success = 0,
    Error_Json = 1001,  //Json��������
    RPCFailed = 1002,  //RPC�������
    VarifyExpired = 1003, //��֤�����
    VarifyCodeErr = 1004, //��֤�����
    UserExist = 1005,       //�û��Ѿ�����
    PasswdErr = 1006,    //�������
    EmailNotMatch = 1007,  //���䲻ƥ��
    PasswdUpFailed = 1008,  //��������ʧ��
    PasswdInvalid = 1009,   //�������ʧ��
    TokenInvalid = 1010,   //TokenʧЧ
    UidInvalid = 1011,  //uid��Ч
};

// Defer��
class Defer {
public:
    // ����һ��lambda���ʽ���ߺ���ָ��
    Defer(std::function<void()> func) : func_(func) {}

    // ����������ִ�д���ĺ���
    ~Defer() {
        func_();
    }

private:
    std::function<void()> func_;
};

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"