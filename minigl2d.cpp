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

position shape::get_pos()
{
    return pos;
}

void shape::translate(position pos)
{
    this->pos = this->pos + pos;
    uniforms_updated = false;
}

void shape::set_pos(position pos)
{
    translate(pos - this->pos);
    uniforms_updated = false;
}

void shape::set_scale(vec2 s)
{
    scale_vec = s;
    uniforms_updated = false;
}

void shape::scale(vec2 s)
{
    scale_vec *= s;
    uniforms_updated = false;
}

std::vector<color> get_color_vec(color col, int size)
{
    std::vector<color> vec;
    for (int i = 0; i < size; i++)
        vec.push_back({ col.red(), col.green(), col.blue()});
    return vec;
}

void shape::render(pixels w, pixels h)
{
    pipeline.init(vertex_shader, fragment_shader);
    if (!uniforms_updated) {
        mat4 move = glm::translate(mat4(1.0f),
            vec3(pos.x, pos.y, 0.0f) /  vec3(0.5f * w.px, 0.5f * h.px, 1.0f));
        mat4 scaler = glm::scale(mat4(1.0f),
            ((float)unit_len.px) *
            vec3(1.0f/w.px, 1.0f/h.px, 1.0f) *
            vec3(scale_vec.x, scale_vec.y, 0.0f));

        pipeline["MVP"] = move * scaler;
        uniforms_updated = true;
    }
    if (!vertices_updated) {
        pipeline["vPosition"] = base_vertices;
        pipeline["vColor"] = base_fragments;
        vertices_updated = true;
    }
    pipeline.render(w, h);
}

shape::shape(std::vector<glm::vec3> base_vertices, std::vector<color> base_fragments, pixels unit_len, position pos) :
    base_vertices(base_vertices),
    base_fragments(base_fragments),
    unit_len(unit_len),
    pos(pos),
    scale_vec(1.0f, 1.0f),
    uniforms_updated(false),
    vertices_updated(false)
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
    cursor_pos = position(xpos-width/2.0f, -ypos +height/2.0f);
}

void render2d::animate(
    window2d& win,
    int fps,
    std::vector<shape> shapes,
    std::function<void(std::vector<shape>&, events)> func)
{
    // init this window, must do this first so that we can have the context for the other 
    // opengl functionality
    window my_window(win.width, win.height, win.name); 
    my_window.set_background_color(win.col);
    
    // TODO: scale all the shapes and init the render pipeline
    double interval = 1.0 / fps;
	
    double prev_time = glfwGetTime();

    double print_last_time = prev_time;
    int nframes = 0;
    
    my_window.render_and_listen(key_callback, cursor_callback,
    [&]{
        double curr_time = glfwGetTime();
        if (curr_time - prev_time > interval) {
            prev_time = curr_time;
            events e;
            for (int i = 0; i < NUM_KEYS; i++)
                e.pressed_keys[i] = noticed_keys[i];
            e.cursor_pos = cursor_pos;

            func(shapes, e);
            
            for (int i = 0; i < NUM_KEYS; i++)
                noticed_keys[i] = pressed_keys[i] && noticed_keys[i];
        }
        if (curr_time - print_last_time > 1)
        {
            std::cout << "between frames: " << 1000.0f / nframes << " ms" <<std::endl;
            print_last_time = curr_time;
            nframes = 0;
        }
        // render all of the shapes 
        for (shape& s : shapes) {
            s.render(win.width, win.height);
        }
        nframes++;
    });

}

} // namespace minigl
