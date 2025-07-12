#include <string>

#include "format.hpp"
#include "level.hpp"
#include "sink.hpp"
#include "util.hpp"

enum class TimeGap { GAP_SECOND, GAP_MINUTE, GAP_HOUR, GAP_DAY };

class RollSinkByTime : public wyllog::LogSink {
public:
    using ptr = std::shared_ptr<RollSinkByTime>;
    // 传入文件名，和单个文件的上限，构造输出流
    RollSinkByTime(const std::string &basename, TimeGap gap_type)
        : _basename(basename) {
        switch (gap_type) {
            case TimeGap::GAP_SECOND:
                _gap_size = 1;
                break;
            case TimeGap::GAP_MINUTE:
                _gap_size = 60;
                break;
            case TimeGap::GAP_HOUR:
                _gap_size = 3600;
                break;
            case TimeGap::GAP_DAY:
                _gap_size = 3600 * 24;
                break;
        }
        _cur_gap = getCurGap();
        // 创建指定目录
        wyllog::file::createDirectory(wyllog::file::path(_basename));
    }
    // 将日志消息写到指定文件
    void log(const char *data, size_t len) {
        initLogFile();
        _ofs.write(data, len);
        assert(_ofs.good());
    }

    size_t getCurGap() {
        return _gap_size == 1 ? wyllog::date::now()
                              : wyllog::date::now() % _gap_size;
    }

private:
    void initLogFile() {
        if (_ofs.is_open() == false || getCurGap() != _cur_gap) {
            _ofs.close();
            std::string name = createFilename();
            _ofs.open(name, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
            _cur_gap = getCurGap();
        }
    }

    // 创建文件
    std::string createFilename() {
        time_t time = wyllog::date::now();
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
        ss << ".log";
        return ss.str();
    }

private:
    std::string _basename;  // 基础文件名
    std::ofstream _ofs;
    size_t _cur_gap;   // 当前所在段
    size_t _gap_size;  // 一个段的大小
};

int main() {
    wyllog::LogMsg msg(wyllog::LogLevel::Value::DEBUG, "root", "main.c", 18,
                       "一切正常");
    wyllog::Formatter fomatter("xyzxyz%%abc[%d{%H:%M:%S}] %m%n");
    std::string str = fomatter.format(msg);
    wyllog::LogSink::ptr std_lsp =
        wyllog::SinkFactory::create<wyllog::StdoutSink>();
    // wyllog::LogSink::ptr file_lsp =
    //     wyllog::SinkFactory::create<wyllog::FileSink>("./logfile/test.log");
    // wyllog::LogSink::ptr roll_lsp =
    //     wyllog::SinkFactory::create<wyllog::RollSinkBySize>("./logs/roll.log",
    //                                                         1024 * 1024);
    wyllog::LogSink::ptr time_lsp = wyllog::SinkFactory::create<RollSinkByTime>(
        "./logs/roll.log", TimeGap::GAP_SECOND);
    time_t start = wyllog::date::now();
    while (wyllog::date::now() - 5 < start) {
        time_lsp->log(str.c_str(), str.size());
    }
    // std_lsp->log(str.c_str(), str.size());
    // file_lsp->log(str.c_str(), str.size());
    // size_t cur_size = 0;
    // size_t count = 0;
    // while (cur_size < 1024 * 1024 * 10) {
    //     std::string tmp = str + " - " + std::to_string(count++);
    //     roll_lsp->log(tmp.c_str(), tmp.size());
    //     cur_size += tmp.size();
    // }

    return 0;
}
