#include <iostream>
#include "minigl2d.hpp"

using namespace glm;
using namespace minigl;

int main()
{
    std::cout << "in simple move!\n";
    window2d my_win(1200_px, 800_px, color(colors::forest_green), "Simple Move Demo");
    std::vector<shape> initial_world { circle(50_px), };
    
    render2d::animate(my_win, 60, initial_world, 
    [&](std::vector<shape> world, events e) {
        if (e.pressed_keys[A_KEY])
            world[0].translate(position(-3, 0));
        if (e.pressed_keys[D_KEY])
            world[0].translate(position(3, 0));
        if (e.pressed_keys[W_KEY])
            world[0].translate(position(0, 3));
        if (e.pressed_keys[S_KEY])
            world[0].translate(position(0, -3));
        return world;
    });
}
