#include <memory>
#include <string>

#include "format.hpp"
#include "level.hpp"
#include "logger.hpp"
#include "sink.hpp"
#include "util.hpp"

int main() {
    std::unique_ptr<wlog::LoggerBuilder> builder =
        std::make_unique<wlog::LocalLoggerBuilder>();
    builder->buildName("sync_logger");
    builder->buildLimitLevel(wlog::LogLevel::Value::WARNING);
    builder->buildType(wlog::LoggerType::SYNC);
    builder->buildFommatter("%p%m%n");
    builder->buildSink<wlog::StdoutSink>();
    builder->buildSink<wlog::FileSink>("./logfile/test.log");
    builder->buildSink<wlog::RollSinkBySize>("./logs/roll.log", 1024 * 1024);

    wlog::SyncLogger::ptr logger = builder->build();

    const std::string str = "测试日志-";
    logger->debug(__FILE__, __LINE__, "%d %s", 1, str.c_str());
    logger->info(__FILE__, __LINE__, "%d %s", 1, str.c_str());
    logger->warn(__FILE__, __LINE__, "%d %s", 1, str.c_str());
    logger->error(__FILE__, __LINE__, "%d %s", 1, str.c_str());
    logger->fatal(__FILE__, __LINE__, "%d %s", 1, str.c_str());

    size_t cur_size = 0;
    size_t count = 0;
    while (cur_size < 1024 * 1024 * 10) {
        std::string tmp = str + " - " + std::to_string(count++);
        logger->fatal(__FILE__, __LINE__, "%d %s", 1, tmp.c_str());
        cur_size += 30;
    }

    return 0;
}
