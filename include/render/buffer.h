#pragma once

#include "glew.h" // or appropriate OpenGL header for GLuint, glGenBuffers, etc.

namespace btm {
    class va_buffer {
        GLuint vao_id;
    public:
        va_buffer() {
            vao_id = 0;
        }
        ~va_buffer() {}
        bool is_valid() const { return vao_id != 0; }
        bool create() {
            if (vao_id != 0) {
                glDeleteVertexArrays(1, &vao_id);
            }
            glGenVertexArrays(1, &vao_id);
            return vao_id != 0;
        }
        void destroy() {
            if (vao_id != 0) {
                glDeleteVertexArrays(1, &vao_id);
                vao_id = 0;
            }
        }
        void bind() const {
            glBindVertexArray(vao_id);
        }
        void unbind() const {
            glBindVertexArray(0);
        }
    };

    class gpu_buffer {
        GLuint buffer_id;
    public:
        gpu_buffer() {
            buffer_id = 0;
        }
        ~gpu_buffer() {}
        bool is_valid() const { return buffer_id != 0; }
        bool create() {
            if (buffer_id != 0) {
                glDeleteBuffers(1, &buffer_id);
            }
            glGenBuffers(1, &buffer_id);
            return buffer_id != 0;
        }
        void destroy() {
            if (buffer_id != 0) {
                glDeleteBuffers(1, &buffer_id);
                buffer_id = 0;
            }
        }

        void bind() const {
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        }
        void set_data(size_t size, const void* data, GLenum usage) {
            bind();
            glBufferData(GL_ARRAY_BUFFER, size, data, usage);
        }
        void set_sub_data(size_t size, const void* data) {
            bind();
            glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
        }
    };
} // namespace btm
