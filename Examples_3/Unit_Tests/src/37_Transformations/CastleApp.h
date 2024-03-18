#ifndef __CASTLEAPP__H__
#define __CASTLEAPP__H__

// Renderer
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

#include "Command.h"
#include "CmdRing.h"
#include "QueueWrapper.h"
#include "SwapChainWrapper.h"
//#include "IAppCommand.h"
#include "projections.h"
#include "UniformSet.h"
#include "PipelineWrapper.h"
#include "TextureSet.h"
#include "UniformSet.h"
#include "BufferResource.h"

class CastleApp: public IApp
{
private:

    UniformBlockSky skyUniformHostBlock;

    // wrappers

    CmdRing           CommandRing;
    QueueWrapper*     graphicsQueue = NULL;
    SwapChainWrapper* chain = NULL;
    PipelineWrapper*  skyBoxDrawPipeline;
    TextureSet*       skyBoxTextures;
    UniformSet*       skyUniforms;
    BufferResource*   skyBoxVertexBuffer;

    // Buffers

    std::vector<Buffer*> vertexBuffers;

    // frame couting

    int frameIndex = 0;
    int totalFrameBuffers;

    void incrementFrameIndex();


    //Queue* pGraphicsQueue = NULL;
    //Renderer*  pRenderer = NULL;
    //SwapChain* pSwapChain = NULL;

    //// Depth Buffer
    //RenderTarget* pDepthBuffer = NULL;

    ////// Root Signature
    ////RootSignature* pRootSignature = NULL;

    //// texture Descriptor Set
    //DescriptorSet* pDescriptorSetTexture = { NULL };

    //// Uniform Descriptor Set
    //DescriptorSet* pDescriptorSetUniforms = { NULL };

    //bool addSwapChain();

    //bool addDepthBuffer();

    //void addDescriptorSets();

    //void removeDescriptorSets();

    //void addRootSignatures();

    //void removeRootSignatures();

    //void addShaders();

    //void removeShaders();

    //void addPipelines();

    //void removePipelines();

    //void prepareDescriptorSets();

    //void initMarkers();

    //void checkMarkers();

    //void resetMarkers(Cmd* pCmd);

    static void commandsToRecord(void*);

    void Draw() override;
};

#endif