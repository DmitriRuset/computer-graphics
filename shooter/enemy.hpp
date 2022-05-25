#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>

const double Pi = 3.14159265;


class Enemy {
public:
    Enemy(glm::vec3 pos, glm::vec3 dim, float degree_angle): pos_(pos) {
        double to_rads = (degree_angle * Pi) / 180.;
        rot_.x = dim.x * sin(to_rads / 2);
        rot_.y = dim.y * sin(to_rads / 2);
        rot_.z = dim.z * sin(to_rads / 2);
        rot_.w = cos(to_rads / 2);
    }

    glm::vec3 GetPos() {
        return pos_;
    }

    glm::quat GetRot() {
        return rot_;
    }

private:
    glm::vec3 pos_;
    glm::quat rot_;
};