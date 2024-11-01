#include <string_view>
#include <cstring>

#include "pico/stdlib.h"

void LOG(std::string_view msg) {
    constexpr size_t kMaxLogSize = 256;
    char buf[kMaxLogSize + 1];
    const size_t log_length = std::min(msg.size(), kMaxLogSize);
    memcpy(buf, msg.data(), log_length);
    buf[log_length] = 0;
    stdio_puts(buf);
}

int main() {
    stdio_init_all();
    while (true) {
        LOG("Hello Supercon 2024!");
        sleep_ms(500);
    }
}
