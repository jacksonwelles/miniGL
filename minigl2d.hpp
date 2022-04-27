#pragma once

#include "minigl.hpp"
#include <iostream>
#include <functional>
#include <glm/glm.hpp>


namespace minigl
{

const int DEFAULT_WIDTH = 400;
const int DEFAULT_HEIGHT = 400;

struct position
{
    position();
    position(int x, int y);
    int x, y;
    position operator+(const position& p);
    position operator-(const position& p);
};

struct window2d
{
    window2d(pixels width, pixels height, color col, std::string name);
    pixels width, height;
    std::string name;
    color col;
};

class shape
{
public:
    shape() = delete;
    shape
    (
        std::vector<glm::vec3> base_vertices,
        std::vector<color> base_fragments,
        pixels unit_len,
        position pos
    );
    void translate(position pos);
    void set_pos(position pos);
    position get_pos();
protected:
    pixels unit_len;
    position pos;
    std::vector<glm::vec3> base_vertices;
    std::vector<color> base_fragments;
    minigl::shader vertex_shader = minigl::shader(minigl::shader_types::vertex);
    minigl::shader fragment_shader = minigl::shader(minigl::shader_types::fragment);
    int window_width, window_height;
    void scale_and_translate_base(position pos);
friend class render2d;
};

class triangle : public shape
{
public:
    triangle(pixels side_len); // equilateral
};

class circle : public shape
{
public:
    circle(pixels radius);
    circle(pixels radius, position pos);
};

struct render2d
{
    static void animate(
        window2d& win,
        int fps,
        std::vector<shape> sprites,
        std::function<std::vector<shape>(std::vector<shape>)> func);
};


} // namespace minigl