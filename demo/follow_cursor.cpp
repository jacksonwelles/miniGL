#include <iostream>
#include <minigl2d.hpp>

using namespace minigl;
using namespace glm;

int main()
{
    window2d my_win(1200_px, 800_px, color(colors::forest_green), "Follow Cursor Demo");
    std::vector<shape> initial_world;
    for (int i = 0; i < 1; i++)
    {
        initial_world.push_back(circle(50_px, colors::light_blue));

    }
    
    render2d::animate(my_win, 60, initial_world, 
    [&](std::vector<shape> &world, events e) {
        shape& s = world[0];
        position rel_pos = e.cursor_pos - s.get_pos();
        float dist = distance(e.cursor_pos ,s.get_pos());
        if (dist > 5.0f) {
            position unit = (rel_pos * 6.0f) / dist;
            s.translate(unit);
        }
    });
}
