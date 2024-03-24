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
#include "BufferResource.h"
#include "PipelineWrapper.h"
#include "Signature.h"
#include "TextureSet.h"
#include "UniformSet.h"
#include "UserInterfaceWrapper.h"

#include "SkyBoxObj.h"
#include "CastleObj.h"

class CastleApp: public IApp
{
private:

    // wrappers

    CmdRing              CommandRing = {};
    QueueWrapper*        graphicsQueue = NULL;
    SwapChainWrapper*    chain = NULL;
    RenderTargetWrapper* depthBuffer = NULL;

    PipelineWrapper*     skyBoxDrawPipeline = NULL;
    TextureSet*          skyBoxTextures = NULL;
    UniformSet*          skyUniforms = NULL;
    BufferResource*      skyBoxVertexBuffer = NULL;
    std::vector<Buffer*> skyBoxVertexBuffers;

    PipelineWrapper*     castleDrawPipeline = NULL;
    TextureSet*          castleTextures = NULL;
    UniformSet*          castleUniforms = NULL;
    BufferResource*      castleIndexBuffer = NULL;
    BufferResource*      castleVertexBuffer = NULL;
    std::vector<Buffer*> castleVertexBuffers;

    Signature* rootSignature = NULL;

    // User Interface
    UserInterfaceWrapper UI;

    // Buffers


    // frame couting

    uint32_t frameIndex = 0;
    uint32_t totalFrameBuffers = 2;

    // Scene objects

    SkyBoxObj* skyBox = NULL;
    CastleObj* castle = NULL;

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