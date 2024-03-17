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

    Command* getCommandByIndex(uint32_t index);

private:
    GpuCmdRingElement element;

};
