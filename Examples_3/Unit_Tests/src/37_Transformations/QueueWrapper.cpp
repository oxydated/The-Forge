#include "QueueWrapper.h"
#include "Command.h"
#include "SwapChainWrapper.h"
#include "RendererWrapper.h"
#include "../../../../Common_3/Resources/ResourceLoader/Interfaces/IResourceLoader.h"

QueueWrapper::QueueWrapper()
{
    // create Queue
    QueueDesc queueDesc = {};
    queueDesc.mType = QUEUE_TYPE_GRAPHICS;
    queueDesc.mFlag = QUEUE_FLAG_INIT_MICROPROFILE;
    addQueue(RendererWrapper::getRenderer(), &queueDesc, &queue);
}

QueueWrapper::~QueueWrapper() { removeQueue(RendererWrapper::getRenderer(), queue); }

void QueueWrapper::submit(Command* command, Fence* signalFence, Semaphore** waitSemaphores, Semaphore** signalSemaphores)
{
    FlushResourceUpdateDesc flushUpdateDesc = {};
    flushUpdateDesc.mNodeIndex = 0;
    flushResourceUpdates(&flushUpdateDesc);
    Semaphore* waitSemaphoresArray[2] = { flushUpdateDesc.pOutSubmittedSemaphore, waitSemaphores[0] };

    QueueSubmitDesc submitDesc = {};
    submitDesc.mCmdCount = 1;
    submitDesc.mSignalSemaphoreCount = 1;
    submitDesc.mWaitSemaphoreCount = TF_ARRAY_COUNT(waitSemaphoresArray);
    Cmd* cmd = command->getCommand();
    submitDesc.ppCmds = &cmd;

    submitDesc.ppSignalSemaphores = signalSemaphores;
    submitDesc.ppWaitSemaphores = waitSemaphoresArray;
    submitDesc.pSignalFence = signalFence;
    queueSubmit(queue, &submitDesc);
}

#ifndef __not_in_the_path_yet__

void QueueWrapper::present(uint32_t swapchainImageIndex, SwapChainWrapper* chain, uint32_t waitSemaphoresCount, Semaphore** waitSemaphores)
{
    QueuePresentDesc presentDesc = {};
    presentDesc.mIndex = swapchainImageIndex;
    presentDesc.mWaitSemaphoreCount = waitSemaphoresCount;
    presentDesc.pSwapChain = chain->getSwapChain();
    presentDesc.ppWaitSemaphores = waitSemaphores;
    presentDesc.mSubmitDone = true;
    queuePresent(queue, &presentDesc);
}

void QueueWrapper::waitForIdleQueue(SwapChainWrapper* chain, IApp* app)
{
    SwapChain* pSwapChain = chain->getSwapChain();
    if (pSwapChain->mEnableVsync != app->mSettings.mVSyncEnabled)
    {
        waitQueueIdle(queue);
        ::toggleVSync(RendererWrapper::getRenderer(), &pSwapChain);
    }
}

void QueueWrapper::waitForIdleQueue() { waitQueueIdle(queue); }

Queue* QueueWrapper::getQueue() { return queue; }

#endif