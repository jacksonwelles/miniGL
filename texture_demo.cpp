#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "minigl.hpp"
// GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace minigl;
using namespace std;

static vector<vec3> plane_buff = {
    {-1, -1, 0},
    {1,1,0},
    {1,-1,0},
    {-1,-1,0},
    {1, 1, 0},
    {-1, 1, 0}
};
static vector<vec2> uvs = {
    {0,0},
    {1,1},
    {1,0},
    {0,0},
    {1,1},
    {0,1}
};

int main(void)
{
    window my_window = window(1024_px, 768_px, "Tutorial"); 
    if (!my_window.ok()) {return 1;}

    shader vertex_shader(shader_types::vertex);
    shader fragment_shader(shader_types::fragment);

    vertex_shader.add_attribute<vec3>("pos");
    vertex_shader.add_attribute<vec2>("uv");
    vertex_shader.define_shader(R"(
        out vec2 frag_uv;
        void main()
        {
            gl_Position = vec4(pos, 1);
            frag_uv = uv;
        }
    )");
    fragment_shader.add_uniform<int>("swap");
    fragment_shader.add_uniform<texture>("tex1");
    fragment_shader.add_uniform<texture>("tex2");
    fragment_shader.define_shader(R"(
        out vec3 color;
        in vec2 frag_uv;
        void main()
        {
            if (swap == 0) {
                color = texture(tex1, frag_uv).rgb;
            } else {
                color = texture(tex2, frag_uv).rgb;
            }
        }
    )");

    render_pipeline triangle(vertex_shader, fragment_shader);
    triangle.update_vertex_attr("pos", plane_buff);
    triangle.update_vertex_attr("uv", uvs);
    texture tex1(200_px, 150_px, colors::white);
    for (int i = 0; i < tex1.height() - 1; i++) {
        for (int j = 0; j < tex1.width() - 1; j++) {
            if ((i + j) % 2) {
                tex1[i][j] = colors::black;
            } else {
                tex1[i][j] = colors::red;
            }
        }
    }
    texture tex2(200_px, 150_px, colors::orange);
     for (int i = 0; i < tex2.height() - 1; i++) {
        for (int j = 0; j < tex2.width() - 1; j++) {
            if ((i + j) % 2) {
                tex2[i][j] = colors::green;
            } else {
                tex2[i][j] = colors::yellow;
            }
        }
    }
    triangle.update_uniform("swap", 1);
    
    triangle.update_uniform("tex1", tex1);
    triangle.update_uniform("tex2", tex2);
    my_window.set_background_color(colors::green);
    double last_time = glfwGetTime();
    int count = 0;
    my_window.render([&](){
        double current_time = glfwGetTime();
        if (current_time - last_time > 0.5) {
            triangle.update_uniform("swap", (count+1) % 2 );
            count++;
            last_time = current_time;
        }
        triangle.render();
    });
}