#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <array>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <common/shader.hpp>
#include <common/controls.hpp>

#include <shooter/plumbob.hpp>
#include <shooter/window.hpp>
#include <shooter/colored_figures.hpp>
#include <shooter/enemy.hpp>


static KeyboardAndMouse* controller = nullptr;

Enemy CreateEnemyRandom() {
    const int MinDistance = 2;
    const int MaxDistance = 20;


    float x_rot = rand() % 51 - 25;
    float y_rot = rand() % 51 - 25;
    float z_rot = rand() % 51 - 25;

    glm::vec3 rot_dim(x_rot, y_rot, z_rot);
    rot_dim = glm::normalize(rot_dim);


    float x_pos = (rand() % (MaxDistance - MinDistance) + MinDistance) / glm::sqrt(3);
    float y_pos = (rand() % (MaxDistance - MinDistance) + MinDistance) / glm::sqrt(3);
    float z_pos = (rand() % (MaxDistance - MinDistance) + MinDistance) / glm::sqrt(3);

    glm::vec3 pos(x_pos, y_pos, z_pos);

    pos += controller->getCameraPosition();

    float angle = rand() % 360; // because in degrees

    return {pos, rot_dim, angle};
}

int main() {

    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    Window window;
    KeyboardAndMouse interaction;

    controller = &interaction;

    window.CreateWindow(1680, 1050, "Second task");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);


    glm::vec3 pos = {0, 0, 0};

    PlumBob pb; // Model

    ColoredFigures figures{pb, "Enemy.vertexshader", "Enemy.fragmentshader"}; // For drawing model

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    const int MaxEnemies = 20;

    int cnt_enemies = 0;
    int ticks = 0;

    do{
        // Clear the screen
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        interaction.computeMatricesFromInputs(window);

        figures.Draw(Projection);

        ++ticks;

        if (ticks % 50 == 0) {
            std::cout << "Pos: " << controller->getCameraPosition().x << " " <<
            controller->getCameraPosition().y << " " << controller->getCameraPosition().z << std::endl;
        }

        if (ticks % 100 == 0 && cnt_enemies < MaxEnemies) {
            cnt_enemies++;
            Enemy enemy = CreateEnemyRandom();
            figures.AddEnemy(enemy);
            auto coords = enemy.GetPos();
            std::cout << "New enemy on coords: " << coords.x << " " << coords.y << " " << coords.z << std::endl;
        }

        // Swap buffers
        window.SwapBuffers();
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while(!window.ExitTime());

    // Cleanup VBO
    glDeleteVertexArrays(1, &VertexArrayID);

    return 0;
}

