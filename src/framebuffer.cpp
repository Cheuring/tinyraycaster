#include "framebuffer.h"

void FrameBuffer::clear(const uint32_t color) {
    std::fill(img.begin(), img.end(), color);
}

void FrameBuffer::set_pixel(const size_t x, const size_t y, const uint32_t color) {
    assert(img.size()==w*h&&x<w && y<h);
    img[x + y*w] = color;
}

void FrameBuffer::draw_rectangle(const size_t x, const size_t y, const size_t W, const size_t H, const uint32_t color) {
    assert(img.size()==w*h);
    for (size_t i=0; i<W; i++) {
        for (size_t j=0; j<H; j++) {
            size_t cx = x+i;
            size_t cy = y+j;
            if (cx>=w || cy>=h) continue; // no need to check for negative values (unsigned variables)
            img[cx + cy*w] = color;
        }
    }
}