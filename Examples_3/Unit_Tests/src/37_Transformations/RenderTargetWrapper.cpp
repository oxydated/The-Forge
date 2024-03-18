#include "RenderTargetWrapper.h"

RenderTargetWrapper::RenderTargetWrapper(RenderTarget* rt): renderTarget(rt) {}

uint32_t RenderTargetWrapper::getWidth() { return renderTarget->mWidth; }

uint32_t RenderTargetWrapper::getHeight() { return renderTarget->mHeight; }

RenderTarget* RenderTargetWrapper::getRenderTarget() { return renderTarget; }
