#include <cstdio>
#include <cstdlib>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>

class Triangle {
public:
    explicit Triangle(const GLfloat* coords, size_t size) {
        glGenBuffers(1, &vertex_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        glBufferData(GL_ARRAY_BUFFER, size, coords, GL_STATIC_DRAW);
    }

    void Draw(size_t attrib) const {
        glEnableVertexAttribArray(attrib);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        glVertexAttribPointer(
                attrib,             // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                nullptr            // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

        glDisableVertexAttribArray(attrib);
    }

    ~Triangle() {
        glDeleteBuffers(1, &vertex_buffer_);
    }
private:
    GLuint vertex_buffer_{};
};

glm::mat4 ViewMatrix;

glm::mat4 getViewMatrix(){
    return ViewMatrix;
}



// Let camera moves like in showcases. y = 0


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 );
// Initial horizontal angle : toward -Z
float h_angle = 0;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;


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

    GLFWwindow* window;

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow( 1024, 768, "First task", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint program_red = LoadShaders("RedTriangleVS.vertexshader", "RedTriangleFS.fragmentshader");
    GLuint program_blue = LoadShaders("BlueTriangleVS.vertexshader", "BlueTriangleFS.fragmentshader");


    static const GLfloat g_vertex_buffer_data_red[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f,  1.0f, 0.0f,
    };

    static const GLfloat g_vertex_buffer_data_blue[] = {
            -1.0f, 1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
            0.0f,  -1.0f, 0.0f,
    };

    Triangle triangle_red{g_vertex_buffer_data_red, sizeof(g_vertex_buffer_data_red)};
    Triangle triangle_blue{g_vertex_buffer_data_blue, sizeof(g_vertex_buffer_data_blue)};

    GLuint Matrix_red = glGetUniformLocation(program_red, "MVP");
    GLuint Matrix_blue = glGetUniformLocation(program_blue, "MVP");

//    glm::mat4 View = glm::lookAt(
//            glm::vec3(0,0,2),
//            glm::vec3(0,0,0),
//            glm::vec3(0,1,0)
//    );

    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    do{
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT );


        computeMatricesFromInputs();
        glm::mat4 View = getViewMatrix();
        glm::mat4 MVP = Projection * View * Model;

        // Use our shader
        glUseProgram(program_blue);
        glUniformMatrix4fv(Matrix_blue, 1, GL_FALSE, &MVP[0][0]);
        triangle_blue.Draw(1);

        glUseProgram(program_red);
        glUniformMatrix4fv(Matrix_red, 1, GL_FALSE, &MVP[0][0]);
        triangle_red.Draw(0);



        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(program_red);
    glDeleteProgram(program_blue);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

