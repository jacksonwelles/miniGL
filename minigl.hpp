#ifndef _SIMPLEGL_H_
#define _SIMPLEGL_H_
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <concepts>
#include <type_traits>
#include <string>
#include <ostream>
#include <vector>
#include <map>
#include <utility>
#include <typeinfo>
#include <typeindex>

namespace minigl
{
    class render_pipeline;

    enum class colors
    {
        red,
        white,
        yellow,
        purple,
        light_blue,
        orange,
        green,
        blue,
        cyan,
        black,
        violet,
        turqoise,
        lilac,
        dark_grey,
        dark_purple,
        bronze,
        dark_blue,
        forest_green,
        brown,
    };

    enum class shapes
    {
        square,
        circle,
        triangle,
        rectangle,
    };

    class color : public glm::vec4
    {
    public:
        constexpr color(float r, float g, float b, float a = 1) : glm::vec4{r, g, b, a} {};
        color(colors c);
        float &red(void) { return glm::vec4::operator[](0); }
        float &green(void) { return glm::vec4::operator[](1); }
        float &blue(void) { return glm::vec4::operator[](2); }
        float &alpha(void) { return glm::vec4::operator[](3); }
        const float &red(void) const { return glm::vec4::operator[](0); }
        const float &green(void) const { return glm::vec4::operator[](1); }
        const float &blue(void) const { return glm::vec4::operator[](2); }
        const float &alpha(void) const { return glm::vec4::operator[](3); }
    };
    std::ostream &operator<<(std::ostream &os, const color &c);

    class pixels
    {
    public:
        int px;
        constexpr pixels(int x) : px{x} {};
    };
    constexpr pixels operator"" _px(unsigned long long arg)
    {
        return pixels(arg);
    }

    class window
    {
    private:
        GLFWwindow *window_ptr;
        bool window_ok = false;

    public:
        bool ok(void);
        window(pixels width, pixels height, std::string name);
        ~window();
        window &operator=(const window &) = delete;
        window(const window &) = delete;
        void set_background_color(color c);
        template <typename C, typename... R>
        requires std::invocable<C, R...>
        void render(C oper, R &&...args)
        {
            // Ensure we can capture the escape key being pressed below
            glfwSetInputMode(window_ptr, GLFW_STICKY_KEYS, GL_TRUE);
            // Enable depth test
            glEnable(GL_DEPTH_TEST);
            // Accept fragment if it closer to the camera than the former one
            glDepthFunc(GL_LESS);
            do
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                oper(std::forward<R>(args)...);
                // Swap buffers
                glfwSwapBuffers(window_ptr);
                glfwPollEvents();
            } while (glfwGetKey(window_ptr, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
                     glfwWindowShouldClose(window_ptr) == 0);
        }
        void render();
    };

    template <typename T>
    concept valid_uniform =
        std::same_as<T, typename glm::vec3> ||
        std::same_as<T, typename glm::vec4> ||
        std::same_as<T, typename glm::mat3> ||
        std::same_as<T, typename glm::mat4> ||
        std::same_as<T, int> ||
        std::same_as<T, color> ||
        std::same_as<T, float>;
    template <typename T>
    concept valid_vertex_attribute =
        std::same_as<T, float> ||
        std::same_as<T, typename glm::vec2> ||
        std::same_as<T, typename glm::vec3> ||
        std::same_as<T, typename glm::vec4> ||
        std::same_as<T, color>;
    enum class shader_types
    {
        vertex,
        fragment,
    };
    class shader
    {
    private:
        std::map<std::string, std::type_index> uniform_map;
        std::map<std::string, std::type_index> attribute_map;
        std::string shader_body;
        shader_types type;

    public:
        shader(shader_types t) : type{t} {}
        template <typename T>
        requires valid_uniform<T>
        void add_uniform(std::string name)
        {
            uniform_map.insert({name, std::type_index(typeid(T))});
        }
        template <typename T>
        requires valid_vertex_attribute<T>
        void add_attribute(std::string name)
        {
            attribute_map.insert({name, std::type_index(typeid(T))});
        }
        void define_shader(std::string body);
        friend render_pipeline;
    };

    class render_pipeline
    {
    private:
        struct attr_node
        {
            GLuint array_num;
            GLuint buffer_id;
            GLsizei shape_size;
        };
        GLsizei min_verticies;
        GLuint vao_id;
        GLuint shader_program_id;
        float *vertex_buffer;
        std::map<std::string, std::pair<std::type_index, GLuint>> uniform_map;
        std::map<std::string, std::pair<std::type_index, attr_node>> attribute_map;
        std::string generate_shader_includes(const shader &s);
        void generate_uniforms(const shader &s);
        void generate_attributes(const shader &s);

    public:
        render_pipeline(const shader &vert_shader, const shader &frag_shader);
        ~render_pipeline();
        template <typename T>
        requires valid_uniform<T>
        void update_uniform(std::string name, const T &new_value)
        {
            auto pair = uniform_map.at(name);
            if (pair.first != std::type_index(typeid(T)))
            {
                throw std::runtime_error("uniform type does not match");
            }
            glUseProgram(shader_program_id);
            if constexpr (std::same_as<T, typename glm::mat4>)
            {
                glUniformMatrix4fv(pair.second, 1, GL_FALSE, &new_value[0][0]);
            }
            else if constexpr (std::same_as<T, typename glm::mat3>)
            {
                glUniformMatrix3fv(pair.second, 1, GL_FALSE, &new_value[0][0]);
            }
            else if constexpr (std::same_as<T, typename glm::vec3>)
            {
                glUniform3f(pair.second, new_value[0], new_value[1], new_value[2]);
            }
            else if constexpr (std::same_as<T, typename glm::vec4>)
            {
                glUniform4f(pair.second, new_value[0], new_value[1], new_value[2], new_value[3]);
            }
            else if constexpr (std::same_as<T, int>)
            {
                glUniform1i(pair.second, new_value);
            }
            else if constexpr (std::same_as<T, float>)
            {
                glUniform1f(pair.second, new_value);
            }
            else if constexpr (std::same_as<T, color>)
            {
                glUniform4f(pair.second, new_value[0], new_value[1], new_value[2], new_value[3]);
            }
        }
        template <typename T>
        requires valid_vertex_attribute<T>
        void update_vertex_attr(std::string attr_name, const std::vector<T> &new_value)
        {
            auto pair = attribute_map.at(attr_name);
            if (pair.first != std::type_index(typeid(T))) {
                throw std::runtime_error("attribute type does not match");
            }
            int vertex_size;
            if constexpr (std::same_as<T, float>) {
                vertex_size = 1;
            } else {
                vertex_size = T::length();
            }
            if (new_value.size() == 0) {
                return;
            }
            if (new_value.size() < min_verticies || min_verticies == 0) {
                min_verticies = new_value.size();
            }
            glUseProgram(shader_program_id);
            glEnableVertexAttribArray(pair.second.array_num);
            glBindBuffer(GL_ARRAY_BUFFER, pair.second.buffer_id);
            glBufferData(GL_ARRAY_BUFFER, min_verticies * sizeof(T), new_value.data(), GL_DYNAMIC_DRAW);
            glVertexAttribPointer(
                pair.second.array_num,
                vertex_size,
                GL_FLOAT,
                GL_FALSE,
                0,
                (void *)0);
        }
        void render(void);
    };
}

#endif // _SIMPLEGL_H_
