#include "SwapChainWrapper.h"
#include "RendererWrapper.h"
#include "RenderTargetWrapper.h"
#include "QueueWrapper.h"
#include "../../../../Common_3/Application/Interfaces/IApp.h"

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

RenderTargetWrapper* SwapChainWrapper::getRenderTargetByIndex(uint32_t index)
{
    return RenderTargetWrapper::createRenderTargetWrapper(swapChain->ppRenderTargets[index]);
}

SwapChainWrapper* SwapChainWrapper::createSwapChainWrapper(QueueWrapper* queueWrapper, IApp* app)
{
    SwapChain*    newSwapChain = NULL;
    Queue*        queue = queueWrapper->getQueue();
    SwapChainDesc swapChainDesc = {};
    swapChainDesc.mWindowHandle = app->pWindow->handle;
    swapChainDesc.mPresentQueueCount = 1;
    swapChainDesc.ppPresentQueues = &queue;
    swapChainDesc.mWidth = app->mSettings.mWidth;
    swapChainDesc.mHeight = app->mSettings.mHeight;
    swapChainDesc.mImageCount = getRecommendedSwapchainImageCount(RendererWrapper::getRenderer(), &app->pWindow->handle);
    swapChainDesc.mColorFormat = getSupportedSwapchainFormat(RendererWrapper::getRenderer(), &swapChainDesc, COLOR_SPACE_SDR_SRGB);
    swapChainDesc.mColorSpace = COLOR_SPACE_SDR_SRGB;
    swapChainDesc.mEnableVsync = app->mSettings.mVSyncEnabled;
    swapChainDesc.mFlags = SWAP_CHAIN_CREATION_FLAG_ENABLE_FOVEATED_RENDERING_VR;
    ::addSwapChain(RendererWrapper::getRenderer(), &swapChainDesc, &newSwapChain);
    if (!newSwapChain)
    {
        return nullptr;
    }
    return new SwapChainWrapper(newSwapChain);
}

SwapChainWrapper::SwapChainWrapper(SwapChain* pSwapChain): swapChain(pSwapChain) {}

#endif
