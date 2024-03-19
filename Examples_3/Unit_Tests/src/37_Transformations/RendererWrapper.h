#pragma once
#include <string>
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"


class RendererWrapper
{
public:
    static Renderer* getRenderer();
    static bool      initRenderer(std::string);

private:
    static Renderer* renderer;
};
