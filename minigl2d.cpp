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

shape::shape(std::vector<glm::vec3> base_vData, std::vector<color> base_fData, int unit_len) :
    base_vData(base_vData), 
    base_fData(base_fData),
    unit_len(unit_len)
{
    // define default vertex and fragment shaders
	vertex_shader.add_attribute<glm::vec3>("vPosition");
	vertex_shader.add_attribute<color>("vColor");
    vertex_shader.define_shader(R"(
		out vec3 fColor;
        void main() {
            gl_Position = vec4(vPosition, 1.0);
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
    // for now set fData to the base_fData
    fData = base_fData;
    set_window_size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

// scale the vertex buffer data in all of the shapes to fit the window
void shape::set_window_size(int width, int height)
{
    window_width = width;
    window_height = height;
    fit_to_window();
}

position shape::get_pos()
{
    return pos;
}

//
// looks at the base_vData (default vertex buffer values) and scales
// them to be of proper size given the window dimensions
//
void shape::fit_to_window()
{
	float aspect = (1.0f * window_width) / window_height;
	mat4 scaling_mat;
	if (window_width > window_height)
		scaling_mat = scale(mat4(1.0f), vec3(1/aspect,1.0f,0.0f));
	else
		scaling_mat = scale(mat4(1.0f), vec3(1.0f,aspect,0.0f));
    std::vector<vec3> vec;
	for (int i = 0; i < base_vData.size(); i++) {
		vec4 elem(base_vData[i], 1);
		elem = scaling_mat * elem;
        vec.push_back(elem);
	}
    vData = vec;
}

//
// scale the vertex buffer data in 2D by some factor
//
void shape::scale_vData()
{
	float small = std::min(window_width, window_height) * 1.0f;
	float ratio = unit_len / small;
	mat4 scaling_mat = scale(mat4(1.0f), vec3(ratio,ratio,0.0f));
	for (int i = 0; i < vData.size(); i++) {
		vec4 elem(vData[i], 1);
		elem = scaling_mat * elem;
		vData[i] = elem;
	}
}

//
// translate the vertex buffer data by some number of pixels
// this calculation depends on the current size of the window
//
void shape::translate_vData(int xpos, int ypos)
{
	float xshift = (2.0f * xpos) / window_width;
	float yshift = (2.0f * ypos) / window_height;
	mat4 scaling_mat = glm::translate(mat4(1.0f), vec3(xshift, yshift, 0.0f));
	for (int i = 0; i < vData.size(); i++) {
		vec4 elem(vData[i], 1);
		elem = scaling_mat * elem;
		vData[i] = elem;
	}
}

void shape::translate(position pos)
{
    this->pos = this->pos + pos;
    translate_vData(pos.x, pos.y);
}

void shape::set_pos(position pos)
{
    translate(pos - this->pos);
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

circle::circle(int radius) :
    shape(
        std::vector<vec3> {
            {1.000f, 0.000f, 0.000f},
            {0.866f, 0.500f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.866f, 0.500f, 0.000f},
            {0.500f, 0.866f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.500f, 0.866f, 0.000f},
            {0.000f, 1.000f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.000f, 1.000f, 0.000f},
            {-0.500f, 0.866f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.500f, 0.866f, 0.000f},
            {-0.866f, 0.500f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.866f, 0.500f, 0.000f},
            {-1.000f, 0.000f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-1.000f, 0.000f, 0.000f},
            {-0.866f, -0.500f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.866f, -0.500f, 0.000f},
            {-0.500f, -0.866f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.500f, -0.866f, 0.000f},
            {-0.000f, -1.000f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {-0.000f, -1.000f, 0.000f},
            {0.500f, -0.866f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.500f, -0.866f, 0.000f},
            {0.866f, -0.500f, 0.000f},
            {0.000f, 0.000f, 0.000f},
            {0.866f, -0.500f, 0.000f},
            {1.000f, -0.000f, 0.000f},
            {0.000f, 0.000f, 0.000f},
        },
        std::vector<color> {
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
            {1.000f, 0.000f, 0.000f},
        },
        radius / 2
    )
{
}

window2d::window2d(int width, int height, std::string name) :
    width(width),
    height(height),
    name(name)
{
}

void render2d::animate(
    window2d& win,
    int fps,
    std::vector<shape> shapes,
    std::function<std::vector<shape>(std::vector<shape>)> func)
{
	window my_window = window(pixels(win.width), pixels(win.height), win.name); 
    if (!my_window.ok()) 
    {
        std::cout << "failed to open window\n";
        return;
    }
        
   	// scale the shapes appropriately given the window size 
    for (shape& s : shapes) {
        s.set_window_size(win.width, win.height);
        s.scale_vData();
    }
    
	my_window.set_background_color(color(colors::forest_green));

    double interval = 1.0 / fps;
	
    double prev_time = glfwGetTime();
    my_window.render([&]{
        double curr_time = glfwGetTime();
        if (curr_time - prev_time > interval) {
            prev_time = curr_time;
            shapes = func(shapes);
        }
        
       	// render all of the shapes 
        for (shape& s : shapes) {
            render_pipeline p(s.vertex_shader, s.fragment_shader);
            p.update_vertex_attr("vPosition", s.vData);
            p.update_vertex_attr("vColor", s.fData);
            p.render(win.width, win.height);
        }
    });

}

} // namespace minigl
