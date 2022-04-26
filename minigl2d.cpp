#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "minigl2d.hpp"

using namespace glm;

namespace minigl
{

position::position() : 
    x(0),
    y(0)
{
}

position::position(int x, int y) : x(x), y(y)
{
}

position position::operator+(const position& p)
{
    position retP;
    retP.x = this->x + p.x;
    retP.y = this->y + p.y;
    return retP;
}

position position::operator-(const position& p)
{
    position retP;
    retP.x = this->x - p.x;
    retP.y = this->y - p.y;
    return retP;
}

position shape::get_pos()
{
    return pos;
}

void shape::translate(position pos)
{
    this->pos = this->pos + pos;
}

void shape::set_pos(position pos)
{
    translate(pos - this->pos);
}

shape::shape(std::vector<glm::vec3> base_vertices, std::vector<color> base_fragments, pixels unit_len) :
    base_vertices(base_vertices),
    base_fragments(base_fragments),
    unit_len(unit_len)
{
}

triangle::triangle(int side_len) :
    shape(
        std::vector<vec3> {
            {-1.0f, -1.0f,  0.0f},
            {1.0f,  -1.0f,  0.0f},
            {0.0f,  0.866f, 0.0f},
        }, 
        std::vector<color> {
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
        },
        side_len / 2
    )
{
}

window2d::window2d(pixels width, pixels height, color col, std::string name) :
    width(width),
    height(height),
    name(name),
    col(col)
{
}

void render2d::animate(
    window2d& win,
    int fps,
    std::vector<shape> shapes,
    std::function<std::vector<shape>(std::vector<shape>)> func)
{
    // init this window, must do this first so that we can have the context for the other 
    // opengl functionality
    window my_window(win.width, win.height, win.name); 
    my_window.set_background_color(win.col);
    
    // TODO: scale all the shapes and init the render pipeline

    my_window.render([&]{

    });

}

} // namespace minigl
