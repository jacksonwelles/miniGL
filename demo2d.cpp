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
    Window2d my_win(width, height, "Bouncing Ball");
    std::vector<Shape> initialWorld = { Circle(radius), };
    
    int g = 1;
    int vel = 0;
    int fps = 60;
    Render2d::animate(my_win, fps, initialWorld, 
    [&](std::vector<Shape> world) {
        int dt = 1;
        Shape& ball = world[0];
        auto relative_pos = ball.get_pos().y - radius/2 + height/2;
        if (relative_pos <= 0) {
            vel = -vel;
            ball.translate(Position(0, -relative_pos));
        }
        vel = vel - g * dt;
        ball.set_pos(Position(0, ball.get_pos().y + (vel - g * dt) * dt));
        
        return world;
    });
}

