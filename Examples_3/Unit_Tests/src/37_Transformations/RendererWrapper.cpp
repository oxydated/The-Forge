#include "RendererWrapper.h"

Renderer* RendererWrapper::renderer = NULL;

Renderer* RendererWrapper::getRenderer() { return renderer; }