#ifndef _LIFE_H_
#define _LIFE_H_

#include <stdlib.h>
#include <memory.h>
#include "pico/stdlib.h"

#define AGE_THRESHOLD       4
#define MAX_DENSITY         0.075f
#define CC_SPAWN_HYSTERISIS 0.0125f
#define CC_KILL_HYSTERISIS  0.05f
#define CC_MAX_DELTA        0.25f
#define CC_MAX_DELAY        32.0f
#define CC_SUDEATH_AGE      4
#define CC_SUDEATH_CHANCE   100
#define CC_OLDEATH_AGE      200
#define CC_OLDEATH_CHANCE   750


typedef union {
    struct {
        bool alive      : 1;
        bool last       : 1;
        bool spawn      : 1;
        bool kill       : 1;
        uint neighbors  : 4;
        uint age        : 8;
    };

    uint16_t value;
} cell_t;


class Life
{
    public:
        uint16_t size;
        uint16_t width;
        uint16_t height;
        uint32_t cell_count;
        uint32_t gen_count;
        uint32_t average_age;
        float activity;
        float density;
        cell_t* world;

        ~Life(void);
        Life(
            uint16_t width,
            uint16_t height
        );

        void reset();
        void update();
        void random();

        void addCell(uint16_t x, uint16_t y);
        void subCell(uint16_t x, uint16_t y);
        void addNeighbor(uint16_t x, uint16_t y);
        void subNeighbor(uint16_t x, uint16_t y);

        void addPattern(bool* pattern, uint8_t sx, uint8_t sy, uint16_t x, uint16_t y, bool flipx = false, bool flipy = false, bool inv = false);

        void addGlider(uint16_t x, uint16_t y, bool flipx = false, bool flipy = false);
        void addRandomGliderAnywhere();
        void addRandomGliderEdge();

        void cellControl(float target);
};

#endif