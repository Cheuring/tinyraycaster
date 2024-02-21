#ifndef RAYCAST_H
#define RAYCAST_H
#include <vector>
#include "map.h"
#include "framebuffer.h"
#include "texture.h"
#include "player.h"
#include "sprite.h"

struct GameState {
    Map map;
    Player player;
    std::vector<Sprite> monsters;
    Texture tex_walls;
    Texture tex_monst;
};

void render(FrameBuffer &fb, Map &map, Player &player, Texture &tex_walls,Texture &tex_monst, std::vector<Sprite> &sprites);

#endif