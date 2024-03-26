#ifndef _SHARP_H_
#define _SHARP_H_

#include <stdlib.h>
#include <memory.h>
#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "smd.pio.h"


#define BLACK   0
#define WHITE   1
#define INVERSE 2

#define SHARPMEMORYDISPLAY_WRITECMD     0x01
#define SHARPMEMORYDISPLAY_VCOM         0x02
#define SHARPMEMORYDISPLAY_CLEAR        0x04

class SharpMemoryDisplay
{
    private:
        /* framebuffer */
        uint16_t fbsize;
        uint8_t *fb;

        /* hardware */
        pio_hw_t *pio;
        uint sm;
        int write(const uint8_t *src, size_t len);

        uint8_t cs_pin;
        uint8_t vcom = SHARPMEMORYDISPLAY_VCOM;

    public:
        ~SharpMemoryDisplay(void);
        SharpMemoryDisplay(
            uint16_t w = 400,
            uint16_t h = 240,
            pio_hw_t *pio = pio0,
            uint8_t clk_pin = 14,
            uint8_t do_pin = 15,
            uint8_t cs_pin = 13
        );

        uint16_t width, height;

        void fill(uint8_t color);
        void pixel(int16_t x, int16_t y, uint8_t color);
        void hline(int16_t x, int16_t y, int16_t w, uint8_t color);
        void vline(int16_t x, int16_t y, int16_t h, uint8_t color);
        bool getPixel(int16_t x, int16_t y);
        bool* getBuffer(void);
        void clear(void);
        void refresh(void);

        void clearDisplay(void);
};


#endif
