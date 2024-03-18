#include "Command.h"
#include "RenderTargetWrapper.h"
#include "PipelineWrapper.h"
#include "TextureSet.h"
#include "UniformSet.h"

void Command::BindRenderTargetAndLoad(RenderTargetWrapper* pRenderTarget, RenderTargetWrapper* pDepthBuffer)
{
    BindRenderTargetsDesc bindRenderTargets = {};
    bindRenderTargets = {};
    bindRenderTargets.mRenderTargetCount = 1;
    bindRenderTargets.mRenderTargets[0] = { pRenderTarget->getRenderTarget(), LOAD_ACTION_LOAD };
    if (pDepthBuffer)
    {
        bindRenderTargets.mDepthStencil = { pDepthBuffer->getRenderTarget(), LOAD_ACTION_LOAD };
    }
    cmdBindRenderTargets(cmd, &bindRenderTargets);
}

void Command::BindRenderTargetAndClean(RenderTargetWrapper* pRenderTarget, RenderTargetWrapper* pDepthBuffer)
{
    BindRenderTargetsDesc bindRenderTargets = {};
    bindRenderTargets = {};
    bindRenderTargets.mRenderTargetCount = 1;
    bindRenderTargets.mRenderTargets[0] = { pRenderTarget->getRenderTarget(), LOAD_ACTION_CLEAR };
    if (pDepthBuffer)
    {
        bindRenderTargets.mDepthStencil = { pDepthBuffer->getRenderTarget(), LOAD_ACTION_CLEAR };
    }
    cmdBindRenderTargets(cmd, &bindRenderTargets);
}

void Command::UnbindRenderTarget() { cmdBindRenderTargets(cmd, NULL); }

void Command::BindPipeline(PipelineWrapper* pipelineWrapper) { cmdBindPipeline(cmd, pipelineWrapper->getPipeline()); }

void Command::BindTextureSet(TextureSet* textures, uint32_t index) { cmdBindDescriptorSet(cmd, index, textures->getDescriptorSet()); }

void Command::BindUniformSet(UniformSet* uniforms, uint32_t index) { cmdBindDescriptorSet(cmd, index, uniforms->getDescriptorSet()); }

void Command::BindVertexBuffer(uint32_t bufferCount, Buffer** buffers, const uint32_t* strides, const uint64_t offsets)
{
    cmdBindVertexBuffer(cmd, 1, buffers, strides, NULL);
}

void Command::ResourceBarrier(uint32_t numBufferBarriers, BufferBarrier* bufferBarriers, uint32_t numTextureBarriers, TextureBarrier* textureBarriers,
                              uint32_t numRenderTargetBarriers, RenderTargetBarrier* renderTargetBarriers)
{
    cmdResourceBarrier(cmd, numBufferBarriers, bufferBarriers, numTextureBarriers, textureBarriers, numRenderTargetBarriers,
                       renderTargetBarriers);
}

void Command::setViewPort(float x, float y, float width, float height, float min, float max) 
{
    cmdSetViewport(cmd,  x,  y,  width,  height, min, max);
}

void Command::setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    cmdSetScissor(cmd, x, y, width, height); }

void Command::recordCommand(std::function<void(void*)> commandsToRecord, void* data) { 
    beginCmd(cmd);
    commandsToRecord(data);
    endCmd(cmd);
}

void Command::draw(uint32_t vertexCount, uint32_t firstVertex) { cmdDraw(cmd, vertexCount, firstVertex); }

Cmd* Command::getCommand() { return cmd; }
