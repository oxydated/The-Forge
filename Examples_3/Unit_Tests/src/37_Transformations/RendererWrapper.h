#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class RendererWrapper
{
public:
    static Renderer* getRenderer();

private:
    static Renderer* renderer;
};
