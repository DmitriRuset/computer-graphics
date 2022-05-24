#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

class ColoredFigure {
public:
    explicit ColoredFigure(const GLfloat* coords, size_t size_coords,
                           const GLfloat* colors, size_t size_colors,
                           uint32_t triangle_cnt): triangles_cnt_(triangle_cnt) {
        glGenBuffers(1, &vertex_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        glBufferData(GL_ARRAY_BUFFER, size_coords, coords, GL_STATIC_DRAW);

        glGenBuffers(1, &color_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
        glBufferData(GL_ARRAY_BUFFER, size_colors, colors, GL_STATIC_DRAW);
    }

    void Draw() const {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        glVertexAttribPointer(
                0,             // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                nullptr            // array buffer offset
        );

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
        glVertexAttribPointer(
                1,             // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                nullptr            // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, triangles_cnt_ * 3);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }

    ~ColoredFigure() {
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &color_buffer_);
    }
private:
    uint32_t triangles_cnt_{};
    GLuint vertex_buffer_{};
    GLuint color_buffer_{};
};
