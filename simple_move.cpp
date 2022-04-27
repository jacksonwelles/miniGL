#include <iostream>
#include "minigl2d.hpp"

using namespace glm;
using namespace minigl;

int main()
{
    window2d my_win(1200_px, 800_px, color(colors::forest_green), "Simple Move Demo");
    std::vector<shape> initial_world { rectangle(100_px, 200_px, colors::cyan) };

    render2d::animate(my_win, 30, initial_world, 
    [&](std::vector<shape> world, events e) {
        if (e.pressed_keys[A_KEY])
            world[0].translate(position(-6, 0));
        if (e.pressed_keys[D_KEY])
            world[0].translate(position(6, 0));
        if (e.pressed_keys[W_KEY])
            world[0].translate(position(0, 6));
        if (e.pressed_keys[S_KEY])
            world[0].translate(position(0, -6));
        return world;
    });
}
