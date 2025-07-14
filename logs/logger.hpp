// 日志器模块
//   1. 抽象出日志器基类
//   2. 实现子类（同步 & 异步）
#include <atomic>
#include <cstdarg>
#include <mutex>

#include "format.hpp"
#include "level.hpp"
#include "message.hpp"
#include "sink.hpp"
#include "util.hpp"

namespace wlog {
class Logger {
public:
    using ptr = std::shared_ptr<Logger>;
    Logger(const std::string &logger_name, LogLevel::Value &limit_level,
           const Formatter::ptr &fommatter, std::vector<LogSink::ptr> sinks)
        : _logger_name(logger_name),
          _limit_level(limit_level),
          _formatter(fommatter),
          _sinks(sinks.begin(), sinks.end()) {}
    // 构造消息，格式化，输出
    // 分为五种
    void debug(const std::string file, const size_t line, const std::string fmt,
               ...) {
        // 1.检查限制等级
        if (LogLevel::Value::DEBUG < _limit_level) {
            return;
        }
        // 2.根据fmt和不定参组织字符串
        va_list ap;
        va_start(ap, fmt);
        char *res;
        int ret = vasprintf(&res, fmt.c_str(), ap);
        if (ret == -1) {
            std::cerr << "vasprintf出错了" << std::endl;
            return;
        }
        va_end(ap);
        // 序列化并输出
        serialize(LogLevel::Value::DEBUG, file, line, res);
        // 注意需要释放这里的res
        free(res);
    }
    void info(const std::string file, size_t line, const std::string fmt, ...) {
        // 1.检查限制等级
        if (LogLevel::Value::INFO < _limit_level) {
            return;
        }
        // 2.根据fmt和不定参组织字符串
        va_list ap;
        va_start(ap, fmt);
        char *res;
        int ret = vasprintf(&res, fmt.c_str(), ap);
        if (ret == -1) {
            std::cerr << "vasprintf出错了" << std::endl;
            return;
        }
        va_end(ap);
        // 序列化并输出
        serialize(LogLevel::Value::INFO, file, line, res);
        // 注意需要释放这里的res
        free(res);
    }
    void warn(const std::string file, size_t line, const std::string fmt, ...) {
        // 1.检查限制等级
        if (LogLevel::Value::WARNING < _limit_level) {
            return;
        }
        // 2.根据fmt和不定参组织字符串
        va_list ap;
        va_start(ap, fmt);
        char *res;
        int ret = vasprintf(&res, fmt.c_str(), ap);
        if (ret == -1) {
            std::cerr << "vasprintf出错了" << std::endl;
            return;
        }
        va_end(ap);
        // 序列化并输出
        serialize(LogLevel::Value::WARNING, file, line, res);
        // 注意需要释放这里的res
        free(res);
    }
    void error(const std::string file, size_t line, const std::string fmt,
               ...) {
        // 1.检查限制等级
        if (LogLevel::Value::ERROR < _limit_level) {
            return;
        }
        // 2.根据fmt和不定参组织字符串
        va_list ap;
        va_start(ap, fmt);
        char *res;
        int ret = vasprintf(&res, fmt.c_str(), ap);
        if (ret == -1) {
            std::cerr << "vasprintf出错了" << std::endl;
            return;
        }
        va_end(ap);
        // 序列化并输出
        serialize(LogLevel::Value::ERROR, file, line, res);
        // 注意需要释放这里的res
        free(res);
    }
    void fatal(const std::string file, size_t line, const std::string fmt,
               ...) {
        // 1.检查限制等级
        if (LogLevel::Value::FATAL < _limit_level) {
            return;
        }
        // 2.根据fmt和不定参组织字符串
        va_list ap;
        va_start(ap, fmt);
        char *res;
        int ret = vasprintf(&res, fmt.c_str(), ap);
        if (ret == -1) {
            std::cerr << "vasprintf出错了" << std::endl;
            return;
        }
        va_end(ap);
        // 序列化并输出
        serialize(LogLevel::Value::FATAL, file, line, res);
        // 注意需要释放这里的res
        free(res);
    }

protected:
    void serialize(const LogLevel::Value level, const std::string file,
                   const size_t line, char *str) {
        // 3.构建msg对象，再组织成字符串
        LogMsg msg(level, _logger_name, file, line, str);
        std::stringstream ss;
        _formatter->format(ss, msg);
        // 4.调用接口进行输出
        log(ss.str().c_str(), ss.str().size());
    }
    // 将实际的输出操作设为抽象接口，具体输出方式（同步或异步）子类实现
    virtual void log(const char *data, size_t len = 0) = 0;

protected:
    std::mutex _mutex;
    std::string _logger_name;
    std::atomic<LogLevel::Value> _limit_level;  // 日志输出限制等级
    Formatter::ptr _formatter;                  // 格式化
    std::vector<LogSink::ptr> _sinks;           // 日志落地位置（可以多选）
};

class SyncLogger : public Logger {
public:
    SyncLogger(const std::string &logger_name, LogLevel::Value &limit_level,
               const Formatter::ptr &fommatter, std::vector<LogSink::ptr> sinks)
        : Logger(logger_name, limit_level, fommatter, sinks) {}

protected:
    virtual void log(const char *data, size_t len = 0) override {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_sinks.empty()) return;
        for (auto &sink : _sinks) {
            sink->log(data, len);
        }
    }
};

};  // namespace wlog
