#pragma once
#include "InputDevice.h"
#include "Engine/BaseApplication.h"
class KeyBoard :
    public InputDevice
{
public:

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

