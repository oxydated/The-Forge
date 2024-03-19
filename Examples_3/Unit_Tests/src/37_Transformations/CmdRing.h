#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

#include "CmdRingElement.h"

class QueueWrapper;

class CmdRing
{
public:
    CmdRing(QueueWrapper*);

    void remove();

    CmdRingElement getNextElement(bool cyclePool, uint32_t cmdCount);

private:
    GpuCmdRing cmdRing;
};