#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class IApp;

class RenderTargetWrapper
{
public:

    uint32_t getWidth();

    uint32_t getHeight();

    RenderTarget* getRenderTarget();

    static RenderTargetWrapper* createRenderTargetWrapper(IApp*);

    static RenderTargetWrapper* createRenderTargetWrapper(RenderTarget*);

private:
    RenderTarget* renderTarget;

    RenderTargetWrapper(RenderTarget* rt);
};
