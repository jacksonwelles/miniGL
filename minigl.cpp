#include "minigl.hpp"
#include <iostream>
#include <exception>
#include <functional>
#include <sstream>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
namespace minigl
{
    const std::map<std::type_index, std::string> shader_translations = {
        {std::type_index(typeid(texture)), "sampler2D"},
        {std::type_index(typeid(int)), "int"},
        {std::type_index(typeid(float)), "float"},
        {std::type_index(typeid(glm::vec2)), "vec2"},
        {std::type_index(typeid(glm::vec3)), "vec3"},
        {std::type_index(typeid(glm::vec4)), "vec4"},
        {std::type_index(typeid(glm::mat3)), "mat3"},
        {std::type_index(typeid(glm::mat4)), "mat4"},
        {std::type_index(typeid(color)), "vec4"}};
    color::color(colors c)
    {
        switch (c)
        {
        case colors::red:
            *this = color(1.0f, 0.0f, 0.0f, 1.0f);
            break;
        case colors::white:
            *this = color(1.0f, 1.0f, 1.0f, 1.0f);
            break;
        case colors::yellow:
            *this = color(1.0f, 1.0f, 0.0f, 1.0f);
            break;
        case colors::purple:
            *this = color(1.0f, 0.0f, 1.0f, 1.0f);
            break;
        case colors::light_blue:
            *this = color(0.0f, 1.0f, 1.0f, 1.0f);
            break;
        case colors::orange:
            *this = color(1.0f, 0.5f, 0.0f, 1.0f);
            break;
        case colors::green:
            *this = color(0.0f, 1.0f, 0.0f);
            break;
        case colors::blue:
            *this = color(0.0f, 0.0f, 1.0f);
            break;
        case colors::cyan:
            *this = color(0.5f, 1.0f, 1.0f);
            break;
        case colors::black:
            *this = color(0.0f, 0.0f, 0.0f);
            break;
        case colors::violet:
            *this = color(0.5f, 0.5f, 0.5f);
            break;
        case colors::turqoise:
            *this = color(0.0f, 0.5f, 0.5f);
            break;
        case colors::lilac:
            *this = color(2.0f, 0.5f, 1.0f);
            break;
        case colors::dark_grey:
            *this = color(0.1f, 0.1f, 0.1f);
            break;
        case colors::dark_purple:
            *this = color(0.1f, 0.0f, 0.1f);
            break;
        case colors::bronze:
            *this = color(0.1f, 0.1f, 0.0f);
            break;
        case colors::dark_blue:
            *this = color(0.0f, 0.1f, 0.1f);
            break;
        case colors::forest_green:
            *this = color(0.0f, 0.1f, 0.0f);
            break;
        case colors::brown:
            *this = color(0.1f, 0.0f, 0.0f);
            break;
        }
    }
    std::ostream &operator<<(std::ostream &os, const color &c)
    {
        os << "r: " << c.red() << ", g: " << c.green() << ", b: " << c.blue() << ", a: " << c.alpha();
        return os;
    }

    texture::texture(const pixels &width, const pixels &height, const color &background = colors::black)
    {
        w = width.px;
        h = height.px;
        for (int i = 0; i < w * h; i++)
        {
            tex_data.push_back(background);
        }
    }

    std::span<color> texture::operator[](int i)
    {
        if (i >= h)
            throw std::out_of_range("minigl::texture::operator[]");
        return std::span(tex_data.begin() + i * w, w);
    }

    window::window(pixels width, pixels height, std::string name)
    {
        glfwSetErrorCallback([](int code, const char *description)
                             { std::cerr << "Error Code: " << code << " - " << description << std::endl; });
        if (!glfwInit())
        {
            std::cerr << "GLFW Init Failed" << std::endl;
            return;
        }
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        do
        {
            if (!(window_ptr = glfwCreateWindow(width.px, height.px, name.c_str(), nullptr, nullptr)))
            {
                std::cerr << "GLFW Window Creation Failed" << std::endl;
                break;
            }
            glfwGetFramebufferSize(window_ptr, &window_width, &window_height);
            glfwMakeContextCurrent(window_ptr);
            if (glewInit() != GLEW_OK)
            {
                std::cerr << "GLEW Init Failed" << std::endl;
                break;
            }
            window_ok = true;

             // Ensure we can capture the escape key being pressed below
            glfwSetInputMode(window_ptr, GLFW_STICKY_KEYS, GL_TRUE);
            // Enable depth test
            glEnable(GL_DEPTH_TEST);
            // Accept fragment if it closer to the camera than the former one
            glDepthFunc(GL_LESS);
        } while (0);

        if (!window_ok)
        {
            glfwTerminate();
        }
    }
    window::~window()
    {
        if (window_ok)
        {
            glfwTerminate();
        }
    }
    bool window::ok(void)
    {
        return window_ok;
    }
    void window::set_background_color(color c)
    {
        glClearColor(c.red(), c.green(), c.blue(), c.alpha());
    }
    void window::render(void)
    {
        auto t = []()
        { return; };
        window::render(t);
    }

