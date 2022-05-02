
#include <iostream>
#include <cmath>
#include <minigl2d.hpp>

using namespace minigl;
using namespace glm;

int main()
{
    window2d my_win(1200_px, 800_px, color(colors::dark_grey), "Pong");
    std::vector<shape> initial_world { 
        circle(15_px, colors::dark_grey),
        rectangle(150_px, 20_px, colors::dark_grey, position(0, -300)),
        rectangle(1100_px, 700_px, colors::cyan), 
    };

    position ball_vel(7,7);
    int paddle_vel = 7;
    bool game_over = false;
    
    render2d::animate(my_win, 60, initial_world, 
    [&](std::vector<shape> &world, events e) {
        if (game_over) {
            world.clear();
            world.push_back(rectangle(1200_px, 800_px, colors::black));
        } else {
            shape& ball = world[0];
            shape& paddle = world[1];
            
            // move paddle 
            position rel_pos = paddle.get_pos() - e.cursor_pos;
            if (std::abs(rel_pos[0]) > 5) {
                if (rel_pos[0] > 0)
                    paddle.translate(-paddle_vel, 0);
                else if (rel_pos[0] < 0)
                    paddle.translate(paddle_vel, 0);
            }

            // move ball 
            ball.translate(ball_vel);

            // about to hit left wall or right wall
            if (ball.get_pos()[0] + ball_vel[0] >= 535 || ball.get_pos()[0] + ball_vel[0] <= -535)
                ball_vel[0] = -ball_vel[0];
            // about to hit the top wall
            if (ball.get_pos()[1] + ball_vel[1] >= 335)
                ball_vel[1] = -ball_vel[1];
            // about to hit bottom wall
            if (ball.get_pos()[1] + ball_vel[1] <= -335)
                game_over = true; 
            // about to hit top of paddle
            if (ball.get_pos()[1] + ball_vel[1] <= -290 && 
                ball.get_pos()[0] + ball_vel[0] >= paddle.get_pos()[0] - 75 &&
                ball.get_pos()[0] + ball_vel[0] <= paddle.get_pos()[0] + 75)
                ball_vel[1] = -ball_vel[1];
        }
    });
}
