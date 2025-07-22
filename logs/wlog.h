#pragma once

#include <cstdarg>

#include "logger.hpp"

namespace wlog {
// 1. 获取指定日志器的全局接口
Logger::ptr getLogger(const std::string& name) {
    return wlog::LoggerManager::getInstance().getLogger(name);
}

Logger::ptr rootLogger() {
    return wlog::LoggerManager::getInstance().rootLogger();
}

// 2. 使用宏函数进行代理
#define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__);
#define info(fmt, ...) info(__FILE__, __LINE__, fmt, ##__VA_ARGS__);
#define warning(fmt, ...) warning(__FILE__, __LINE__, fmt, ##__VA_ARGS__);
#define error(fmt, ...) error(__FILE__, __LINE__, fmt, ##__VA_ARGS__);
#define fatal(fmt, ...) fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__);

// 3. 使用宏函数, 直接通过默认日志器进行标准输出的打印
#define DEBUG(fmt, ...) wlog::rootLogger()->debug(fmt, ##__VA_ARGS__);
#define INFO(fmt, ...) wlog::rootLogger()->info(fmt, ##__VA_ARGS__);
#define WARNING(fmt, ...) wlog::rootLogger()->warning(fmt, ##__VA_ARGS__);
#define ERROR(fmt, ...) wlog::rootLogger()->error(fmt, ##__VA_ARGS__);
#define FATAL(fmt, ...) wlog::rootLogger()->fatal(fmt, ##__VA_ARGS__);

}  // namespace wlog
