#include <iostream>

// 饿汉方式
// class Singleton {
// private:
//     static Singleton _eton;
//     int _data;
//     Singleton() : _data(0) {}
//     ~Singleton() {}
//
//     Singleton(const Singleton&) = delete;
//     const Singleton& operator=(const Singleton&) = delete;
//
// public:
//     static Singleton& getInstance() { return _eton; }
//     int getData() { return _data; }
// };
//
// Singleton Singleton::_eton;

// 懒汉方式
// 自c++11之后可行

class Singleton {
private:
    int _data;

private:
    Singleton() : _data(0) { std::cout << "构造单例对象" << std::endl; }
    ~Singleton() {}

    Singleton(const Singleton&) = delete;
    const Singleton& operator=(const Singleton&) = delete;

public:
    static Singleton& getInstance() {
        static Singleton _eton;
        return _eton;
    }
    int getData() { return _data; }
};

int main() {
    std::cout << Singleton::getInstance().getData() << std::endl;
    return 0;
}
