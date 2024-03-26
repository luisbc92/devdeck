#include "smd.h"

#define TOGGLE_VCOM                                     \
    do {                                                \
        vcom = vcom ? 0x00 : SHARPMEMORYDISPLAY_VCOM;   \
    } while(0);                                         \


SharpMemoryDisplay::SharpMemoryDisplay(
    uint16_t w,
    uint16_t h,
    pio_hw_t* pio,
    uint8_t clk_pin,
    uint8_t do_pin,
    uint8_t cs_pin
) : width(w), height(h), pio(pio), cs_pin(cs_pin)
{
    // initialize hw
    uint offset = pio_add_program(pio, &smd_program);
    sm = pio_claim_unused_sm(pio, true);
    smd_program_init(pio, sm, offset, clk_pin, do_pin, 5.0);
    gpio_init(cs_pin);
    gpio_set_dir(cs_pin, GPIO_OUT);
    gpio_put(cs_pin, 0);

    // framebuffer
    fbsize = (width * height) / 8;
    fb = (uint8_t *) malloc(fbsize);

    // clear display
    clearDisplay();
    clear();
};


SharpMemoryDisplay::~SharpMemoryDisplay(void)
{
    if (fb) free(fb);
};


/* special implementation of spi_write_blocking that writes in lsb */
int SharpMemoryDisplay::write(const uint8_t *src, size_t len)
{
    for (int i = 0; i < len; ++i) {
        smd_put(pio, sm, src[i]);
    }

    return len;
};


void SharpMemoryDisplay::refresh(void)
{
    uint8_t data;
    uint16_t i, current_line;

    // enable display
    gpio_put(cs_pin, 1);

    // toggle vcom
    data = vcom | SHARPMEMORYDISPLAY_WRITECMD;
    TOGGLE_VCOM;
    write(&data, 1);

    uint8_t bytes_per_line = width / 8;
    uint16_t total_bytes = (width * height) / 8;

    for (i = 0; i < total_bytes; i+= bytes_per_line) {
        uint8_t line[bytes_per_line + 2];

        // send address byte
        current_line = ((i + 1) / (width / 8)) + 1;
        line[0] = current_line;

        // copy this line
        memcpy(line + 1, fb + i, bytes_per_line);

        // send end of line
        line[bytes_per_line + 1] = 0x00;

        // transfer
        write(line, bytes_per_line + 2);
    }

    // send trailing 8 bits for last line
    data = 0x00;
    write(&data, 1);
    gpio_put(cs_pin, 0);
    sleep_us(50);
};


void SharpMemoryDisplay::clearDisplay(void)
{
    gpio_put(cs_pin, 1);

    uint8_t data[2] = {vcom | SHARPMEMORYDISPLAY_CLEAR, 0x00};
    write(data, 2);
    TOGGLE_VCOM;

    gpio_put(cs_pin, 0);
};

void SharpMemoryDisplay::clear(void)
{
    memset(fb, 0xFF, fbsize);
};


void SharpMemoryDisplay::fill(uint8_t color)
{
    switch (color) {
        case WHITE:
            memset(fb, 0xFF, fbsize);
            break;
        case BLACK:
            memset(fb, 0x00, fbsize);
            break;
    }
};


void SharpMemoryDisplay::pixel(int16_t x, int16_t y, uint8_t color)
{
    if ((x >= 0) && (x < width) && (y >= 0) && (y < height)) {
        uint16_t i = (y * width + x) / 8; 
        switch (color) {
            case WHITE:
                fb[i] |= (1 << (x & 7));
                break;
            case BLACK:
                fb[i] &= ~(1 << (x & 7));
                break;
            case INVERSE:
                fb[i] ^= (1 << (x & 7));
                break;
        }
    }
}

void SharpMemoryDisplay::vline(int16_t x, int16_t y, int16_t h, uint8_t color)
{
    if ((x >= 0) && (x < width))
    {
        // clip left
        if (y < 0) {
            h += y;
            y = 0;
        }

        // clip right
        if ((y + h) > height) h = height - y;

        if (h > 0) {
            uint8_t *p = &fb[(y * width + x) / 8], mask = 1 << (x & 7);
            switch (color) {
                case WHITE:
                    while (h--) *(p += width / 8) |= mask;
                    break;
                case BLACK:
                    mask = ~mask;
                    while (h--) *(p += width / 8) &= mask;
                    break;
                case INVERSE:
                    while (h--) *(p += width / 8) ^= mask;
                    break;
            }
        }
    }
}

void SharpMemoryDisplay::hline(int16_t x, int16_t y, int16_t w, uint8_t color)
{
    if ((y >= 0) && (y < height))
    {
        // clip left
        if (x < 0) {
            w += x;
            x = 0;
        }

        // clip right
        if ((x + w) > width) {
            w = w - x;
        }

        if (w > 0) {
            uint8_t *p = &fb[(y * width + x) / 8];

            // first partial byte
            uint8_t mod = x & 7;
            if (mod) {
                // mask off the high n bits to set
                mod = 8 - mod;
                uint8_t mask = ~(0xFF >> mod);

                // adjust mask if not reaching end of this byte
                if (w < mod) mask &= (0xFF >> (mod - w));

                switch(color) {
                    case WHITE:
                        *p |= mask;
                        break;
                    case BLACK:
                        *p &= ~mask;
                        break;
                    case INVERSE:
                        *p ^= mask;
                        break;
                }
            p += 1;
            }

            // more to go?
            if (w >= mod) {
                w -= mod;

                // write solid bytes
                if (w >= 8) {
                    if (color == INVERSE) {
                        do {
                            *p ^= 0xFF;
                            p += 1;
                            w -= 8;
                        } while (w >= 8);
                    } else {
                        uint8_t val = (color != BLACK) ? 0xFF : 0;
                        do {
                            *p = val;
                            p += 1;
                            w -= 8;
                        } while (w >= 8);
                    }
                }

            }

            // final partial byte
            if (w) {
                mod = w & 7;
                uint8_t mask = (1 << mod) - 1;
                switch (color) {
                    case WHITE:
                        *p |= mask;
                        break;
                    case BLACK:
                        *p &= ~mask;
                        break;
                    case INVERSE:
                        *p ^= mask;
                        break;
                }
            }
        }
    }
}