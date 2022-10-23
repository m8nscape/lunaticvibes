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

static ImFont* pImguiFont = NULL;

// prototype
vScene::vScene(eMode mode, unsigned rate, bool backgroundInput) :
    AsyncLooper("Scene Update", std::bind(&vScene::_updateAsync1, this), rate),
    _input(1000, backgroundInput)
{
    // Disable skin caching for now. dst options are changing all the time
    SkinMgr::unload(mode);
    SkinMgr::load(mode, gInCustomize && mode != eMode::THEME_SELECT);
    _skin = SkinMgr::get(mode);

    int notificationPosY = 480;
    int notificationWidth = 640;
    const int notificationHeight = 20;

    if (_skin && !gInCustomize)
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
    Path fontPath = getSysMonoFontPath(NULL, &faceIndex);
    const int textHeight = 24;
    _fNotifications = std::make_shared<TTFFont>(fontPath.u8string().c_str(), int(textHeight * 1.5), faceIndex);
    _texNotificationsBG = std::make_shared<TextureFull>(0x000000ff);
    for (size_t i = 0; i < _sNotifications.size(); ++i)
    {
        _sNotifications[i] = std::make_shared<SpriteText>(_fNotifications, IndexText(size_t(IndexText::_OVERLAY_NOTIFICATION_0) + i), TextAlign::TEXT_ALIGN_LEFT, textHeight);
        _sNotifications[i]->setLoopTime(0);
        _sNotificationsBG[i] = std::make_shared<SpriteStatic>(_texNotificationsBG);
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
    {
        _sTopLeft = std::make_shared<SpriteText>(_fNotifications, IndexText::_OVERLAY_TOPLEFT, TextAlign::TEXT_ALIGN_LEFT, textHeight);
        _sTopLeft->setOutline(1, Color(0, 0, 0, 255));
        _sTopLeft->setLoopTime(0);
        _sTopLeft2 = std::make_shared<SpriteText>(_fNotifications, IndexText::_OVERLAY_TOPLEFT2, TextAlign::TEXT_ALIGN_LEFT, textHeight);
        _sTopLeft2->setOutline(1, Color(0, 0, 0, 255));
        _sTopLeft2->setLoopTime(0);
        _sTopLeft3 = std::make_shared<SpriteText>(_fNotifications, IndexText::_OVERLAY_TOPLEFT3, TextAlign::TEXT_ALIGN_LEFT, textHeight);
        _sTopLeft3->setOutline(1, Color(0, 0, 0, 255));
        _sTopLeft3->setLoopTime(0);
        _sTopLeft4 = std::make_shared<SpriteText>(_fNotifications, IndexText::_OVERLAY_TOPLEFT4, TextAlign::TEXT_ALIGN_LEFT, textHeight);
        _sTopLeft4->setOutline(1, Color(0, 0, 0, 255));
        _sTopLeft4->setLoopTime(0);
        RenderKeyFrame f;
        f.time = 0;
        f.param.rect = Rect(0, 0, notificationWidth, textHeight);
        f.param.accel = RenderParams::CONSTANT;
        f.param.color = 0xffffffff;
        f.param.blend = BlendMode::ALPHA;
        f.param.filter = true;
        f.param.angle = 0;
        f.param.center = Point(0, 0);
        _sTopLeft->appendKeyFrame(f);
        f.param.rect.y += textHeight;
        _sTopLeft2->appendKeyFrame(f);
        f.param.rect.y += textHeight;
        _sTopLeft3->appendKeyFrame(f);
        f.param.rect.y += textHeight;
        _sTopLeft4->appendKeyFrame(f);
    }

    if (pImguiFont == NULL)
    {
        ImFontConfig fontConfig;
        std::string fontName;
        Path fontPath = getSysFontPath(&fontName);
        strncpy(fontConfig.Name, fontName.c_str(), std::max(sizeof(fontConfig.Name) - 1, fontName.length()));
        ImFontAtlas& fontAtlas = *ImGui::GetIO().Fonts;
        pImguiFont = fontAtlas.AddFontFromFileTTF(fontPath.u8string().c_str(), 24, &fontConfig, fontAtlas.GetGlyphRangesChineseFull());
    }

    _input.register_p("DEBUG_TOGGLE", std::bind(&vScene::DebugToggle, this, std::placeholders::_1, std::placeholders::_2));

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
}

vScene::~vScene() 
{
    assert(!_input.isRunning());
    assert(!isRunning());
    _input.unregister_r("SKIN_MOUSE_RELEASE");
    _input.unregister_h("SKIN_MOUSE_DRAG");
    _input.unregister_p("SKIN_MOUSE_CLICK");
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
        _sTopLeft->update(t);
        _sTopLeft2->update(t);
        _sTopLeft3->update(t);
        _sTopLeft4->update(t);

        // update videos
        TextureVideo::updateAll();
    }

    // ImGui
    if (!gInCustomize || _scene == eScene::CUSTOMIZE)
    {
        ImGuiNewFrame();

        ImGui::PushFont(pImguiFont);

        _updateImgui();

        ImGui::PopFont();

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

void vScene::draw() const
{
    if (_skin)
    {
        _skin->draw();
    }

    _sTopLeft->updateText();
    _sTopLeft->draw();
    _sTopLeft2->updateText();
    _sTopLeft2->draw();
    _sTopLeft3->updateText();
    _sTopLeft3->draw();
    _sTopLeft4->updateText();
    _sTopLeft4->draw();

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
}

void vScene::_updateAsync1()
{
    _updateAsync();
    gFrameCount[FRAMECOUNT_IDX_SCENE]++;
}

void vScene::_updateImgui()
{
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