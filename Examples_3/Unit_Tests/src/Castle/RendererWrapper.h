#pragma once
#include <string>
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"


class RendererWrapper
{
public:
    static Renderer* getRenderer();
    static bool      initRenderer(const char* appName);
    static void      remove();

private:
    static Renderer* renderer;
};
