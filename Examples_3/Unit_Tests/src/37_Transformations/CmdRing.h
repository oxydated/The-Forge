#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

#include "CmdRingElement.h"

class CmdRing
{
public:
    CmdRing();

    void remove();

    CmdRingElement getNextElement(bool cyclePool, uint32_t cmdCount);

private:
    GpuCmdRing cmdRing;
};