#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class RenderTargetWrapper;

class SwapChainWrapper
{
public:
    RenderTargetWrapper* acquireNextImage();
};
