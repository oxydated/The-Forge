#include "CastleApp.h"
#include "RenderTargetWrapper.h"
#include "RendererWrapper.h"

// We only need Two sets of resources (one in flight and one being used on CPU)
const uint32_t gDataBufferCount = 2;

DEFINE_APPLICATION_MAIN(CastleApp)

struct commandRecordObjects
{
    Command*             cmd;
    RenderTargetWrapper* renderTarget;
    RenderTargetWrapper* depthBuffer;
    PipelineWrapper*     pipeline;
    TextureSet*          textures;
    UniformSet*          uniforms;
    Buffer**             indexBuffer;
    Buffer**             vertexBuffer;
    uint32_t             frameIndex;
    uint32_t             indicesCount;
    uint32_t             vertexStride;
};

void CastleApp::incrementFrameIndex() { frameIndex = (frameIndex + 1) % totalFrameBuffers; }

void CastleApp::Exit()
{
    // exit UI

    UI.ExitUIAlternative();

    // remove vertexBuffer

    if (skyBoxVertexBuffer)
    {
        delete skyBoxVertexBuffer;
        skyBoxVertexBuffer = nullptr;
    }
    skyBoxVertexBuffers.clear();

    if (castleVertexBuffer)
    {
        delete castleVertexBuffer;
        castleVertexBuffer = nullptr;    
    }
    castleVertexBuffers.clear();

    // remove indexBuffer

    if (castleIndexBuffer)
    {
        delete castleIndexBuffer;
        castleIndexBuffer = nullptr;    
    }

    // remove GpuCmdRing

    CommandRing.remove();

    // exit resouce loader

    exitResourceLoaderInterface(RendererWrapper::getRenderer());

    // remove Queue

    if (graphicsQueue)
    {
        delete graphicsQueue;
        graphicsQueue = nullptr;    
    }

    // exit Renderer

    RendererWrapper::remove();

    // delete scene objects

    if (skyBox)
    {
        delete skyBox;
        skyBox = nullptr;
    }
}

bool CastleApp::Init()
{

    // FILE PATHS
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_SHADER_BINARIES, "CompiledShaders");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_TEXTURES, "Textures");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_FONTS, "Fonts");
    fsSetPathForResourceDir(pSystemFileIO, RM_DEBUG, RD_SCREENSHOTS, "Screenshots");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_SCRIPTS, "Scripts");
    fsSetPathForResourceDir(pSystemFileIO, RM_DEBUG, RD_DEBUG, "Debug");

    // Initialize scene Objects

    skyBox = new SkyBoxObj(this);
    castle = new CastleObj(this);

    // window and renderer setup
    // check for init success
    if (!RendererWrapper::initRenderer(GetName()))
        return false;

    // create Queue
    graphicsQueue = new QueueWrapper();

    // create GPUCmdRing
    CommandRing = CmdRing(graphicsQueue, totalFrameBuffers);

    // create Image Acquisition - SwapChain semaphore (moved to SwapChain creation)

    // init resource loader interface (The Forge stuff)
    initResourceLoaderInterface(RendererWrapper::getRenderer());

    // Loads Skybox vertex buffer 
    skyBoxVertexBuffer = new BufferResource(skyBox->getVertexData(), skyBox->getVertexDataSize(), DESCRIPTOR_TYPE_VERTEX_BUFFER,
                                            RESOURCE_MEMORY_USAGE_GPU_ONLY);
    skyBoxVertexBuffers = { skyBoxVertexBuffer->getBuffer() };

    // Loads Castle vertex buffer
    castleVertexBuffer = new BufferResource(castle->getVertexData(), castle->getVertexDataSize(), DESCRIPTOR_TYPE_VERTEX_BUFFER,
                                            RESOURCE_MEMORY_USAGE_GPU_ONLY);
    castleVertexBuffers = { castleVertexBuffer->getBuffer() };

    // Loads Castle index buffer
    castleIndexBuffer = new BufferResource(castle->getIndexData(), castle->getIndexDataSize(), DESCRIPTOR_TYPE_INDEX_BUFFER,
                                            RESOURCE_MEMORY_USAGE_GPU_ONLY);

    // initialize UI and input

    if (!UI.initUIAlternative(this))
    {
        return false;
    }

    return true;
}

