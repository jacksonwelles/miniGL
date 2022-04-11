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

    shader basic_vert_shader(shader_types::vertex);
    shader basic_frag_shader(shader_types::fragment);
    shader life_frag_shader(shader_types::fragment);
    


    basic_vert_shader.add_attribute<vec3>("pos");
    basic_vert_shader.add_attribute<vec2>("uv");
    basic_vert_shader.add_output<vec2>("frag_uv");
    basic_vert_shader.define_shader(R"(
        void main()
        {
            gl_Position = vec4(pos, 1);
            frag_uv = uv;
        }
    )");

    basic_frag_shader.add_uniform<texture>("tex");
    basic_frag_shader.add_output<vec3>("color");
    basic_frag_shader.add_output<vec3>("passback");
    basic_frag_shader.add_input<vec2>("frag_uv");
    basic_frag_shader.define_shader(R"(
        void main()
        {
            color = texture(tex, frag_uv).rgb;
            passback = color;
        }
    )");

    life_frag_shader.add_uniform<vec2>("texel_size");
    life_frag_shader.add_uniform<int>("started");
    life_frag_shader.add_uniform<texture>("tex");
    life_frag_shader.add_uniform<texture>("start");
    life_frag_shader.add_output<vec3>("color");
    life_frag_shader.add_input<vec2>("frag_uv");
    life_frag_shader.define_shader(R"(
        void game_of_life(in sampler2D tex, out vec3 result)
        {
            result = vec3(0,0,0); //dead
            float sum = 0;
            float alive = round(texture(tex, frag_uv).r);
            sum += round(texture(tex, frag_uv + vec2(texel_size.x,0)).r);
            sum += round(texture(tex, frag_uv + vec2(0,texel_size.y)).r);
            sum += round(texture(tex, frag_uv + vec2(-texel_size.x,0)).r);
            sum += round(texture(tex, frag_uv + vec2(0,-texel_size.y)).r);
            sum += round(texture(tex, frag_uv + vec2(texel_size.x,-texel_size.y)).r);
            sum += round(texture(tex, frag_uv + vec2(texel_size.x,texel_size.y)).r);
            sum += round(texture(tex, frag_uv + vec2(-texel_size.x,texel_size.y)).r);
            sum += round(texture(tex, frag_uv + vec2(-texel_size.x,-texel_size.y)).r);
            
            if (alive > 0) {
                if (sum >= 2 && sum <= 3){
                    result = vec3(1,1,1); //alive
                }
            } else {
                if (sum ==3) {
                    result = vec3(1,1,1);
                }
            }
        }
        void main()
        {
            if (started == 0) {
                game_of_life(start, color);
            } else {
                game_of_life(tex, color);
            }
        }
    )");

    render_pipeline life_pipeline(my_window, basic_vert_shader, life_frag_shader);
    render_pipeline plane_pipeline(my_window, basic_vert_shader, basic_frag_shader);

    pixels height {30};
    pixels width {40};
    texture life_start(width, height, colors::black);
    vector<pair<int, int>> infi_cell = {
        {0, 0},
        {0, 2},
        {0, 4},
        {1, 1},
        {1, 2},
        {1, 4},
        {2, 3},
        {2, 4},
        {3, 0},
        {4, 0},
        {4, 1},
        {4, 2}, 
        {4, 4}
    };
    for (auto pair : infi_cell)
    {
       life_start[10 + pair.first][10 + pair.second]  = colors::white;
    }

    pipeline_connect("tex", life_pipeline, "passback", plane_pipeline, width, height);
    pipeline_connect("tex", plane_pipeline, "color", life_pipeline, width, height);
    life_pipeline.update_vertex_attr("pos", plane_buff);
    life_pipeline.update_vertex_attr("uv", uvs);
    life_pipeline.update_uniform("start", life_start);
    life_pipeline.update_uniform("started", 0);
    life_pipeline.update_uniform("texel_size", vec2(1.0/width.px, 1.0/height.px));
    plane_pipeline.update_vertex_attr("pos", plane_buff);
    plane_pipeline.update_vertex_attr("uv", uvs);


    life_pipeline.render_textures();
    double last_time = glfwGetTime();
    int count = 0;
    bool active = false;
    my_window.render([&](){
        double current_time = glfwGetTime();
        if (current_time - last_time > 0.5) {
            plane_pipeline.render_textures();
            if (!active) {
                life_pipeline.update_uniform("started", 1);
                active = true;
            }
            last_time = current_time;
            life_pipeline.render_textures();
        }
        plane_pipeline.render();
    });
}