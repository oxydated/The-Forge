#pragma once

// Interfaces
#include "../../../../Common_3/Application/Interfaces/IApp.h"
#include "../../../../Common_3/Application/Interfaces/ICameraController.h"
#include "../../../../Common_3/Application/Interfaces/IFont.h"
#include "../../../../Common_3/Application/Interfaces/IInput.h"
#include "../../../../Common_3/Application/Interfaces/IProfiler.h"
#include "../../../../Common_3/Application/Interfaces/IScreenshot.h"
#include "../../../../Common_3/Application/Interfaces/IUI.h"
#include "../../../../Common_3/Game/Interfaces/IScripting.h"
#include "../../../../Common_3/Utilities/Interfaces/IFileSystem.h"
#include "../../../../Common_3/Utilities/Interfaces/ILog.h"
#include "../../../../Common_3/Utilities/Interfaces/ITime.h"

#include "../../../../Common_3/Utilities/RingBuffer.h"

// Renderer
#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"
#include "../../../../Common_3/Resources/ResourceLoader/Interfaces/IResourceLoader.h"

#include "RendererWrapper.h"

class RenderTargetWrapper;
class QueueWrapper;

class UserInterfaceWrapper
{
public:
    bool initUIAlternative(IApp* app);

    void loadUI(ReloadDesc* pReloadDesc, IApp* app, RenderTargetWrapper* renderTargetWrapper, QueueWrapper* queueWrapper);

    void ExitUIAlternative();

    void UpdateUI(float deltaTime, IApp* app);

    bool CameraSetup(IApp* app);

    void removeCamera();

    ICameraController* getCameraController();

private:
    // UI global variables

    ICameraController* cameraController = NULL;
    UIComponent*       pGuiWindow = NULL;
    uint32_t           gFontID = 0;

};
