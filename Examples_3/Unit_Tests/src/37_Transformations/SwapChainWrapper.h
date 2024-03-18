#pragma once

#include <vector>
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class RenderTargetWrapper;

struct acquiredRenderTarget
{
    RenderTargetWrapper* renderTarget;
    uint32_t             swapchainImageIndex;
};

class SwapChainWrapper
{
public:
    acquiredRenderTarget acquireNextImage();
    Semaphore*           getImageAcquiredSemaphore();
    SwapChain*           getSwapChain();

private:
    SwapChain* swapChain = NULL;
    Semaphore* imageAcquiredSemaphore = NULL;
};
