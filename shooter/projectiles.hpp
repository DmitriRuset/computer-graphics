#pragma once

#include <GL/glew.h>
#include <vector>

#include <GLFW/glfw3.h>

#include <glm/gtc/quaternion.hpp>
#include <shooter/fireball.hpp>
#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/texture.hpp>

#include <set>
#include <memory>

class Projectiles {
public:
    explicit Projectiles(const std::string& v_shader, const std::string& f_shader) {
        program_fig_ = LoadShaders(v_shader.c_str(), f_shader.c_str());

        matrix_fig_ = glGetUniformLocation(program_fig_, "MVP");

        texture_ = loadDDS("uvmap.DDS");

        texture_fig_ = glGetUniformLocation(program_fig_, "textureSampler");

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;

        if (!loadOBJ("fireball.obj", vertices, uv, normals)) {
            exit(1);
        }
        vertices_cnt = vertices.size();


        glGenBuffers(1, &vertex_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        glBufferData(GL_ARRAY_BUFFER, vertices_cnt * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &uv_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
        glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2), &uv[0], GL_STATIC_DRAW);
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
        glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
        glVertexAttribPointer(
                1,             // attribute 0. No particular reason for 0, but must match the layout in the shader.
                2,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                nullptr            // array buffer offset
        );

        glUseProgram(program_fig_);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glUniform1i(texture_fig_, 0);

        for (auto& coord: coords_) {
            glm::mat4 model_matrix = glm::mat4(glm::translate(glm::mat4(1.0), coord->GetPos()));
            glm::mat4 view_matrix = getViewMatrix();
            glm::mat4 MVP = projection_matrix * view_matrix * model_matrix;

            glUniformMatrix4fv(matrix_fig_, 1, GL_FALSE, &MVP[0][0]);

            // Draw the triangles!
            glDrawArrays(GL_TRIANGLES, 0, vertices_cnt);
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }

    void AddFireball(Fireball& enemy) {
        coords_.insert(std::make_shared<Fireball>(enemy));
    }

    void Move() {
        for (auto& coord: coords_) {
            coord->Move();
        }
    }

    void DestroyFireball(const std::shared_ptr<Fireball>& fireball) {
        coords_.erase(fireball);
    }

    void Clear() {
        coords_.clear();
    }

    const auto& GetItems() const {
        return coords_;
    }

    ~Projectiles() {
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &uv_buffer_);
        glDeleteProgram(program_fig_);
        glDeleteTextures(1, &texture_);
    }
private:
    GLuint vertex_buffer_{};
    GLuint uv_buffer_{};

    uint32_t vertices_cnt{};

    GLuint texture_{};
    GLuint matrix_fig_{};
    GLuint texture_fig_{};
    GLuint program_fig_{};

    std::set<std::shared_ptr<Fireball>> coords_;
};

