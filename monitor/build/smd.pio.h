// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// --- //
// smd //
// --- //

#define smd_wrap_target 0
#define smd_wrap 1

static const uint16_t smd_program_instructions[] = {
            //     .wrap_target
    0x6001, //  0: out    pins, 1         side 0     
    0xb042, //  1: nop                    side 1     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program smd_program = {
    .instructions = smd_program_instructions,
    .length = 2,
    .origin = -1,
};

static inline pio_sm_config smd_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + smd_wrap_target, offset + smd_wrap);
    sm_config_set_sideset(&c, 1, false, false);
    return c;
}

    static inline void smd_program_init(PIO pio, uint sm, uint offset, uint clk_pin, uint data_pin, float clk_div)
    {
        pio_gpio_init(pio, data_pin);
        pio_gpio_init(pio, clk_pin);
        pio_sm_set_consecutive_pindirs(pio, sm, data_pin, 1, true);
        pio_sm_set_consecutive_pindirs(pio, sm, clk_pin, 1, true);
        pio_sm_config c = smd_program_get_default_config(offset);
        sm_config_set_sideset_pins(&c, clk_pin);
        sm_config_set_out_pins(&c, data_pin, 1);
        sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
        sm_config_set_clkdiv(&c, clk_div);
        sm_config_set_out_shift(&c, true, true, 8);     // we need LSB
        pio_sm_init(pio, sm, offset, &c);
        pio_sm_set_enabled(pio, sm, true);
    }
    static inline void smd_put(PIO pio, uint sm, uint8_t x)
    {
        pio_sm_put_blocking(pio, sm, x);
    }

#endif

