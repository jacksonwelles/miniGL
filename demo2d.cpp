#include <iostream>
#include <vector>
#include "minigl2d.hpp"

using namespace minigl;
using namespace glm;

int main()
{
    int height = 1000;
    int width = 700;
    int radius = 200;
    window2d my_win(width, height, "Bouncing Ball");
    std::vector<shape> initialWorld = { circle(radius), };
    
    int g = 1;
    int vel = 0;
    int fps = 60;
    render2d::animate(my_win, fps, initialWorld, 
    [&](std::vector<shape> world) {
        int dt = 1;
        shape& ball = world[0];
        auto relative_pos = ball.get_pos().y - radius/2 + height/2;
        if (relative_pos <= 0) {
            vel = -vel;
            ball.translate(position(0, -relative_pos));
        }
        vel = vel - g * dt;
        ball.set_pos(position(0, ball.get_pos().y + (vel - g * dt) * dt));
        
        return world;
    });
}

