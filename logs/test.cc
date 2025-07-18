#include <memory>
#include <string>

#include "buffer.hpp"
#include "format.hpp"
#include "level.hpp"
#include "logger.hpp"
#include "sink.hpp"
#include "util.hpp"

int main() {
    // std::ifstream ifs;
    // ifs.open("./logfile/test.log", std::ios::binary);
    // if (!ifs.is_open()) {
    //     std::cout << "open_error" << std::endl;
    //     return 0;
    // }
    // ifs.seekg(0, std::ios::end);
    // size_t fsize = ifs.tellg();
    // ifs.seekg(0, std::ios::beg);
    // std::string body;
    // body.resize(fsize);
    // ifs.read(&body[0], fsize);
    //
    // ifs.close();
    // std::ofstream ofs;
    // ofs.open("./logfile/tmp.log", std::ios::binary);
    // if (!ofs.is_open()) {
    //     std::cout << "open error" << std::endl;
    // }
    //
    // wlog::Buffer buffer;
    // for (int i = 0; i < body.size(); i++) {
    //     buffer.push(&body[i], 1);
    // }
    // int read_size = buffer.readableSize();
    // for (int i = 0; i < read_size; i++) {
    //     ofs.write(buffer.begin(), 1);
    //     buffer.moveReader(1);
    // }
    // ofs.close();

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
