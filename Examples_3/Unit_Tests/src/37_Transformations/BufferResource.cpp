#include "BufferResource.h"
#include "../../../../Common_3/Resources/ResourceLoader/Interfaces/IResourceLoader.h"

BufferResource::BufferResource(const void* data, uint64_t dataSize, DescriptorType type, ResourceMemoryUsage usage)
{
    // Loads buffer
    BufferLoadDesc bufferDesc = {};
    bufferDesc.mDesc.mDescriptors = type;
    bufferDesc.mDesc.mMemoryUsage = usage;
    bufferDesc.mDesc.mSize = dataSize;
    bufferDesc.pData = data;
    bufferDesc.ppBuffer = &buffer;
    addResource(&bufferDesc, NULL);
}