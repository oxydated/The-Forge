#pragma once

#include <vector>
#include <functional>
#include "IAppCommand.h"
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class IResourceSet;
class RenderTargetWrapper;
class BufferResource;
class QueueWrapper;

struct RenderTargetWrapperBarrier
{
    RenderTargetWrapper* renderTarget;
    ResourceState        fromState;
    ResourceState        ToState;
};

//typedef void (IAppCommand::*commandsToRecord)(void*);

class Command
{
public:
    Command(Cmd*);

    void BindSet(IResourceSet*);

    void BindRenderTargetAndLoad(RenderTargetWrapper*, RenderTargetWrapper*);

    void BindRenderTargetAndClean(RenderTargetWrapper*, RenderTargetWrapper*);

    void UnbindRenderTarget();

    void BindVertexBuffer(std::vector<BufferResource*>);

    void ResourceBarrier(std::vector<RenderTargetWrapperBarrier*>);

    void setViewPort(float x, float y, float width, float height, float min, float max);

    void setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    void recordCommand(std::function<void(void*)>);

    void submit(QueueWrapper*);

private:
    Cmd* cmd;
};
