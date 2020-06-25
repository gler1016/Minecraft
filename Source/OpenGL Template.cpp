#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core\OpenGL Classes\VertexBuffer.h"
#include "Core\OpenGL Classes\VertexArray.h"
#include "Core\OpenGL Classes\IndexBuffer.h"
#include "Core\OpenGL Classes\Shader.h"
#include "Core\OpenGL Classes\Texture.h"

#include "Core\Renderer\Renderer.h"
#include "Core\FpsCamera.h"
#include "Core\CubeRenderer.h"
//#include "Core\Camera.h"
//
//#include "Core\Chunk.h"

#include <iostream>

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

using namespace GLClasses;

void CameraMove(GLFWwindow* window, double xpos, double ypos);

Minecraft::FPSCamera camera(45.0f, SCR_WIDTH / SCR_HEIGHT, 0.1, 100.0f);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);
    glewInit();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, CameraMove);
    glEnable(GL_DEPTH_TEST);

    Texture texture("Core\\Resources\\grass_block.png");


    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
    Minecraft::CubeRenderer cb;
    Minecraft::Chunk chunk;
    Minecraft::Renderer renderer;

    float angle = 0;
    const int block_count = 16;

    float* cube_address;

    glm::vec3 cube_position = glm::vec3(0.0f, 0.0f, -4.0f);

    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            for (int k = 0; k < 16; k++)
            {
                chunk.AddBlock(Minecraft::BlockType::Dirt, glm::vec3(i, j, k));
            }
        }
    }

    chunk.Construct();

    while (!glfwWindowShouldClose(window))
    {
        angle += 3;
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        renderer.RenderChunk(&chunk, &camera);
        //cb.RenderCube(cube_position, &texture, 0, camera.GetViewProjection());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    const float camera_speed = 0.25f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ChangePosition(camera.GetFront() * camera_speed);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ChangePosition(-(camera.GetFront() * camera_speed));

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ChangePosition(-(camera.GetRight() * camera_speed));

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ChangePosition(camera.GetRight() * camera_speed);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ChangePosition(camera.GetUp() * camera_speed);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ChangePosition(-(camera.GetUp() * camera_speed));
}

void CameraMove(GLFWwindow* window, double xpos, double ypos)
{
    camera.UpdateOnMouseMovement(xpos, ypos);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    camera.SetAspect(width / height);
}
