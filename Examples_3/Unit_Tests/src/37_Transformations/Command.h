#pragma once

#include <vector>
#include <functional>
#include "IAppCommand.h"
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class IResourceSet;
class RenderTargetWrapper;
class BufferResource;
class QueueWrapper;
class PipelineWrapper;
class TextureSet;
class UniformSet;

//typedef void (IAppCommand::*commandsToRecord)(void*);

class Command
{
public:
    Command(Cmd*);

    void BindSet(IResourceSet*);

    void BindRenderTargetAndLoad(RenderTargetWrapper*, RenderTargetWrapper*);

    void BindRenderTargetAndClean(RenderTargetWrapper*, RenderTargetWrapper*);

    void UnbindRenderTarget();

    void BindPipeline(PipelineWrapper*);

    void BindTextureSet(TextureSet*, uint32_t index);

    void BindUniformSet(UniformSet*, uint32_t index);

    void BindVertexBuffer(uint32_t bufferCount, Buffer** buffers, const uint32_t* strides, const uint64_t offsets);

    void ResourceBarrier(uint32_t numBufferBarriers, BufferBarrier*, uint32_t numTextureBarriers, TextureBarrier*,
                         uint32_t numRenderTargetBarriers, RenderTargetBarrier*);

    void setViewPort(float x, float y, float width, float height, float min, float max);

    void setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    void recordCommand(std::function<void(void*)>, void*);

    void submit(QueueWrapper*);

    void draw(uint32_t vertexCount, uint32_t firstVertex);

    Cmd* getCommand();

private:
    Cmd* cmd;
};
