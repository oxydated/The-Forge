#include "UserInterfaceWrapper.h"

#include "../../../../Common_3/Application/Interfaces/IApp.h"

#include "RenderTargetWrapper.h"
#include "QueueWrapper.h"


ICameraController* gCameraController = NULL;


bool UserInterfaceWrapper::initUIAlternative(IApp* app)
{
    // Load fonts
    FontDesc font = {};
    font.pFontPath = "TitilliumText/TitilliumText-Bold.otf";
    fntDefineFonts(&font, 1, &gFontID);

    FontSystemDesc fontRenderDesc = {};
    fontRenderDesc.pRenderer = RendererWrapper::getRenderer();
    if (!initFontSystem(&fontRenderDesc))
        return false; // report?

    // Initialize Forge User Interface Rendering
    UserInterfaceDesc uiRenderDesc = {};
    uiRenderDesc.pRenderer = RendererWrapper::getRenderer();
    initUserInterface(&uiRenderDesc);

    // Camera setup
    if (!CameraSetup(app))
        return false;

    waitForAllResourceLoads();

    return true;
}

void UserInterfaceWrapper::loadUI(ReloadDesc* pReloadDesc, IApp* app, RenderTargetWrapper* renderTargetWrapper, QueueWrapper* queueWrapper)
{
    UserInterfaceLoadDesc uiLoad = {};
    uiLoad.mColorFormat = renderTargetWrapper->getRenderTarget()->mFormat;
    uiLoad.mHeight = app->mSettings.mHeight;
    uiLoad.mWidth = app->mSettings.mWidth;
    uiLoad.mLoadType = pReloadDesc->mType;
    loadUserInterface(&uiLoad);

    FontSystemLoadDesc fontLoad = {};
    fontLoad.mColorFormat = renderTargetWrapper->getRenderTarget()->mFormat;
    fontLoad.mHeight = app->mSettings.mHeight;
    fontLoad.mWidth = app->mSettings.mWidth;
    fontLoad.mLoadType = pReloadDesc->mType;
    loadFontSystem(&fontLoad);

    initScreenshotInterface(RendererWrapper::getRenderer(), queueWrapper->getQueue());
}

void UserInterfaceWrapper::ExitUIAlternative()
{
    exitScreenshotInterface();
    removeCamera();
    exitUserInterface();
    exitFontSystem();
}

void UserInterfaceWrapper::UpdateUI(float deltaTime, IApp* app)
{
    updateInputSystem(deltaTime, app->mSettings.mWidth, app->mSettings.mHeight);

    cameraController->update(deltaTime);
}

