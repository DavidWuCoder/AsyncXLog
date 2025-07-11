#include <iostream>

#include "format.hpp"
#include "level.hpp"
#include "util.hpp"

int main() {
    wyllog::LogMsg msg(wyllog::LogLevel::Value::DEBUG, "root", "main.c", 18,
                       "一切正常");
    wyllog::Formatter fomatter("xyzxyz%%abc[%d{%H:%M:%S}] %m%n");
    std::string str = fomatter.format(msg);
    std::cout << str << std::endl;
    return 0;
}
