#include "CastleApp.h"
#include "RenderTargetWrapper.h"

// We only need Two sets of resources (one in flight and one being used on CPU)
const uint32_t gDataBufferCount = 2;
//
//bool CastleApp::addSwapChain()
//{
//    SwapChainDesc swapChainDesc = {};
//    swapChainDesc.mWindowHandle = pWindow->handle;
//    swapChainDesc.mPresentQueueCount = 1;
//    //swapChainDesc.ppPresentQueues = &pGraphicsQueue;
//    swapChainDesc.mWidth = mSettings.mWidth;
//    swapChainDesc.mHeight = mSettings.mHeight;
//    //swapChainDesc.mImageCount = getRecommendedSwapchainImageCount(pRenderer, &pWindow->handle);
//    //swapChainDesc.mColorFormat = getSupportedSwapchainFormat(pRenderer, &swapChainDesc, COLOR_SPACE_SDR_SRGB);
//    swapChainDesc.mColorSpace = COLOR_SPACE_SDR_SRGB;
//    swapChainDesc.mEnableVsync = mSettings.mVSyncEnabled;
//    swapChainDesc.mFlags = SWAP_CHAIN_CREATION_FLAG_ENABLE_FOVEATED_RENDERING_VR;
//    //::addSwapChain(pRenderer, &swapChainDesc, &pSwapChain);
//
//    //return pSwapChain != NULL;
//    return false;
//}
//
//bool CastleApp::addDepthBuffer()
//{
//    // Add depth buffer
//    RenderTargetDesc depthRT = {};
//    depthRT.mArraySize = 1;
//    depthRT.mClearValue.depth = 0.0f;
//    depthRT.mClearValue.stencil = 0;
//    depthRT.mDepth = 1;
//    depthRT.mFormat = TinyImageFormat_D32_SFLOAT;
//    depthRT.mStartState = RESOURCE_STATE_DEPTH_WRITE;
//    depthRT.mHeight = mSettings.mHeight;
//    depthRT.mSampleCount = SAMPLE_COUNT_1;
//    depthRT.mSampleQuality = 0;
//    depthRT.mWidth = mSettings.mWidth;
//    depthRT.mFlags = TEXTURE_CREATION_FLAG_ON_TILE | TEXTURE_CREATION_FLAG_VR_MULTIVIEW;
//    //addRenderTarget(pRenderer, &depthRT, &pDepthBuffer);
//
//    //return pDepthBuffer != NULL;
//    return false;
//}
//
//void CastleApp::addDescriptorSets()
//{
//    //DescriptorSetDesc desc = { pRootSignature, DESCRIPTOR_UPDATE_FREQ_NONE, 1 };
//    //addDescriptorSet(pRenderer, &desc, &pDescriptorSetTexture);
//    //desc = { pRootSignature, DESCRIPTOR_UPDATE_FREQ_PER_FRAME, gDataBufferCount * 2 };
//    //addDescriptorSet(pRenderer, &desc, &pDescriptorSetUniforms);
//}
//
//void CastleApp::removeDescriptorSets()
//{
//    //removeDescriptorSet(pRenderer, pDescriptorSetTexture);
//    //removeDescriptorSet(pRenderer, pDescriptorSetUniforms);
//}

#ifndef __not_in_the_path_yet__

struct commandRecordObjects
{
    Command*             cmd;
    RenderTargetWrapper* renderTarget;
    RenderTargetWrapper* depthBuffer;
    PipelineWrapper* pipeline;
    TextureSet* textures;
    UniformSet* uniforms;
    BufferResource* vertexBuffer;
};

void CastleApp::Draw() 
{
    // Wait for Idle Queue
    graphicsQueue->waitForIdleQueue(chain);

    // acquire next image index in Swapchain and get respective RenderTarget

    RenderTargetWrapper* pRenderTarget = chain->acquireNextImage();

    // get element from CmdRing

    CmdRingElement* elem = CommandRing->getNextElement();

    // Stall if CPU is running "gDataBufferCount" frames ahead of GPU

    elem->waitForFence();

    //if (pRenderer->pGpu->mSettings.mGpuBreadcrumbs)
    //{
    //    // Check breadcrumb markers
    //    checkMarkers();
    //}

    // update uniform

    skyUniform->update(&skyUniformHostBlock, sizeof(UniformBlockSky));

    // Reset cmd pool for this frame

    elem->resetCommandPool();

    // record command buffer

    Command* cmd = elem->getCommandByIndex(0);

    cmd->recordCommand(&CastleApp::commandsToRecord);

    // simply record the screen cleaning command

    // draw skybox

    // Queue Submit
    graphicsQueue->submit();

    // Queue present
    graphicsQueue->present();

    // Closing draw, preparing for next image in Swapchain
}
#else
void CastleApp::Draw()
{
}

#endif

void CastleApp::commandsToRecord(void* data)
{
    commandRecordObjects* recObjs = static_cast<commandRecordObjects*>(data);
    float                 width = recObjs->renderTarget->getWidth();
    float                 height = recObjs->renderTarget->getHeight();

    // if (pRenderer->pGpu->mSettings.mGpuBreadcrumbs)
    //{
    //     // Reset markers values
    //     resetMarkers(cmd);
    // }

    // RenderTargetBarrier barriers[] = {
    //     { pRenderTarget, RESOURCE_STATE_PRESENT, RESOURCE_STATE_RENDER_TARGET },
    // };
    // cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 1, barriers);
    RenderTargetWrapperBarrier barrier = { recObjs->renderTarget, RESOURCE_STATE_PRESENT, RESOURCE_STATE_RENDER_TARGET };
    recObjs->cmd->ResourceBarrier({ &barrier });

    //// simply record the screen cleaning command

    // BindRenderTargetsDesc bindRenderTargets = {};
    // bindRenderTargets.mRenderTargetCount = 1;
    // bindRenderTargets.mRenderTargets[0] = { pRenderTarget, LOAD_ACTION_CLEAR };
    // bindRenderTargets.mDepthStencil = { pDepthBuffer, LOAD_ACTION_CLEAR };
    // cmdBindRenderTargets(cmd, &bindRenderTargets);
    recObjs->cmd->BindRenderTargetAndClean(recObjs->renderTarget, recObjs->depthBuffer);

    // cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);
    recObjs->cmd->setViewPort(0, 0, width, height, 0, 1);

    // cmdSetScissor(cmd, 0, 0, pRenderTarget->mWidth, pRenderTarget->mHeight);
    recObjs->cmd->setScissor(0, 0, width, height);

    // const uint32_t skyboxVbStride = sizeof(float) * 4;

    //// draw skybox
    // cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 1.0f, 1.0f);

    // cmdBindPipeline(cmd, pSkyBoxDrawPipeline);

    // cmdBindDescriptorSet(cmd, 0, pDescriptorSetTexture);

    // cmdBindDescriptorSet(cmd, gFrameIndex * 2 + 0, pDescriptorSetUniforms);

    // cmdBindVertexBuffer(cmd, 1, &pSkyBoxVertexBuffer, &skyboxVbStride, NULL);

    // cmdDraw(cmd, 36, 0);

    // cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);

    // bindRenderTargets = {};
    // bindRenderTargets.mRenderTargetCount = 1;
    // bindRenderTargets.mRenderTargets[0] = { pRenderTarget, LOAD_ACTION_LOAD };
    // cmdBindRenderTargets(cmd, &bindRenderTargets);

    // cmdDrawUserInterface(cmd);

    // cmdBindRenderTargets(cmd, NULL);

    // barriers[0] = { pRenderTarget, RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_PRESENT };
    // cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 1, barriers);
}
