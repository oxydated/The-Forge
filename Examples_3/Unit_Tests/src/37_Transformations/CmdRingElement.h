#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"
#include "../../../../Common_3/Utilities/RingBuffer.h"

class Command;

class CmdRingElement
{
public:
    CmdRingElement(GpuCmdRingElement);

    void waitForFence();

    void resetCommandPool();

    Semaphore* getSemaphore();

    Command* getCommandByIndex(uint32_t index);

    Fence* getFence();

private:
    GpuCmdRingElement elem;

};
