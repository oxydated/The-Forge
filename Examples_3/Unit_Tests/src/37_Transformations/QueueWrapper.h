#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class Command;
class SwapChainWrapper;

class QueueWrapper
{
public:
    QueueWrapper();

    void submit();

    void present();

    void waitForIdleQueue(SwapChainWrapper*);

private:
    Queue* pQueue;
};
