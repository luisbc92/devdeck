#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "smd.h"
#include "life.h"

#define SCALE 2


int main()
{
    std::srand(6345);
    SharpMemoryDisplay *display = new SharpMemoryDisplay();
    Life *life = new Life(display->width / SCALE, display->height / SCALE);

    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    while (true) {
        display->fill(BLACK);
        life->update();

        for (int y = 0; y < life->height; y++) {
            for (int x = 0; x < life->width; x++) {
                cell_t cell = life->world[y * life->width + x];
                if (cell.alive)
                    for (int i = 0; i < SCALE; i++) {
                        for (int j = 0; j < SCALE; j++) {
                            display->pixel(x*SCALE+i, y*SCALE+j, WHITE);
                        }
                    }
            }
        }


        float target = (float)adc_read() / (float)(1 << 12);
        life->cellControl(target);

        display->refresh();

        printf("D: %0.3f C: %d A: %0.2f -> %0.2f\n", life->density, life->cell_count, life->gen_count, life->activity, target);
    }

    return 0;
}
