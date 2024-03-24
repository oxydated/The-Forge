#include "PipelineWrapper.h"

#include "../../../../Common_3/Utilities/Math/MathTypes.h"

#include "RenderTargetWrapper.h"
#include "RendererWrapper.h"
#include "Signature.h"

PipelineWrapper::PipelineWrapper(Signature* rootSignature, std::string shaderName, VertexLayout vertexLayout,
                                 RenderTargetWrapper* swapChainRT,
                                 RenderTargetWrapper* depthBufferRT, CullMode cullingMode)
{
    RasterizerStateDesc rasterizerStateDesc = {};
    rasterizerStateDesc.mCullMode = cullingMode;

    RenderTarget* swapChainRenderTarget = swapChainRT->getRenderTarget();

    GraphicsPipelineDesc pipelineSettings = {};
    pipelineSettings.pVertexLayout = &vertexLayout;
    pipelineSettings.mPrimitiveTopo = PRIMITIVE_TOPO_TRI_LIST;
    pipelineSettings.mRenderTargetCount = 1;
    pipelineSettings.pColorFormats = &swapChainRenderTarget->mFormat;
    pipelineSettings.mSampleCount = swapChainRenderTarget->mSampleCount;
    pipelineSettings.mSampleQuality = swapChainRenderTarget->mSampleQuality;
    pipelineSettings.pRootSignature = rootSignature->getRootSignature();
    ;
    pipelineSettings.mVRFoveatedRendering = true;
    pipelineSettings.pRasterizerState = &rasterizerStateDesc;
    pipelineSettings.pShaderProgram = rootSignature->getShaderByName(shaderName);

    DepthStateDesc depthStateDesc = {};

    if (depthBufferRT)
    {
        depthStateDesc.mDepthTest = true;
        depthStateDesc.mDepthWrite = true;
        depthStateDesc.mDepthFunc = CMP_GEQUAL;
        RenderTarget* depthBufferRenderTarget = depthBufferRT->getRenderTarget();
        pipelineSettings.pDepthState = &depthStateDesc;
        pipelineSettings.mDepthStencilFormat = depthBufferRenderTarget->mFormat;
    }
    else
    {
        pipelineSettings.pDepthState = NULL;
    }

    PipelineDesc desc = {};
    desc.mType = PIPELINE_TYPE_GRAPHICS;
    desc.mGraphicsDesc = pipelineSettings;

    addPipeline(RendererWrapper::getRenderer(), &desc, &pipeline);
}

Pipeline* PipelineWrapper::getPipeline() { return pipeline; }

PipelineWrapper::~PipelineWrapper() { removePipeline(RendererWrapper::getRenderer(), pipeline); }
