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


class shape
{
public:
    shape() = default;
    shape(const shape& s): shape(s.base_vData, s.base_fData, s.unit_len){};
    shape(std::vector<glm::vec3> base_vdata, std::vector<color> base_fdata, int unit_len);
    void translate(position pos);
    void set_pos(position pos);
    position get_pos();
protected:
    int window_width, window_height;
    int unit_len;   // pixel lenght of one unit of the base vertex data
    position pos;
    minigl::shader vertex_shader = minigl::shader(minigl::shader_types::vertex);
    minigl::shader fragment_shader = minigl::shader(minigl::shader_types::fragment);
    // this is the buffer data that define the shape before any transformations 
    std::vector<glm::vec3> base_vData;
    std::vector<color> base_fData;
    // this is the buffer data after transformations, eg. translation, resizing 
    // per window size, etc.
    std::vector<glm::vec3> vData;
    std::vector<color> fData;

    render_pipeline pipe;
    
    void set_window_size(int width, int height);
    void fit_to_window();
    void scale_vData();
    void translate_vData(int xpos, int ypos);
friend class render2d;
};

class triangle : public shape
{
public:
    triangle(int side_len); // equilateral
};

class circle : public shape
{
public:
    circle(int radius);
};

struct window2d
{
    window2d(int width, int height, std::string name);
    int width, height;
    std::string name;
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