#include "4J_Libraries_Source/fui/fui.h"
#include "4J_Libraries_Source/fui/fuiFile.h"
#include "4J_Libraries_Source/fui/fuiRenderNode.h"
#include "4J_Libraries_Source/fui/node/FJ_FuiNodeStage.h"
#include "Minecraft.Client/platform/NX/Platform.h"
#include "Minecraft.Client/ui/ConsoleUIController.h"
#include "Minecraft.Client/ui/scene/UILayer.h"
#include "Minecraft.Client/ui/scene/UIScene.h"
#include "Minecraft.Client/ui/scene/control/UIControl.h"
#include "Minecraft.World/ArrayWithLength.h"
#include "Minecraft.Core/System.h"
#include <string>

const int dword_7100E23090[22]
    = {0, 7, 8, 9, 3, 4, 6, 5, 1, 2, 0xA, 0xB, 0x64, 0xC, 0x11, 0x12, 0x64, 0x64, 0x64, 0x64, 0, 7};

// It's matching but... some of those variable inits should be moved to header
UIScene::UIScene(int padID, UILayer* uiLayer) {
    this->mPadID = padID;
    this->qword30 = nullptr;
    this->mFuiFile = nullptr;
    this->mFocusRelated = 0;
    this->mHideLowerScenes = 0;
    this->byte_e9 = 0;
    this->mHidden = 0;
    this->bool_ad = 0;
    this->mVisible = 1;
    this->byte_96 = 0;
    this->mControlFocus = -1;
    this->mControlChildFocus = 0;
    this->mOpacity = 1.0;
    this->mIsInitializedMovie = 0;
    this->byte_ea = 1;
    this->dwordF8 = 0;
    this->mUILayer = uiLayer;
    this->mBackScene = nullptr;
    this->mCallbackUniqueId = nullptr;
    this->wstring_8 = L"";
    this->bool_28 = 0;
}

void UIScene::customDrawFui(void* a1, char const* a2, fuiRect* a3) {
    ((UIScene*)a1)->customDraw(a2, a3);
}

// NON_MATCHING: some crap, with setting the stageWidth/stageHeight, see below
void UIScene::loadMovie() {
    EnterCriticalSection(&ConsoleUIController::unk_71017BE928);
    std::wstring moviePath = this->getMoviePath();

    fui* fui = fui::sInstance;

    int v3;
    if (fui->getResolution()) {
        moviePath.append(L"1080.fui");
        v3 = 0;
    } else {
        moviePath.append(L"720.fui");
        v3 = 1;
    }
    this->mResType = v3;
    arrayWithLength<uchar> movieData = gConsoleUIController.getMovieData(moviePath.c_str());
    this->mFuiFile = fui->load(movieData, true, fui->getResolution());

    this->mFuiFile->getRootNode()->mFuiNodeStage->setCallbackScene(this);
    this->mFuiFile->setCustomDrawCallback(UIScene::customDrawFui, this);

    /// this is non matching
    this->mStageWidth = this->mFuiFile->getStageWidth();
    this->mStageWidth2 = this->mStageWidth;
    this->mStageHeight = this->mFuiFile->getStageHeight();
    this->mStageHeight2 = this->mStageHeight;
    ///

    LeaveCriticalSection(&ConsoleUIController::unk_71017BE928);
}

// NON_MATCHING: it's creating temp instance of fui::sInstance, idk how to avoid that
void UIScene::sendInputToMovie(int key, bool a3, bool a4, bool a5) {
    if (this->mFuiFile)
        fui::sInstance->dispatchKeyboardEvent(this->mFuiFile, a4, this->convertGameActionToFuiKeycode(key));
}

// symbol from WiiU doesn't say that argument is unsigned int so let's follow that
int UIScene::convertGameActionToFuiKeycode(int gameAction) {
    return (unsigned int)gameAction <= 0x15 ? dword_7100E23090[gameAction] : 100;
}

void UIScene::initialiseMovie() {
    this->loadMovie();
    this->mapElementsAndNames();
    this->updateSafeZone();

    this->mIsInitializedMovie = true;
}

bool UIScene::needsReloaded() {
    return !this->mFuiFile && (!this->stealsFocus() || this->mFocusRelated);
}

bool UIScene::hasMovie() {
    return this->mFuiFile != nullptr;
}

