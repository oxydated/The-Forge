#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class RenderTargetWrapper
{
public:
    float getWidth();
    float getHeight();

    RenderTarget* getRenderTarget();

private:
    RenderTarget* pRenderTarget;
};
