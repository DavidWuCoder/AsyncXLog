// 日志器模块
//   1. 抽象出日志器基类
//   2. 实现子类（同步 & 异步）
//   3. 引入建造者类
#include <atomic>
#include <cstdarg>
#include <mutex>
#include <unordered_map>

#include "format.hpp"
#include "level.hpp"
#include "looper.hpp"
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

    const std::string &getName() { return _logger_name; }

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
class AsyncLogger : public Logger {
public:
    AsyncLogger(const std::string &logger_name, LogLevel::Value &limit_level,
                const Formatter::ptr &fommatter,
                std::vector<LogSink::ptr> sinks, LooperType looper_type)
        : Logger(logger_name, limit_level, fommatter, sinks),
          _looper(std::make_shared<AsyncLooper>(
              std::bind(&AsyncLogger::asyncLog, this, std::placeholders::_1),
              looper_type)) {}

protected:
    virtual void log(const char *data, size_t len = 0) override {
        _looper->push(data, len);
    }

    // 实际落地函数
    void asyncLog(Buffer &buffer) {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_sinks.empty()) return;
        for (auto &sink : _sinks) {
            sink->log(buffer.begin(), buffer.readableSize());
        }
    }

    AsyncLooper::ptr _looper;
};

// 使用建造者模式构造日志器
enum class LoggerType { SYNC, ASYNC };

// 1. 抽象日志器建造者类（这里先决定同步还是异步）
class LoggerBuilder {
public:
    LoggerBuilder()
        : _logger_type(LoggerType::ASYNC),
          _limit_level(LogLevel::Value::DEBUG),
          _looper_type(LooperType::SAFE) {}
    void buildType(const LoggerType &logger_type) {
        _logger_type = logger_type;
    }

    void enableUnsafeAsync() { _looper_type = LooperType::UNSAFE; }

    void buildName(const std::string logger_name) {
        _logger_name = logger_name;
    }
    void buildLimitLevel(const LogLevel::Value &limit_level) {
        _limit_level = limit_level;
    }
    void buildFommatter(
        const std::string pattern = "[%d{%H:%M:%S}][%t][%c][%p][%f:%l]%T%m%n") {
        _formatter = std::make_shared<Formatter>(pattern);
    }
    template <typename SinkType, typename... Args>
    void buildSink(Args &&...args) {
        auto sink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
        _sinks.push_back(sink);
    }
    virtual Logger::ptr build() = 0;

protected:
    LoggerType _logger_type;
    std::string _logger_name;
    LogLevel::Value _limit_level;      // 日志输出限制等级
    Formatter::ptr _formatter;         // 格式化
    std::vector<LogSink::ptr> _sinks;  // 日志落地位置（可以多选）
    LooperType _looper_type;
};

// 2. 派生出具体的建造者类型（局部或全局）
// 局部
class LocalLoggerBuilder : public LoggerBuilder {
public:
    Logger::ptr build() override {
        assert(!_logger_name.empty());  // 用户一定要设置日志器名称
        if (_formatter.get() == nullptr) {
            buildFommatter();
        }
        if (_sinks.empty()) {
            buildSink<StdoutSink>();
        }
        if (_logger_type == LoggerType::ASYNC) {
            return std::make_shared<AsyncLogger>(
                _logger_name, _limit_level, _formatter, _sinks, _looper_type);
        }
        return std::make_shared<SyncLogger>(_logger_name, _limit_level,
                                            _formatter, _sinks);
    }
};

// 日志器管理类（单例）
class LoggerManager {
public:
    static LoggerManager &getInstance() {
        static LoggerManager eton;
        return eton;
    }

    void addLogger(Logger::ptr &logger) {
        if (hasLogger(logger->getName())) return;
        std::lock_guard<std::mutex> guard(_mutex);
        _loggers.insert(std::make_pair(logger->getName(), logger));
    }

    bool hasLogger(const std::string &logger_name) {
        std::lock_guard<std::mutex> guard(_mutex);
        auto it = _loggers.find(logger_name);
        if (it == _loggers.end()) {
            return false;
        }
        return true;
    }

    Logger::ptr gerLogger(const std::string &logger_name) {
        // std::cout << "开始查找" << std::endl;
        std::lock_guard<std::mutex> guard(_mutex);
        auto it = _loggers.find(logger_name);
        if (it == _loggers.end()) {
            // std::cout << "没找到" << std::endl;
            return Logger::ptr();
        }
        // std::cout << "找到了" << std::endl;
        return it->second;
    }

private:
    LoggerManager() {
        std::unique_ptr<wlog::LoggerBuilder> builder(
            new wlog::LocalLoggerBuilder());
        builder->buildName("_root_logger");
        _root_logger = builder->build();
        addLogger(_root_logger);
    }
    ~LoggerManager() {}

private:
    std::mutex _mutex;
    Logger::ptr _root_logger;  // 默认日志器
    std::unordered_map<std::string, Logger::ptr> _loggers;
};

// 全局
class GlobalLoggerBuilder : public LoggerBuilder {
public:
    Logger::ptr build() override {
        assert(!_logger_name.empty());  // 用户一定要设置日志器名称
        if (_formatter.get() == nullptr) {
            buildFommatter();
        }
        if (_sinks.empty()) {
            buildSink<StdoutSink>();
        }
        Logger::ptr logger;
        if (_logger_type == LoggerType::ASYNC) {
            logger = std::make_shared<AsyncLogger>(
                _logger_name, _limit_level, _formatter, _sinks, _looper_type);
        } else {
            logger = std::make_shared<SyncLogger>(_logger_name, _limit_level,
                                                  _formatter, _sinks);
        }
        LoggerManager::getInstance().addLogger(logger);
        return logger;
    }
};

};  // namespace wlog
