#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include <minigl.hpp>
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
    window my_window = window(1024_px, 768_px, "Game of Life"); 
    if (!my_window.ok()) {return 1;}

    shader basic_vert_shader(shader_types::vertex);
    shader basic_frag_shader(shader_types::fragment);
    shader life_frag_shader(shader_types::fragment);
    


    basic_vert_shader.add_attribute<vec3>("pos");
    basic_vert_shader.add_attribute<vec2>("uv");
    basic_vert_shader.define_shader(R"(
        out vec2 frag_uv;
        void main()
        {
            gl_Position = vec4(pos, 1);
            frag_uv = uv;
        }
    )");

    basic_frag_shader.add_uniform<texture>("tex");
    basic_frag_shader.define_shader(R"(
        in vec2 frag_uv;
        out vec3 color;
        void main()
        {
            color = texture(tex, frag_uv).rgb;
        }
    )");

    life_frag_shader.add_uniform<vec2>("texel_size");
    life_frag_shader.add_uniform<texture>("tex");
    life_frag_shader.define_shader(R"(
        in vec2 frag_uv;
        out vec3 color;
        void main()
        {
            color = vec3(0,0,0); //dead
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
                    color = vec3(1,1,1); //alive
                }
            } else {
                if (sum ==3) {
                    color = vec3(1,1,1);
                }
            }
        }
    )");

    render_pipeline life_pipeline(basic_vert_shader, life_frag_shader);
    render_pipeline plane_pipeline(basic_vert_shader, basic_frag_shader);

    pixels height {150};
    pixels width {200};
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
       life_start[50 + pair.first][75 + pair.second]  = colors::white;
    }
    for (auto pair : infi_cell)
    {
       life_start[50 + pair.first][125 + pair.second]  = colors::white;
    }

    life_pipeline["pos"] = plane_buff;
    life_pipeline["uv"] = uvs;
    life_pipeline["texel_size"] = vec2(1.0/width.px, 1.0/height.px);
    plane_pipeline["pos"] = plane_buff;
    plane_pipeline["uv"] = uvs;

    life_pipeline["tex"] = life_start;
    plane_pipeline["tex"] = life_pipeline.render_to_texture(width, height);

    double last_time = glfwGetTime();
    my_window.render([&](){
        double current_time = glfwGetTime();
        if (current_time - last_time > 0.2) {
            life_pipeline["tex"] = plane_pipeline.render_to_texture(width, height);
            plane_pipeline["tex"] = life_pipeline.render_to_texture(width, height);
            last_time = current_time;
        }
        plane_pipeline.render(my_window.width(), my_window.height());
    });
}