#pragma once

#include <vector>
#include <string>

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"
#include "../../../../Common_3/Resources/ResourceLoader/Interfaces/IResourceLoader.h"

class Signature;

class TextureSet
{
public:
    TextureSet(Signature*, std::vector<std::string>);

    DescriptorSet* getDescriptorSet();

private:
    DescriptorSet* pDescriptorSetTexture;
};