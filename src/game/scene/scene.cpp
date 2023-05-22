#include "common/pch.h"

#include "scene.h"
#include "config/config_mgr.h"

#include "game/graphics/texture_extra.h"
#include "game/skin/skin_mgr.h"

#include "imgui.h"
#include "game/skin/skin_lr2_debug.h"
#include "game/runtime/generic_info.h"
#include "game/runtime/i18n.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

#include "game/data/data_types.h"

namespace lunaticvibes
{

// prototype
SceneBase::SceneBase(SkinType skinType, unsigned rate, bool backgroundInput) :
    AsyncLooper("Scene Update", std::bind(&SceneBase::_updateAsync1, this), rate),
    _input(1000, backgroundInput)
{
    unsigned inputPollingRate = ConfigMgr::get("P", cfg::P_INPUT_POLLING_RATE, 1000);
    if (inputPollingRate != 1000)
    {
        _input.setRate(inputPollingRate);
    }

    // Disable skin caching for now. dst options are changing all the time
    SkinMgr::unload(skinType);
    SkinMgr::load(skinType, skinType != SkinType::THEME_SELECT);
    pSkin = SkinMgr::get(skinType);

    int notificationPosY = 480;
    int notificationWidth = 640;
    const int notificationHeight = 20;

    if (pSkin && skinType != SkinType::THEME_SELECT)
    {
        int x, y;
        switch (pSkin->info.resolution)
        {
        case 1: x = 1280; y = 720; break;
        case 2: x = 1920; y = 1080; break;
        default: x = 640; y = 480; break;
        }
        graphics_resize_canvas(x, y);
        notificationPosY = y;
        notificationWidth = x;
    }

    int faceIndex;
    Path fontPath = getSysMonoFontPath(NULL, &faceIndex, i18n::getCurrentLanguage());
    const int textHeight = 24;
    _fNotifications = std::make_shared<TTFFont>(fontPath.u8string().c_str(), int(textHeight * 1.5), faceIndex);
    _texNotificationsBG = std::make_shared<TextureFull>(0x000000ff);
    for (size_t i = 0; i < _sNotifications.size(); ++i)
    {
        SpriteText::SpriteTextBuilder textBuilder;
        textBuilder.font = _fNotifications;
        textBuilder.align = SpriteText::TextAlignType::Left;
        textBuilder.ptsize = textHeight;
        textBuilder.textCallback = std::bind([](size_t i) { return SystemData.overlayTextManager.getNotification(i); }, i);
        _sNotifications[i] = textBuilder.build();
        _sNotifications[i]->setMotionLoopTo(0);

        SpriteStatic::SpriteStaticBuilder bgBuilder;
        bgBuilder.texture = _texNotificationsBG;
        _sNotificationsBG[i] = bgBuilder.build();
        _sNotificationsBG[i]->setMotionLoopTo(0);

        notificationPosY -= notificationHeight;
        MotionKeyFrame f;
        f.time = 0;
        f.param.rect = Rect(0, notificationPosY, notificationWidth, notificationHeight);
        f.param.accel = MotionKeyFrameParams::CONSTANT;
        f.param.blend = BlendMode::ALPHA;
        f.param.filter = true;
        f.param.angle = 0;
        f.param.center = Point(0, 0);
        f.param.color = 0xffffff80;
        _sNotificationsBG[i]->appendMotionKeyFrame(f);

        f.param.rect.y += (notificationHeight - textHeight) / 2;
        f.param.rect.h = textHeight;
        f.param.color = 0xffffffff;        
        _sNotifications[i]->appendMotionKeyFrame(f);
    }

    _input.register_p("DEBUG_TOGGLE", std::bind(&SceneBase::DebugToggle, this, std::placeholders::_1, std::placeholders::_2));

    _input.register_p("GLOBALFUNC", std::bind(&SceneBase::GlobalFuncKeys, this, std::placeholders::_1, std::placeholders::_2));

    _input.register_p("SKIN_MOUSE_CLICK", std::bind(&SceneBase::MouseClick, this, std::placeholders::_1, std::placeholders::_2));
    _input.register_h("SKIN_MOUSE_DRAG", std::bind(&SceneBase::MouseDrag, this, std::placeholders::_1, std::placeholders::_2));
    _input.register_r("SKIN_MOUSE_RELEASE", std::bind(&SceneBase::MouseRelease, this, std::placeholders::_1, std::placeholders::_2));

    if (pSkin &&
        !(SystemData.gNextScene == SceneType::SELECT && skinType == SkinType::THEME_SELECT))
    {
        resetTimers();

        for (int i = 0; i < 4; i++)
            SystemData.overlayTopLeftText[i] = "";

        // Skin may be cached. Reset mouse status
        pSkin->setHandleMouseEvents(true);
    }
}

SceneBase::~SceneBase()
{
    assert(!_input.isRunning());
    assert(!isRunning());
    _input.unregister_r("SKIN_MOUSE_RELEASE");
    _input.unregister_h("SKIN_MOUSE_DRAG");
    _input.unregister_p("SKIN_MOUSE_CLICK");
    _input.unregister_p("GLOBALFUNC");
    _input.unregister_p("DEBUG_TOGGLE");
    sceneEnding = true;
}

void SceneBase::update()
{
    Time t;
    SystemData.sceneUpdateTime = t;

    if (pSkin)
    {
        // update skin
        pSkin->update();
        auto [x, y] = _input.getCursorPos();
        pSkin->update_mouse(x, y);

        checkAndStartTextEdit();

        SystemData.overlayTextManager.clearExpiredNotification();

        // update top-left texts
        for (size_t i = 0; i < _sNotifications.size(); ++i)
        {
            _sNotifications[i]->update(t);
            _sNotificationsBG[i]->update(t);
        }

#ifndef VIDEO_DISABLED
        // update videos
        TextureVideo::updateAll();
#endif
    }

    // ImGui
    if (_type == SceneType::CUSTOMIZE)
    {
        ImGuiNewFrame();

        updateImgui();

        ImGui::Render();
    }
}

void SceneBase::MouseClick(InputMask& m, const Time& t)
{
    if (!pSkin) return;
    if (m[Input::Pad::M1])
    {
        auto [x, y] = _input.getCursorPos();
        pSkin->update_mouse_click(x, y);
    }
}

void SceneBase::MouseDrag(InputMask& m, const Time& t)
{
    if (!pSkin) return;
    if (m[Input::Pad::M1])
    {
        auto [x, y] = _input.getCursorPos();
        pSkin->update_mouse_drag(x, y);
    }
}

void SceneBase::MouseRelease(InputMask& m, const Time& t)
{
    if (!pSkin) return;
    if (m[Input::Pad::M1])
    {
        pSkin->update_mouse_release();
    }
}

bool SceneBase::queuedScreenshot = false;
bool SceneBase::queuedFPS = false;
bool SceneBase::showFPS = false;

void SceneBase::draw() const
{
    if (pSkin)
    {
        pSkin->draw();
    }

    {
        auto l = SystemData.overlayTextManager.acquire();
        // draw notifications at the bottom. One string per line
        for (size_t i = 0; i < SystemData.overlayTextManager.MAX_NOTIFICATIONS; ++i)
        {
            const auto& s = SystemData.overlayTextManager.getNotification(i);
            if (s.empty()) break;
            _sNotificationsBG[i]->draw();
            _sNotifications[i]->updateText();
            _sNotifications[i]->draw();
        }
    }

    if (queuedScreenshot)
    {
        Path p = "screenshot";
        p /= (boost::format("LV %04d-%02d-%02d %02d-%02d-%02d.png")
            % SystemData.dateYear
            % SystemData.dateMonthOfYear
            % SystemData.dateDayOfMonth
            % SystemData.timeHour
            % SystemData.timeMin
            % SystemData.timeSec).str();

        graphics_screenshot(p);

        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCREENSHOT);
        queuedScreenshot = false;
    }

