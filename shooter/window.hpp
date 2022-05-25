#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>
#include <cstdlib>

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
    return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
    return ProjectionMatrix;
}


class Window {
    friend class KeyboardAndMouse;
public:
    Window() { //NOLINT
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    int CreateWindow(int width, int height, const char* title,
                     GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr) {
        window_ = glfwCreateWindow(width, height, title, monitor, share);
        width_ = width;
        height_ = height;

        if(window_ == nullptr){
            fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
            getchar();
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window_);

        glewExperimental = true;
        if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to initialize GLEW\n");
            getchar();
            glfwTerminate();
            return -1;
        }

        glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);

        return 0;
    }

    void SwapBuffers() {
        glfwSwapBuffers(window_);
    }

    bool ExitTime() {
        return glfwGetKey(window_, GLFW_KEY_ESCAPE ) == GLFW_PRESS ||
               glfwWindowShouldClose(window_) != 0;
    }

    ~Window() {
        glfwTerminate();
    }
private:
    GLFWwindow* window_{nullptr};
    int width_;
    int height_;
};

class KeyboardAndMouse {
public:
    void computeMatricesFromInputs(const Window& w) {

        auto window = w.window_;
        int width = w.width_;
        int height = w.height_;

        // glfwGetTime is called only once, the first time this function is called
        static double lastTime = glfwGetTime();

        // Compute time difference between current and last frame
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);

        // Get mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Reset mouse position for next frame
        glfwSetCursorPos(window, width / 2., height / 2.);

        // Compute new orientation
        horizontalAngle += mouseSpeed * float(width / 2. - xpos);
        verticalAngle += mouseSpeed * float(height / 2. - ypos);

        // Direction : Spherical coordinates to Cartesian coordinates conversion
        glm::vec3 direction(
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle)
        );

        // Right vector
        glm::vec3 right = glm::vec3(
                sin(horizontalAngle - 3.14f / 2.0f),
                0,
                cos(horizontalAngle - 3.14f / 2.0f)
        );

        // Up vector
        glm::vec3 up = glm::cross(right, direction);

        // Move forward
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            position += direction * deltaTime * speed;
        }
        // Move backward
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            position -= direction * deltaTime * speed;
        }
        // Strafe right
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            position += right * deltaTime * speed;
        }
        // Strafe left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            position -= right * deltaTime * speed;
        }

        float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

        // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
        // Camera matrix
        ViewMatrix = glm::lookAt(
                position,           // Camera is here
                position + direction, // and looks here : at the same position, plus "direction"
                up                  // Head is up (set to 0,-1,0 to look upside-down)
        );

        // For the next frame, the "last time" will be "now"
        lastTime = currentTime;
    }

    glm::vec3 getCameraPosition() {
        return position;
    }
private:
    // Initial position : on +Z
    glm::vec3 position = glm::vec3( 0, 0, 0 );
    // Initial horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // Initial vertical angle : none
    float verticalAngle = 0.0f;
    // Initial Field of View
    float initialFoV = 45.0f;

    float speed = 3.0f; // 3 units / second
    float mouseSpeed = 0.003f;
};

