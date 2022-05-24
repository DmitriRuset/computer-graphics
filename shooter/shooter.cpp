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

#include <shooter/plumbob.hpp>
#include <shooter/window.hpp>
#include <shooter/colored_figure.hpp>


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

