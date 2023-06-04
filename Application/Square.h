#pragma once
#include "Application.h"

class Square : public Application
{
public:

    void OnInit() override;
    void OnUpdate(float ts) override;

    virtual void MoveBackward() override;
    virtual void MoveForward() override;
    virtual void MoveLeft() override;
    virtual void MoveRight() override;
    virtual void MoveUp() override;
    virtual void MoveDown() override;
};