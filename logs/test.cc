#include <string>

#include "format.hpp"
#include "level.hpp"
#include "logger.hpp"
#include "sink.hpp"
#include "util.hpp"

int main() {
    std::string logger_name = "sync_logger";
    wlog::LogLevel::Value limit_level = wlog::LogLevel::Value::WARNING;
    wlog::Formatter::ptr fomatter = std::make_shared<wlog::Formatter>();
    wlog::LogSink::ptr std_lsp = wlog::SinkFactory::create<wlog::StdoutSink>();
    wlog::LogSink::ptr file_lsp =
        wlog::SinkFactory::create<wlog::FileSink>("./logfile/test.log");
    wlog::LogSink::ptr roll_lsp =
        wlog::SinkFactory::create<wlog::RollSinkBySize>("./logs/roll.log",
                                                        1024 * 1024);
    std::vector<wlog::LogSink::ptr> sinks = {std_lsp, file_lsp, roll_lsp};
    wlog::SyncLogger logger(logger_name, limit_level, fomatter, sinks);

    const std::string str = "测试日志-";
    logger.debug(__FILE__, __LINE__, "%d %s", 1, str.c_str());
    logger.info(__FILE__, __LINE__, "%d %s", 1, str.c_str());
    logger.warn(__FILE__, __LINE__, "%d %s", 1, str.c_str());
    logger.error(__FILE__, __LINE__, "%d %s", 1, str.c_str());
    logger.fatal(__FILE__, __LINE__, "%d %s", 1, str.c_str());

    size_t cur_size = 0;
    size_t count = 0;
    while (cur_size < 1024 * 1024 * 10) {
        std::string tmp = str + " - " + std::to_string(count++);
        logger.fatal(__FILE__, __LINE__, "%d %s", 1, tmp.c_str());
        cur_size += 60;
    }

    return 0;
}
