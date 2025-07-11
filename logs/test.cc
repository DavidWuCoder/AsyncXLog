#include <iostream>

#include "level.hpp"
#include "util.hpp"

int main() {
    // std::cout << wyllog::date::now() << std::endl;
    // std::string pathname = "./abc/def/gh";
    // const std::string path = wyllog::file::path(pathname);
    // std::cout << path << std::endl;
    // std::cout << "是否存在: " << wyllog::file::exists(path) << std::endl;
    // wyllog::file::createDirectory(path);
    // std::cout << "是否存在: " << wyllog::file::exists(path) << std::endl;
    std::cout << wyllog::LogLevel::toString(wyllog::LogLevel::Value::DEBUG)
              << std::endl;
    std::cout << wyllog::LogLevel::toString(wyllog::LogLevel::Value::INFO)
              << std::endl;
    std::cout << wyllog::LogLevel::toString(wyllog::LogLevel::Value::ERROR)
              << std::endl;
    std::cout << wyllog::LogLevel::toString(wyllog::LogLevel::Value::WARNING)
              << std::endl;
    std::cout << wyllog::LogLevel::toString(wyllog::LogLevel::Value::FATAL)
              << std::endl;
    std::cout << wyllog::LogLevel::toString(wyllog::LogLevel::Value::OFF)
              << std::endl;
    return 0;
}