bool UserInterfaceWrapper::CameraSetup(IApp* app)
{
    // Initialize camera controller
    CameraMotionParameters cmp{ 160.0f, 600.0f, 200.0f };
    vec3                   camPos{ 48.0f, 48.0f, 20.0f };
    vec3                   lookAt{ vec3(0) };

    cameraController = initFpsCameraController(camPos, lookAt);
    cameraController->setMotionParameters(cmp);

    gCameraController = cameraController;

    InputSystemDesc inputDesc = {};
    inputDesc.pRenderer = RendererWrapper::getRenderer();
    inputDesc.pWindow = app->pWindow;
    inputDesc.pJoystickTexture = "circlepad.tex";
    if (!initInputSystem(&inputDesc))
        return false;

    // App Actions
    InputActionDesc actionDesc = { DefaultInputActions::DUMP_PROFILE_DATA,
                                   [](InputActionContext* ctx)
                                   {
                                       dumpProfileData(((Renderer*)ctx->pUserData)->pName);
                                       return true;
                                   },
                                   RendererWrapper::getRenderer() };
    addInputAction(&actionDesc);
    actionDesc = { DefaultInputActions::TOGGLE_FULLSCREEN,
                   [](InputActionContext* ctx)
                   {
                       WindowDesc* winDesc = ((IApp*)ctx->pUserData)->pWindow;
                       if (winDesc->fullScreen)
                           winDesc->borderlessWindow
                               ? setBorderless(winDesc, getRectWidth(&winDesc->clientRect), getRectHeight(&winDesc->clientRect))
                               : setWindowed(winDesc, getRectWidth(&winDesc->clientRect), getRectHeight(&winDesc->clientRect));
                       else
                           setFullscreen(winDesc);
                       return true;
                   },
                   this };
    addInputAction(&actionDesc);
    actionDesc = { DefaultInputActions::EXIT, [](InputActionContext* ctx)
                   {
                       requestShutdown();
                       return true;
                   } };
    addInputAction(&actionDesc);
    InputActionCallback onAnyInput = [](InputActionContext* ctx)
    {
        if (ctx->mActionId > UISystemInputActions::UI_ACTION_START_ID_)
        {
            uiOnInput(ctx->mActionId, ctx->mBool, ctx->pPosition, &ctx->mFloat2);
        }

        return true;
    };

    typedef bool              (*CameraInputHandler)(InputActionContext* ctx, DefaultInputActions::DefaultInputAction action);
    static CameraInputHandler onCameraInput = [](InputActionContext* ctx, DefaultInputActions::DefaultInputAction action)
    {
        if (*(ctx->pCaptured))
        {
            float2 delta = uiIsFocused() ? float2(0.f, 0.f) : ctx->mFloat2;
            switch (action)
            {
            case DefaultInputActions::ROTATE_CAMERA:
                gCameraController->onRotate(delta);
                break;
            case DefaultInputActions::TRANSLATE_CAMERA:
                gCameraController->onMove(delta);
                break;
            case DefaultInputActions::TRANSLATE_CAMERA_VERTICAL:
                gCameraController->onMoveY(delta[0]);
                break;
            default:
                break;
            }
        }
        return true;
    };
    actionDesc = { DefaultInputActions::CAPTURE_INPUT,
                   [](InputActionContext* ctx)
                   {
                       setEnableCaptureInput(!uiIsFocused() && INPUT_ACTION_PHASE_CANCELED != ctx->mPhase);
                       return true;
                   },
                   NULL };
    addInputAction(&actionDesc);
    actionDesc = { DefaultInputActions::ROTATE_CAMERA,
                   [](InputActionContext* ctx) { return onCameraInput(ctx, DefaultInputActions::ROTATE_CAMERA); }, NULL };
    addInputAction(&actionDesc);
    actionDesc = { DefaultInputActions::TRANSLATE_CAMERA,
                   [](InputActionContext* ctx) { return onCameraInput(ctx, DefaultInputActions::TRANSLATE_CAMERA); }, NULL };
    addInputAction(&actionDesc);
    actionDesc = { DefaultInputActions::TRANSLATE_CAMERA_VERTICAL,
                   [](InputActionContext* ctx) { return onCameraInput(ctx, DefaultInputActions::TRANSLATE_CAMERA_VERTICAL); }, NULL };
    addInputAction(&actionDesc);
    actionDesc = { DefaultInputActions::RESET_CAMERA, [](InputActionContext* ctx)
                   {
                       if (!uiWantTextInput())
                           gCameraController->resetView();
                       return true;
                   } };
    addInputAction(&actionDesc);
    GlobalInputActionDesc globalInputActionDesc = { GlobalInputActionDesc::ANY_BUTTON_ACTION, onAnyInput, this };
    setGlobalInputAction(&globalInputActionDesc);

    return true;
}

void UserInterfaceWrapper::unloadUI(ReloadDesc* pReloadDesc)
{
    unloadFontSystem(pReloadDesc->mType);
    unloadUserInterface(pReloadDesc->mType);
}

void UserInterfaceWrapper::removeCamera()
{
    exitInputSystem();

    exitCameraController(cameraController);
}

ICameraController* UserInterfaceWrapper::getCameraController() { return cameraController; }
