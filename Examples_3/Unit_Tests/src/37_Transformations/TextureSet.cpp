#include "TextureSet.h"
#include "Signature.h"
#include "RendererWrapper.h"

TextureSet::TextureSet(Signature* rootSignature, std::vector<textureParams> params)
{ 
    // Loads textures
    for (auto& param : params)
    {
        Texture*        tex = NULL;
        TextureLoadDesc textureDesc = {};
        textureDesc.pFileName = param.fileName.c_str();
        textureDesc.ppTexture = &tex;
        // Textures representing color should be stored in SRGB or HDR format
        textureDesc.mCreationFlag = TEXTURE_CREATION_FLAG_SRGB;
        addResource(&textureDesc, NULL);    
    }

    waitForAllResourceLoads();

    /// create DescriptorSets

    DescriptorSetDesc desc = { rootSignature->getRootSignature(), DESCRIPTOR_UPDATE_FREQ_NONE, 1 };
    addDescriptorSet(RendererWrapper::getRenderer(), &desc, &pDescriptorSetTexture);

    // Prepare descriptor sets

    std::vector<DescriptorData> descParams(textures.size());
    for (uint32_t i = 0; i < textures.size(); ++i)
    {
        DescriptorData descParams[1] = {};
        descParams[0].pName = params[i].textureName.c_str();
        descParams[0].ppTextures = &textures[i];
    }
    updateDescriptorSet(RendererWrapper::getRenderer(), 0, pDescriptorSetTexture, 1, descParams.data());
}

DescriptorSet* TextureSet::getDescriptorSet() { return pDescriptorSetTexture; }