void UIScene::updateSafeZone() {
    double v1 = 0.0, v2 = 0.0, v4 = 0.0, v5 = 0.0;

    switch (this->mUILayer->getViewPort()) {
    case 1:
        v1 = this->getSafeZoneHalfHeight();
        v4 = v5 = v2;
        break;
    case 2:
        v2 = this->getSafeZoneHalfHeight();
        v4 = v5 = v1;
        break;
    case 3:
        v4 = this->getSafeZoneHalfWidth();
        v2 = v5 = v1;
        break;
    case 4:
        v5 = this->getSafeZoneHalfWidth();
        v2 = v1;
        v4 = v1;
        break;
    case 5:
        v1 = this->getSafeZoneHalfHeight();
        v4 = this->getSafeZoneHalfWidth();
        v5 = v2;
        break;
    case 6:
        v1 = this->getSafeZoneHalfHeight();
        v5 = this->getSafeZoneHalfWidth();
        v4 = v2;
        break;
    case 7:
        v2 = this->getSafeZoneHalfHeight();
        v4 = this->getSafeZoneHalfWidth();
        v5 = v1;
        break;
    case 8:
        v2 = this->getSafeZoneHalfHeight();
        v5 = this->getSafeZoneHalfWidth();
        v4 = v1;
        break;
    default:
        v1 = this->getSafeZoneHalfHeight();
        v2 = this->getSafeZoneHalfHeight();
        v4 = this->getSafeZoneHalfWidth();
        v5 = this->getSafeZoneHalfWidth();
        break;
    }

    this->setSafeZone(v1, v2, v4, v5);
}

void UIScene::updateViewportTouchOffset() {
    C4JRender::eViewportType viewPortType = this->mUILayer->getViewPort();
    gConsoleUIController.updateViewportTouchOffset(viewPortType);
}

bool UIScene::mapElementsAndNames() {
    return true;
}

int UIScene::getSubSceneType() const {
    return 0;
}

void UIScene::tick() {
    if (!this->mHidden) {
        if (this->mHideLowerScenes)
            this->byte_96 = 1;

        this->tickTimers();

        // had to do it hacky way as idk how to force compiler to output same thing it does in target, unless
        // that's how it looked in source
        for (auto i = this->mUIControls.begin(); i != this->mUIControls.end(); ++i) {
            UIControl* uiControl = *i;
            uiControl->tick();
        }

        this->mHideLowerScenes = true;
    }
}

void UIScene::handleTimerComplete(int) {}

bool UIScene::stealsFocus() {
    return true;
}

bool UIScene::hasFocus(int padID) {
    return this->mFocusRelated && this->mPadID == padID;
}

void UIScene::updateTooltips() {
    if (!gConsoleUIController.IsReloadingSkin()) {
        gConsoleUIController.SetTooltips(this->mPadID, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                         -1, -1, 0, 0);
    }
}

void UIScene::updateComponents() {}
void UIScene::handleGainFocus(bool) {}
void UIScene::handleLoseFocus() {}

bool UIScene::hidesLowerScenes() {
    return this->mHideLowerScenes;
}

bool UIScene::blocksInput() {
    return false;
}

void* UIScene::GetMainPanel() {
    return nullptr;
}

void UIScene::customDraw(char const*, fuiRect*) {}
void UIScene::handleInput(int, int, bool, bool, bool, bool&) {}
void UIScene::handleDestroy() {}
void UIScene::handlePreUnloadForReload() {}
void UIScene::handlePreReload() {}
void UIScene::handleReload() {}
void UIScene::handlePress(int, int) {}
void UIScene::handleFocusChange(int, int) {}
void UIScene::handleInitFocus(int, int) {}
void UIScene::handleCheckboxElementToggled(int, int, bool) {}
void UIScene::handleSliderMove(int, int) {}
void UIScene::handleSliderElementMove(int, int, int) {}
void UIScene::handleAnimationEnd() {}
void UIScene::handleSelectionChanged(int) {}
void UIScene::handleRequestMoreData(int, bool) {}
void UIScene::handleTouchBoxRebuild() {}
void UIScene::HandleDLCMountingComplete() {}
void UIScene::HandleDLCInstalled() {}
void UIScene::HandleDLCLicenseChange() {}
void UIScene::HandleTMSBanFileRetrieved() {}
void UIScene::HandleMessage(EUIMessage, void*) {}
void UIScene::HandleSaveDeviceRemoved(int) {}
void UIScene::handleUnlockFullVersion() {}
void UIScene::handleTouchInput(unsigned int, int, int, int, bool, bool, bool) {}