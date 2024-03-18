#pragma once

#include <vector>
#include <utility>
#include <map>
#include <string>
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class PipelineWrapper
{
public:
    PipelineWrapper(Renderer*, std::vector<std::pair<ShaderSemantic, TinyImageFormat>>, std::vector<Shader>, std::map<std::string, Sampler*>,
             RasterizerStateDesc, DepthStateDesc);

    PipelineWrapper(Renderer*, std::vector<std::pair<ShaderSemantic, TinyImageFormat>>, std::vector<Shader>, std::map<std::string, Sampler*>,
             RasterizerStateDesc);

    void remove();

    Pipeline* getPipeline();

private:
    Pipeline* pPipeline = NULL;
    Renderer* pRenderer = NULL;
};