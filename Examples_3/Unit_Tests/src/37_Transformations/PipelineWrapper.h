#pragma once

#include <vector>
#include <utility>
#include <map>
#include <string>
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class RenderTargetWrapper;
class Signature;

class PipelineWrapper
{
public:
    PipelineWrapper(Signature* rootSignature, std::string shaderName, VertexLayout vertexLayout, RenderTargetWrapper* swapChainRT,
                    RenderTargetWrapper* depthBufferRT,
                    CullMode cullingMode);

    void remove();

    Pipeline* getPipeline();

    ~PipelineWrapper();

private:
    Pipeline* pipeline = NULL;
};