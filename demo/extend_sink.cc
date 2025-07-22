// 扩展一个输出方式：按照时间来滚动文件
#include <unistd.h>

#include "../logs/wlog.h"

enum class TimeGap { GAP_SECOND, GAP_MINUTE, GAP_HOUR, GAP_DAY };

class RollSinkByTime : public wlog::LogSink {
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
        wlog::file::createDirectory(wlog::file::path(_basename));
    }
    // 将日志消息写到指定文件
    void log(const char *data, size_t len) {
        initLogFile();
        _ofs.write(data, len);
        assert(_ofs.good());
    }

    size_t getCurGap() {
        return _gap_size == 1 ? wlog::date::now()
                              : wlog::date::now() % _gap_size;
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
        time_t time = wlog::date::now();
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
    std::unique_ptr<wlog::LoggerBuilder> builder =
        std::make_unique<wlog::GlobalLoggerBuilder>();
    builder->buildName("async_logger");
    builder->buildLimitLevel(wlog::LogLevel::Value::WARNING);
    builder->buildType(wlog::LoggerType::ASYNC);
    builder->buildFommatter("[%c][%p][%f:%l]%m%n");
    builder->enableUnsafeAsync();

    // 设置落地方式, 为1秒钟切换文件
    builder->buildSink<RollSinkByTime>("./logs/roll-sink-by-time.log",
                                       TimeGap::GAP_SECOND);

    builder->build();

    wlog::Logger::ptr logger = wlog::getLogger("async_logger");

    const std::string str = "测试日志";
    time_t start = wlog::date::now();
    while (wlog::date::now() - 5 < start) {
        logger->fatal(str.c_str());
        usleep(1000);
    }

    return 0;
}
