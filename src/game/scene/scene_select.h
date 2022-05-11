#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <memory>
#include "scene.h"
#include "scene_context.h"

enum class eSelectState
{
    PREPARE,
    SELECT,
    SEARCH,
    PANEL1,     // play option
    FADEOUT,
};

class vChartFormat;
class SceneSelect : public vScene
{
private:
    eSelectState _state;
    InputMask _inputAvailable;
    
    // navigate input
    bool isHoldingUp = false;
    bool isHoldingDown = false;
    bool isScrollingByAxis = false;
    bool isInVersionList = false;
    Time scrollTimestamp = -1;
    Time selectDownTimestamp = -1;

    // imgui
    bool imguiShow = false;

public:
    SceneSelect();
    virtual ~SceneSelect();

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    std::function<void()> _updateCallback;
    void updatePrepare();
    void updateSelect();
    void updateSearch();
    void updatePanel(unsigned idx);
    void updateFadeout();

protected:
    // Inner-state updates

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, const Time&);
    void inputGameHold(InputMask&, const Time&);
    void inputGameRelease(InputMask&, const Time&);

private:
    void inputGamePressSelect(InputMask&, const Time&);
    void inputGameHoldSelect(InputMask&, const Time&);
    void inputGameReleaseSelect(InputMask&, const Time&);
    void inputGameAxisSelect(InputAxisPlus&, const Time&);

    void inputGamePressPanel(InputMask&, const Time&);
    void inputGameHoldPanel(InputMask&, const Time&);
    void inputGameReleasePanel(InputMask&, const Time&);

private:
    void loadSongList();
    void _navigateUpBy1(const Time& t);
    void _navigateDownBy1(const Time& t);
    void _navigateEnter(const Time& t);
    void _navigateBack(const Time& t);
    void _decide();
    void _navigateVersionEnter(const Time& t);
    void _navigateVersionBack(const Time& t);
    bool _closeAllPanels(const Time& t);

private:
    // imgui Dialogs
    void _imguiSampleDialog();
    void _imguiSettings();
};
