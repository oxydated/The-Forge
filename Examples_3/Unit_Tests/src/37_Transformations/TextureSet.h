#pragma once

#include <vector>
#include <string>

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"
#include "../../../../Common_3/Resources/ResourceLoader/Interfaces/IResourceLoader.h"

class Signature;

struct textureParams
{
    std::string fileName;
    std::string textureName;
};

class TextureSet
{
public:
    TextureSet(Signature* rootSignature, std::vector<textureParams> params);

    DescriptorSet* getDescriptorSet();

private:
    DescriptorSet* pDescriptorSetTexture;
    std::vector<Texture*> textures;
};