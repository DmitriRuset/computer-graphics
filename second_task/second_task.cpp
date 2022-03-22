#include <cstdio>
#include <cstdlib>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>

class Window {
public:
    Window() {
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    int CreateWindow(int width, int height, const char* title,
                      GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr) {
        window_ = glfwCreateWindow(width, height, title, monitor, share);

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

glm::mat4 ViewMatrix;

glm::mat4 getViewMatrix(){
    return ViewMatrix;
}

// Let camera moves like in showcases. y = const
// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 );
// Initial horizontal angle : toward -Z
float h_angle = 0;

float speed = 3.0f; // 3 units / second

void computeMatricesFromInputs(){

    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);


    h_angle += deltaTime * speed;

    position = glm::vec3(2 * cos(h_angle), 1, 2 * sin(h_angle));


    // Camera matrix
    ViewMatrix = glm::lookAt(
            position,           // Camera is here
            glm::vec3(0, 0, 0), // and looks here : here are triangles
            glm::vec3(0, 1, 0)
    );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}



int main(void) {

    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    Window window;

    window.CreateWindow(1024, 768, "Second task");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint program_fig = LoadShaders("Figure.vertexshader", "Figure.fragmentshader");


    static const GLfloat g_vertex_buffer_data[] = {
            0.0f, -1.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            0.5f, 0.0f, -0.86f,

            0.0f, -1.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            0.5f, 0.0f, 0.86f,

            0.0f, -1.0f, 0.0f,
            0.5f, 0.0f, -0.86f,
            0.5f, 0.0f, 0.86f,

            0.0f, 1.0f, 0.0f,
            -1.0f, -0.0f, 0.0f,
            0.5f, -0.0f, -0.86f,

            0.0f, 1.0f, 0.0f,
            -1.0f, -0.0f, 0.0f,
            0.5f, -0.0f, 0.86f,

            0.0f, 1.0f, 0.0f,
            0.5f, -0.0f, -0.86f,
            0.5f, -0.0f, 0.86f,

            };
    static const GLfloat g_color_buffer_data[] = {
            0.583f, 0.771f, 0.014f,
            0.609f, 0.115f, 0.436f,
            0.327f, 0.483f, 0.844f,
            0.822f, 0.569f, 0.201f,
            0.140f, 0.616f, 0.489f,
            0.945f, 0.719f, 0.592f,
            0.053f, 0.959f, 0.120f,
            0.167f, 0.620f, 0.077f,
            0.055f, 0.953f, 0.042f,
            0.435f, 0.602f, 0.223f,
            0.310f, 0.747f, 0.185f,
            0.597f, 0.770f, 0.761f,
            0.559f, 0.436f, 0.730f,
            0.483f, 0.596f, 0.789f,
            0.014f, 0.184f, 0.576f,
            0.302f, 0.455f, 0.848f,
            0.673f, 0.211f, 0.457f,
            0.982f, 0.099f, 0.879f};


    ColoredFigure figure{g_vertex_buffer_data, sizeof(g_vertex_buffer_data),
                               g_color_buffer_data, sizeof(g_color_buffer_data), 6};

    GLuint Matrix_red = glGetUniformLocation(program_fig, "MVP");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    do{
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        computeMatricesFromInputs();
        glm::mat4 View = getViewMatrix();
        glm::mat4 MVP = Projection * View * Model;

        glUseProgram(program_fig);
        glUniformMatrix4fv(Matrix_red, 1, GL_FALSE, &MVP[0][0]);
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

