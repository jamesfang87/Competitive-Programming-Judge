#include <iostream>
#include <stdlib.h>
#include <thread>
int main() {
    using std::operator""s;
    std::this_thread::sleep_for(2s);
    std::cout << 1;
    return 0;
}
