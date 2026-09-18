#pragma once

#include <string>
#include <vector>

#include "glew.h"
#include "vector.h"

using namespace btm;

namespace btm {
    class gl_shader {
        GLuint program;
        GLint old_program;
    public:
        gl_shader(GLuint _program = 0) :program(_program), old_program(0) {}
        ~gl_shader() {
            destroy();
        }
        GLuint set_program(GLuint _program) {
            destroy();
            program = _program;
            return program;
        }
        GLuint get_program() const {
            return program;
        }
        bool create() {
            return true;
        }
        void destroy() {
            if (program) {
                glDeleteProgram(program);
                program = 0;
            }
        }
        void use() {
            glGetIntegerv(GL_CURRENT_PROGRAM, &old_program);
            glUseProgram(program);
        }
        void end() {
            glUseProgram(old_program);
        }
        void set_uniform(const std::string& name, int value) const {
            glUniform1i(glGetUniformLocation(program, name.c_str()), value);
        }
        void set_uniform(const std::string& name, float value) const {
            glUniform1f(glGetUniformLocation(program, name.c_str()), value);
        }
        void set_uniform(const std::string& name, const fmat4& mat) const {
            glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, (float*)mat.data);
        }
        void set_uniform(const std::string& name, const fmat3& mat) const {
            glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, (float*)mat.data);
        }
        void set_uniform(const std::string& name, const fvec4& value) const {
            glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, (float*)value.data);
        }
        void set_uniform(const std::string& name, const fvec3& value) const {
            glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, (float*)value.data);
        }
    };

    gl_shader* create_shader_f(const std::string& vertex_shader_file, const std::string& fragment_shader_file);
    gl_shader* create_shader_s(const char* vertex_shader_source, const char* fragment_shader_source);

    // shader created from these functions is NOT automatically destroyed, you must call destroy() on it when done
    gl_shader* create_shader_raw_f(const std::string& vertex_shader_file, const std::string& fragment_shader_file);
    gl_shader* create_shader_raw_s(const char* vertex_shader_source, const char* fragment_shader_source);

    bool load_shader(const std::string& vertex_shader_file, const std::string& fragment_shader_file, gl_shader* shader);
}