bool CastleApp::Load(ReloadDesc* pReloadDesc)
{
    if (pReloadDesc->mType & RELOAD_TYPE_SHADER)
    {
        /// Create Samplers
        /// load shaders
        /// create RootSignatures

        rootSignature = new Signature({ "uSampler0" }, { { "SkyBoxDrawShader", { "skybox.vert", "skybox.frag" } },
                                                         { "CastleDrawShader", { "castle.vert", "basic.frag" } } });

        /// create DescriptorSets
        /// prepare DescriptorSets (that could be a single step)
        
        skyBoxTextures = new TextureSet(rootSignature, skyBox->getTexturesToLoad());

        skyUniforms =
            new UniformSet(rootSignature, { { "SkyboxUniformBuffer", "uniformBlock", sizeof(skyBox->getUniformDataSize()) } }, totalFrameBuffers);

        castleTextures = new TextureSet(rootSignature, castle->getTexturesToLoad());

        castleUniforms =
            new UniformSet(rootSignature, { { "CastleUniformBuffer", "uniformBlock", castle->getUniformDataSize() } }, totalFrameBuffers);
    }

    if (pReloadDesc->mType & (RELOAD_TYPE_RESIZE | RELOAD_TYPE_RENDERTARGET))
    {
        /// create SwapChain

        chain = SwapChainWrapper::createSwapChainWrapper(graphicsQueue, this);
        if (!chain)
            return false;

        /// create DepthBuffer

        depthBuffer = RenderTargetWrapper::createRenderTargetWrapper(this);
        if (!depthBuffer)
            return false;
    }

    if (pReloadDesc->mType & (RELOAD_TYPE_SHADER | RELOAD_TYPE_RENDERTARGET))
    {
        /// create pipelines

        skyBoxDrawPipeline = new PipelineWrapper(rootSignature, "SkyBoxDrawShader", skyBox->getVertexLayout(),
                                                 chain->getRenderTargetByIndex(0), nullptr, CULL_MODE_NONE);

        castleDrawPipeline = new PipelineWrapper(rootSignature, "CastleDrawShader", castle->getVertexLayout(),
                                                 chain->getRenderTargetByIndex(0), depthBuffer, CULL_MODE_NONE);
    }

    // UI stuff

    UI.loadUI(pReloadDesc, this, chain->getRenderTargetByIndex(0), graphicsQueue);

    return true;
}

void CastleApp::Unload(ReloadDesc* pReloadDesc)
{
    // wait for idle queue

    graphicsQueue->waitForIdleQueue();

    // unload UI

    UI.unloadUI(pReloadDesc);

    if (pReloadDesc->mType & (RELOAD_TYPE_SHADER | RELOAD_TYPE_RENDERTARGET))
    {
        //remove Pipelines

        if (skyBoxDrawPipeline)
        {
            delete skyBoxDrawPipeline;
            skyBoxDrawPipeline = nullptr;
        }

        if (castleDrawPipeline)
        {
            delete castleDrawPipeline;
            castleDrawPipeline = nullptr;
        }
    }

    if (pReloadDesc->mType & (RELOAD_TYPE_RESIZE | RELOAD_TYPE_RENDERTARGET))
    {
        //remove SwapChain

        if (chain)
        {
            delete chain;
            chain = nullptr;        
        }

        // remove depthBuffer

        if (depthBuffer)
        {
            delete depthBuffer;
            depthBuffer = nullptr;
        }
    }

    if (pReloadDesc->mType & RELOAD_TYPE_SHADER)
    {
        // remove textures

        if (skyBoxTextures)
        {
            delete skyBoxTextures;
            skyBoxTextures = nullptr;
        }

        if (castleTextures)
        {
            delete castleTextures;
            castleTextures = nullptr;
        }

        // remove Uniforms

        if (skyUniforms)
        {
            delete skyUniforms;
            skyUniforms = nullptr;
        }

        if (castleUniforms)
        {
            delete castleUniforms;
            castleUniforms = nullptr;
        }

        // remove rootSignature

        if (rootSignature)
        {
            delete rootSignature;
            rootSignature = nullptr;
        }
    }
}

void CastleApp::Update(float deltaTime)
{
    UI.UpdateUI(deltaTime, this);

    /************************************************************************/
    // Scene Update
    /************************************************************************/
    static float currentTime = 0.0f;
    currentTime += deltaTime * 1000.0f;

    // update objects in scene
    
    skyBox->update(deltaTime, UI.getCameraController());

    castle->update(deltaTime, UI.getCameraController());
}

CastleApp::CastleApp():IApp() {}

