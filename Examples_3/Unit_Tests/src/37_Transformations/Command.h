#pragma once

#include <vector>
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class IResourceSet;
class RenderTargetWrapper;
class BufferResource;
class QueueWrapper;

class Command
{
public:
    Command(Cmd*);

    void BindSet(IResourceSet*);

    void BindRenderTargetAndLoad(RenderTargetWrapper*, RenderTargetWrapper*);

    void BindRenderTargetAndClean(RenderTargetWrapper*, RenderTargetWrapper*);

    void BindVertexBuffer(std::vector<BufferResource*>);

    void ResourceBarrier(std::vector<RenderTargetWrapper*>);

    void build();

    void submit(QueueWrapper*);

private:
    Cmd* cmd;
};
