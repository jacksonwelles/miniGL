#include <iostream>
#include "minigl2d.hpp"

using namespace minigl;
using namespace glm;

int main()
{
	int width = 700;
	int height = 1000;
	int radius = 100;
	int initial_pos = 350;
    window2d my_win(pixels(width), pixels(height), color(colors::forest_green), "Bouncing Ball Demo");
    std::vector<shape> initial_world = { circle(pixels(radius), position(0, initial_pos)), };
    
    int g = 1;
    int vel = 0;
    int fps = 60;
    render2d::animate(my_win, fps, initial_world, 
    [&](std::vector<shape> &world, events e) {
        int dt = 1;
        shape& ball = world[0];
        int relative_pos = ball.get_pos().y - radius + height/2;
        if (relative_pos <= 0) {
            vel = -vel;
            ball.translate(position(0, -relative_pos));
        }
        vel = vel - g * dt;
        ball.set_pos(position(0, ball.get_pos().y + (vel - g * dt) * dt));
    });
}

