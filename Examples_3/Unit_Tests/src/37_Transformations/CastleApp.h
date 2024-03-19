#ifndef __CASTLEAPP__H__
#define __CASTLEAPP__H__

// Renderer
#include "../../../../Common_3/Application/Interfaces/IApp.h"
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

#include "CmdRing.h"
#include "Command.h"
#include "QueueWrapper.h"
#include "RenderTargetWrapper.h"
#include "SwapChainWrapper.h"
// #include "IAppCommand.h"
#include "BufferResource.h"
#include "PipelineWrapper.h"
#include "Signature.h"
#include "TextureSet.h"
#include "UniformSet.h"
#include "projections.h"

class CastleApp: public IApp
{
private:

    UniformBlockSky skyUniformHostBlock;

    // wrappers

    CmdRing              CommandRing = {};
    QueueWrapper*        graphicsQueue = NULL;
    SwapChainWrapper*    chain = NULL;
    RenderTargetWrapper* depthBuffer = NULL;
    PipelineWrapper*     skyBoxDrawPipeline = NULL;
    TextureSet*          skyBoxTextures = NULL;
    UniformSet*          skyUniforms = NULL;
    BufferResource*      skyBoxVertexBuffer = NULL;
    Signature*           rootSignature = NULL;

    // Buffers

    std::vector<Buffer*> vertexBuffers;

    // frame couting

    int frameIndex = 0;
    int totalFrameBuffers = 2;

    void incrementFrameIndex();

    static void commandsToRecord(void*);

public:
    CastleApp();

    virtual bool Init() override;
    virtual void Exit() override;

    virtual bool Load(ReloadDesc* pReloadDesc) override;
    virtual void Unload(ReloadDesc* pReloadDesc) override;

    virtual void Update(float deltaTime) override;

    virtual void Draw() override;

    virtual const char* GetName() override;
};

#endif