// 日志落地模块
//   1. 抽象出落地基类
//   2. 实现不同子类
//   3. 用简单工厂进行创建与表示的分离
#pragma once
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "util.hpp"

namespace wlog {
class LogSink {
public:
    using ptr = std::shared_ptr<LogSink>;
    ~LogSink() {}
    virtual void log(const char *data, size_t len) = 0;
};

// 落地方向：标准输出
class StdoutSink : public LogSink {
public:
    using ptr = std::shared_ptr<StdoutSink>;
    // 将日志消息写到标准输出
    void log(const char *data, size_t len) { std::cout.write(data, len); }
};

// 落地方向：指定文件
class FileSink : public LogSink {
public:
    using ptr = std::shared_ptr<FileSink>;
    // 传入文件名，构造输出流
    FileSink(const std::string &pathname) : _pathname(pathname) {
        // 创建指定目录
        wlog::file::createDirectory(wlog::file::path(_pathname));
    }

    // 将日志消息写到指定文件
    void log(const char *data, size_t len) {
        if (_ofs.is_open() == false) {
            // 打开文件
            _ofs.open(_pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }
        _ofs.write(data, len);
        assert(_ofs.good());
    }

private:
    std::string _pathname;
    std::ofstream _ofs;
};

// 落地方向：按照指定文件大小滚动文件
class RollSinkBySize : public LogSink {
public:
    using ptr = std::shared_ptr<RollSinkBySize>;
    // 传入文件名，和单个文件的上限，构造输出流
    RollSinkBySize(const std::string &basename, size_t max_size)
        : _basename(basename),
          _max_size(max_size),
          _cur_size(0),
          _name_count(0) {
        // 创建指定目录
        wlog::file::createDirectory(wlog::file::path(_basename));
    }
    // 将日志消息写到指定文件
    void log(const char *data, size_t len) {
        initLogFile();
        _ofs.write(data, len);
        assert(_ofs.good());
        _cur_size += len;
    }

private:
    void initLogFile() {
        if (_ofs.is_open() == false || _cur_size >= _max_size) {
            _ofs.close();
            std::string name = createFilename();
            _ofs.open(name, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
            _cur_size = 0;
        }
    }

    // 创建文件
    std::string createFilename() {
        time_t time = date::now();
        struct tm t;
        localtime_r(&time, &t);
        std::stringstream ss;
        ss << _basename;
        ss << t.tm_year + 1900;
        ss << t.tm_mon;
        ss << t.tm_mday;
        ss << t.tm_hour;
        ss << t.tm_min;
        ss << t.tm_sec;
        ss << "-";
        ss << _name_count++;
        ss << ".log";
        return ss.str();
    }

private:
    std::string _basename;  // 基础文件名
    std::ofstream _ofs;
    size_t _max_size;  // 单个文件的大小上限
    size_t _cur_size;  // 当前文件大小
    size_t _name_count;
};

class SinkFactory {
public:
    template <class Type, class... Args>
    static LogSink::ptr create(Args &&...args) {
        return std::make_shared<Type>(std::forward<Args>(args)...);
    }
};
}  // namespace wlog
