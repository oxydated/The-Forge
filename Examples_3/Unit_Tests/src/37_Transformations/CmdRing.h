#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

#include "CmdRingElement.h"

class QueueWrapper;

class CmdRing
{
public:
    CmdRing();

    CmdRing(QueueWrapper* queue, uint32_t poolCount);

    void remove();

    CmdRingElement getNextElement(bool cyclePool, uint32_t cmdCount);

private:
    GpuCmdRing cmdRing;
};