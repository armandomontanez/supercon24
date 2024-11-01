#include "pico/stdlib.h"
#include "log.h"

int main() {
    stdio_init_all();
    while (true) {
        LOG(DEBUG, "Hello Supercon 2024!");
        sleep_ms(500);
    }
}
