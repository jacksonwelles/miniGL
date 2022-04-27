#pragma once

#include "minigl.hpp"
#include <iostream>
#include <functional>
#include <glm/glm.hpp>


namespace minigl
{

const int NUM_KEYS = 104;
const int E_KEY = 0;
const int A_KEY = 1;
const int D_KEY = 2;
const int W_KEY = 3;
const int S_KEY = 4;
const std::vector<int> GLFW_KEYS = { GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S};

using position = glm::vec2;

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
    shape(shape &&s) = default;
    shape
    (
        std::vector<glm::vec3> base_vertices,
        std::vector<color> base_fragments,
        pixels unit_len,
        position pos
    );
    shape(const shape &s):shape(s.base_vertices, s.base_fragments, s.unit_len, s.pos){};
    void translate(position pos);
    void set_pos(position pos);
    void set_scale(glm::vec2 s);
    void scale(glm::vec2 s);
    void render(pixels w, pixels h);
    position get_pos();
    
protected:
    bool uniforms_updated;
    bool vertices_updated;
    render_pipeline pipeline;
    pixels unit_len;
    position pos;
    glm::vec2 scale_vec;
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
    circle(pixels radius, color col);
    circle(pixels radius, color col, position pos);
    circle(pixels radius, position pos);
};

class rectangle : public shape
{
public:
    rectangle(pixels width, pixels height, color col, position pos);
    rectangle(pixels width, pixels height);
    rectangle(pixels width, pixels height, color col);
    rectangle(pixels width, pixels height, position pos);
protected:
};

struct events
{
    bool pressed_keys[NUM_KEYS];
    position cursor_pos;
};

class render2d
{
public:
    static void animate(
        window2d& win,
        int fps,
        std::vector<shape> sprites,
        std::function<void(std::vector<shape>&, events)> func);
};


} // namespace minigl