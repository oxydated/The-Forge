#ifndef __CASTLEAPP__H__
#define __CASTLEAPP__H__

#include "../../../../Common_3/Application/Interfaces/IApp.h"

// Renderer
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class CastleApp: public IApp
{
private:

    Queue* pGraphicsQueue = NULL;
    Renderer*  pRenderer = NULL;
    SwapChain* pSwapChain = NULL;

    // Depth Buffer
    RenderTarget* pDepthBuffer = NULL;

    //// Root Signature
    //RootSignature* pRootSignature = NULL;

    // texture Descriptor Set
    DescriptorSet* pDescriptorSetTexture = { NULL };

    // Uniform Descriptor Set
    DescriptorSet* pDescriptorSetUniforms = { NULL };

    bool addSwapChain();

    bool addDepthBuffer();

    void addDescriptorSets();

    void removeDescriptorSets();

    //void addRootSignatures();

    //void removeRootSignatures();

    //void addShaders();

    void removeShaders();

    void addPipelines();

    void removePipelines();

    void prepareDescriptorSets();

    void initMarkers();

    void checkMarkers();

    void resetMarkers(Cmd* pCmd);
};

#endif