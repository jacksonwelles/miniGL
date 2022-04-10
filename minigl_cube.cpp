// Include standard headers
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


static vector<vec3> g_vertex_buffer_data = {
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

static vector<color> g_color_buffer_data = {
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
int main( void )
{

    window my_window = window(1024_px, 768_px, "Tutorial"); 
    if (!my_window.ok()) {return 1;}

    shader vertex_shader(shader_types::vertex), fragment_shader(shader_types::fragment);
    vertex_shader.add_uniform<glm::mat4>("MVP");
    vertex_shader.add_attribute<glm::vec3>("vertex_position");
    vertex_shader.add_attribute<color>("vertex_color");
    vertex_shader.define_shader(R"(
        out vec3 fragment_color;
        void main() {
            gl_Position = MVP * vec4(vertex_position, 1);
            fragment_color = vertex_color.rgb;
        }    
    )");

    fragment_shader.define_shader(R"(
        in vec3 fragment_color;
        out vec3 color;
        void main() {
            color = fragment_color;
        }
    )");

    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(-6,7,8), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 mvp = Projection * View * Model;

    my_window.set_background_color(color(colors::forest_green));

    vector<render_pipeline> pipelines;

    double last_time = glfwGetTime();
    double f_last_time = glfwGetTime();
    int counter = 0;
    int nframes = 0;
    my_window.render([&]{
        double current_time = glfwGetTime();
        double f_current_time = current_time;
        if (current_time - last_time > .01){
            pipelines.emplace_back( vertex_shader, fragment_shader);
            pipelines[pipelines.size()-1].update_uniform("MVP", Projection* View * translate(Model, vec3(0,0,0.001*counter)));
            pipelines[pipelines.size()-1].update_vertex_attr("vertex_position", g_vertex_buffer_data);
            pipelines[pipelines.size()-1].update_vertex_attr("vertex_color", g_color_buffer_data);
            counter++;
            last_time = current_time;
            
        }
        if (f_current_time - f_last_time > 2)
        {
            f_last_time = f_current_time;
            cout << "between frames: " << 1000.0/ double(nframes) <<" ms" <<endl;
            nframes = 0;
        }
        nframes++;
        for (auto &t : pipelines)
        {
            t.render();
        }
    });
    return 0;
}