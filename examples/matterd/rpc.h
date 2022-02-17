#ifndef MATTERD_RPC_H
#define MATTERD_RPC_H

#include <controller/CHIPDeviceControllerFactory.h>
#include <platform/CHIPDeviceLayer.h>

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>

namespace chip {
namespace rpc {

class RPCServer
{
public:
    chip::Controller::DeviceCommissioner * controller;

    static RPCServer * GetInstance()
    {
        return &mInstance;
    }

private:
    static RPCServer mInstance;
};

template <typename T>
class RunOnMatterThreadContext
{
public:
    RunOnMatterThreadContext(std::function<T()> func) : func_(func) {}

    static void Callback(chip::System::Layer *, void * ctx)
    {
        RunOnMatterThreadContext<T> * this_ = reinterpret_cast<RunOnMatterThreadContext<T> *>(ctx);

        {
            std::unique_lock<std::mutex> lk(this_->mutex_);

            this_->return_value_ = this_->func_();

            this_->executed_ = true;
            this_->cv_.notify_one();
        }
    }

    static T Wait(std::unique_ptr<RunOnMatterThreadContext<T>> && ctx)
    {
        std::unique_lock<std::mutex> lk(ctx->mutex_);

        ctx->cv_.wait(lk, [&ctx]() { return ctx->executed_; });

        return ctx->return_value_;
    }

private:
    std::condition_variable cv_;
    std::mutex mutex_;
    bool executed_ = false;
    std::function<T()> func_;
    T return_value_;
};

template <typename T>
T RunOnMatterThreadAndWait(std::function<T()> func)
{
    auto ctx = std::make_unique<RunOnMatterThreadContext<T>>(func);
    DeviceLayer::SystemLayer().ScheduleWork(RunOnMatterThreadContext<T>::Callback, ctx.get());
    return RunOnMatterThreadContext<T>::Wait(std::move(ctx));
}

int Init();

} // namespace rpc
} // namespace chip

#endif // MATTERD_RPC_H
