#include "CastleApp.h"
#include "RenderTargetWrapper.h"
#include "RendererWrapper.h"

// We only need Two sets of resources (one in flight and one being used on CPU)
const uint32_t gDataBufferCount = 2;

#ifndef __not_in_the_path_yet__

DEFINE_APPLICATION_MAIN(CastleApp)

std::vector<std::string> skyboxTextureFileNames = { "Skybox_right1.tex", "Skybox_left2.tex",  "Skybox_top3.tex", "Skybox_bottom4.tex",
                                                      "Skybox_front5.tex", "Ground_texture.dds" /*, "Skybox_back6.tex"*/ };

std::vector<textureParams> skyboxTextureParameters = { { "Skybox_right1.tex", "RightText" },
                                                       { "Skybox_left2.tex", "LeftText" },
                                                       { "Skybox_top3.tex", "TopText" },
                                                       { "Skybox_bottom4.tex", "BotText" },
                                                       { "Skybox_front5.tex", "FrontText" },
                                                       { "Ground_texture.dds", "BackText" } /*, "Skybox_back6.tex"*/ };



// Generate sky box vertex buffer
const float gSkyBoxPoints[] = {
    10.0f,  -10.0f, -10.0f, 6.0f, // -z
    -10.0f, -10.0f, -10.0f, 6.0f,   -10.0f, 10.0f,  -10.0f, 6.0f,   -10.0f, 10.0f,
    -10.0f, 6.0f,   10.0f,  10.0f,  -10.0f, 6.0f,   10.0f,  -10.0f, -10.0f, 6.0f,

    -10.0f, -10.0f, 10.0f,  2.0f, //-x
    -10.0f, -10.0f, -10.0f, 2.0f,   -10.0f, 10.0f,  -10.0f, 2.0f,   -10.0f, 10.0f,
    -10.0f, 2.0f,   -10.0f, 10.0f,  10.0f,  2.0f,   -10.0f, -10.0f, 10.0f,  2.0f,

    10.0f,  -10.0f, -10.0f, 1.0f, //+x
    10.0f,  -10.0f, 10.0f,  1.0f,   10.0f,  10.0f,  10.0f,  1.0f,   10.0f,  10.0f,
    10.0f,  1.0f,   10.0f,  10.0f,  -10.0f, 1.0f,   10.0f,  -10.0f, -10.0f, 1.0f,

    -10.0f, -10.0f, 10.0f,  5.0f, // +z
    -10.0f, 10.0f,  10.0f,  5.0f,   10.0f,  10.0f,  10.0f,  5.0f,   10.0f,  10.0f,
    10.0f,  5.0f,   10.0f,  -10.0f, 10.0f,  5.0f,   -10.0f, -10.0f, 10.0f,  5.0f,

    -10.0f, 10.0f,  -10.0f, 3.0f, //+y
    10.0f,  10.0f,  -10.0f, 3.0f,   10.0f,  10.0f,  10.0f,  3.0f,   10.0f,  10.0f,
    10.0f,  3.0f,   -10.0f, 10.0f,  10.0f,  3.0f,   -10.0f, 10.0f,  -10.0f, 3.0f,

    10.0f,  -10.0f, 10.0f,  4.0f, //-y
    10.0f,  -10.0f, -10.0f, 4.0f,   -10.0f, -10.0f, -10.0f, 4.0f,   -10.0f, -10.0f,
    -10.0f, 4.0f,   -10.0f, -10.0f, 10.0f,  4.0f,   10.0f,  -10.0f, 10.0f,  4.0f,
};
uint64_t skyBoxDataSize = 4 * 6 * 6 * sizeof(float);

struct commandRecordObjects
{
    Command*             cmd;
    RenderTargetWrapper* renderTarget;
    RenderTargetWrapper* depthBuffer;
    PipelineWrapper*     pipeline;
    TextureSet*          textures;
    UniformSet*          uniforms;
    Buffer**             vertexBuffer;
    uint32_t             frameIndex;
};

void CastleApp::incrementFrameIndex() { frameIndex = (frameIndex + 1) % totalFrameBuffers; }

void CastleApp::Exit() { UI.ExitUIAlternative(); }

