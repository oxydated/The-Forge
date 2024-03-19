#include "RendererWrapper.h"

Renderer* RendererWrapper::renderer = NULL;

Renderer* RendererWrapper::getRenderer() { return renderer; }

bool RendererWrapper::initRenderer(const char* appName)
{
    // window and renderer setup
    RendererDesc settings;
    memset(&settings, 0, sizeof(settings));
    settings.mD3D11Supported = true;
    settings.mGLESSupported = true;
    ::initRenderer(appName, &settings, &renderer);

    // check for init success
    if (!renderer)
        return false;
    return true;
}
