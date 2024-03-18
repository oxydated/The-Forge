#include "CmdRingElement.h"
#include "RendererWrapper.h"
#include "Command.h"

void CmdRingElement::waitForFence() 
{ 
    // Stall if CPU is running frames ahead of GPU
    FenceStatus fenceStatus;
    getFenceStatus(RendererWrapper::getRenderer(), elem.pFence, &fenceStatus);
    if (fenceStatus == FENCE_STATUS_INCOMPLETE)
        waitForFences(RendererWrapper::getRenderer(), 1, &elem.pFence);
}

void CmdRingElement::resetCommandPool() { resetCmdPool(RendererWrapper::getRenderer(), elem.pCmdPool); }

Semaphore* CmdRingElement::getSemaphore() { return elem.pSemaphore; }

Command* CmdRingElement::getCommandByIndex(uint32_t index) { return new Command(elem.pCmds[index]); }

Fence* CmdRingElement::getFence() { return elem.pFence; }
