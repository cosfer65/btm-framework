#include "glew.h"
#include "shaders.h"
#include <fstream>
#include <sstream>

namespace btm {
    static GLuint CompileShader(GLenum type, const char* source)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
        }

        return shader;
    }

    static GLuint CreateProgram(GLuint vs, GLuint fs)
    {
        GLuint program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (!success)
        {
            char log[512];
            glGetProgramInfoLog(program, 512, nullptr, log);
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        return program;
    }

    static int LoadShaderFromFile(const std::string& filename, std::string& source)
    {
        std::ifstream shaderFile(filename);
        if (!shaderFile.is_open())
            return 0;
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        source = shaderStream.str();
        return 1;
    }

    class shader_manager {
    public:
        std::vector<gl_shader*> shader_list;
        shader_manager() = default;
        ~shader_manager() {
            cleanup();
        }
        void cleanup() {
            for (auto shader : shader_list) {
                delete shader;
            }
            shader_list.clear();
        }

        gl_shader* create_shader_s(const char* vertex_shader_source, const char* fragment_shader_source) {
            // compile the shader source code
            GLuint vs = CompileShader(GL_VERTEX_SHADER, vertex_shader_source);
            GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragment_shader_source);
            // link the shader program
            GLuint program = CreateProgram(vs, fs);
            // delete the shader objects, they are no longer needed after linking
            glDeleteShader(vs);
            glDeleteShader(fs);
            // create a new gl_shader object and add it to the list
            // the list will be used to cleanup the shaders when the program ends
            gl_shader* shader = new gl_shader(program);
            shader_list.push_back(shader);
            // return the new shader object
            return shader;
        }
        gl_shader* create_shader_f(const std::string& vertex_shader_file, const std::string& fragment_shader_file) {
            std::string vertex_source, fragment_source;
            // load the shader source code from file
            LoadShaderFromFile(vertex_shader_file, vertex_source);
            LoadShaderFromFile(fragment_shader_file, fragment_source);

            return create_shader_s(vertex_source.c_str(), fragment_source.c_str());
        }
    };

    gl_shader* create_shader_f(const std::string& vertex_shader_file, const std::string& fragment_shader_file) {
        static shader_manager g_shader_manager;
        return g_shader_manager.create_shader_f(vertex_shader_file, fragment_shader_file);
    }
    gl_shader* create_shader_s(const char* vertex_shader_source, const char* fragment_shader_source) {
        static shader_manager g_shader_manager;
        return g_shader_manager.create_shader_s(vertex_shader_source, fragment_shader_source);
    }
    gl_shader* create_shader_raw_s(const char* vertex_shader_source, const char* fragment_shader_source) {
        return new gl_shader(CreateProgram(CompileShader(GL_VERTEX_SHADER, vertex_shader_source), CompileShader(GL_FRAGMENT_SHADER, fragment_shader_source)));
    }
    gl_shader* create_shader_raw_f(const std::string& vertex_shader_file, const std::string& fragment_shader_file) {
        std::string vertex_source, fragment_source;
        // load the shader source code from file
        LoadShaderFromFile(vertex_shader_file, vertex_source);
        LoadShaderFromFile(fragment_shader_file, fragment_source);

        return create_shader_raw_s(vertex_source.c_str(), fragment_source.c_str());
    }
    bool load_shader(const std::string& vertex_shader_file, const std::string& fragment_shader_file, gl_shader* shader) {
        std::string vertex_source, fragment_source;
        // load the shader source code from file
        LoadShaderFromFile(vertex_shader_file, vertex_source);
        LoadShaderFromFile(fragment_shader_file, fragment_source);
        GLuint vs = CompileShader(GL_VERTEX_SHADER, vertex_source.c_str());
        GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragment_source.c_str());
        GLuint program = CreateProgram(vs, fs);
        glDeleteShader(vs);
        glDeleteShader(fs);
        shader->set_program(program);
        return true;
    }
}