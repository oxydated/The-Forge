#include "CmdRing.h"

CmdRingElement CmdRing::getNextElement(bool cyclePool, uint32_t cmdCount)
{
    return getNextGpuCmdRingElement(&cmdRing, cyclePool, cmdCount);
}