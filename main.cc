#include "pico/stdlib.h"
#include "log.h"
#include "petal_matrix.h"

int main() {
    stdio_init_all();
    PetalMatrix pm;
    pm.Init();
    while (true) {
        for (size_t i = 0; i < 8; i++) {
            for (size_t j = 0; j < 10; j++) {
                if (j < 7) {
                    pm.LedState(i, j, true);
                }
                if (j >= 3) {
                    pm.LedState(i, j - 3, false);
                }
                sleep_ms(100);
            }
        }
    }
}