    if (queuedFPS)
    {
        showFPS = !showFPS;
        queuedFPS = false;
    }
}

void SceneBase::_updateAsync1()
{
    _updateAsync();

    if (_type != SceneType::CUSTOMIZE)
        gFrameCount[FRAMECOUNT_IDX_SCENE]++;
}

void SceneBase::updateImgui()
{
    assert(IsMainThread());

    bool showTextOverlay = false;
    if (showFPS)
    {
        showTextOverlay = true;
    }
    for (size_t i = 0; i < 4; ++i)
    {
        if (!SystemData.overlayTopLeftText[i].empty())
        {
            showTextOverlay = true;
            break;
        }
    }
    if (showTextOverlay)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.f, 0.f, 0.f, 0.4f });
        if (ImGui::Begin("##textoverlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (showFPS)
            {
                ImGui::PushID("##fps");
                ImGui::Text("FPS: Render %d | Input %d | Update %d",
                    SystemData.currentRenderFPS,
                    SystemData.currentInputFPS,
                    SystemData.currentUpdateFPS);
                ImGui::PopID();
            }

            static const char* overlayTextID[] =
            {
                "##overlaytext1",
                "##overlaytext2",
                "##overlaytext3",
                "##overlaytext4",
            };
            size_t count = 0;
            for (size_t i = 0; i < 4; ++i)
            {
                if (!SystemData.overlayTopLeftText[i].empty())
                {
                    ImGui::PushID(overlayTextID[count++]);
                    ImGui::TextUnformatted(SystemData.overlayTopLeftText[i].c_str());
                    ImGui::PopID();
                }
            }

            ImGui::End();
        }
        ImGui::PopStyleColor();
    }

