#include "raycast.h"
#include "utils.h"
#include <iostream>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cassert>

int wall_x_texcoord(const float x, const float y, Texture &tex_walls) {
    float hitx = x - floor(x+.5); // hitx and hity contain (signed) fractional parts of x and y,
    float hity = y - floor(y+.5); // they vary between -0.5 and +0.5, and one of them is supposed to be very close to 0
    int tex = hitx*tex_walls.size;
    if (std::abs(hity)>std::abs(hitx)) // we need to determine whether we hit a "vertical" or a "horizontal" wall (w.r.t the map)
        tex = hity*tex_walls.size;
    if (tex<0) // do not forget x_texcoord can be negative, fix that
        tex += tex_walls.size;
    assert(tex>=0 && tex<(int)tex_walls.size);
    return tex;
}

void map_show_sprite(Sprite &sprite,FrameBuffer &fb,Map &map){
    const size_t rect_w = fb.w/(map.w*2);
    const size_t rect_h = fb.h/map.h;
    fb.draw_rectangle(sprite.x*rect_w-3, sprite.y*rect_h-3, 6, 6, pack_color(255, 0, 0));
}

void draw_sprite(Sprite &sprite, std::vector<float>& depth_buffer, FrameBuffer &fb, Player &player, Texture &tex_sprites) {
    // absolute direction from the player to the sprite (in radians)
    float sprite_dir = atan2(sprite.y - player.y, sprite.x - player.x);
    // remove unnecessary periods from the relative direction
    while (sprite_dir - player.a >  M_PI) sprite_dir -= 2*M_PI; 
    while (sprite_dir - player.a < -M_PI) sprite_dir += 2*M_PI;

    // distance from the player to the sprite
    assert(sprite.distance_to_player>0);
    size_t sprite_screen_size = std::min(2000, static_cast<int>(fb.h/sprite.distance_to_player));
    // do not forget the 3D view takes only a half of the framebuffer, thus fb.w/2 for the screen width
    int h_offset = (sprite_dir - player.a)*(fb.w/2)/(player.fov) + (fb.w/2)/2 - sprite_screen_size/2;
    int v_offset = fb.h/2 - sprite_screen_size/2;

    for (size_t i=0; i<sprite_screen_size; i++) {
        if (h_offset+int(i)<0 || h_offset+i>=fb.w/2) continue;
        if (depth_buffer[h_offset+i]<sprite.distance_to_player) continue; // this sprite column is behind the wall
        for (size_t j=0; j<sprite_screen_size; j++) {
            if (v_offset+int(j)<0 || v_offset+j>=fb.h) continue;
            uint32_t color = tex_sprites.get(i*tex_sprites.size/sprite_screen_size, j*tex_sprites.size/sprite_screen_size, sprite.tex_id);
            uint8_t r,g,b,a;
            unpack_color(color, r, g, b, a);
            if (a>128)
            fb.set_pixel(fb.w/2 + h_offset+i, v_offset+j, color);
        }
    }
}

void render(FrameBuffer &fb, Map &map, Player &player, Texture &tex_walls,Texture &tex_monst, std::vector<Sprite> &sprites) {
    fb.clear(pack_color(255, 255, 255)); // clear the screen
    const size_t rect_w = fb.w/(map.w*2); // size of one map cell on the screen
    const size_t rect_h = fb.h/map.h;
    for (size_t j=0; j<map.h; j++) {  // draw the map
        for (size_t i=0; i<map.w; i++) {
            if (map.is_empty(i, j)) continue; // skip empty spaces
            size_t rect_x = i*rect_w;
            size_t rect_y = j*rect_h;
            size_t texid = map.get(i, j);
            assert(texid<tex_walls.count);
            fb.draw_rectangle(rect_x, rect_y, rect_w, rect_h, tex_walls.get(0, 0, texid)); // the color is taken from the upper left pixel of the texture #texid
        }
    }
    std::vector<float> depth_buffer(fb.w/2, 1e3); // the depth buffer is initialized with large values
    for (size_t i=0; i<fb.w/2; i++) { // draw the visibility cone AND the "3D" view
        float angle = player.a-player.fov/2 + player.fov*i/float(fb.w/2);
        for (float t=0; t<20; t+=.01) { // ray marching loop
            float x = player.x + t*cos(angle);
            float y = player.y + t*sin(angle);
            fb.set_pixel(x*rect_w, y*rect_h, pack_color(160, 160, 160)); // this draws the visibility cone

            if (map.is_empty(x, y)) continue;

            size_t texid = map.get(x, y); // our ray touches a wall, so draw the vertical column to create an illusion of 3D
            assert(texid<tex_walls.count);
            float dist = t*cos(angle-player.a);
            size_t column_height = fb.h/dist;
            depth_buffer[i] = dist;
            int x_texcoord = wall_x_texcoord(x, y, tex_walls);
            std::vector<uint32_t> column = tex_walls.get_scaled_column(texid, x_texcoord, column_height);
            int pix_x = i + fb.w/2; // we are drawing at the right half of the screen, thus +fb.w/2
            for (size_t j=0; j<column_height; j++) { // copy the texture column to the framebuffer
                int pix_y = j + fb.h/2 - column_height/2;
                if (pix_y>=0 && pix_y<(int)fb.h) {
                    fb.set_pixel(pix_x, pix_y, column[j]);
                }
            }
            break;
        } // ray marching loop
    } // field of view ray sweeping

    // for(size_t i=0;i<sprites.size();++i){
    //     sprites[i].distance_to_player = sqrt((player.x-sprites[i].x)*(player.x-sprites[i].x) + (player.y-sprites[i].y)*(player.y-sprites[i].y));
    // }
    // std::sort(sprites.rbegin(), sprites.rend());
    for (size_t i=0; i<sprites.size(); i++) {
        map_show_sprite(sprites[i],fb,map);
        draw_sprite(sprites[i],depth_buffer , fb, player, tex_monst);
    }
}
/*
int main() {
    FrameBuffer fb{1024, 512, std::vector<uint32_t>(1024*512, pack_color(255, 255, 255))};
    Player player{3.456, 2.345, 1.523, M_PI/3.};
    Map map;
    Texture tex_walls("../walltext.png");
    Texture tex_monst("../monsters.png");
    if (!tex_walls.count||!tex_monst.count) {
        std::cerr << "Failed to load textures" << std::endl;
        return -1;
    }
    std::vector<Sprite> sprites{{3.523, 3.812, 2,0},{1.834, 8.765, 0,0}, {5.323, 5.365, 1,0}, {4.123, 10.265, 1,0} };

    // for (size_t frame=0; frame<1; frame++) {
    //     std::stringstream ss;
    //     ss << std::setfill('0') << std::setw(5) << frame << ".ppm";
    //     player.a += 2*M_PI/360;
    //     render(fb, map, player, tex_walls, tex_monst, sprites);
    //     drop_ppm_image(ss.str(), fb.img, fb.w, fb.h);
    // }

    render(fb, map, player, tex_walls, tex_monst, sprites);
    drop_ppm_image("out.ppm", fb.img, fb.w, fb.h);
    std::cout<<"Done"<<std::endl;
    return 0;
}

*/