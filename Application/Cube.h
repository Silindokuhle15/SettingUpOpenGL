#pragma once
#include "Application/Application.h"
#include "Engine/ObjectLoader.h"

class Cube :
    public Application
{
public:
    glm::mat4 m_Transform;

    void OnInit();
};