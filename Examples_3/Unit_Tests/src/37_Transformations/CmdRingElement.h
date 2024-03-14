#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"
#include "../../../../Common_3/Utilities/RingBuffer.h"

class CmdRingElement
{
public:
    CmdRingElement(GpuCmdRingElement);

    void waitForFence();

private:
    GpuCmdRingElement element;

};
