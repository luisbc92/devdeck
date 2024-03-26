#include "life.h"


bool GLIDER[3][3] = {
    {0, 1, 0},
    {0, 0, 1},
    {1, 1, 1}
};

bool CROSS[3][3] = {
    {0, 1, 0},
    {1, 1, 1},
    {0, 1, 0}
};


Life::Life(uint16_t width, uint16_t height) : width(width), height(height)
{
    world = (cell_t*) malloc(width * height * sizeof(cell_t));
    size = width * height;
    reset();
};


Life::~Life()
{
    if (world) free(world);
}


void Life::reset()
{
    memset(world, 0x0000, width * height * sizeof(cell_t));
    average_age = 0;
    cell_count = 0;
    gen_count = 0;
}


void Life::random()
{
    reset();
    for (int i=0; i < width * height; i++) {
        world[i].spawn = std::rand() % 2;
    }
}


void Life::addCell(uint16_t x, uint16_t y)
{
    uint16_t idx = y * width + x;
    if (!(world[idx].alive || world[idx].spawn))
        world[idx].spawn = true;
}


void Life::subCell(uint16_t x, uint16_t y)
{
    uint16_t idx = y * width + x;
    if (world[idx].alive)
        world[idx].kill = true;
}


void Life::addNeighbor(uint16_t x, uint16_t y)
{
    for (int yi = -1; yi <= 1; yi++) {
        for (int xi = -1; xi <= 1; xi++) {
            int16_t tx = x + xi;
            int16_t ty = y + yi;
            if (!((tx == x) && (ty == y))) {
                if (tx < 0) tx = width - 1;
                if (ty < 0) ty = height - 1;
                if (tx >= width) tx = 0;
                if (ty >= height) ty = 0;
                world[ty * width + tx].neighbors++;
            }
        }
    }
}


void Life::subNeighbor(uint16_t x, uint16_t y)
{
    for (int yi = -1; yi <= 1; yi++) {
        for (int xi = -1; xi <= 1; xi++) {
            int16_t tx = x + xi;
            int16_t ty = y + yi;
            if (!((tx == x) && (ty == y))) {
                if (tx < 0) tx = width - 1;
                if (ty < 0) ty = height - 1;
                if (tx >= width) tx = 0;
                if (ty >= height) ty = 0;
                world[ty * width + tx].neighbors--;
            }
        }
    }
}


void Life::update()
{
    cell_t* cell;
    uint16_t x, y;
    uint32_t average_count;

    // deferred state changes
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            cell = &world[y * width + x];
            if (cell->spawn) {                  // if marked for spawn
                addNeighbor(x, y);              // add to its neighbors
                cell->alive = true;             // set to alive
                cell->spawn = false;            // clear spawn
                cell->age = 0;                  // reset age
                cell_count++;                   // increase cell count
            } else
            if (cell->kill) {                   // if marked for kill
                subNeighbor(x, y);              // sub from its neighbors
                cell->alive = false;            // set to dead
                cell->kill = false;             // clear kill marker
                cell_count--;                   // decrease cell count
            }

        }
    }

    average_count = 0;
    average_age = 0;

    // check game of life rules
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            cell = &world[y * width + x];
            // if cell is active
            if (cell->value > 0) {
                if (cell->alive) {
                    // cell dies without 2-3 neighbors
                    if ((cell->neighbors != 2) && (cell->neighbors != 3)) {
                        cell->kill = true;

                    } else {
                        // increase age, without overflowing
                        if (cell->age < 255) cell->age++;

                        // count average age (above threshold)
                        if (cell->age > AGE_THRESHOLD) {
                            average_age += cell->age;
                            average_count++;
                        }
                    }
                } else {
                    if (cell->neighbors == 3) {
                        cell->spawn = true;
                    }
                }
            }
        }
    }

    // average age of cells above age threshold
    average_age /= average_count;

    // cell density relative to size of world
    density = (float)cell_count / (float)(width * height);

    // activity estimated around average age of living cells and potential for growth
    activity = cell_count > 0 ? (1.0f - (average_age / 255.0f)) * (density / MAX_DENSITY) : 0.0f;

    gen_count++;
}


void Life::addPattern(bool* pattern, uint8_t sx, uint8_t sy, uint16_t x, uint16_t y, bool flipx, bool flipy, bool inv)
{
    uint16_t i, ii, j, jj;
    for (int j = 0; j < sy; j++) {
        jj = flipy ? sy - 1 - j : j;
        for (int i = 0; i < sx; i++) {
            ii = flipx ? sx - 1 - i : i;
            if (pattern[jj * sy + ii])
                if (!inv)
                    addCell(x + i, y + j);
                else
                    subCell(x + i, y + j);
        }
    }
}


void Life::addGlider(uint16_t x, uint16_t y, bool flipx, bool flipy)
{
    addPattern((bool*)GLIDER, 3, 3, x, y, flipx, flipy);
}


void Life::addRandomGliderAnywhere()
{
    int x = rand() % (width - 3);
    int y = rand() % (height - 3);
    bool flipx = rand() % 2;
    bool flipy = rand() % 2;
    addGlider(x, y, flipx, flipy);
}


void Life::addRandomGliderEdge()
{
    int edge = rand() % 4;
    int x = rand() % (width - 3);
    int y = rand() % (height - 3);
    bool dir = rand() % 2;
    
    switch (edge) {
        case 0: // top
            addGlider(x, 0, dir, true);
            break;

        case 1: // bottom
            addGlider(x, height - 3, dir, false);
            break;

        case 2: // left
            addGlider(0, y, true, dir);
            break;

        case 3: // right
            addGlider(width - 3, y, false, dir);
            break;
    }
}


void Life::cellControl(float target)
{
    float delta = target - activity;
    
    // promote growth
    if (delta > CC_SPAWN_HYSTERISIS) {
        float f = (CC_MAX_DELTA - delta) * (1.0f / CC_MAX_DELTA);
        int freq = (int)(f * f * CC_MAX_DELAY);

        if (f > 0.0f && gen_count % freq == 0)
            addRandomGliderAnywhere();

        if (f < 0.0f)
            while (freq--)
                addRandomGliderAnywhere();
    }

    for (int i=0; i<size; i++) {
        cell_t *cell = &world[i];
        if (cell->alive) {
            // young sudden death syndrome
            if (delta < -CC_KILL_HYSTERISIS && cell->age < CC_SUDEATH_AGE && rand() % CC_SUDEATH_CHANCE == 0)
                cell->kill = true;

            // old age death
            if (cell->age > CC_OLDEATH_AGE && rand() % CC_OLDEATH_CHANCE == 0) {
                uint16_t x = i % width;
                uint16_t y = i / width;
                if (x > 1 && y > 1 && x < width - 1 && y < height - 1)
                    addPattern((bool*)CROSS, 3, 3, x, y, false, false, true);
            }
        }
    }
}
