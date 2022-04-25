#pragma once

#include "minigl.hpp"
#include <iostream>
#include <functional>
#include <glm/glm.hpp>


namespace minigl
{

const int DEFAULT_WIDTH = 400;
const int DEFAULT_HEIGHT = 400;

struct Position
{
    Position();
    Position(int x, int y);
    int x, y;
    Position operator+(const Position& p);
    Position operator-(const Position& p);
};


class Shape
{
public:
    Shape() = default;
    Shape(std::vector<glm::vec3> base_vdata, std::vector<color> base_fdata, int unit_len);
    void translate(Position pos);
    void set_pos(Position pos);
    Position get_pos();
protected:
    int window_width, window_height;
    int unit_len;   // pixel lenght of one unit of the base vertex data
    Position pos;
    minigl::shader vertex_shader = minigl::shader(minigl::shader_types::vertex);
    minigl::shader fragment_shader = minigl::shader(minigl::shader_types::fragment);
    // this is the buffer data that define the shape before any transformations 
    std::vector<glm::vec3> base_vData;
    std::vector<color> base_fData;
    // this is the buffer data after transformations, eg. translation, resizing 
    // per window size, etc.
    std::vector<glm::vec3> vData;
    std::vector<color> fData;
    
    void set_window_size(int width, int height);
    void fit_to_window();
    void scale_vData();
    void translate_vData(int xpos, int ypos);
friend class Render2d;
};

class Triangle : public Shape
{
public:
    Triangle(int side_len); // equilateral
};

class Circle : public Shape
{
public:
    Circle(int radius);
};

struct Window2d
{
    Window2d(int width, int height, std::string name);
    int width, height;
    std::string name;
};

struct Render2d
{
    static void animate(
        Window2d& win,
        int fps,
        std::vector<Shape> sprites,
        std::function<std::vector<Shape>(std::vector<Shape>)> func);
};


} // namespace minigl