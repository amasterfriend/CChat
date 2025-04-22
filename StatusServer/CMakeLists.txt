#指定cmake的最小版本
cmake_minimum_required(VERSION 3.10)

#指定项目名
project(statusserver LANGUAGES CXX)
#大概是引入当前目录的头文件
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#设置c++17标准
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)


#mysql-conn相关
#找到openssl库，这个mysql要用
find_package(OpenSSL REQUIRED)

#这里强行指定了mysql-connector链接的具体的动态库文件，jdbc风格按理说该链接到这个文件，但是不指定貌似链接器会乱来
set(MYSQL_CPPCONN_LIB "/usr/lib64/libmysqlcppconn.so" CACHE FILEPATH "MySQL Connector/C++ library")

message(STATUS "Using MySQL Connector/C++ library: ${MYSQL_CPPCONN_LIB}")

find_package(jsoncpp REQUIRED)

find_package(Threads REQUIRED)
#这里有boost库需要用的模块
find_package(Boost 1.82 COMPONENTS system filesystem atomic thread REQUIRED)
#下面是grpc和protobuf相关的设置，我直接从up主的博客抄的，具体原理我也不清楚
set(protobuf_MODULE_COMPATIBLE TRUE)

find_package(Protobuf CONFIG REQUIRED)
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)

set(_REFLECTION gRPC::grpc++_reflection)

# Find gRPC installation

# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.

find_package(gRPC CONFIG REQUIRED)

message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)

#这里把所有源文件放到了两个变量里面去
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB PBSOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)

#添加可执行文件
add_executable(statusserver ${SOURCES} ${PBSOURCES})

#指定项目要与哪些库链接
target_link_libraries(statusserver ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF} 
Boost::filesystem 

Boost::system

Boost::atomic

Boost::thread

jsoncpp

hiredis

redis++

${MYSQL_CPPCONN_LIB}

OpenSSL::SSL

OpenSSL::Crypto

)

#复制config.ini文件到build 目录
#每次执行cmake会检查一遍config.ini有没有变化，有变化就复制到build目录下
set(Targetconfig ${CMAKE_CURRENT_SOURCE_DIR}/src/config.ini)
set(OutPutDir ${CMAKE_BINARY_DIR})

add_custom_command(TARGET statusserver POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${Targetconfig}" "${OutPutDir}/config.ini")