#include <iostream>
#include <algorithm>
#include "minigl2d.hpp"

using namespace minigl;
using namespace glm;

int main()
{
    window2d my_win(1200_px, 800_px, color(colors::dark_grey), "Basic Paint");
    shape canvas = rectangle(1200_px, 800_px);
    texture tex = texture(300, 200, colors::white);
    canvas.attach_tex(tex);
    std::vector<shape> initial_world { 
        canvas,
    };

    render2d::animate(my_win, 144, initial_world, 
    [&](std::vector<shape> &world, events e) {
        if (e.pressed_keys[A_KEY]) {
            int x_pixel = (e.cursor_pos[0] + 600) / 4;
            int y_pixel = (e.cursor_pos[1] + 400) / 4;
            if (x_pixel > 0 && y_pixel > 0 && x_pixel < 300 && y_pixel < 200) {
                tex[y_pixel][x_pixel] = colors::black;
                world[0].attach_tex(tex);
            }
        }
    });
}
