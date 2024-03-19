#include "SwapChainWrapper.h"
#include "RendererWrapper.h"
#include "RenderTargetWrapper.h"

#ifndef __not_in_path_yet__

acquiredRenderTarget SwapChainWrapper::acquireNextImageFromSwapChain()
{
    acquiredRenderTarget acquiredImageFromSwapChain = {};
    acquireNextImage(RendererWrapper::getRenderer(), swapChain, imageAcquiredSemaphore, NULL,
                     &(acquiredImageFromSwapChain.swapchainImageIndex));
    acquiredImageFromSwapChain.renderTarget =
        RenderTargetWrapper::createRenderTargetWrapper(swapChain->ppRenderTargets[acquiredImageFromSwapChain.swapchainImageIndex]);
    return acquiredImageFromSwapChain;
}

Semaphore* SwapChainWrapper::getImageAcquiredSemaphore() { return imageAcquiredSemaphore; }

SwapChain* SwapChainWrapper::getSwapChain() { return swapChain; }

#endif
