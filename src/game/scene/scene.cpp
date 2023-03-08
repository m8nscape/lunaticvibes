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
vScene::vScene(eMode mode, unsigned rate, bool backgroundInput) :
    AsyncLooper("Scene Update", std::bind(&vScene::_updateAsync1, this), rate),
    _input(1000, backgroundInput)
{
    unsigned inputPollingRate = ConfigMgr::get("P", cfg::P_INPUT_POLLING_RATE, 1000);
    if (inputPollingRate != 1000)
    {
        _input.setRate(inputPollingRate);
    }

    // Disable skin caching for now. dst options are changing all the time
    SkinMgr::unload(mode);
    SkinMgr::load(mode, gInCustomize && mode != eMode::THEME_SELECT);
    _skin = SkinMgr::get(mode);

    int notificationPosY = 480;
    int notificationWidth = 640;
    const int notificationHeight = 20;

    if (_skin && !gInCustomize && mode != eMode::THEME_SELECT)
    {
        int x, y;
        switch (_skin->info.resolution)
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
        _sNotifications[i]->setLoopTime(0);

        SpriteStatic::SpriteStaticBuilder bgBuilder;
        bgBuilder.texture = _texNotificationsBG;
        _sNotificationsBG[i] = bgBuilder.build();
        _sNotificationsBG[i]->setLoopTime(0);

        notificationPosY -= notificationHeight;
        RenderKeyFrame f;
        f.time = 0;
        f.param.rect = Rect(0, notificationPosY, notificationWidth, notificationHeight);
        f.param.accel = RenderParams::CONSTANT;
        f.param.blend = BlendMode::ALPHA;
        f.param.filter = true;
        f.param.angle = 0;
        f.param.center = Point(0, 0);
        f.param.color = 0xffffff80;
        _sNotificationsBG[i]->appendKeyFrame(f);

        f.param.rect.y += (notificationHeight - textHeight) / 2;
        f.param.rect.h = textHeight;
        f.param.color = 0xffffffff;
        _sNotifications[i]->appendKeyFrame(f);
    }

    _input.register_p("DEBUG_TOGGLE", std::bind(&vScene::DebugToggle, this, std::placeholders::_1, std::placeholders::_2));

    _input.register_p("GLOBALFUNC", std::bind(&vScene::GlobalFuncKeys, this, std::placeholders::_1, std::placeholders::_2));

    _input.register_p("SKIN_MOUSE_CLICK", std::bind(&vScene::MouseClick, this, std::placeholders::_1, std::placeholders::_2));
    _input.register_h("SKIN_MOUSE_DRAG", std::bind(&vScene::MouseDrag, this, std::placeholders::_1, std::placeholders::_2));
    _input.register_r("SKIN_MOUSE_RELEASE", std::bind(&vScene::MouseRelease, this, std::placeholders::_1, std::placeholders::_2));

    if (_skin && 
        !(gNextScene == eScene::SELECT && mode == eMode::THEME_SELECT))
    {
        State::resetTimer();

        State::set(IndexText::_OVERLAY_TOPLEFT, "");

        // Skin may be cached. Reset mouse status
        _skin->setHandleMouseEvents(true);
    }

    if (!gInCustomize && mode == eMode::THEME_SELECT || gInCustomize && mode != eMode::THEME_SELECT)
        _input.disableCountFPS();
}

vScene::~vScene() 
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

void vScene::update()
{
    Time t;
    gUpdateContext.updateTime = t;

    if (_skin)
    {
        // update skin
        _skin->update();
        auto [x, y] = _input.getCursorPos();
        _skin->update_mouse(x, y);

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

        // update videos
        TextureVideo::updateAll();
    }

    // ImGui
    if (!gInCustomize || _scene == eScene::CUSTOMIZE)
    {
        ImGuiNewFrame();

        _updateImgui();

        ImGui::Render();
    }
}

void vScene::MouseClick(InputMask& m, const Time& t)
{
    if (!_skin) return;
    if (m[Input::Pad::M1])
    {
        auto [x, y] = _input.getCursorPos();
        _skin->update_mouse_click(x, y);
    }
}

void vScene::MouseDrag(InputMask& m, const Time& t)
{
    if (!_skin) return;
    if (m[Input::Pad::M1])
    {
        auto [x, y] = _input.getCursorPos();
        _skin->update_mouse_drag(x, y);
    }
}

void vScene::MouseRelease(InputMask& m, const Time& t)
{
    if (!_skin) return;
    if (m[Input::Pad::M1])
    {
        _skin->update_mouse_release();
    }
}

bool vScene::queuedScreenshot = false;
bool vScene::queuedFPS = false;
bool vScene::showFPS = false;

void vScene::draw() const
{
    if (_skin)
    {
        _skin->draw();
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

void vScene::_updateAsync1()
{
    _updateAsync();

    if (!gInCustomize && _scene != eScene::CUSTOMIZE || gInCustomize && _scene == eScene::CUSTOMIZE)
        gFrameCount[FRAMECOUNT_IDX_SCENE]++;
}

void vScene::_updateImgui()
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

void vScene::DebugToggle(InputMask& p, const Time& t)
{
#ifdef _DEBUG
    if (!(!gInCustomize || _scene == eScene::CUSTOMIZE)) return;

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

bool vScene::isInTextEdit() const
{
    return inTextEdit;
}

IndexText vScene::textEditType() const
{
    return inTextEdit ? _skin->textEditType() : IndexText::INVALID;
}

void vScene::startTextEdit(bool clear)
{
    if (_skin)
    {
        _skin->startTextEdit(clear);
        inTextEdit = true;
    }
}

void vScene::stopTextEdit(bool modify)
{
    if (_skin)
    {
        inTextEdit = false;
        _skin->stopTextEdit(modify);
    }
}


void vScene::GlobalFuncKeys(InputMask& m, const Time& t)
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