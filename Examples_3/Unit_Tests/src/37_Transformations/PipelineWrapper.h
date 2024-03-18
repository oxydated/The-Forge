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
    PipelineWrapper(Signature*, RenderTargetWrapper*, RenderTargetWrapper*);

    void remove();

    Pipeline* getPipeline();

private:
    Pipeline* pipeline = NULL;
    Renderer* pRenderer = NULL;
};