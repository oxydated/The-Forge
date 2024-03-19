#include "CmdRing.h"
#include "QueueWrapper.h"
#include "RendererWrapper.h"

CmdRing::CmdRing(): cmdRing() {}

CmdRing::CmdRing(QueueWrapper* queue, uint32_t poolCount)
{
    // create GPUCmdRing

    GpuCmdRingDesc cmdRingDesc = {};
    cmdRingDesc.pQueue = queue->getQueue();
    cmdRingDesc.mPoolCount = poolCount;
    cmdRingDesc.mCmdPerPoolCount = 1;
    cmdRingDesc.mAddSyncPrimitives = true;
    addGpuCmdRing(RendererWrapper::getRenderer(), &cmdRingDesc, &cmdRing);
}

CmdRingElement CmdRing::getNextElement(bool cyclePool, uint32_t cmdCount)
{
    return getNextGpuCmdRingElement(&cmdRing, cyclePool, cmdCount);
}