bool CastleApp::Init()
{

    // FILE PATHS
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_SHADER_BINARIES, "CompiledShaders");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_TEXTURES, "Textures");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_FONTS, "Fonts");
    fsSetPathForResourceDir(pSystemFileIO, RM_DEBUG, RD_SCREENSHOTS, "Screenshots");
    fsSetPathForResourceDir(pSystemFileIO, RM_CONTENT, RD_SCRIPTS, "Scripts");
    fsSetPathForResourceDir(pSystemFileIO, RM_DEBUG, RD_DEBUG, "Debug");

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

    //// Loads Skybox Textures -> also creates the texture resources represented by the DescriptorSet (moved to TextureSet)

    //// Creates Sampler (moved to Signature)

    // Loads Skybox vertex buffer (creates it from the global array defined somewhere else) and creates the vertexBuffer resource
    skyBoxVertexBuffer = new BufferResource(gSkyBoxPoints, skyBoxDataSize, DESCRIPTOR_TYPE_VERTEX_BUFFER, RESOURCE_MEMORY_USAGE_GPU_ONLY);
    vertexBuffers = { skyBoxVertexBuffer->getBuffer() };

    //// loads skybox uniform buffer (moved to UniformSet)

    // more bread crumbs

    // initialize UI and input

    if (!UI.initUIAlternative(this))
    {
        return false;
    }

    // start frameIndex

    return true;
}

bool CastleApp::Load(ReloadDesc* pReloadDesc)
{
    if (pReloadDesc->mType & RELOAD_TYPE_SHADER)
    {
        /// Create Samplers
        /// load shaders
        /// create RootSignatures

        rootSignature = new Signature({ "uSampler0" }, { { "SkyBoxDrawShader", { "skybox.vert", "skybox.frag" } } });

        /// create DescriptorSets
        /// prepare DescriptorSets (that could be a single step)
        /// 
        skyBoxTextures = new TextureSet(rootSignature, skyboxTextureParameters);
        skyUniforms =
            new UniformSet(rootSignature, { { "SkyboxUniformBuffer", "uniformBlock", sizeof(UniformBlockSky) } }, totalFrameBuffers);
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
        /// create pipeline

        //addPipelines();
        skyBoxDrawPipeline = new PipelineWrapper(rootSignature, "SkyBoxDrawShader", chain->getRenderTargetByIndex(0), depthBuffer);
    }

    // UI stuff

    UI.loadUI(pReloadDesc, this, chain->getRenderTargetByIndex(0), graphicsQueue);

    return true;
}

void CastleApp::Unload(ReloadDesc* pReloadDesc) {}

void CastleApp::Update(float deltaTime)
{
    UI.UpdateUI(deltaTime, this);

    /************************************************************************/
    // Scene Update
    /************************************************************************/
    static float currentTime = 0.0f;
    currentTime += deltaTime * 1000.0f;

    // update camera with time
    mat4 viewMat = UI.getCameraController()->getViewMatrix();

    const float  aspectInverse = (float)mSettings.mHeight / (float)mSettings.mWidth;
    const float  horizontal_fov = PI / 2.0f;
    CameraMatrix projMat = CameraMatrix::perspectiveReverseZ(horizontal_fov, aspectInverse, 0.1f, 1000.0f);

    viewMat.setTranslation(vec3(0));
    skyUniformHostBlock = {};
    skyUniformHostBlock.mProjectView = projMat * viewMat;
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

    //if (pRenderer->pGpu->mSettings.mGpuBreadcrumbs)
    //{
    //    // Check breadcrumb markers
    //    checkMarkers();
    //}

    // update uniform

    skyUniforms->update(frameIndex, &skyUniformHostBlock, sizeof(UniformBlockSky));

    // Reset cmd pool for this frame

    elem.resetCommandPool();

    // record command buffer

    Command* cmd = elem.getCommandByIndex(0);

    commandRecordObjects recObjs = {};

    recObjs.cmd = cmd;
    recObjs.renderTarget = acquiredImageFromSwapchain.renderTarget;
    recObjs.depthBuffer = depthBuffer;
    recObjs.pipeline = skyBoxDrawPipeline;
    recObjs.textures = skyBoxTextures;
    recObjs.uniforms = skyUniforms;
    recObjs.vertexBuffer = vertexBuffers.data();
    recObjs.frameIndex = frameIndex;

    cmd->recordCommand(&CastleApp::commandsToRecord, &recObjs);

    // simply record the screen cleaning command

    // draw skybox

    // Queue Submit

    Semaphore* imageAcquiredSemaphore = chain->getImageAcquiredSemaphore();
    Semaphore* waitSemaphore = elem.getSemaphore();

    graphicsQueue->submit(cmd, elem.getFence(), &imageAcquiredSemaphore, &waitSemaphore);

    // Queue present
    // &elem.pSemaphore wait
    graphicsQueue->present(acquiredImageFromSwapchain.swapchainImageIndex, chain, 1, &waitSemaphore);

    // Closing draw, preparing for next image in Swapchain
    incrementFrameIndex();
}

