#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "minigl2d.hpp"

using namespace glm;

namespace minigl
{

// TODO: these are globals! figure out a way to get rid of this is possible!
bool pressed_keys[NUM_KEYS];
bool noticed_keys[NUM_KEYS];
position cursor_pos;

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

position position::operator*(const int a)
{
    position retP;
    retP.x = this->x * a;
    retP.y = this->y * a;
    return retP;
}

position position::operator/(const int a)
{
    position retP;
    retP.x = this->x / a;
    retP.y = this->y / a;
    return retP;
}

double position::distance_to(const position& p)
{
    return std::sqrt(std::pow(p.x - this->x, 2) + std::pow(p.y - this->y, 2));
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

std::vector<color> get_color_vec(color col, int size)
{
    std::vector<color> vec;
    for (int i = 0; i < size; i++)
        vec.push_back({ col.red(), col.green(), col.blue()});
    return vec;
}

shape::shape(std::vector<glm::vec3> base_vertices, std::vector<color> base_fragments, pixels unit_len, position pos) :
    base_vertices(base_vertices),
    base_fragments(base_fragments),
    unit_len(unit_len),
    pos(pos)
{
    // for this constructor just predefine the vertex and fragment scripts
    // define default vertex and fragment shaders, we would want another constructor eventually
    // that allows for custom vertex and fragment shaders
	vertex_shader.add_attribute<glm::vec3>("vPosition");
	vertex_shader.add_attribute<color>("vColor");
    vertex_shader.add_uniform<glm::mat4>("MVP");
    vertex_shader.define_shader(R"(
		out vec3 fColor;
        void main() {
            gl_Position = MVP * vec4(vPosition, 1.0);
			fColor = vColor.rgb;
        }    
    )");
	fragment_shader.define_shader(R"(
		in vec3 fColor;
        out vec3 color;
        void main() {
            color = fColor;
        }
    )");
}

triangle::triangle(pixels side_len) :
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
        pixels(side_len.px/2),
        position(0,0)
    )
{
}

circle::circle(pixels radius, color col, position pos) :
    shape(
        std::vector<vec3> {
            {1.000f, 0.000f, 0.000f},
            {0.940f, 0.342f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.940f, 0.342f, 0.000f},
            {0.766f, 0.643f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.766f, 0.643f, 0.000f},
            {0.500f, 0.866f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.500f, 0.866f, 0.000f},
            {0.174f, 0.985f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.174f, 0.985f, 0.000f},
            {-0.174f, 0.985f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.174f, 0.985f, 0.000f},
            {-0.500f, 0.866f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.500f, 0.866f, 0.000f},
            {-0.766f, 0.643f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.766f, 0.643f, 0.000f},
            {-0.940f, 0.342f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.940f, 0.342f, 0.000f},
            {-1.000f, 0.000f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-1.000f, 0.000f, 0.000f},
            {-0.940f, -0.342f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.940f, -0.342f, 0.000f},
            {-0.766f, -0.643f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.766f, -0.643f, 0.000f},
            {-0.500f, -0.866f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.500f, -0.866f, 0.000f},
            {-0.174f, -0.985f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.174f, -0.985f, 0.000f},
            {0.174f, -0.985f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.174f, -0.985f, 0.000f},
            {0.500f, -0.866f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.500f, -0.866f, 0.000f},
            {0.766f, -0.643f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.766f, -0.643f, 0.000f},
            {0.940f, -0.342f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.940f, -0.342f, 0.000f},
            {1.000f, -0.000f, 0.000f},
            {0.000f, 0.000f, 0.000f},
        }, 
        get_color_vec(col, 54),
        pixels(2 * radius.px),
        pos
    )
{
}

circle::circle(pixels radius, color col) :
    circle(radius, col, position(0,0))
{
}

circle::circle(pixels radius) :
    circle(radius, color(colors::red), position(0,0))
{
}

circle::circle(pixels radius, position pos) :
    circle(radius, color(colors::red), pos)
{
}

rectangle::rectangle(pixels width, pixels height, color col, position pos) :
    shape(
        std::vector<vec3> {
            {-1.0f, 1.0f * height.px/width.px, 0.0f},
            {-1.0f,  -1.0f * height.px/width.px, 0.0f},
            {1.0f,  -1.0f * height.px/width.px, 0.0f},
            {1.0f, 1.0f * height.px/width.px, 0.0f},
            {-1.0f, 1.0f * height.px/width.px, 0.0f},
            {1.0f,  -1.0f * height.px/width.px, 0.0f},
        }, 
        get_color_vec(col, 6),
        pixels(width),
        pos
    )
{
}
    
rectangle::rectangle(pixels width, pixels height) : 
    rectangle(width, height, colors::red, position(0,0))
{
}

rectangle::rectangle(pixels width, pixels height, color col) : 
    rectangle(width, height, col, position(0,0))
{
}

rectangle::rectangle(pixels width, pixels height, position pos) : 
    rectangle(width, height, colors::red, pos)
{
}

window2d::window2d(pixels width, pixels height, color col, std::string name) :
    width(width),
    height(height),
    name(name),
    col(col)
{
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    for (int i = 0; i < GLFW_KEYS.size(); i++) {
        if (key == GLFW_KEYS[i] && action == GLFW_PRESS) {
            pressed_keys[i] = true;
            noticed_keys[i] = true;
        } else if (key == GLFW_KEYS[i] && action == GLFW_RELEASE) {
            pressed_keys[i] = false;
        }
    }
}

static void cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    cursor_pos = position(xpos-width/2, -ypos+height/2);
}

