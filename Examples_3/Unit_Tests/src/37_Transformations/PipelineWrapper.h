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
    PipelineWrapper(Signature* rootSignature, std::string shaderName, RenderTargetWrapper* swapChainRT, RenderTargetWrapper* depthBufferRT);

    void remove();

    Pipeline* getPipeline();

    ~PipelineWrapper();

private:
    Pipeline* pipeline = NULL;
};