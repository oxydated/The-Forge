#pragma once

#include <vector>
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class BufferResource
{
public:
    BufferResource(const void* data, uint64_t dataSize, DescriptorType type, ResourceMemoryUsage usage);

    ~BufferResource();

    Buffer* getBuffer();
    
    static std::vector<Buffer*> makeBuffersArray(std::vector<BufferResource*>);

private:
    Buffer* buffer = NULL;
};