void render2d::animate(
    window2d& win,
    int fps,
    std::vector<shape> shapes,
    std::function<std::vector<shape>(std::vector<shape>, events)> func)
{
    // init this window, must do this first so that we can have the context for the other 
    // opengl functionality
    window my_window(win.width, win.height, win.name); 
    my_window.set_background_color(win.col);
    
    // TODO: scale all the shapes and init the render pipeline
    double interval = 1.0 / fps;
	
    double prev_time = glfwGetTime();
        
    // define the matrix to scale the shapes depending on the window size 
    float aspect = (1.0f * win.width.px) / win.height.px;
    mat4 scaled;
    if (win.width.px > win.height.px)
        scaled = scale(mat4(1.0f), vec3(1/aspect,1.0f,0.0f));
    else
        scaled = scale(mat4(1.0f), vec3(1.0f,aspect,0.0f));
    
    
    my_window.render_and_listen(key_callback, cursor_callback,
    [&]{
        double curr_time = glfwGetTime();
        if (curr_time - prev_time > interval) {
            prev_time = curr_time;
            events e;
            for (int i = 0; i < NUM_KEYS; i++)
                e.pressed_keys[i] = noticed_keys[i];
            e.cursor_pos = cursor_pos;

            shapes = func(shapes, e);
            
            for (int i = 0; i < NUM_KEYS; i++)
                noticed_keys[i] = pressed_keys[i] && noticed_keys[i];
        }
        
        // render all of the shapes 
        for (shape& s : shapes) {
            // TODO: doing the matrix calculation and creating a render pipeline for each frame is 
            // expensive! Optimize this once we get something working. As they say, preoptimization
            // is the root of all evil!

            // scale the shape to the desired size
            float ratio = s.unit_len.px / (std::min(win.width.px, win.height.px) * 1.0f);
	        mat4 mvp = scale(scaled, vec3(ratio,ratio,0.0f));

            // translate the shape based on its current position
            float xshift = (2.0f * s.pos.x) / s.unit_len.px;
	        float yshift = (2.0f * s.pos.y) / s.unit_len.px;
            mvp = glm::translate(mvp, vec3(xshift, yshift, 0.0f));

            render_pipeline p(s.vertex_shader, s.fragment_shader);
            
            p["MVP"] = mvp;
            p["vPosition"] = s.base_vertices;
            p["vColor"] = s.base_fragments;
            p.render(win.width, win.height);
        }
    });

}

} // namespace minigl
