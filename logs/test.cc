#include <iostream>
#include <string>

#include "format.hpp"
#include "level.hpp"
#include "sink.hpp"
#include "util.hpp"

int main() {
    wyllog::LogMsg msg(wyllog::LogLevel::Value::DEBUG, "root", "main.c", 18,
                       "一切正常");
    wyllog::Formatter fomatter("xyzxyz%%abc[%d{%H:%M:%S}] %m%n");
    std::string str = fomatter.format(msg);
    wyllog::LogSink::ptr std_lsp =
        wyllog::SinkFactory::create<wyllog::StdoutSink>();
    wyllog::LogSink::ptr file_lsp =
        wyllog::SinkFactory::create<wyllog::FileSink>("./logfile/test.log");
    wyllog::LogSink::ptr roll_lsp =
        wyllog::SinkFactory::create<wyllog::RollSinkBySize>("./logs/roll.log",
                                                            1024 * 1024);
    std_lsp->log(str.c_str(), str.size());
    file_lsp->log(str.c_str(), str.size());
    size_t cur_size = 0;
    size_t count = 0;
    while (cur_size < 1024 * 1024 * 10) {
        std::string tmp = str + " - " + std::to_string(count++);
        roll_lsp->log(tmp.c_str(), tmp.size());
        cur_size += tmp.size();
    }

    return 0;
}
