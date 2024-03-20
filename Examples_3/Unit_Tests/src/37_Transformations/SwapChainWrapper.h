#pragma once

#include <vector>
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class RenderTargetWrapper;
class QueueWrapper;
class IApp;

struct acquiredRenderTarget
{
    RenderTargetWrapper* renderTarget;
    uint32_t             swapchainImageIndex;
};

class SwapChainWrapper
{
public:
    acquiredRenderTarget acquireNextImageFromSwapChain();
    Semaphore*           getImageAcquiredSemaphore();
    SwapChain*           getSwapChain();
    RenderTargetWrapper* getRenderTargetByIndex(uint32_t index);
    

    static SwapChainWrapper* createSwapChainWrapper(QueueWrapper*, IApp*);

    ~SwapChainWrapper();

private:
    SwapChainWrapper(SwapChain* pSwapChain, Semaphore* pSemaphore);
    SwapChain* swapChain = NULL;
    Semaphore* imageAcquiredSemaphore = NULL;
};
