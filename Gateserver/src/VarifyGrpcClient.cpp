#include "VarifyGrpcClient.h"

RPConPool::RPConPool(size_t poolSize, std::string host, std::string port) : poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
    for (size_t i = 0; i < poolSize_; ++i) {

        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
            grpc::InsecureChannelCredentials());

        connections_.push(VarifyService::NewStub(channel));
    }
}

RPConPool::~RPConPool()
{
    std::lock_guard<std::mutex> lock(mutex_);
    Close();
    while (!connections_.empty()) {
        connections_.pop();
    }
}

std::unique_ptr<VarifyService::Stub> RPConPool::getConnection()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this] {
        if (b_stop_) {
            return true;
        }
        return !connections_.empty();
        });
    //如果停止则直接返回空指针
    if (b_stop_) {
        return  nullptr;
    }
    auto context = std::move(connections_.front());
    connections_.pop();
    return context;
}

void RPConPool::returnConnection(std::unique_ptr<VarifyService::Stub> context)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (b_stop_) {
        return;
    }
    connections_.push(std::move(context));
    cond_.notify_one();
}


GetVarifyRsp VerifyGrpcClient::GetVarifyCode(std::string email)
{
    {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto it = cache_.find(email);
    if (it != cache_.end()) {
        auto now = std::chrono::steady_clock::now();
        if (now - it->second.second < std::chrono::seconds(60)) { // 缓存有效期60秒
            return it->second.first; // 直接返回缓存
        }
    }
}
    ClientContext context;
    GetVarifyRsp reply;
    GetVarifyReq request;
    request.set_email(email);
    auto stub = pool_->getConnection();
    Status status = stub->GetVarifyCode(&context, request, &reply);

    if (status.ok()) {
        pool_->returnConnection(std::move(stub));
    }
    else {
        pool_->returnConnection(std::move(stub));
        reply.set_error(ErrorCodes::RPCFailed);
    }
    // 成功才缓存
    if (reply.error() == 0) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    cache_[email] = { reply, std::chrono::steady_clock::now() };
    }
    return reply;
}
