#include <execution>
#include "scene.h"
#include "common/beat.h"
#include "game/data/data.h"
#include "game/skin/skin_mgr.h"
#include "scene_context.h"
#include "config/config_mgr.h"
#include "imgui.h"

// prototype
vScene::vScene(eMode mode, unsigned rate, bool backgroundInput) :
    _input(1000, backgroundInput)
{
    _looper = AsyncLooper::addLooper((uintptr_t)this, "Scene Update", std::bind(&vScene::_updateAsync, this), rate);

    if (SkinMgr::get(mode) == nullptr)
        SkinMgr::load(mode);
    _skin = SkinMgr::get(mode);

    int notificationPosY = ConfigMgr::General()->get(cfg::V_RES_Y, CANVAS_HEIGHT);
    int notificationWidth = ConfigMgr::General()->get(cfg::V_RES_X, CANVAS_WIDTH);
    const int notificationHeight = 24;
#if defined _WIN32
    TCHAR windir[MAX_PATH];
    GetWindowsDirectory(windir, MAX_PATH);
    std::string fontPath = windir;
    fontPath += "\\Fonts\\msgothic.ttc";
#elif defined LINUX
    StringContent fontPath = "/usr/share/fonts/tbd.ttf"
#endif
    _fNotifications = std::make_shared<TTFFont>(fontPath.c_str(), notificationHeight);
    _texNotificationsBG = std::make_shared<TextureFull>(0x000000ff);
    for (size_t i = 0; i < _sNotifications.size(); ++i)
    {
        _sNotifications[i] = std::make_shared<SpriteText>(_fNotifications, eText(size_t(eText::_OVERLAY_NOTIFICATION_0) + i), TextAlign::TEXT_ALIGN_LEFT, notificationHeight);
        _sNotifications[i]->setLoopTime(0);
        _sNotificationsBG[i] = std::make_shared<SpriteStatic>(_texNotificationsBG);
        _sNotificationsBG[i]->setLoopTime(0);

        notificationPosY -= notificationHeight;
        RenderKeyFrame f;
        f.time = 0;
        f.param.rect = Rect(0, notificationPosY, notificationWidth, notificationHeight);
        f.param.accel = RenderParams::CONSTANT;
        f.param.color = 0xffffffff;
        f.param.blend = BlendMode::ALPHA;
        f.param.filter = false;
        f.param.angle = 0;
        f.param.center = Point(0, 0);
        _sNotifications[i]->appendKeyFrame(f);

        f.param.color = 0xffffff80;
        _sNotificationsBG[i]->appendKeyFrame(f);
    }
    {
        int textHeight = 16;
        _sTopLeft = std::make_shared<SpriteText>(_fNotifications, eText::_OVERLAY_TOPLEFT, TextAlign::TEXT_ALIGN_LEFT, textHeight);
        _sTopLeft->setLoopTime(0);
        RenderKeyFrame f;
        f.time = 0;
        f.param.rect = Rect(0, 0, notificationWidth, textHeight);
        f.param.accel = RenderParams::CONSTANT;
        f.param.color = 0xffffffff;
        f.param.blend = BlendMode::ALPHA;
        f.param.filter = false;
        f.param.angle = 0;
        f.param.center = Point(0, 0);
        _sTopLeft->appendKeyFrame(f);
    }

	Time t;

    //gNumbers.reset();
    //gSliders.reset();
    //gSwitches.reset();
    gTimers.reset();
    gTimers.set(eTimer::SCENE_START, t.norm());
    gTimers.set(eTimer::START_INPUT, t.norm() + (_skin ? _skin->info.timeIntro : 0));

    gTexts.set(eText::_OVERLAY_TOPLEFT, "");

    _input.register_p("SKIN_MOUSE_CLICK", std::bind(&vScene::MouseClick, this, std::placeholders::_1, std::placeholders::_2));
    _input.register_h("SKIN_MOUSE_DRAG", std::bind(&vScene::MouseDrag, this, std::placeholders::_1, std::placeholders::_2));
    _input.register_r("SKIN_MOUSE_RELEASE", std::bind(&vScene::MouseRelease, this, std::placeholders::_1, std::placeholders::_2));

    // Skin may be cached. Reset mouse status
    _skin ? _skin->setHandleMouseEvents(true) : __noop;
}

vScene::~vScene() 
{
    AsyncLooper::removeLooper((uintptr_t)this);
    _input.unregister_r("SKIN_MOUSE_RELEASE");
    _input.unregister_h("SKIN_MOUSE_DRAG");
    _input.unregister_p("SKIN_MOUSE_CLICK");
    sceneEnding = true; 
}

void vScene::update()
{
    Time t;
    gUpdateContext.updateTime = t;

    if (_skin)
    {
        _skin->update();
        auto [x, y] = _input.getCursorPos();
        _skin->update_mouse(x, y);
    }

    std::unique_lock lock(gOverlayContext._mutex);
    // notifications expire check
    while (!gOverlayContext.notifications.empty() && (t - gOverlayContext.notifications.begin()->first).norm() > 10 * 1000) // 10s
    {
        gOverlayContext.notifications.pop_front();
    }
    // update texts
    auto itNotifications = gOverlayContext.notifications.rbegin();
    for (size_t i = 0; i < _sNotifications.size(); ++i)
    {
        if (itNotifications != gOverlayContext.notifications.rend())
        {
            gTexts.queue(eText(size_t(eText::_OVERLAY_NOTIFICATION_0) + i), itNotifications->second);
            ++itNotifications;
        }
        else
        {
            gTexts.queue(eText(size_t(eText::_OVERLAY_NOTIFICATION_0) + i), "");
        }
    }
    gTexts.flush();
    for (size_t i = 0; i < _sNotifications.size(); ++i)
    {
        _sNotifications[i]->update(t);
        _sNotificationsBG[i]->update(t);
    }
    _sTopLeft->update(t);
}

void vScene::MouseClick(InputMask& m, const Time& t)
{
    if (m[Input::Pad::M1])
    {
        auto [x, y] = _input.getCursorPos();
        _skin->update_mouse_click(x, y);
    }
}

void vScene::MouseDrag(InputMask& m, const Time& t)
{
    if (m[Input::Pad::M1])
    {
        auto [x, y] = _input.getCursorPos();
        _skin->update_mouse_drag(x, y);
    }
}

void vScene::MouseRelease(InputMask& m, const Time& t)
{
    if (m[Input::Pad::M1])
    {
        _skin->update_mouse_release();
    }
}

void vScene::draw() const
{
    _skin ? _skin->draw() : __noop;
    _sTopLeft->draw();

    {
        std::shared_lock lock(gOverlayContext._mutex);
        if (!gOverlayContext.notifications.empty())
        {
            // draw notifications at the bottom. One string per line
            auto itNotification = gOverlayContext.notifications.rbegin();
            for (size_t i = 0; i < gOverlayContext.notifications.size(); ++i)
            {
                const auto& [timestamp, text] = *itNotification;
                _sNotificationsBG[i]->draw();
                _sNotifications[i]->draw();
            }
        }
        if (gOverlayContext.popupListShow && !gOverlayContext.popupList.empty())
        {
            // TODO draw list
        }
    }
}
