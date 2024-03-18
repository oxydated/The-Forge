#include "UniformSet.h"

void UniformSet::update(uint32_t index, const void* source, size_t size)
{
    // update uniform

    BufferUpdateDesc skyboxViewProjCbv = { uniformBuffer[index] };
    beginUpdateResource(&skyboxViewProjCbv);
    memcpy(skyboxViewProjCbv.pMappedData, source, size);
    endUpdateResource(&skyboxViewProjCbv);
}