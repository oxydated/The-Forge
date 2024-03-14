#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class Command;

class GPUQueue
{
public:
    GPUQueue();

    void submit();

    void present();

private:
    Queue* pQueue;
};
