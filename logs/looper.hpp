// 异步日志的工作线程封装
#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "buffer.hpp"
namespace wlog {
using Func = std::function<void(Buffer&)>;

enum class LooperType { SAFE, UNSAFE };

class AsyncLooper {
public:
    using ptr = std::shared_ptr<AsyncLooper>;
    AsyncLooper(const Func& cb, LooperType looper_type = LooperType::SAFE)
        : _running(true),
          _thread(std::thread(&AsyncLooper::threadEntry, this)),
          _looper_type(looper_type),
          _callback(cb) {}
    ~AsyncLooper() { stop(); }
    void stop() {
        _running = false;
        _cond_con.notify_all();  // 唤醒所有的工作线程
        _cond_pro.notify_all();
        _thread.join();  // 等待工作线程退出
    }
    void push(const char* data, int len) {
        // 1. 无限扩容，用于测试
        // 2. 阻塞式，安全
        std::unique_lock<std::mutex> lock(_mutex);
        // 条件变量
        if (_looper_type == LooperType::SAFE)
            _cond_pro.wait(
                lock, [&]() { return len <= _pro_buffer.writeableSize(); });
        // 添加数据
        _pro_buffer.push(data, len);
        // 唤醒消费者
        _cond_con.notify_one();
    }

private:
    void threadEntry() {
        while (1) {
            {
                // 运行标志设为否且数据处理完毕，再退出，否则会导致数据处理不完全
                if (!_running && _pro_buffer.empty()) {
                    break;
                }
                std::unique_lock<std::mutex> lock(_mutex);
                // 工作线程退出或者生产者缓冲区有数据唤醒线程
                if (_looper_type == LooperType::SAFE) {
                    _cond_con.wait(lock, [&]() {
                        return !_running || !_pro_buffer.empty();
                    });
                }
                // 交换两个缓冲区
                _con_buffer.swap(_pro_buffer);
                // 唤醒全部生产者
                _cond_pro.notify_all();
            }
            // 处理数据
            _callback(_con_buffer);
            // 初始化消费者缓冲区
            _con_buffer.reset();
        }
    }

private:
    bool _running;       // 工作停止标志
    Buffer _pro_buffer;  // 生产缓冲区
    Buffer _con_buffer;  // 消费缓冲区
    std::mutex _mutex;
    std::condition_variable _cond_pro;  // 生产者条件变量
    std::condition_variable _cond_con;  // 消费者条件变量
    std::thread _thread;                // 消费线程
    Func _callback;
    LooperType _looper_type;
};
}  // namespace wlog
