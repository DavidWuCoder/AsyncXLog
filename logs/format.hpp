#pragma once
#include <cassert>
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <vector>

#include "level.hpp"
#include "message.hpp"

namespace wlog {
// 格式化子项的基类
class FormatItem {
public:
    using ptr = std::shared_ptr<FormatItem>;
    virtual void format(std::ostream &out, const LogMsg &msg) = 0;
};
// 有效载荷-日志等级-日志器名称-线程ID-时间-文件名-行号-制表符-换行-其他
class MsgFormatItem : public FormatItem {
public:
    void format(std::ostream &out, const LogMsg &msg) override {
        out << msg._payload;
    }
};
class LevelFormatItem : public FormatItem {
public:
    void format(std::ostream &out, const LogMsg &msg) override {
        out << LogLevel::toString(msg._level);
    }
};
class LoggerFormatItem : public FormatItem {
public:
    void format(std::ostream &out, const LogMsg &msg) override {
        out << msg._logger;
    }
};
class ThreadIdFormatItem : public FormatItem {
public:
    void format(std::ostream &out, const LogMsg &msg) override {
        out << msg._tid;
    }
};
class TimeFormatItem : public FormatItem {
public:
    TimeFormatItem(const std::string &fmt = "%H:%M:%S") : _time_fmt(fmt) {}
    void format(std::ostream &out, const LogMsg &msg) override {
        struct tm t;
        localtime_r(&msg._c_time, &t);
        char tmp[32];
        memset(tmp, 0, sizeof(tmp));
        strftime(tmp, 31, _time_fmt.c_str(), &t);
        out << tmp;
    }

private:
    std::string _time_fmt;
};
class FileFormatItem : public FormatItem {
public:
    void format(std::ostream &out, const LogMsg &msg) override {
        out << msg._file;
    }
};
class LineFormatItem : public FormatItem {
public:
    void format(std::ostream &out, const LogMsg &msg) override {
        out << msg._line;
    }
};
class TableFormatItem : public FormatItem {
public:
    void format(std::ostream &out, const LogMsg &msg) override { out << "\t"; }
};
class NlineFormatItem : public FormatItem {
public:
    void format(std::ostream &out, const LogMsg &msg) override { out << "\n"; }
};
class OtherFormatItem : public FormatItem {
public:
    OtherFormatItem(const std::string str) : _str(str) {}
    void format(std::ostream &out, const LogMsg &msg) override { out << _str; }

private:
    std::string _str;
};
// %d 日期，包含子项时分秒{%H:%M:%S}
// %t 线程ID
// %c 日志器名称
// %p 日志等级
// %f 所在文件名
// %l 所在行号
// %T 表示制表符
// %m 表示消息主体
// %n 表示换行
class Formatter {
public:
    using ptr = std::shared_ptr<Formatter>;
    Formatter(
        const std::string &pattern = "[%d{%H:%M:%S}][%t][%c][%p][%f:%l]%T%m%n")
        : _pattern(pattern) {
        assert(parsePattern());
    }
    // 对msg格式化
    void format(std::ostream &out, LogMsg &msg) {
        for (auto &item : _items) {
            item->format(out, msg);
        }
    }
    std::string format(LogMsg &msg) {
        std::stringstream ss;
        format(ss, msg);
        return ss.str();
    }

private:
    // 解析格式化字符串
    bool parsePattern() {
        // "sg{}fsg%d{%H:%M:%S}%Tsdf%t%T[%p]%T[%c]%T%f:%l%T%m%n"
        std::string key;
        std::string val;
        std::vector<std::pair<std::string, std::string>> format_order;
        size_t pos = 0;
        while (pos < _pattern.size()) {
            if (_pattern[pos] != '%') {
                // 处理普通字符串
                val.push_back(_pattern[pos]);
                pos++;
                continue;
            }
            // 到这里pos指向一个%
            pos++;
            if (pos == _pattern.size()) {
                std::cout << "%之后没有字符, 格式错误" << std::endl;
                return false;
            }
            if (_pattern[pos] == '%') {
                val.push_back('%');
                pos++;
                continue;
            }
            // 到这里说明pos指向一个格式化字符
            if (!val.empty()) format_order.emplace_back("", val);
            val.clear();

            key = _pattern[pos];
            pos++;
            if (pos < _pattern.size() && _pattern[pos] == '{') {
                pos++;
                while (pos < _pattern.size() && _pattern[pos] != '}') {
                    val.push_back(_pattern[pos]);
                    pos++;
                }
                if (pos == _pattern.size()) {
                    std::cout << "{}没有配对，格式错误" << std::endl;
                    return false;
                }
                // 这时pos在'}'
                pos++;
            }
            format_order.emplace_back(key, val);
            key.clear();
            val.clear();
        }
        // 根据得到的数据初始化不同的子项
        for (auto &it : format_order) {
            _items.emplace_back(createItem(it.first, it.second));
        }
        return true;
    }

private:
    // 根据不同的格式创建不同的子项
    FormatItem::ptr createItem(const std::string &key, const std::string &val) {
        // %d 日期，包含子项时分秒{%H:%M:%S}
        // %t 线程ID
        // %c 日志器名称
        // %p 日志等级
        // %f 所在文件名
        // %l 所在行号
        // %T 表示制表符
        // %m 表示消息主体
        // %n 表示换行
        if (key == "d") return std::make_shared<TimeFormatItem>(val);
        if (key == "t") return std::make_shared<ThreadIdFormatItem>();
        if (key == "c") return std::make_shared<LoggerFormatItem>();
        if (key == "p") return std::make_shared<LevelFormatItem>();
        if (key == "f") return std::make_shared<FileFormatItem>();
        if (key == "l") return std::make_shared<LineFormatItem>();
        if (key == "T") return std::make_shared<TableFormatItem>();
        if (key == "m") return std::make_shared<MsgFormatItem>();
        if (key == "n") return std::make_shared<NlineFormatItem>();
        if (key.empty()) return std::make_shared<OtherFormatItem>(val);
        std::cout << "没有对应的格式化字符：%" << key << std::endl;
        abort();
        return FormatItem::ptr();
    }

private:
    std::string _pattern;
    std::vector<FormatItem::ptr> _items;
};
}  // namespace wlog
