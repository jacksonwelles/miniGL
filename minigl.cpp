#include "minigl.hpp"
#include <iostream>
#include <exception>
#include <functional>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
namespace minigl
{
    const std::map<std::type_index, std::string> shader_translations = {
        {std::type_index(typeid(int)), "int"},
        {std::type_index(typeid(float)), "float"},
        {std::type_index(typeid(glm::vec3)), "vec3"},
        {std::type_index(typeid(glm::vec4)), "vec4"},
        {std::type_index(typeid(glm::mat3)), "mat3"},
        {std::type_index(typeid(glm::mat4)), "mat4"},
        {std::type_index(typeid(color)), "vec4"}
    };
    color::color(colors c)
    {
        switch(c)
        {
            case colors::red: *this = color(1.0f, 0.0f, 0.0f, 0.0f); break;
            case colors::white: *this = color(1.0f, 1.0f, 1.0f, 0.0f); break;
            case colors::yellow: *this = color(1.0f, 1.0f, 0.0f, 0.0f); break;
            case colors::purple: *this = color(1.0f, 0.0f, 1.0f, 0.0f); break;
            case colors::light_blue: *this = color(0.0f, 1.0f, 1.0f, 1.0f); break;
            case colors::orange: *this = color(1.0f, 0.5f, 0.0f, 0.0f); break;
            case colors::green: *this = color(0.0f, 1.0f, 0.0f); break;
            case colors::blue: *this = color(0.0f, 0.0f, 1.0f); break;
            case colors::cyan: *this = color(0.5f, 1.0f, 1.0f); break;
            case colors::black: *this = color(0.0f, 0.0f, 0.0f); break;
            case colors::violet: *this = color(0.5f, 0.5f, 0.5f); break;
            case colors::turqoise: *this = color(0.0f, 0.5f, 0.5f); break;
            case colors::lilac: *this = color(2.0f, 0.5f, 1.0f); break;
            case colors::dark_grey: *this = color(0.1f, 0.1f, 0.1f); break;
            case colors::dark_purple: *this = color(0.1f, 0.0f, 0.1f); break;
            case colors::bronze: *this = color(0.1f, 0.1f, 0.0f); break;
            case colors::dark_blue: *this = color(0.0f, 0.1f, 0.1f); break;
            case colors::forest_green: *this = color(0.0f, 0.1f, 0.0f); break;
            case colors::brown: *this = color(0.1f, 0.0f, 0.0f); break;
        }
    }
    std::ostream& operator<<(std::ostream &os, const color &c)
    {
        os << "r: " << c.red() <<", g: " << c.green() << ", b: " << c.blue() << ", a: " << c.alpha();
        return os;
    }

    window::window(pixels width, pixels height, std::string name)
    {
        glfwSetErrorCallback([](int code, const char* description){
            std::cerr << "Error Code: " << code << " - " << description << std::endl;
        });
        if (!glfwInit()){
            std::cerr << "GLFW Init Failed" << std::endl;
            return;
        }
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        do {
            if (!(window_ptr = glfwCreateWindow(width.px, height.px, name.c_str(), nullptr, nullptr))) {
                std::cerr << "GLFW Window Creation Failed" << std::endl;
                break;
            }
            glfwMakeContextCurrent(window_ptr);
            if (glewInit() != GLEW_OK) {
                std::cerr << "GLEW Init Failed" << std::endl;
                break;
            }
            window_ok = true;
        } while (0);
        if (!window_ok) {
            glfwTerminate();
        }
    }
    window::~window()
    {
        if (window_ok) {
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
        auto t = [](){return;};
        window::render(t);
    }

    void shader::define_shader(std::string body)
    {
        shader_body = body;
    }

    GLuint compile_shader(shader_types type, std::string shader_source)
    {
        GLuint shader_id;
        if (type == shader_types::vertex){
            shader_id = glCreateShader(GL_VERTEX_SHADER);
        } else if (type == shader_types::fragment) {
            shader_id = glCreateShader(GL_FRAGMENT_SHADER);
        }

        GLint result = GL_FALSE;
        int info_log_length;
        char const * src_ptr = shader_source.c_str();
        glShaderSource(shader_id, 1, &src_ptr , NULL);
	    glCompileShader(shader_id);

        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        if ( info_log_length > 0) {
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
        if ( info_log_length > 0) {
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
        for (auto pair : s.uniform_map)
        {
            shader_includes << "uniform " << shader_translations.at(pair.second)
                << " " << pair.first << ";" << std::endl;
        }
        int count = 0;
        for (auto pair : s.attribute_map)
        {
            shader_includes << "layout(location = " << count << ") in " <<
                shader_translations.at(pair.second) << " " << pair.first << ";" << std::endl;
            count++;
        }
        return shader_includes.str();
    }

    void render_pipeline::generate_uniforms(const shader &s)
    {
        for (auto pair : s.uniform_map) {
            GLuint uni_id = glGetUniformLocation(shader_program_id, pair.first.c_str());
            uniform_map.insert({pair.first, {pair.second, uni_id}});
        }
    }

    void render_pipeline::generate_attributes(const shader &s)
    {
        GLuint count = 0;
        for (auto pair : s.attribute_map) {
            GLuint buffer_id;
            glGenBuffers(1, &buffer_id);
            attribute_map.insert({pair.first,
                {pair.second, {.array_num = count, .buffer_id = buffer_id, .shape_size = 0}}});
            count++;
        }
    }

    render_pipeline::render_pipeline(const shader &vert_shader, const shader &frag_shader)
    {
        glGenVertexArrays(1, &vao_id);
        glBindVertexArray(vao_id);
        min_verticies = 0;

        GLuint v_shader_id = glCreateShader(GL_VERTEX_SHADER);
	    GLuint f_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

        std::string vert_shader_source = generate_shader_includes(vert_shader) + vert_shader.shader_body;
        std::string frag_shader_source = generate_shader_includes(frag_shader) + frag_shader.shader_body;

        shader_program_id = create_program(vert_shader_source, frag_shader_source);
        
        generate_uniforms(vert_shader);
        generate_uniforms(frag_shader);

        generate_attributes(vert_shader);
    }

    render_pipeline::~render_pipeline()
    {
        for (auto pair : attribute_map) {
            glDeleteBuffers(1, &pair.second.second.buffer_id);
        }
        glDeleteProgram(shader_program_id);
        glDeleteVertexArrays(1, &vao_id);
    }

    void render_pipeline::render(void)
    {
        glUseProgram(shader_program_id);
        int size = attribute_map.size();
        for (GLuint i = 0; i < size; i++) {
            glEnableVertexAttribArray(i);
        }
        glDrawArrays(GL_TRIANGLES, 0, 36);
        for (GLuint i = 0; i < size; i++) {
            glDisableVertexAttribArray(i);
        }
    }
}