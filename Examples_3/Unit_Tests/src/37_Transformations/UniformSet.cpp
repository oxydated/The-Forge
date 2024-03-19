#include "UniformSet.h"
#include "Signature.h"
#include "RendererWrapper.h"

UniformSet::UniformSet(Signature* rootSignature, std::vector<uniformParams> params)
{
    BufferLoadDesc ubDesc = {};
    ubDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
    ubDesc.mDesc.mFlags = BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT;
    ubDesc.pData = NULL;
    for (auto &param : params)
    {
        Buffer* uniBuf = NULL;
        ubDesc.mDesc.pName = param.name.c_str();
        ubDesc.mDesc.mSize = param.bufferSize;
        ubDesc.ppBuffer = &uniBuf;
        addResource(&ubDesc, NULL);
        uniformBuffers.push_back(uniBuf);
    }
    uniformBuffers.shrink_to_fit();

    /// create DescriptorSets

    DescriptorSetDesc desc = { rootSignature->getRootSignature(), DESCRIPTOR_UPDATE_FREQ_PER_FRAME, (uint32_t)uniformBuffers.size() };
    addDescriptorSet(RendererWrapper::getRenderer(), &desc, &pDescriptorSetUniforms);

    // Prepare descriptor sets

    for (uint32_t i = 0; i < uniformBuffers.size(); ++i)
    {
        DescriptorData descParams[1] = {};
        descParams[0].pName = params[i].uniformName.c_str();
        descParams[0].ppBuffers = &uniformBuffers[i];
        updateDescriptorSet(RendererWrapper::getRenderer(), i, pDescriptorSetUniforms, 1, descParams);
    }
}

void UniformSet::update(uint32_t index, const void* source, size_t size)
{
    // update uniform

    BufferUpdateDesc skyboxViewProjCbv = { uniformBuffers[index] };
    beginUpdateResource(&skyboxViewProjCbv);
    memcpy(skyboxViewProjCbv.pMappedData, source, size);
    endUpdateResource(&skyboxViewProjCbv);
}

DescriptorSet* UniformSet::getDescriptorSet() { return pDescriptorSetUniforms; }
