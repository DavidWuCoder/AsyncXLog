#include <iostream>

void xprintf() { std::cout << std::endl; }

template <class T, class... Args>
void xprintf(const T &v, Args &&...args) {
    std::cout << v;
    if (sizeof...(args) > 0) {
        xprintf(std::forward<Args>(args)...);
    } else {
        xprintf();
    }
}

int main() {
    xprintf("hello ");
    xprintf("hello ", "world");
    xprintf("hello ", "world", 123);
    return 0;
}
