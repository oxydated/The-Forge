#pragma once

#include <vector>
#include <string>

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"
#include "../../../../Common_3/Resources/ResourceLoader/Interfaces/IResourceLoader.h"

class Signature;

struct uniformParams
{
    std::string name;
    std::string uniformName;
    uint64_t    bufferSize;
};

class UniformSet
{
public:
    UniformSet(Signature* rootSignature, std::vector<uniformParams> params, uint32_t numBuffers);

    void update(uint32_t index, const void* source, size_t size);

    DescriptorSet* getDescriptorSet();

private:
    DescriptorSet*       pDescriptorSetUniforms;
    std::vector<Buffer*> uniformBuffers;
};
