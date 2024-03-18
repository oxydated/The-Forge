#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class RenderTargetWrapper
{
public:
    RenderTargetWrapper(RenderTarget* rt);

    uint32_t getWidth();

    uint32_t getHeight();

    RenderTarget* getRenderTarget();

private:
    RenderTarget* renderTarget;
};