    pipeline_bracket_helper &pipeline_bracket_helper::operator=(render_to_texture_helper helper)
    {
        render_to_texture(*pipeline, name, *helper.pipeline, helper.width, helper.height);
        return *this;
    }

    void shader::define_shader(std::string body)
    {
        shader_body = body;
    }

    GLuint compile_shader(shader_types type, std::string shader_source)
    {
        GLuint shader_id;
        if (type == shader_types::vertex)
        {
            shader_id = glCreateShader(GL_VERTEX_SHADER);
        }
        else if (type == shader_types::fragment)
        {
            shader_id = glCreateShader(GL_FRAGMENT_SHADER);
        }

        GLint result = GL_FALSE;
        int info_log_length;
        char const *src_ptr = shader_source.c_str();
        glShaderSource(shader_id, 1, &src_ptr, NULL);
        glCompileShader(shader_id);

        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if (info_log_length > 0)
        {
            std::vector<char> err_message(info_log_length + 1);
            glGetShaderInfoLog(shader_id, info_log_length, NULL, &err_message[0]);
            std::cerr << &err_message[0] << std::endl;
            throw std::runtime_error("shader compilation failed");
        }
        return shader_id;
    }

    GLuint create_program(std::string v_shader_source, std::string f_shader_source)
    {
        GLuint v_shader_id = compile_shader(shader_types::vertex, v_shader_source);
        GLuint f_shader_id = compile_shader(shader_types::fragment, f_shader_source);
        GLuint program_id = glCreateProgram();
        glAttachShader(program_id, v_shader_id);
        glAttachShader(program_id, f_shader_id);
        glLinkProgram(program_id);

        GLint result = GL_FALSE;
        int info_log_length;
        glGetProgramiv(program_id, GL_LINK_STATUS, &result);
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if (info_log_length > 0)
        {
            std::vector<char> err_message(info_log_length + 1);
            glGetProgramInfoLog(program_id, info_log_length, NULL, &err_message[0]);
            std::cerr << &err_message[0] << std::endl;
            throw std::runtime_error("linking compilation failed");
        }

        glDetachShader(program_id, v_shader_id);
        glDetachShader(program_id, f_shader_id);

        glDeleteShader(v_shader_id);
        glDeleteShader(f_shader_id);

        return program_id;
    }

    std::string render_pipeline::generate_shader_includes(const shader &s)
    {
        std::stringstream shader_includes;
        shader_includes << "#version 330 core" << std::endl;
        int count = 0;
        for (auto pair : s.uniform_map)
        {
            shader_includes << "uniform " << shader_translations.at(pair.second)
                            << " " << pair.first << ";" << std::endl;
        }
        for (auto pair : s.attribute_map)
        {
            shader_includes << "layout(location = " << count << ") in " << shader_translations.at(pair.second) << " " << pair.first << ";" << std::endl;
            count++;
        }
        return shader_includes.str();
    }

    void render_pipeline::generate_uniforms(const shader &s)
    {
        int tex_count = 0;
        for (auto pair : s.uniform_map)
        {
            GLuint uni_id = glGetUniformLocation(shader_program_id, pair.first.c_str());
            GLuint tex_id = 0;
            GLuint tex_num = 0;
            if (pair.second == std::type_index(typeid(texture)))
            {
                glGenTextures(1, &tex_id);
                tex_num = tex_count++;
                texture_ids.push_back(tex_id);
            }
            uniform_map.insert({pair.first, {.type_id = pair.second, .uniform_id = uni_id, .texture_num = tex_num, .texture_id = tex_id}});
        }
    }

    void render_pipeline::generate_attributes(const shader &s)
    {
        GLuint count = 0;
        for (auto pair : s.attribute_map)
        {
            GLuint buffer_id;
            glGenBuffers(1, &buffer_id);
            attribute_map.insert({pair.first, {.type_id = pair.second, .array_num = count, .buffer_id = buffer_id, .shape_size = 0}});
            count++;
        }
    }
    
