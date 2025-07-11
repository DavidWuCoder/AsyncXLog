// 日志消息类：
// 意义：中间存储一条消息所需的各项要素
//   1. 日志输出时间
//   2. 日志等级
//   3. 日志器名称
//   4. 源文件名称
//   5. 源代码行号
//   6. 线程ID
//   7. 日志的有效消息
#pragma once

#include <thread>

#include "level.hpp"
#include "util.hpp"

namespace wyllog {
struct LogMsg {
    time_t _c_time;                  // 当前时间
    wyllog::LogLevel::Value _level;  // 日志等级
    std::string _logger;             // 日志器名称
    std::string _file;               // 文件名称
    size_t _line;                    // 行号
    std::thread::id _tid;            // 线程ID
    std::string _payload;            // 有效消息

    LogMsg(wyllog::LogLevel::Value level, const std::string &logger,
           const std::string file, size_t line, const std::string &&msg)
        : _c_time(date::now()),
          _level(level),
          _logger(logger),
          _file(file),
          _line(line),
          _tid(std::this_thread::get_id()),
          _payload(std::move(msg)) {}
};
}  // namespace wyllog