void CastleApp::Draw()
{
    // Wait for Idle Queue
    graphicsQueue->waitForIdleQueue(chain, this);

    // acquire next image index in Swapchain and get respective RenderTarget

    acquiredRenderTarget acquiredImageFromSwapchain = chain->acquireNextImageFromSwapChain();

    // get element from CmdRing

    CmdRingElement elem = CommandRing.getNextElement(true, 1);

    // Stall if CPU is running "gDataBufferCount" frames ahead of GPU

    elem.waitForFence();

    // update uniform

    skyUniforms->update(frameIndex, skyBox->getUniformData(), skyBox->getUniformDataSize());

    castleUniforms->update(frameIndex, castle->getUniformData(), castle->getUniformDataSize());

    // Reset cmd pool for this frame

    elem.resetCommandPool();

    // record command buffer

    Command* cmd = elem.getCommandByIndex(0);

    commandRecordObjects skyRecObjs = {};
    skyRecObjs.cmd = cmd;
    skyRecObjs.renderTarget = acquiredImageFromSwapchain.renderTarget;
    skyRecObjs.depthBuffer = depthBuffer;
    skyRecObjs.pipeline = skyBoxDrawPipeline;
    skyRecObjs.textures = skyBoxTextures;
    skyRecObjs.uniforms = skyUniforms;
    skyRecObjs.indexBuffer = nullptr;
    skyRecObjs.vertexBuffer = skyBoxVertexBuffers.data();
    skyRecObjs.frameIndex = frameIndex;

    Buffer*              localCastleIndexBuffer = castleIndexBuffer->getBuffer();
    commandRecordObjects castleRecObjs = {};
    castleRecObjs.cmd = cmd;
    castleRecObjs.renderTarget = acquiredImageFromSwapchain.renderTarget;
    castleRecObjs.depthBuffer = depthBuffer;
    castleRecObjs.pipeline = castleDrawPipeline;
    castleRecObjs.textures = castleTextures;
    castleRecObjs.uniforms = castleUniforms;
    castleRecObjs.indexBuffer = &localCastleIndexBuffer;
    castleRecObjs.vertexBuffer = castleVertexBuffers.data();
    castleRecObjs.frameIndex = frameIndex;
    castleRecObjs.indicesCount = (uint32_t)castle->getNumIndexes();
    castleRecObjs.vertexStride = castle->getVertexStride();

    commandRecordObjects arrayCmdRec[2];
    arrayCmdRec[0] = skyRecObjs;
    arrayCmdRec[1] = castleRecObjs;

    cmd->recordCommand(&CastleApp::commandsToRecord, arrayCmdRec);

    // Queue Submit

    Semaphore* imageAcquiredSemaphore = chain->getImageAcquiredSemaphore();
    Semaphore* waitSemaphore = elem.getSemaphore();

    graphicsQueue->submit(cmd, elem.getFence(), &imageAcquiredSemaphore, &waitSemaphore);

    // Queue present

    graphicsQueue->present(acquiredImageFromSwapchain.swapchainImageIndex, chain, 1, &waitSemaphore);

    // Closing draw, preparing for next image in Swapchain

    incrementFrameIndex();
}

void CastleApp::commandsToRecord(void* data)
{
    commandRecordObjects* arrayCmdRec = static_cast<commandRecordObjects*>(data);
    commandRecordObjects& skyRecObjs = arrayCmdRec[0];
    commandRecordObjects& castleRecObjs = arrayCmdRec[1];



    uint32_t width = skyRecObjs.renderTarget->getWidth();
    uint32_t height = skyRecObjs.renderTarget->getHeight();

    RenderTargetBarrier barriers[] = { skyRecObjs.renderTarget->getRenderTarget(), RESOURCE_STATE_PRESENT, RESOURCE_STATE_RENDER_TARGET };
    skyRecObjs.cmd->ResourceBarrier(0, NULL, 0, NULL, 1, barriers);

    //// simply record the screen cleaning command

    skyRecObjs.cmd->BindRenderTargetAndClean(skyRecObjs.renderTarget, skyRecObjs.depthBuffer);

    //// draw skybox

    skyRecObjs.cmd->setViewPort(0, 0, (float)width, (float)height, 1, 1);

    skyRecObjs.cmd->setScissor(0, 0, width, height);

    skyRecObjs.cmd->BindPipeline(skyRecObjs.pipeline);

    skyRecObjs.cmd->BindTextureSet(skyRecObjs.textures, 0);

    skyRecObjs.cmd->BindUniformSet(skyRecObjs.uniforms, skyRecObjs.frameIndex * 2 + 0);

    const uint32_t skyboxVbStride = sizeof(float) * 4;

    skyRecObjs.cmd->BindVertexBuffer(1, skyRecObjs.vertexBuffer, &skyboxVbStride, NULL);

    skyRecObjs.cmd->draw(36, 0);

    //// draw castle

    castleRecObjs.cmd->setViewPort(0, 0, (float)width, (float)height, 0, 1);

    castleRecObjs.cmd->BindPipeline(castleRecObjs.pipeline);

    castleRecObjs.cmd->BindTextureSet(castleRecObjs.textures, 0);

    castleRecObjs.cmd->BindUniformSet(castleRecObjs.uniforms, skyRecObjs.frameIndex * 2 + 0);

    castleRecObjs.cmd->BindIndexBuffer(castleRecObjs.indexBuffer[0], INDEX_TYPE_UINT32, 0);

    const uint32_t castleStride = castleRecObjs.vertexStride;

    castleRecObjs.cmd->BindVertexBuffer(1, castleRecObjs.vertexBuffer, &castleStride, NULL);

    castleRecObjs.cmd->drawIndexed(castleRecObjs.indicesCount, 0, 0);
    //castleRecObjs.cmd->draw(1000, 0);

    ////

    skyRecObjs.cmd->BindRenderTargetAndLoad(skyRecObjs.renderTarget, NULL);

    skyRecObjs.cmd->UnbindRenderTarget();

    barriers[0] = { skyRecObjs.renderTarget->getRenderTarget(), RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_PRESENT };
    skyRecObjs.cmd->ResourceBarrier(0, NULL, 0, NULL, 1, barriers);
}

const char* CastleApp::GetName() { return "Castle"; }

