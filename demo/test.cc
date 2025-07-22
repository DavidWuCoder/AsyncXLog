// 基本输出功能示例

#include "../logs/wlog.h"

void test_root_logger() {
    // 直接使用全局接口，默认打印到标准输出
    const std::string str = "测试日志";
    DEBUG("%d %s", 1, str.c_str());
    INFO("%d %s", 2, str.c_str());
    WARNING("%d %s", 3, str.c_str());
    ERROR("%d %s", 4, str.c_str());
    FATAL("%d %s", 5, str.c_str());
}

// 按照配置的选项进行输出
void test_log(const std::string logger_name) {
    wlog::Logger::ptr logger =
        wlog::LoggerManager::getInstance().getLogger(logger_name);
    const std::string str = "测试日志";
    logger->debug("%d %s", 1, str.c_str());
    logger->info("%d %s", 2, str.c_str());
    logger->warning("%d %s", 3, str.c_str());
    logger->error("%d %s", 4, str.c_str());
    logger->fatal("%d %s", 5, str.c_str());
}

int main() {
    // 默认日志输出
    test_root_logger();

    std::unique_ptr<wlog::LoggerBuilder> builder =
        std::make_unique<wlog::GlobalLoggerBuilder>();

    // 配置选项
    builder->buildName("async_logger");
    // 限制日志输出等级
    builder->buildLimitLevel(wlog::LogLevel::Value::WARNING);
    // 选择日志器类型（同步、异步）
    builder->buildType(wlog::LoggerType::ASYNC);
    // 日志的格式化字符串
    builder->buildFommatter("[%c][%p][%f:%l]%m%n");
    // 三种落地方向
    // 1. 标准输出
    builder->buildSink<wlog::StdoutSink>();
    // 2. 写入文件
    builder->buildSink<wlog::FileSink>("./logfile/test.log");
    // 3. 按大小滚动文件（达到文件大小则切换文件）
    builder->buildSink<wlog::RollSinkBySize>("./logs/roll.log", 1024 * 1024);

    builder->build();

    test_log("async_logger");

    return 0;
}
