#include "RenderTargetWrapper.h"
#include "RendererWrapper.h"
#include "../../../../Common_3/Application/Interfaces/IApp.h"

RenderTargetWrapper::RenderTargetWrapper(RenderTarget* rt): renderTarget(rt) {}

uint32_t RenderTargetWrapper::getWidth() { return renderTarget->mWidth; }

uint32_t RenderTargetWrapper::getHeight() { return renderTarget->mHeight; }

RenderTarget* RenderTargetWrapper::getRenderTarget() { return renderTarget; }

RenderTargetWrapper::~RenderTargetWrapper() { removeRenderTarget(RendererWrapper::getRenderer(), renderTarget); }

RenderTargetWrapper* RenderTargetWrapper::createRenderTargetWrapper(IApp* app)
{
    // Add depth buffer

    RenderTarget*    newRenderTarget = NULL;
    RenderTargetDesc depthRT = {};
    depthRT.mArraySize = 1;
    depthRT.mClearValue.depth = 0.0f;
    depthRT.mClearValue.stencil = 0;
    depthRT.mDepth = 1;
    depthRT.mFormat = TinyImageFormat_D32_SFLOAT;
    depthRT.mStartState = RESOURCE_STATE_DEPTH_WRITE;
    depthRT.mHeight = app->mSettings.mHeight;
    depthRT.mSampleCount = SAMPLE_COUNT_1;
    depthRT.mSampleQuality = 0;
    depthRT.mWidth = app->mSettings.mWidth;
    depthRT.mFlags = TEXTURE_CREATION_FLAG_ON_TILE | TEXTURE_CREATION_FLAG_VR_MULTIVIEW;
    addRenderTarget(RendererWrapper::getRenderer(), &depthRT, &newRenderTarget);

    if (!newRenderTarget)
    {
        return nullptr;
    }

    return new RenderTargetWrapper(newRenderTarget);
}

RenderTargetWrapper* RenderTargetWrapper::createRenderTargetWrapper(RenderTarget* newRenderTarget)
{
    return new RenderTargetWrapper(newRenderTarget);
}
