#include "Signature.h"

#include "../../../../Common_3/Resources/ResourceLoader/Interfaces/IResourceLoader.h"

#include "RendererWrapper.h"

Signature::Signature(std::vector<std::string> samplerNames, std::vector<shaderCreationParameters> shaderParams) 
{
    // Creates Sampler

    std::vector<const char*> samplerNamesVector;
    std::vector<Sampler*>    samplersVector;
    for (auto& samplerName : samplerNames)
    {
        Sampler*    newSampler = NULL;
        SamplerDesc samplerDesc = { FILTER_LINEAR,
                                    FILTER_LINEAR,
                                    MIPMAP_MODE_NEAREST,
                                    ADDRESS_MODE_CLAMP_TO_EDGE,
                                    ADDRESS_MODE_CLAMP_TO_EDGE,
                                    ADDRESS_MODE_CLAMP_TO_EDGE };
        addSampler(RendererWrapper::getRenderer(), &samplerDesc, &newSampler);
        samplers[samplerName] = newSampler;
        samplerNamesVector.push_back(samplerName.c_str());
        samplersVector.push_back(newSampler);
    }
    samplerNamesVector.shrink_to_fit();
    samplersVector.shrink_to_fit();

	// load shaders

    int shaderIndex = 0; 
    std::vector<std::string> shaderNames;
    std::vector<Shader*>     shaderVector;
    for (auto& param : shaderParams)
    {
        Shader*        newShader = NULL;
        ShaderLoadDesc shaderDesc = {};
        for (auto& stage : param.shaderStageFileNames)
        {
            shaderDesc.mStages[shaderIndex++].pFileName = stage.c_str();
        }

        addShader(RendererWrapper::getRenderer(), &shaderDesc, &newShader);
        shaders[param.shaderName] = newShader;
        shaderNames.push_back(param.shaderName);
        shaderVector.push_back(newShader);  
    }
    shaderNames.shrink_to_fit();
    shaderVector.shrink_to_fit();
	
	// add root signature

    RootSignatureDesc rootDesc = {};
    rootDesc.mStaticSamplerCount = (uint32_t)samplerNamesVector.size();
    rootDesc.ppStaticSamplerNames = samplerNamesVector.data();
    rootDesc.ppStaticSamplers = samplersVector.data();
    rootDesc.mShaderCount = (uint32_t)shaderVector.size();
    rootDesc.ppShaders = shaderVector.data();
    addRootSignature(RendererWrapper::getRenderer(), &rootDesc, &rootSignature);
}

RootSignature* Signature::getRootSignature() { return rootSignature; }

Shader* Signature::getShaderByName(std::string shaderName) { 
    auto i = shaders.find(shaderName);
    if (i != shaders.end())
        return (*i).second;
    return nullptr;
}
