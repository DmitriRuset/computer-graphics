#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/gtc/quaternion.hpp>
#include <shooter/figure.hpp>
#include <shooter/enemy.hpp>

#include <set>
#include <memory>

class ColoredFigures {
public:
    explicit ColoredFigures(IFigure& figure, const std::string& v_shader, const std::string& f_shader):
                            triangles_cnt_(figure.CntTriangles()) {
        glGenBuffers(1, &vertex_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        glBufferData(GL_ARRAY_BUFFER, figure.SizeOf(), figure.GetVertices(), GL_STATIC_DRAW);

        glGenBuffers(1, &color_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
        glBufferData(GL_ARRAY_BUFFER, figure.SizeOf(), figure.GetColors(), GL_STATIC_DRAW);

        program_fig_ = LoadShaders(v_shader.c_str(), f_shader.c_str());

        matrix_fig_ = glGetUniformLocation(program_fig_, "MVP");
    }

    void Draw(const glm::mat4& projection_matrix) const {
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

        glUseProgram(program_fig_);

        for (auto& coord: coords_) {
            glm::mat4 model_matrix = glm::mat4(glm::translate(glm::mat4(1.0), coord->GetPos()) *
                                               glm::toMat4(coord->GetRot()));
            glm::mat4 view_matrix = getViewMatrix();
            glm::mat4 MVP = projection_matrix * view_matrix * model_matrix;

            glUniformMatrix4fv(matrix_fig_, 1, GL_FALSE, &MVP[0][0]);

            // Draw the triangles!
            glDrawArrays(GL_TRIANGLES, 0, triangles_cnt_ * 3);
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }

    void AddEnemy(Enemy& enemy) {
        coords_.insert(std::make_shared<Enemy>(enemy));
    }

    void DestroyEnemy(const std::shared_ptr<Enemy>& enemy) {
        coords_.erase(enemy);
    }

    void Clear() {
        coords_.clear();
    }

    const auto& GetItems() const {
        return coords_;
    }

    ~ColoredFigures() {
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &color_buffer_);
        glDeleteProgram(program_fig_);
    }
private:
    uint32_t triangles_cnt_{};
    GLuint vertex_buffer_{};
    GLuint color_buffer_{};

    GLuint matrix_fig_{};
    GLuint program_fig_{};

    std::set<std::shared_ptr<Enemy>> coords_;
};
