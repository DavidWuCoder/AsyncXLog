// 工具类
//  1.获取当前时间戳
//  2.检查文件是否存在
//  3.获取目录
//  4.创建目录
#pragma once
#include <endian.h>
#include <sys/stat.h>

#include <ctime>
#include <string>

namespace wyllog {
class date {
public:
    static size_t now() { return (size_t)time(nullptr); }
};
class file {
public:
    static bool exists(const std::string &pathname) {
        struct stat st;
        return stat(pathname.c_str(), &st) == 0;
    }
    static std::string path(const std::string &name) {
        if (name.empty()) return ".";
        size_t pos = name.find_last_of("/\\");
        if (pos == std::string::npos) return ".";
        return name.substr(0, pos + 1);
    }
    static void createDirectory(const std::string &path) {
        if (path.empty()) return;
        if (exists(path)) return;
        size_t pos, idx = 0;
        while (idx < path.size()) {
            pos = path.find_first_of("/\\", idx);
            if (pos == std::string::npos) {
                mkdir(path.c_str(), 0755);
                return;
            }
            std::string subdir = path.substr(0, pos + 1);
            if (exists(subdir)) {
                idx = pos + 1;
                continue;
            }
            mkdir(subdir.c_str(), 0755);
            idx = pos + 1;
        }
    }
};
}  // namespace wyllog
