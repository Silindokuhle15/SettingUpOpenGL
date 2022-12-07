#include "KeyBoard.h"

void KeyBoard::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            //Move Forward
            BaseApplication::cam_ptr->MoveForward();

            break;
        case GLFW_KEY_A:
            //Move Left
            BaseApplication::cam_ptr->MoveLeft();
            break;
        case GLFW_KEY_S:
            //Move Backward
            BaseApplication::cam_ptr->MoveBackward();
            break;
        case GLFW_KEY_D:
            //Move Right
            BaseApplication::cam_ptr->MoveRight();
            break;
        case GLFW_KEY_Q:
            BaseApplication::cam_ptr->MoveUp();
            break;

        case GLFW_KEY_Z:
            BaseApplication::cam_ptr->MoveDown();
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
}
