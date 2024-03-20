#include "TextureSet.h"
#include "Signature.h"
#include "RendererWrapper.h"

TextureSet::TextureSet(Signature* rootSignature, std::vector<textureParams> params)
{ 
    // Loads textures

    std::vector<Texture**> texPointers(params.size());
    for (auto& pTex : texPointers)
    {
        pTex = new Texture*;    
    }

    int i = 0;
    for (auto& param : params)
    {
        TextureLoadDesc textureDesc = {};
        textureDesc.pFileName = param.fileName.c_str();
        textureDesc.ppTexture = texPointers[i];
        // Textures representing color should be stored in SRGB or HDR format
        textureDesc.mCreationFlag = TEXTURE_CREATION_FLAG_SRGB;
        addResource(&textureDesc, NULL);
        i++;
    }

    waitForAllResourceLoads();

    for (auto& pTex : texPointers)
    {
        textures.push_back(*pTex);
        delete pTex;
        pTex = nullptr;
    }

    /// create DescriptorSets

    DescriptorSetDesc desc = { rootSignature->getRootSignature(), DESCRIPTOR_UPDATE_FREQ_NONE, 1 };
    addDescriptorSet(RendererWrapper::getRenderer(), &desc, &pDescriptorSetTexture);

    // Prepare descriptor sets

    std::vector<DescriptorData> descParams(textures.size());
    for (uint32_t i = 0; i < textures.size(); ++i)
    {
        //DescriptorData descParams[1] = {};
        descParams[i].pName = params[i].textureName.c_str();
        descParams[i].ppTextures = &textures[i];
    }
    updateDescriptorSet(RendererWrapper::getRenderer(), 0, pDescriptorSetTexture, (uint32_t)textures.size(), descParams.data());
}

TextureSet::~TextureSet()
{
    removeDescriptorSet(RendererWrapper::getRenderer(), pDescriptorSetTexture);
    for (auto& tex : textures)
    {
        if (tex)
        {
            removeResource(tex);
        }
    }
}

DescriptorSet* TextureSet::getDescriptorSet() { return pDescriptorSetTexture; }