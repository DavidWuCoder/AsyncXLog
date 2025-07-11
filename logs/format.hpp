#pragma once
#include <cstring>
#include <ctime>
#include <iostream>

#include "level.hpp"
#include "message.hpp"

namespace wyllog {
// 格式化子项的基类
class FormatItem {
    virtual void format(std::ostream &out, LogMsg &msg) = 0;
};
// 有效载荷-日志等级-日志器名称-线程ID-时间-文件名-行号-制表符-换行-其他
class MsgFormatItem : public FormatItem {
    void format(std::ostream &out, LogMsg &msg) override {
        out << msg._payload;
    }
};
class LevelFormatItem : public FormatItem {
    void format(std::ostream &out, LogMsg &msg) override {
        out << LogLevel::toString(msg._level);
    }
};
class LoggerFormatItem : public FormatItem {
    void format(std::ostream &out, LogMsg &msg) override { out << msg._logger; }
};
class ThreadIdFormatItem : public FormatItem {
    void format(std::ostream &out, LogMsg &msg) override { out << msg._tid; }
};
class TimeFormatItem : public FormatItem {
    TimeFormatItem(const std::string &fmt = "%H:%M:%S") : _time_fmt(fmt) {}
    void format(std::ostream &out, LogMsg &msg) override {
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
    void format(std::ostream &out, LogMsg &msg) override { out << msg._file; }
};
class LineFormatItem : public FormatItem {
    void format(std::ostream &out, LogMsg &msg) override { out << msg._line; }
};
class TableFormatItem : public FormatItem {
    void format(std::ostream &out, LogMsg &msg) override { out << "\t"; }
};
class NlineFormatItem : public FormatItem {
    void format(std::ostream &out, LogMsg &msg) override { out << "\n"; }
};
class OtherFormatItem : public FormatItem {
    OtherFormatItem(const std::string str) : _str(str) {}
    void format(std::ostream &out, LogMsg &msg) override { out << _str; }

private:
    std::string _str;
};
}  // namespace wyllog
