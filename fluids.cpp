#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "minigl.hpp"
// GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
using namespace minigl;
using namespace std;

static const vector<vec3> plane_buff = {
    {-1, -1, 0},
    {1,1,0},
    {1,-1,0},
    {-1,-1,0},
    {1, 1, 0},
    {-1, 1, 0}
};
static const vector<vec2> uvs = {
    {0,0},
    {1,1},
    {1,0},
    {0,0},
    {1,1},
    {0,1}
};

void make_plane(render_pipeline &p)
{
    p["uv"] = uvs;
    p["pos"] = plane_buff;
}

int main(void)
{
    window my_window = window(1080_px, 1080_px, "Tutorial"); 
    if (!my_window.ok()) {return 1;}

    shader pvert(shader_types::vertex);
    pvert.add_attribute<vec3>("pos");
    pvert.add_attribute<vec2>("uv");
    pvert.define_shader(R"(
        out vec2 frag_uv;
        void main()
        {
            gl_Position = vec4(pos,1);
            frag_uv = uv;
        }
    )");

    shader pfrag(shader_types::fragment);
    pfrag.add_uniform<texture>("tex");
    pfrag.define_shader(R"(
        in vec2 frag_uv;
        out vec3 color;
        void main()
        {
            color = texture(tex, frag_uv).rgb;
        }
    )");

    std::string bilerp (R"(
        vec4 texBilerp(in vec2 coord, in vec2 texel_size, in sampler2D my_tex)
        {
            vec4 new;
            float x_tex = coord.x / texel_size.x - 0.5;
            float y_tex = coord.y / texel_size.y - 0.5;
            float xTop = (ceil(x_tex) + 0.5) * texel_size.x;
            float yTop = (ceil(y_tex) + 0.5) * texel_size.y;
            float xBot = (floor(x_tex) + 0.5) * texel_size.x;
            float yBot = (floor(y_tex) + 0.5) * texel_size.y; 
            vec4 A = texture(my_tex, vec2(xTop, yTop));
            vec4 B = texture(my_tex, vec2(xTop, yBot));
            vec4 C = texture(my_tex, vec2(xBot, yTop));
            vec4 D = texture(my_tex, vec2(xBot, yBot));
            new = mix(mix(D, C, (coord.y - yBot)/texel_size.y), 
                mix(B, A, (coord.y - yBot)/texel_size.y),
                (coord.x - xBot)/texel_size.x);
            return new;
        }
    )");

    shader jacobi_shader(shader_types::fragment);
    jacobi_shader.add_uniform<texture>("x");
    jacobi_shader.add_uniform<texture>("b");
    jacobi_shader.add_uniform<vec2>("texel_size");
    jacobi_shader.add_uniform<vec2>("alpha");
    jacobi_shader.add_uniform<vec2>("rBeta");
    jacobi_shader.define_shader(R"(
        in vec2 frag_uv;
        out vec3 color;
        void main()
        {
            vec4 xL = texture(x, frag_uv - vec2(texel_size.x, 0));
            vec4 xR = texture(x, frag_uv + vec2(texel_size.x, 0));
            vec4 xB = texture(x, frag_uv - vec2(0, texel_size.y));
            vec4 xT = texture(x, frag_uv + vec2(0, texel_size.y));
            vec4 bC = texture(b, frag_uv);

            vec4 res = (xL + xR + xB + xT + vec4(alpha.x, alpha.y , 0, 0) * bC) * vec4(rBeta.x, rBeta.y,0,0);
            color = res.xyz;
        }
    )");

    shader divergence_shader(shader_types::fragment);
    divergence_shader.add_uniform<vec2>("texel_size");
    divergence_shader.add_uniform<texture>("velocity");
    divergence_shader.define_shader(R"(
        in vec2 frag_uv;
        out vec3 color;
        void main()
        {
            vec4 wL = texture(velocity, frag_uv - vec2(texel_size.x, 0));
            vec4 wR = texture(velocity, frag_uv + vec2(texel_size.x, 0));
            vec4 wB = texture(velocity, frag_uv - vec2(0, texel_size.y));
            vec4 wT = texture(velocity, frag_uv + vec2(0, texel_size.y));

            color.r = 0.5 * (texel_size.x * (wR.x - wL.x) + texel_size.y * (wT.y - wB.y))
        }
    )");

    shader gradient_shader(shader_types::fragment);
    gradient_shader.add_uniform<vec2>("texel_size");
    gradient_shader.add_uniform<texture>("velocity");
    gradient_shader.add_uniform<texture>("pressure");
    gradient_shader.define_shader(R"(
        in vec2 frag_uv;
        out vec3 color;
        void main()
        {
            vec4 pL = texture(pressure, frag_uv - vec2(texel_size.x, 0)).x;
            vec4 pR = texture(pressure, frag_uv + vec2(texel_size.x, 0)).x;
            vec4 pB = texture(pressure, frag_uv - vec2(0, texel_size.y)).x;
            vec4 pT = texture(pressure, frag_uv + vec2(0, texel_size.y)).x;

            vec4 out = texture(velocity, frag_uv);
            out.xy -= 0.5 * texel_size * vec2(pR - pL, pT- pB);
        }
    )");

    shader boundary_shader(shader_types::fragment);
    boundary_shader.add_uniform<texture>("target");
    boundary_shader.add_uniform<texture>("offset");
    boundary_shader.add_uniform<float>("scale");
    boundary_shader.add_uniform<vec2>("texel_size");
    boundary_shader.define_shader(R"(
        in vec2 frag_uv;
        out vec3 color;
        void main()
        {
            float s = 0;
            vec4 offset = vec4(texture(offset, frag_uv).xy * texel_size, 0, 0);
            if (offset.xy == vec2(0,0)) {
                s = 1;
            } else {
                s = scale;
            }
            color = s * texture(target, frag_uv + offset.xy).rgb;
        }
    )");



    shader advect_shader(shader_types::fragment);
    advect_shader.add_uniform<texture>("velocity");
    advect_shader.add_uniform<texture>("target");
    advect_shader.add_uniform<vec2>("texel_size");
    advect_shader.add_uniform<float>("timestep");
    advect_shader.define_shader(bilerp + R"(
        in vec2 frag_uv;
        out vec3 color;
        void main()
        {
            vec2 pos = frag_uv - timestep * texel_size * texture(velocity, frag_uv).rg;
            color = texBilerp(pos, texel_size, target).rgb;
        }
        
    )");
    pixels w = 1080;//my_window.width();
    pixels h = 1080;//my_window.height();
    vec2 texel_size(1.0f/w.px, 1.0f/h.px);
    vec2 point(h.px/2, w.px/2);
    float radius = 150;
    texture velocity(w, h, colors::black);
    for (int i = 0; i < velocity.height(); i++) {
        for (int j = 0; j < velocity.width(); j++) {
            if (glm::distance(point, vec2(i, j)) < radius) {
                vec2 center = vec2(i, j) - point;
                velocity[i][j]= color(1,1, 0, 0);
            }
        }
    }


    texture paint(w, h, colors::black);
    for (int i = 0; i < paint.height(); i++) {
        for (int j = 0; j < paint.width(); j++) {
            if (glm::distance(point, vec2(i, j)) < radius) {
                paint[i][j] = colors::blue;
            }
        }
    }

    texture boundary_offset(w, h, colors::black);
    for (int i = 0; i < boundary_offset.height(); i++) {
        for (int j = 0; j < boundary_offset.width(); j++) {
            float x = 0;
            float y = 0;
            if (i == boundary_offset.height() - 1) {
                y = -1;
            } else if (i == 0) {
                y = 1;
            }
            if (j == boundary_offset.width() - 1) {
                x = -1;
            } else if (j == 0) {
                x = 1;
            }
            boundary_offset[i][j] = color(x, y, 0);
        }
    }

    float timescale = 0.5f;
    float viscosity = 0.5f;
    render_pipeline v_tmp(pvert, pfrag);
    render_pipeline p_tmp(pvert, pfrag);
    make_plane(v_tmp);
    make_plane(p_tmp);
    p_tmp["tex"] = paint;
    v_tmp["tex"] = velocity;

    render_pipeline boundary(pvert, boundary_shader);
    make_plane(boundary);
    boundary["texel_size"] = texel_size;
    boundary["offset"] = boundary_offset;

    render_pipeline ad(pvert, advect_shader);
    make_plane(ad);
    ad["timestep"] = timescale;
    ad["texel_size"] = texel_size;

    render_pipeline jacobi(pvert, jacobi_shader);
    make_plane(jacobi);
    jacobi["texel_size"] = texel_size;

    render_pipeline divergence(pvert, divergence_shader);
    make_plane(divergence);
    divergence["texel_size"] = texel_size;
    
    
    double last = glfwGetTime();
    int nframes = 0;
    my_window.render([&](){
        double current = glfwGetTime();
        if (current - last > 2)
        {
            last = current;
            cout << "between frames: " << 1000.0/ double(nframes) <<" ms" <<endl;
            nframes = 0;
        }
        nframes++;
        boundary["scale"] = -1.0f;
        boundary["target"] = v_tmp.render_to_texture(w,h);
        v_tmp["tex"] = boundary.render_to_texture(w,h);

        ad["velocity"] = v_tmp.render_to_texture(w,h);
        ad["target"] = v_tmp.render_to_texture(w, h);
        v_tmp["tex"] = ad.render_to_texture(w, h);

        vec2 alpha = (texel_size * texel_size) / (viscosity * timescale);
        jacobi["alpha"] = alpha;
        jacobi["rBeta"] = 1.0f / (alpha + vec2(4,4));
        
        for (int i = 0; i < 20; i++)
        {
            jacobi["b"] = v_tmp.render_to_texture(w,h);
            jacobi["x"] = v_tmp.render_to_texture(w,h);
            v_tmp["tex"] = jacobi.render_to_texture(w,h);
        }




        ad["velocity"] = v_tmp.render_to_texture(w,h);
        ad["target"] = p_tmp.render_to_texture(w,h);
        p_tmp["tex"] = ad.render_to_texture(w,h);
        p_tmp.render(my_window.width(), my_window.height());
    });
    


  
}