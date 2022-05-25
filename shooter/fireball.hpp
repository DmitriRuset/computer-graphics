#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>
#include <iostream>

class Fireball {
public:
    Fireball(glm::vec3 pos, glm::vec3 speed_vec): pos_(pos), speed_vec_(speed_vec) {}

    glm::vec3 GetPos() {
        return pos_;
    }

    void Move() {
        pos_ += speed_vec_;
    }

    const float collision_rad = 0.2f;

private:
    glm::vec3 pos_;
    glm::vec3 speed_vec_;
};
