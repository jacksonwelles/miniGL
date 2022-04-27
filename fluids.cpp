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
    window my_window = window(1024_px, 1024_px, "Tutorial"); 
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

    shader mult_frag(shader_types::fragment);
    mult_frag.add_uniform<texture>("tex");
    mult_frag.define_shader(R"(
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

            color = vec3(0.5 * (texel_size.x * (wR.x - wL.x) + texel_size.y * (wT.y - wB.y)), 0, 0);
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
            float pL = texture(pressure, frag_uv - vec2(texel_size.x, 0)).x;
            float pR = texture(pressure, frag_uv + vec2(texel_size.x, 0)).x;
            float pB = texture(pressure, frag_uv - vec2(0, texel_size.y)).x;
            float pT = texture(pressure, frag_uv + vec2(0, texel_size.y)).x;

            vec4 res = texture(velocity, frag_uv) - vec4(0.5 * texel_size * vec2(pR - pL, pT- pB), 0, 0);
            color = res.rgb;
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
                s = 1.0f;
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

    shader advect_dye_shader(shader_types::fragment);
    advect_dye_shader.add_uniform<texture>("velocity");
    advect_dye_shader.add_uniform<texture>("dye");
    advect_dye_shader.add_uniform<vec2>("dye_texel_size");
    advect_dye_shader.add_uniform<vec2>("vel_texel_size");
    advect_dye_shader.add_uniform<float>("timestep");
    advect_dye_shader.define_shader(bilerp + R"(
        in vec2 frag_uv;
        out vec3 color;
        void main()
        {
            vec2 pos = frag_uv - timestep * vel_texel_size * texBilerp(frag_uv, vel_texel_size, velocity).xy;
            color = texBilerp(pos, dye_texel_size, dye).rgb;
        }
    )");

    shader force_shader(shader_types::fragment);
    force_shader.add_uniform<texture>("velocity");
    force_shader.add_uniform<vec2>("location");
    force_shader.add_uniform<vec2>("direction");
    force_shader.add_uniform<float>("scale");
    force_shader.add_uniform<float>("radius");
    force_shader.define_shader(R"(
        in vec2 frag_uv;
        out vec3 color;
        void main()
        {
            float dist = distance(location, frag_uv);
            if (dist < radius)
            {
                color = vec3(texture(velocity, frag_uv).xy + direction * scale * ((radius - dist)/radius), 0);
            }
            else
            {
                color = vec3(texture(velocity, frag_uv)).rgb;
            }
        }
    )");


    pixels dye_w = 1024;
    pixels dye_h = 1024;
    float timescale = 1.0f;
    float viscosity = 0.5;
    pixels sim_w = 256;
    pixels sim_h = 256;
    vec2 sim_texel_size(1.0f/sim_w.px, 1.0f/sim_h.px);
    vec2 dye_texel_size(1.0f/dye_w.px, 1.0f/dye_h.px);
    vec2 dye_point(dye_h.px/2, dye_w.px/2);
    vec2 sim_point(sim_h.px/2, sim_w.px/2);
    float sim_radius = std::min(sim_h.px, sim_w.px) / 4.0f;
    float dye_radius = std::min(dye_h.px, dye_w.px) / 4.0f;
    texture velocity(sim_w, sim_h, colors::black);


    texture dye(dye_w, dye_h, colors::black);
    for (int i = 0; i < dye.height(); i++) {
        for (int j = 0; j < dye.width(); j++) {
            float dist = distance(dye_point, vec2(i,j));
            if (dist < dye_radius) {
                dye[i][j] = color(0, ((dye_radius - dist) / dye_radius), 1.0f);
            }
        }
    }

    texture boundary_offset(sim_w, sim_h, colors::black);
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

    texture pressure_field(sim_w, sim_h, colors::black);
    render_pipeline v_tmp(pvert, pfrag);
    render_pipeline d_tmp(pvert, pfrag);
    render_pipeline p_tmp(pvert, pfrag);
    render_pipeline multer(pvert, mult_frag);
    make_plane(multer);
    make_plane(v_tmp);
    make_plane(d_tmp);
    make_plane(p_tmp);
    d_tmp["tex"] = dye;
    v_tmp["tex"] = velocity;
    p_tmp["tex"] = pressure_field;

    render_pipeline boundary(pvert, boundary_shader);
    make_plane(boundary);
    boundary["texel_size"] = sim_texel_size;
    boundary["offset"] = boundary_offset;

    render_pipeline ad(pvert, advect_shader);
    make_plane(ad);
    ad["timestep"] = timescale;
    ad["texel_size"] = sim_texel_size;

    render_pipeline jacobi(pvert, jacobi_shader);
    make_plane(jacobi);
    jacobi["texel_size"] = sim_texel_size;

    render_pipeline divergence(pvert, divergence_shader);
    make_plane(divergence);
    divergence["texel_size"] = sim_texel_size;

    render_pipeline gradient(pvert, gradient_shader);
    make_plane(gradient);
    gradient["texel_size"] = sim_texel_size;

    render_pipeline dye_ad(pvert, advect_dye_shader);
    make_plane(dye_ad);
    dye_ad["dye_texel_size"] = dye_texel_size;
    dye_ad["vel_texel_size"] = sim_texel_size;
    dye_ad["timestep"] = timescale; 

    render_pipeline apply_force(pvert, force_shader);
    make_plane(apply_force);
    apply_force["scale"] = 7.0f;
    apply_force["radius"] = (sim_radius / 6) * sim_texel_size.x;

    double last = glfwGetTime();
    int nframes = 0;
    vec2 pos = my_window.cursor_pos();
    vec2 last_pos = vec2(std::min(std::max(pos.x / my_window.width(), 0.0f), 1.0f),
                         1.0f - std::min(std::max(pos.y /my_window.height(), 0.0f), 1.0f));
    my_window.render([&](){
        double current = glfwGetTime();
        pos = my_window.cursor_pos();
        pos = vec2(std::min(std::max(pos.x / my_window.width(), 0.0f), 1.0f),
                   1.0f - std::min(std::max(pos.y /my_window.height(), 0.0f), 1.0f));
        if (current - last > 2)
        {
            double x, y;
            last = current;
            cout << "between frames: " << 1000.0/ double(nframes) <<" ms" <<endl;
            nframes = 0;
        }
        nframes++;
        boundary["scale"] = -1.0f;
        boundary["target"] = v_tmp.render_to_texture(sim_w,sim_h);
        v_tmp["tex"] = boundary.render_to_texture(sim_w,sim_h);

        ad["velocity"] = v_tmp.render_to_texture(sim_w,sim_h);
        ad["target"] = v_tmp.render_to_texture(sim_w, sim_h);
        v_tmp["tex"] = ad.render_to_texture(sim_w, sim_h);

        dye_ad["velocity"] = v_tmp.render_to_texture(sim_w, sim_h);
        dye_ad["dye"] = d_tmp.render_to_texture(dye_w, dye_h);
        d_tmp["tex"] = dye_ad.render_to_texture(dye_w, dye_h);

        vec2 alpha = (1.0f / (sim_texel_size * sim_texel_size)) / (viscosity * timescale);
        jacobi["alpha"] = alpha;
        jacobi["rBeta"] = 1.0f / (alpha + vec2(4,4));
        
        jacobi["b"] = v_tmp.render_to_texture(sim_w,sim_h);
        for (int i = 0; i < 30; i++)
        {
           
            jacobi["x"] = v_tmp.render_to_texture(sim_w,sim_h);
            v_tmp["tex"] = jacobi.render_to_texture(sim_w,sim_h);
        }
        apply_force["velocity"] = v_tmp.render_to_texture(sim_w, sim_h);
        apply_force["location"] = pos;
        apply_force["direction"] = pos - last_pos;

        v_tmp["tex"] = apply_force.render_to_texture(sim_w, sim_h);
        divergence["velocity"] = v_tmp.render_to_texture(sim_w,sim_h);
        jacobi["b"] = divergence.render_to_texture(sim_w,sim_h);
        jacobi["alpha"] = -1.0f * (1.0f / (sim_texel_size * sim_texel_size));
        jacobi["rBeta"] = vec2(0.25f, 0.25f);

        boundary["scale"] = 1.0f;
        boundary["target"] = pressure_field;
        for (int i = 0; i < 40; i++)
        {
            jacobi["x"] = boundary.render_to_texture(sim_w,sim_h);
            boundary["target"] = jacobi.render_to_texture(sim_w,sim_h);
        }

        
        gradient["pressure"] = boundary.render_to_texture(sim_w,sim_h);
        boundary["scale"] = -1.0f;
        boundary["target"] = v_tmp.render_to_texture(sim_w, sim_h);
        gradient["velocity"] = boundary.render_to_texture(sim_w, sim_h);
        v_tmp["tex"] = gradient.render_to_texture(sim_w,sim_h);

        d_tmp.render(my_window.width(), my_window.height());
        last_pos = pos;
    });
    


  
}