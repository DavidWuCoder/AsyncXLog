#include <iostream>

#include "util.hpp"

int main() {
    std::cout << wyllog::date::now() << std::endl;
    std::string pathname = "./abc/def/gh";
    const std::string path = wyllog::file::path(pathname);
    std::cout << path << std::endl;
    std::cout << "是否存在: " << wyllog::file::exists(path) << std::endl;
    wyllog::file::createDirectory(path);
    std::cout << "是否存在: " << wyllog::file::exists(path) << std::endl;
    return 0;
}
