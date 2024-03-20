#include "UniformSet.h"
#include "Signature.h"
#include "RendererWrapper.h"

UniformSet::UniformSet(Signature* rootSignature, std::vector<uniformParams> params, uint32_t numBuffers)
{
    BufferLoadDesc ubDesc = {};
    ubDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_CPU_TO_GPU;
    ubDesc.mDesc.mFlags = BUFFER_CREATION_FLAG_PERSISTENT_MAP_BIT;
    ubDesc.pData = NULL;
    for (uint32_t i = 0; i < numBuffers; i++)
    {
        for (auto& param : params)
        {
            Buffer* uniBuf = NULL;
            ubDesc.mDesc.pName = param.name.c_str();
            ubDesc.mDesc.mSize = param.bufferSize;
            ubDesc.ppBuffer = &uniBuf;
            addResource(&ubDesc, NULL);
            uniformBuffers.push_back(uniBuf);
        }
    }
    uniformBuffers.shrink_to_fit();

    /// create DescriptorSets

    DescriptorSetDesc desc = { rootSignature->getRootSignature(), DESCRIPTOR_UPDATE_FREQ_PER_FRAME, numBuffers * 2 };
    addDescriptorSet(RendererWrapper::getRenderer(), &desc, &pDescriptorSetUniforms);

    // Prepare descriptor sets

    std::vector<DescriptorData> descParams(params.size());
    for (uint32_t i = 0; i < uniformBuffers.size(); ++i)
    {
        for (uint32_t j = 0; j < params.size(); j++)
        {
            descParams[j].pName = params[j].uniformName.c_str();
            descParams[j].ppBuffers = &uniformBuffers[i];
            updateDescriptorSet(RendererWrapper::getRenderer(), i * 2 + j, pDescriptorSetUniforms, 1, &descParams[j]);
        }
    }
}

UniformSet::~UniformSet()
{
    removeDescriptorSet(RendererWrapper::getRenderer(), pDescriptorSetUniforms);
    for (auto& buf : uniformBuffers)
    {
        if (buf)
        {
            removeResource(buf);
        }
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
