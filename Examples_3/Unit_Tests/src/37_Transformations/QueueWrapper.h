#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class Command;
class SwapChainWrapper;
class IApp;

class QueueWrapper
{
public:
    QueueWrapper();

    ~QueueWrapper();

    void submit(Command* command, Fence* signalFence, Semaphore** waitSemaphores, Semaphore** signalSemaphores);

    void present(uint32_t swapchainImageIndex, SwapChainWrapper* chain, uint32_t waitSemaphoresCount, Semaphore** waitSemaphores);

    void waitForIdleQueue(SwapChainWrapper* chain, IApp* app);

    void waitForIdleQueue();

    Queue* getQueue();

private:
    Queue* queue = nullptr;
};