void CastleApp::commandsToRecord(void* data)
{
    commandRecordObjects* pRecObjs = static_cast<commandRecordObjects*>(data);
    uint32_t              width = pRecObjs->renderTarget->getWidth();
    uint32_t              height = pRecObjs->renderTarget->getHeight();

    // if (pRenderer->pGpu->mSettings.mGpuBreadcrumbs)
    //{
    //     // Reset markers values
    //     resetMarkers(cmd);
    // }

    // RenderTargetBarrier barriers[] = {
    //     { pRenderTarget, RESOURCE_STATE_PRESENT, RESOURCE_STATE_RENDER_TARGET },
    // };
    // cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 1, barriers);
    RenderTargetBarrier barriers[] = { pRecObjs->renderTarget->getRenderTarget(), RESOURCE_STATE_PRESENT, RESOURCE_STATE_RENDER_TARGET };
    pRecObjs->cmd->ResourceBarrier(0, NULL, 0, NULL, 1, barriers);

    //// simply record the screen cleaning command

    // BindRenderTargetsDesc bindRenderTargets = {};
    // bindRenderTargets.mRenderTargetCount = 1;
    // bindRenderTargets.mRenderTargets[0] = { pRenderTarget, LOAD_ACTION_CLEAR };
    // bindRenderTargets.mDepthStencil = { pDepthBuffer, LOAD_ACTION_CLEAR };
    // cmdBindRenderTargets(cmd, &bindRenderTargets);
    pRecObjs->cmd->BindRenderTargetAndClean(pRecObjs->renderTarget, pRecObjs->depthBuffer);

    // cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);

    //// draw skybox
    // cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 1.0f, 1.0f);
    pRecObjs->cmd->setViewPort(0, 0, (float)width, (float)height, 1, 1);

    // cmdSetScissor(cmd, 0, 0, pRenderTarget->mWidth, pRenderTarget->mHeight);
    pRecObjs->cmd->setScissor(0, 0, width, height);

    // cmdBindPipeline(cmd, pSkyBoxDrawPipeline);
    pRecObjs->cmd->BindPipeline(pRecObjs->pipeline);

    // cmdBindDescriptorSet(cmd, 0, pDescriptorSetTexture);
    pRecObjs->cmd->BindTextureSet(pRecObjs->textures, 0);

    // cmdBindDescriptorSet(cmd, gFrameIndex * 2 + 0, pDescriptorSetUniforms);
    pRecObjs->cmd->BindUniformSet(pRecObjs->uniforms, pRecObjs->frameIndex * 2 + 0);

    const uint32_t skyboxVbStride = sizeof(float) * 4;

    // cmdBindVertexBuffer(cmd, 1, &pSkyBoxVertexBuffer, &skyboxVbStride, NULL);
    pRecObjs->cmd->BindVertexBuffer(1, pRecObjs->vertexBuffer, &skyboxVbStride, NULL);

    // cmdDraw(cmd, 36, 0);
    pRecObjs->cmd->draw(36, 0);

    // cmdSetViewport(cmd, 0.0f, 0.0f, (float)pRenderTarget->mWidth, (float)pRenderTarget->mHeight, 0.0f, 1.0f);

    // bindRenderTargets = {};
    // bindRenderTargets.mRenderTargetCount = 1;
    // bindRenderTargets.mRenderTargets[0] = { pRenderTarget, LOAD_ACTION_LOAD };
    // cmdBindRenderTargets(cmd, &bindRenderTargets);
    pRecObjs->cmd->BindRenderTargetAndLoad(pRecObjs->renderTarget, NULL);

    // cmdDrawUserInterface(cmd);

    // cmdBindRenderTargets(cmd, NULL);
    pRecObjs->cmd->UnbindRenderTarget();

    // barriers[0] = { pRenderTarget, RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_PRESENT };
    // cmdResourceBarrier(cmd, 0, NULL, 0, NULL, 1, barriers);
    barriers[0] = { pRecObjs->renderTarget->getRenderTarget(), RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_PRESENT };
    pRecObjs->cmd->ResourceBarrier(0, NULL, 0, NULL, 1, barriers);
}

const char* CastleApp::GetName() { return "CastleApp"; }

#else
void CastleApp::Draw()
{
}

#endif
