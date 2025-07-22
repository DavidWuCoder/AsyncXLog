#include <chrono>
#include <thread>
#include <vector>

#include "../logs/wlog.h"

void bench(const std::string& logger_name, size_t thread_count,
           size_t msg_count, size_t msg_len) {
    // 1. 获取日志器
    wlog::Logger::ptr logger = wlog::getLogger(logger_name);
    if (logger.get() == nullptr) {
        std::cout << "找不到 " << logger_name << "日志器 " << std::endl;
        return;
    }
    std::cout << "测试日志：" << msg_count
              << " 条，总大小: " << msg_count * msg_len / 1024 << "KB"
              << std::endl;
    // 2. 创建指定长度的消息
    std::string msg(msg_len - 1, 'a');  // 留一位给\n
    // 3. 创建指定数量的线程
    std::vector<std::thread> threads;
    std::vector<double> time_array(thread_count);
    size_t msg_count_per_thread = msg_count / thread_count;
    for (int i = 0; i < thread_count; i++) {
        threads.emplace_back([&, i]() {
            // 4. 线程内部开始计数
            auto start = std::chrono::high_resolution_clock::now();
            // 5. 输出指定数量的消息;
            for (int j = 0; j < msg_count_per_thread; j++) {
                logger->debug("%s", msg.c_str());
            }
            // 6. 线程内部结束计数, 统计相关信息
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> cost = end - start;
            time_array[i] = cost.count();
            std::cout << "\t线程" << i << ": "
                      << "输出数量: " << msg_count_per_thread
                      << ", 耗时: " << cost.count() << "s" << std::endl;
        });
    }
    // 7. 等待线程退出
    for (auto& thread : threads) {
        thread.join();
    }
    // 8. 总耗时是所有线程耗时的最大值
    double cost_total = *max_element(time_array.begin(), time_array.end());
    long long msg_count_per_second = msg_count / cost_total;
    long long msg_size_per_second = (msg_count * msg_len) / (cost_total * 1024);

    std::cout << "\t总耗时: " << cost_total << "s" << std::endl;
    std::cout << "\t每秒输出日志数量：" << msg_count_per_second << "条"
              << std::endl;
    std::cout << "\t每秒输出日志大小：" << msg_size_per_second << "KB"
              << std::endl;
}

void sync_bench() {
    std::unique_ptr<wlog::LoggerBuilder> builder =
        std::make_unique<wlog::GlobalLoggerBuilder>();
    builder->buildName("sync_logger");
    builder->buildFommatter("%m%n");
    builder->buildType(wlog::LoggerType::SYNC);
    builder->buildSink<wlog::FileSink>("./logs/sync.log");
    builder->build();
    bench("sync_logger", 3, 5'000'000, 100);
}

void async_bench() {
    std::unique_ptr<wlog::LoggerBuilder> builder =
        std::make_unique<wlog::GlobalLoggerBuilder>();
    builder->buildName("async_logger");
    builder->buildFommatter("%m%n");
    builder->buildType(wlog::LoggerType::ASYNC);
    builder->enableUnsafeAsync();  // 将实际落地时间排除在外
    builder->buildSink<wlog::FileSink>("./logs/async.log");
    builder->build();
    bench("async_logger", 3, 5'000'000, 100);
}

int main() {
    // sync_bench();
    async_bench();
    return 0;
}
