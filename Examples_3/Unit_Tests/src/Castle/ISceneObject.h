#pragma once

#include <vector>
#include <string>
#include "TextureSet.h"

class ISceneObject
{
public:
    virtual std::vector<textureParams> getTexturesToLoad() = 0;

    virtual const void* getVertexData() = 0;

    virtual uint64_t getVertexDataSize() = 0;

    virtual const void* getUniformData() = 0;

    virtual uint64_t getUniformDataSize() = 0;

    virtual VertexLayout getVertexLayout() = 0;
};
