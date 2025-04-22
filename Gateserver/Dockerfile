#--------------------编译gateserver

#gateserver的编译
FROM shukovsky/base-image AS builder
WORKDIR /root/code/Gateserver
#COPY目录下的最新代码
COPY . .
RUN protoc -I=./src --cpp_out=./src --grpc_out=./src --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) ./src/message.proto
#构建
RUN mkdir build && \
cd build && \
cmake .. -G Ninja -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release && \
ninja

#-------------------构建最终发布镜像
FROM ubuntu:22.04 AS final
WORKDIR /root
RUN apt-get update && apt-get install -y --no-install-recommends \
ca-certificates \
libicu70 \
libssl3 \
libstdc++6 \
libmysqlclient21 \
&& rm -rf /var/lib/apt/lists/*
# 从构建阶段复制必要的库文件
# Boost库
COPY --from=builder /usr/lib/libboost_*.so* /usr/lib/
# JsonCpp库
COPY --from=builder /usr/lib/x86_64-linux-gnu/libjsoncpp* /usr/lib/

# gRPC及相关库
COPY --from=builder /usr/lib/libgrpc*.so* /usr/lib/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libprotobuf.so* /usr/lib/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libabsl*.so* /usr/lib/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libcares.so* /usr/lib/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libre2.so* /usr/lib/
COPY --from=builder /usr/lib/libutf8*.so* /usr/lib/
COPY --from=builder /usr/lib/libupb*.so* /usr/lib/
COPY --from=builder /usr/lib/libaddress_sorting.so* /usr/lib/
COPY --from=builder /usr/lib/libgpr.so* /usr/lib/
# Redis相关库
COPY --from=builder /usr/lib/x86_64-linux-gnu/libredis* /usr/lib/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libhiredis* /usr/lib/
COPY --from=builder /usr/lib/x86_64-linux-gnu/libredis++.so* /usr/lib/
# MySQL C++ 连接器
COPY --from=builder /usr/lib64/libmysqlcppconn*.so* /usr/lib64/
# 拷贝二进制文件
COPY --from=builder /root/code/Gateserver/build/gateserver /root/gateserver/gateserver
COPY --from=builder /root/code/Gateserver/build/config.ini /root/gateserver/config.ini
# 更新动态链接器配置
RUN echo "/usr/lib64" >> /etc/ld.so.conf.d/mysql-connector-cpp.conf && \
ldconfig

WORKDIR /root/gateserver
CMD ["/root/gateserver/gateserver"]