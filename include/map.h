#ifndef MAP_H
#define MAP_H

#include <cstdlib>

struct Map
{
    size_t w, h;
    Map();
    int get(const size_t x, const size_t y) const;
    bool is_empty(const size_t x, const size_t y) const;
};

#endif
