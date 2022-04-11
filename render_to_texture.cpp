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

static vector<vec3> cube_verts = {
    {-1.0f,-1.0f,-1.0f},
    {-1.0f,-1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f,-1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f,-1.0f},
    {1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f,-1.0f},
    {1.0f,-1.0f,-1.0f},
    {1.0f, 1.0f,-1.0f},
    {1.0f,-1.0f,-1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f,-1.0f},
    {1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f, 1.0f},
    {-1.0f,-1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    {-1.0f,-1.0f, 1.0f},
    {1.0f,-1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f,-1.0f,-1.0f},
    {1.0f, 1.0f,-1.0f},
    {1.0f,-1.0f,-1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f,-1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f,-1.0f},
    {-1.0f, 1.0f,-1.0f},
    {1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f,-1.0f},
    {-1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f},
    {1.0f,-1.0f, 1.0f}
};

static vector<color> cube_colors = {
    {0.583f,  0.771f,  0.014f},
    {0.609f,  0.115f,  0.436f},
    {0.327f,  0.483f,  0.844f},
    {0.822f,  0.569f,  0.201f},
    {0.435f,  0.602f,  0.223f},
    {0.310f,  0.747f,  0.185f},
    {0.597f,  0.770f,  0.761f},
    {0.559f,  0.436f,  0.730f},
    {0.359f,  0.583f,  0.152f},
    {0.483f,  0.596f,  0.789f},
    {0.559f,  0.861f,  0.639f},
    {0.195f,  0.548f,  0.859f},
    {0.014f,  0.184f,  0.576f},
    {0.771f,  0.328f,  0.970f},
    {0.406f,  0.615f,  0.116f},
    {0.676f,  0.977f,  0.133f},
    {0.971f,  0.572f,  0.833f},
    {0.140f,  0.616f,  0.489f},
    {0.997f,  0.513f,  0.064f},
    {0.945f,  0.719f,  0.592f},
    {0.543f,  0.021f,  0.978f},
    {0.279f,  0.317f,  0.505f},
    {0.167f,  0.620f,  0.077f},
    {0.347f,  0.857f,  0.137f},
    {0.055f,  0.953f,  0.042f},
    {0.714f,  0.505f,  0.345f},
    {0.783f,  0.290f,  0.734f},
    {0.722f,  0.645f,  0.174f},
    {0.302f,  0.455f,  0.848f},
    {0.225f,  0.587f,  0.040f},
    {0.517f,  0.713f,  0.338f},
    {0.053f,  0.959f,  0.120f},
    {0.393f,  0.621f,  0.362f},
    {0.673f,  0.211f,  0.457f},
    {0.820f,  0.883f,  0.371f},
    {0.982f,  0.099f,  0.879f}
};
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

    shader plane_vert_shader(shader_types::vertex);
    shader plane_frag_shader(shader_types::fragment);

    shader cube_vert_shader(shader_types::vertex);
    shader cube_frag_shader(shader_types::fragment);
    cube_vert_shader.add_attribute<vec3>("vertex_pos");
    cube_vert_shader.add_output<color>("fragment_color");
    cube_vert_shader.add_attribute<color>("vertex_color");
    cube_vert_shader.add_uniform<mat4>("mvp");
    cube_vert_shader.define_shader(R"(
        void main() {
            gl_Position = mvp * vec4(vertex_pos, 1);
            fragment_color = vertex_color;
        }
    )");

    cube_frag_shader.add_input<color>("fragment_color");
    cube_frag_shader.add_output<vec3>("color");
    cube_frag_shader.define_shader(R"(
        void main() {
            color = fragment_color.rgb;
        }
    )");
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(-6,7,8), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 mvp = Projection * View * Model;






    plane_vert_shader.add_attribute<vec3>("pos");
    plane_vert_shader.add_attribute<vec2>("uv");
    plane_vert_shader.add_output<vec2>("frag_uv");
    plane_vert_shader.define_shader(R"(
        void main()
        {
            gl_Position = vec4(pos, 1);
            frag_uv = uv;
        }
    )");
    plane_frag_shader.add_uniform<int>("swap");
    plane_frag_shader.add_uniform<texture>("tex1");
    plane_frag_shader.add_uniform<texture>("tex2");
    plane_frag_shader.add_output<vec3>("color");
    plane_frag_shader.add_input<vec2>("frag_uv");
    plane_frag_shader.define_shader(R"(
        void main()
        {
            if (swap == 0) {
                color = texture(tex1, frag_uv).rgb;
            } else {
                color = texture(tex2, frag_uv).rgb;
            }
        }
    )");

    render_pipeline cube(my_window, cube_vert_shader, cube_frag_shader);
    cube.update_uniform("mvp", mvp);
    cube.update_vertex_attr("vertex_pos", cube_verts);
    cube.update_vertex_attr("vertex_color", cube_colors);

    render_pipeline cube2(my_window, cube_vert_shader, cube_frag_shader);
    cube2.update_uniform("mvp", Projection * 
        glm::lookAt(glm::vec3(6,7,8), glm::vec3(0,0,0), glm::vec3(0,1,0)) * 
        Model);
    cube2.update_vertex_attr("vertex_pos", cube_verts);
    cube2.update_vertex_attr("vertex_color", cube_colors);


    render_pipeline plane(my_window, plane_vert_shader, plane_frag_shader);
    plane.update_vertex_attr("pos", plane_buff);
    plane.update_vertex_attr("uv", uvs);

    my_window.set_background_color(colors::black);
    pipeline_connect("tex1", plane, "color", cube);
    pipeline_connect("tex2", plane, "color", cube2, 400_px, 300_px);
    cube.render_textures();
    cube2.render_textures();
    double last_time = glfwGetTime();
    int count = 0;
    my_window.render([&](){
        double current_time = glfwGetTime();
        if (current_time - last_time > 1) {
            plane.update_uniform("swap", (count+1) % 2 );
            count++;
            last_time = current_time;
        }
        plane.render();
    });
}