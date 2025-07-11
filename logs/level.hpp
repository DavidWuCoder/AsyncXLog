// 日志等级类：
//  1. 枚举日志等级
//  2. 提供将枚举的等级转换成字符串的接口
#pragma once

namespace wyllog {
class LogLevel {
public:
    enum class Value { DEBUG = 0, INFO, WARNING, ERROR, FATAL, OFF };

    static const char *toString(Value value) {
        switch (value) {
            case Value::DEBUG:
                return "DEBUG";
            case Value::INFO:
                return "INFO";
            case Value::WARNING:
                return "WARNING";
            case Value::ERROR:
                return "ERROR";
            case Value::FATAL:
                return "FATAL";
            case Value::OFF:
                return "OFF";
        }
        return "UNKNOW";
    }
};
}  // namespace wyllog