    render_pipeline::render_pipeline()
    {
        is_ok = false;
    }

    render_pipeline::render_pipeline(const shader &vert_shader, const shader &frag_shader)
    {
        glGenVertexArrays(1, &vao_id);
        glBindVertexArray(vao_id);
        glGenFramebuffers(1, &framebuf_id);
        glGenRenderbuffers(1, &depthbuf_id);
        min_verticies = 0;
        connected = false;

        GLuint v_shader_id = glCreateShader(GL_VERTEX_SHADER);
        GLuint f_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

        std::string vert_shader_source = generate_shader_includes(vert_shader) + vert_shader.shader_body;
        std::string frag_shader_source = generate_shader_includes(frag_shader) + frag_shader.shader_body;

        shader_program_id = create_program(vert_shader_source, frag_shader_source);
    
        generate_uniforms(vert_shader);
        generate_uniforms(frag_shader);

        generate_attributes(vert_shader);
        is_ok = true;
    }

    render_pipeline::render_pipeline(render_pipeline &&old)
    {
        vao_id = old.vao_id;
        shader_program_id = old.shader_program_id;
        attribute_map = std::move(old.attribute_map);
        uniform_map = std::move(old.uniform_map);
        texture_ids = std::move(old.texture_ids);
        min_verticies = old.min_verticies;
        connected = old.connected;
        is_ok = true;
        old.is_ok = false;
    }

    render_pipeline::~render_pipeline()
    {
        if (!is_ok)
            return;
        for (auto pair : attribute_map)
        {
            glDeleteBuffers(1, &pair.second.buffer_id);
        }
        for (auto tex_id : texture_ids)
        {
            glDeleteTextures(1, &tex_id);
        }
        glDeleteRenderbuffers(1, &depthbuf_id);
        glDeleteFramebuffers(1, &framebuf_id);
        glDeleteProgram(shader_program_id);
        glDeleteVertexArrays(1, &vao_id);
    }

    bool render_pipeline::ok(void)
    {
        return is_ok;
    }

    void render_pipeline::render_core(void)
    {
        glBindVertexArray(vao_id);
        glUseProgram(shader_program_id);
        int tex_count = texture_ids.size();
        for (GLuint i = 0; i < tex_count; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
        }
        int attr_size = attribute_map.size();
        for (GLuint i = 0; i < attr_size; i++) {
            glEnableVertexAttribArray(i);
        }
        glDrawArrays(GL_TRIANGLES, 0, min_verticies);
        for (GLuint i = 0; i < attr_size; i++) {
            glDisableVertexAttribArray(i);
        }
    }

    void render_pipeline::render(pixels width, pixels height, float x_center, float y_center)
    {
        if (!is_ok) {
            return;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(x_center, y_center, width.px, height.px);
        render_core();
    }

    pipeline_bracket_helper render_pipeline::operator[](std::string name)
    {
        return pipeline_bracket_helper(name, this);
    }

    render_to_texture_helper render_pipeline::render_to_texture(pixels width, pixels height)
    {
        return render_to_texture_helper(this, width, height);
    }

    void render_to_texture(render_pipeline &receiver, std::string rname, render_pipeline &sender,
        pixels width, pixels height)
    {
        if (!receiver.is_ok) return;
        auto node = receiver.uniform_map.at(rname);
        if (node.type_id != std::type_index(typeid(texture)))
        {
            throw std::runtime_error("connect receiver must be texture");
        };

        glBindFramebuffer(GL_FRAMEBUFFER, sender.framebuf_id);

        glBindTexture(GL_TEXTURE_2D, node.texture_id);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width.px, height.px, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindRenderbuffer(GL_RENDERBUFFER, sender.depthbuf_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width.px, height.px);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, sender.depthbuf_id);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, node.texture_id, 0);

        GLenum draw_buf = GL_COLOR_ATTACHMENT0;
        
        glDrawBuffers(1, &draw_buf);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("error establishing framebuffer");
        }
        glUseProgram(receiver.shader_program_id);
        glUniform1i(node.uniform_id, node.texture_num);

        glBindFramebuffer(GL_FRAMEBUFFER, sender.framebuf_id);
        glViewport(0,0,width.px,height.px);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sender.render_core();
    }
}