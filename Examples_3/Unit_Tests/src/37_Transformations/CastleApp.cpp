#include "CastleApp.h"

// We only need Two sets of resources (one in flight and one being used on CPU)
const uint32_t gDataBufferCount = 2;

bool CastleApp::addSwapChain()
{
    SwapChainDesc swapChainDesc = {};
    swapChainDesc.mWindowHandle = pWindow->handle;
    swapChainDesc.mPresentQueueCount = 1;
    swapChainDesc.ppPresentQueues = &pGraphicsQueue;
    swapChainDesc.mWidth = mSettings.mWidth;
    swapChainDesc.mHeight = mSettings.mHeight;
    swapChainDesc.mImageCount = getRecommendedSwapchainImageCount(pRenderer, &pWindow->handle);
    swapChainDesc.mColorFormat = getSupportedSwapchainFormat(pRenderer, &swapChainDesc, COLOR_SPACE_SDR_SRGB);
    swapChainDesc.mColorSpace = COLOR_SPACE_SDR_SRGB;
    swapChainDesc.mEnableVsync = mSettings.mVSyncEnabled;
    swapChainDesc.mFlags = SWAP_CHAIN_CREATION_FLAG_ENABLE_FOVEATED_RENDERING_VR;
    ::addSwapChain(pRenderer, &swapChainDesc, &pSwapChain);

    return pSwapChain != NULL;
}

bool CastleApp::addDepthBuffer()
{
    // Add depth buffer
    RenderTargetDesc depthRT = {};
    depthRT.mArraySize = 1;
    depthRT.mClearValue.depth = 0.0f;
    depthRT.mClearValue.stencil = 0;
    depthRT.mDepth = 1;
    depthRT.mFormat = TinyImageFormat_D32_SFLOAT;
    depthRT.mStartState = RESOURCE_STATE_DEPTH_WRITE;
    depthRT.mHeight = mSettings.mHeight;
    depthRT.mSampleCount = SAMPLE_COUNT_1;
    depthRT.mSampleQuality = 0;
    depthRT.mWidth = mSettings.mWidth;
    depthRT.mFlags = TEXTURE_CREATION_FLAG_ON_TILE | TEXTURE_CREATION_FLAG_VR_MULTIVIEW;
    addRenderTarget(pRenderer, &depthRT, &pDepthBuffer);

    return pDepthBuffer != NULL;
}

void CastleApp::addDescriptorSets()
{
    //DescriptorSetDesc desc = { pRootSignature, DESCRIPTOR_UPDATE_FREQ_NONE, 1 };
    //addDescriptorSet(pRenderer, &desc, &pDescriptorSetTexture);
    //desc = { pRootSignature, DESCRIPTOR_UPDATE_FREQ_PER_FRAME, gDataBufferCount * 2 };
    //addDescriptorSet(pRenderer, &desc, &pDescriptorSetUniforms);
}

void CastleApp::removeDescriptorSets()
{
    removeDescriptorSet(pRenderer, pDescriptorSetTexture);
    removeDescriptorSet(pRenderer, pDescriptorSetUniforms);
}
