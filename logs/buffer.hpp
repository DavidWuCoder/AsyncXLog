// 异步日志缓冲
#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

namespace wlog {
#define DEFAULT_BUFFER_SIZE (1 * 1024 * 1024)
#define THRESHOLD_BUFFER_SIZE (8 * 1024 * 1024)
#define INCREASEMENT_BUFFER_SIZE (1 * 1024 * 1024)

class Buffer {
public:
    Buffer() : _buffer(DEFAULT_BUFFER_SIZE), _writer_idx(0), _reader_idx(0) {}

    // 向缓冲区写入数据
    void push(const char *data, size_t len) {
        expandSize(len);

        std::copy(data, data + len, &_buffer[_writer_idx]);
        moveWriter(len);
    }

    // 返回可读数据的起始地址
    const char *begin() { return &_buffer[_reader_idx]; }

    // 返回可读数据长度
    size_t readableSize() { return _writer_idx - _reader_idx; }

    // 返回可写数据长度
    size_t writeableSize() { return _buffer.size() - _writer_idx; }

    // 移动读指针
    void moveReader(size_t len) {
        assert(len <= readableSize());
        _reader_idx += len;
    }

    // 重置读写位置
    void reset() {
        _reader_idx = 0;
        _writer_idx = 0;
    }

    // 交换
    void swap(Buffer &other) {
        // 交换管理空间
        _buffer.swap(other._buffer);
        // 交换读写指针
        std::swap(_reader_idx, other._reader_idx);
        std::swap(_writer_idx, other._writer_idx);
    }

    // 判断缓冲区是否为空
    bool empty() { return (_reader_idx == _writer_idx); }

private:
    // 移动写指针
    void moveWriter(size_t len) {
        assert(len <= writeableSize());
        _writer_idx += len;
    }

    void expandSize(size_t len) {
        if (writeableSize() >= len) return;
        if (writeableSize() < THRESHOLD_BUFFER_SIZE)
            _buffer.resize(_buffer.size() * 2);
        else {
            _buffer.resize(_buffer.size() + INCREASEMENT_BUFFER_SIZE);
        }
    }

private:
    std::vector<char> _buffer;
    size_t _reader_idx;
    size_t _writer_idx;
};

}  // namespace wlog
