#include <execution>
#include "scene.h"
#include "common/beat.h"
#include "game/runtime/state.h"
#include "game/runtime/generic_info.h"
#include "game/skin/skin_mgr.h"
#include "scene_context.h"
#include "config/config_mgr.h"

#include "imgui.h"
#include "game/skin/skin_lr2_debug.h"
#include "game/runtime/i18n.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

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
    SkinMgr::load(skinType, gInCustomize && skinType != SkinType::THEME_SELECT);
    pSkin = SkinMgr::get(skinType);

    int notificationPosY = 480;
    int notificationWidth = 640;
    const int notificationHeight = 20;

    if (pSkin && !gInCustomize && skinType != SkinType::THEME_SELECT)
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
        textBuilder.textInd = IndexText(size_t(IndexText::_OVERLAY_NOTIFICATION_0) + i);
        textBuilder.align = TextAlign::TEXT_ALIGN_LEFT;
        textBuilder.ptsize = textHeight;
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
        !(gNextScene == SceneType::SELECT && skinType == SkinType::THEME_SELECT))
    {
        State::resetTimer();

        State::set(IndexText::_OVERLAY_TOPLEFT, "");

        // Skin may be cached. Reset mouse status
        pSkin->setHandleMouseEvents(true);
    }

    if (!gInCustomize && skinType == SkinType::THEME_SELECT || gInCustomize && skinType != SkinType::THEME_SELECT)
        _input.disableCountFPS();
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
    gUpdateContext.updateTime = t;

    if (pSkin)
    {
        // update skin
        pSkin->update();
        auto [x, y] = _input.getCursorPos();
        pSkin->update_mouse(x, y);

        checkAndStartTextEdit();

        // update notifications
        {
            std::unique_lock lock(gOverlayContext._mutex);

            // notifications expire check
            while (!gOverlayContext.notifications.empty() && (t - gOverlayContext.notifications.begin()->first).norm() > 10 * 1000) // 10s
            {
                gOverlayContext.notifications.pop_front();
            }

            // update notification texts
            auto itNotifications = gOverlayContext.notifications.rbegin();
            for (size_t i = 0; i < _sNotifications.size(); ++i)
            {
                if (itNotifications != gOverlayContext.notifications.rend())
                {
                    State::set(IndexText(size_t(IndexText::_OVERLAY_NOTIFICATION_0) + i), itNotifications->second);
                    ++itNotifications;
                }
                else
                {
                    State::set(IndexText(size_t(IndexText::_OVERLAY_NOTIFICATION_0) + i), "");
                }
            }
        }

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
    if (!gInCustomize || _type == SceneType::CUSTOMIZE)
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
        std::shared_lock lock(gOverlayContext._mutex);
        if (!gOverlayContext.notifications.empty())
        {
            // draw notifications at the bottom. One string per line
            auto itNotification = gOverlayContext.notifications.rbegin();
            for (size_t i = 0; i < gOverlayContext.notifications.size() && i < _sNotifications.size(); ++i)
            {
                const auto& [timestamp, text] = *itNotification;
                _sNotificationsBG[i]->draw();
                _sNotifications[i]->updateText();
                _sNotifications[i]->draw();
            }
        }
        if (gOverlayContext.popupListShow && !gOverlayContext.popupList.empty())
        {
            // TODO draw list
        }
    }

    if (queuedScreenshot)
    {
        Path p = "screenshot";
        p /= (boost::format("LV %04d-%02d-%02d %02d-%02d-%02d.png")
            % State::get(IndexNumber::DATE_YEAR)
            % State::get(IndexNumber::DATE_MON)
            % State::get(IndexNumber::DATE_DAY)
            % State::get(IndexNumber::DATE_HOUR)
            % State::get(IndexNumber::DATE_MIN)
            % State::get(IndexNumber::DATE_SEC)).str();

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

    if (!gInCustomize && _type != SceneType::CUSTOMIZE || gInCustomize && _type == SceneType::CUSTOMIZE)
        gFrameCount[FRAMECOUNT_IDX_SCENE]++;
}

void SceneBase::updateImgui()
{
    assert(IsMainThread());

    bool showTextOverlay = false;
    if (showFPS) showTextOverlay = true;
    for (size_t i = 0; i < 4; ++i)
    {
        IndexText idx = IndexText(int(IndexText::_OVERLAY_TOPLEFT) + i);
        if (!State::get(idx).empty())
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
                ImGui::Text((boost::format("FPS: Render %d | Input %d | Update %d")
                    % State::get(IndexNumber::FPS)
                    % State::get(IndexNumber::INPUT_DETECT_FPS)
                    % State::get(IndexNumber::SCENE_UPDATE_FPS)).str().c_str());
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
                IndexText idx = IndexText(int(IndexText::_OVERLAY_TOPLEFT) + i);
                if (!State::get(idx).empty())
                {
                    ImGui::PushID(overlayTextID[count++]);
                    ImGui::Text(State::get(idx).c_str());
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
    if (!(!gInCustomize || _type == SceneType::CUSTOMIZE)) return;

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

IndexText SceneBase::textEditType() const
{
    return inTextEdit ? pSkin->textEditType() : IndexText::INVALID;
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