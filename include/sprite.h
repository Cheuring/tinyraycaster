#ifndef SPRITE_H
#define SPRITE_H

#include <cstdlib>
struct Sprite{
    float x,y;
    size_t tex_id;
    float distance_to_player;

    bool operator<(const Sprite &other) const {
        return distance_to_player < other.distance_to_player;
    }
};

#endif