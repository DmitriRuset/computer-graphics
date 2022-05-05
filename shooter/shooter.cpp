#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <array>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>
#include <common/controls.hpp>

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
    }
private:
    uint32_t triangles_cnt_{};
    GLuint vertex_buffer_{};
    GLuint color_buffer_{};
};


struct PlumBob {
public:
    GLfloat* GetVertices() {
        return vertices;
    }

    GLfloat* GetColors() {
        return colors;
    }

    size_t SizeOf() const {
        return 36 * 3 * sizeof(GLfloat);
    }
private:
    GLfloat vertices[36 * 3] = {
            -0.25f,  0.0f,  0.0f,
            -0.125f, 0.0f, 0.216506f,
            0.0f, -0.625f, 0.0f, //

            0.125f, 0.0f, 0.216506f,
            -0.125f, 0.0f, 0.216506f,
            0.0f, -0.625f, 0.0f, //

            0.125f, 0.0f, 0.216506f,
            0.25f, 0.0f, 0.0f,
            0.0f, -0.625f, 0.0f, //

            -0.25f,  0.0f,  0.0f,
            -0.125f, 0.0f, -0.216506f,
            0.0f, -0.625f, 0.0f, //

            0.125f, 0.0f, -0.216506f,
            -0.125f, 0.0f, -0.216506f,
            0.0f, -0.625f, 0.0f, //

            0.125f, 0.0f, -0.216506f,
            0.25f, 0.0f, 0.0f,
            0.0f, -0.625f, 0.0f, //

            -0.25f,  0.0f,  0.0f,
            -0.125f, 0.0f, 0.216506f,
            0.0f, 0.625f, 0.0f, //

            0.125f, 0.0f, 0.216506f,
            -0.125f, 0.0f, 0.216506f,
            0.0f, 0.625f, 0.0f, //

            0.125f, 0.0f, 0.216506f,
            0.25f, 0.0f, 0.0f,
            0.0f, 0.625f, 0.0f, //

            -0.25f,  0.0f,  0.0f,
            -0.125f, 0.0f, -0.216506f,
            0.0f, 0.625f, 0.0f, //

            0.125f, 0.0f, -0.216506f,
            -0.125f, 0.0f, -0.216506f,
            0.0f, 0.625f, 0.0f, //

            0.125f, 0.0f, -0.216506f,
            0.25f, 0.0f, 0.0f,
            0.0f, 0.625f, 0.0f, //
    };

    GLfloat colors[36 * 3] = {
            0.338f, 0.66f, 0.2f,
            0.387f, 0.968f, 0.44f,
            0.288f, 0.6f, 0.144f,

            0.266f, 0.66f, 0.46f,
            0.207f, 0.526f, 0.324f,
            0.305f, 0.694f, 0.203f,

            0.48f, 0.768f, 0.278f,
            0.172f, 0.991f, 0.121f,
            0.236f, 0.529f, 0.276f,

            0.187f, 0.751f, 0.371f,
            0.253f, 0.894f, 0.432f,
            0.417f, 0.911f, 0.309f,

            0.275f, 0.573f, 0.338f,
            0.187f, 0.671f, 0.144f,
            0.3f, 0.741f, 0.488f,

            0.204f, 0.503f, 0.261f,
            0.373f, 0.867f, 0.214f,
            0.46f, 0.844f, 0.287f,

            0.355f, 0.625f, 0.494f,
            0.252f, 0.622f, 0.175f,
            0.315f, 0.642f, 0.19f,

            0.491f, 0.696f, 0.185f,
            0.175f, 0.852f, 0.302f,
            0.351f, 0.844f, 0.135f,

            0.34f, 0.659f, 0.347f,
            0.226f, 0.689f, 0.446f,
            0.419f, 0.519f, 0.42f,

            0.48f, 0.927f, 0.193f,
            0.317f, 0.557f, 0.114f,
            0.241f, 0.872f, 0.243f,

            0.413f, 0.711f, 0.491f,
            0.395f, 0.925f, 0.292f,
            0.468f, 0.963f, 0.238f,

            0.28f, 0.776f, 0.168f,
            0.277f, 0.808f, 0.309f,
            0.407f, 0.96f, 0.361f
    };
};


glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
    return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
    return ProjectionMatrix;
}


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
    float mouseSpeed = 0.005f;
};



int main() {

    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    Window window;
    KeyboardAndMouse interaction;

    window.CreateWindow(1680, 1050, "Second task");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint program_fig = LoadShaders("Figure.vertexshader", "Figure.fragmentshader");

    glm::vec3 pos = {0, 0, 0};

    PlumBob pb;

    ColoredFigure figure{pb.GetVertices(), pb.SizeOf(),
                         pb.GetColors(), pb.SizeOf(), 12};

    GLuint Matrix_fig = glGetUniformLocation(program_fig, "MVP");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    do{
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        interaction.computeMatricesFromInputs(window);
        glm::mat4 View = getViewMatrix();
        glm::mat4 MVP = Projection * View * Model;

        glUseProgram(program_fig);
        glUniformMatrix4fv(Matrix_fig, 1, GL_FALSE, &MVP[0][0]);
        figure.Draw();

        // Swap buffers
        window.SwapBuffers();
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while(!window.ExitTime());

    // Cleanup VBO
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(program_fig);

    return 0;
}

