#include "pico/stdlib.h"
#include "log.h"
#include "petal_matrix.h"

int main() {
    stdio_init_all();
    PetalMatrix pm;
    pm.Init();
    bool set_or_clear = true;
    while (true) {
        for (size_t i = 0; i < 8; i++) {
            for (size_t j = 0; j < 7; j++) {
                if (j < 7) {
                    pm.LedState(i, j, set_or_clear);
                }
                sleep_ms(10);
            }
        }
        set_or_clear = !set_or_clear;
    }
}