#ifdef _DEBUG
    if (imguiShowMonitorLR2DST)
    {
        imguiMonitorLR2DST();
    }
    if (imguiShowMonitorNumber)
    {
        imguiMonitorNumber();
    }
    if (imguiShowMonitorOption)
    {
        imguiMonitorOption();
    }
    if (imguiShowMonitorSlider)
    {
        imguiMonitorSlider();
    }
    if (imguiShowMonitorSwitch)
    {
        imguiMonitorSwitch();
    }
    if (imguiShowMonitorText)
    {
        imguiMonitorText();
    }
    if (imguiShowMonitorBargraph)
    {
        imguiMonitorBargraph();
    }
    if (imguiShowMonitorTimer)
    {
        imguiMonitorTimer();
    }
#endif
}

void SceneBase::DebugToggle(InputMask& p, const Time& t)
{
#ifdef _DEBUG
    if (!(_type == SceneType::CUSTOMIZE)) return;

    if (p[Input::F1])
    {
        imguiShowMonitorLR2DST = !imguiShowMonitorLR2DST;
    }
    if (p[Input::F2])
    {
        imguiShowMonitorNumber = !imguiShowMonitorNumber;
    }
    if (p[Input::F3])
    {
        imguiShowMonitorOption = !imguiShowMonitorOption;
    }
    if (p[Input::F4])
    {
        imguiShowMonitorSlider = !imguiShowMonitorSlider;
    }
    if (p[Input::F5])
    {
        imguiShowMonitorSwitch = !imguiShowMonitorSwitch;
    }
    if (p[Input::F6])
    {
        imguiShowMonitorText = !imguiShowMonitorText;
    }
    if (p[Input::F7])
    {
        imguiShowMonitorBargraph = !imguiShowMonitorBargraph;
    }
    if (p[Input::F8])
    {
        imguiShowMonitorTimer = !imguiShowMonitorTimer;
    }
#endif

}

bool SceneBase::isInTextEdit() const
{
    return inTextEdit;
}

void SceneBase::startTextEdit(bool clear)
{
    if (pSkin)
    {
        pSkin->startTextEdit(clear);
        inTextEdit = true;
    }
}

void SceneBase::stopTextEdit(bool modify)
{
    if (pSkin)
    {
        inTextEdit = false;
        pSkin->stopTextEdit(modify);
    }
}


void SceneBase::GlobalFuncKeys(InputMask& m, const Time& t)
{
    if (m[Input::F6])
    {
        queuedScreenshot = true;
    }

    if (m[Input::F7])
    {
        queuedFPS = true;
    }
}

}
