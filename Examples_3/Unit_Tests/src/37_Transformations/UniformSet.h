#pragma once

#include <vector>
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"
#include "../../../../Common_3/Resources/ResourceLoader/Interfaces/IResourceLoader.h"

class Signature;

class UniformSet
{
public:
    UniformSet(Signature*);

    void update(uint32_t index, const void* source, size_t size);

    DescriptorSet* getDescriptorSet();

private:
    DescriptorSet*       pDescriptorSetUniforms;
    std::vector<Buffer*> uniformBuffer;
};
