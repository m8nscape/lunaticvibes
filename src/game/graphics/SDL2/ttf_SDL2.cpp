#include "graphics_SDL2.h"
#include "common/log.h"

TTFFont::TTFFont(const char* filePath, int ptsize)
{
	// FIXME Maybe we need a global buffer for fonts opened...
	// Current implement may load the file EVERYTIME we declare a text instance.
    _filePath = filePath;    // copy an instance here
    pushMainThreadTask([&]() { _pFont = TTF_OpenFont(_filePath.c_str(), ptsize); });
    if (!_pFont)
        LOG_WARNING << "[TTF] " << filePath << ": " << TTF_GetError();
    else _loaded = true;
}

TTFFont::~TTFFont()
{
    if (!_loaded) return;

    pushMainThreadTask(std::bind(TTF_CloseFont, _pFont));
}


void TTFFont::setStyle(TTFStyle style)
{
    assert(IsMainThread());
    if (!_loaded) return;

    switch (style)
    {
    case TTFStyle::Normal:    pushMainThreadTask(std::bind(TTF_SetFontStyle, _pFont, TTF_STYLE_NORMAL)); break;
    case TTFStyle::Bold:      pushMainThreadTask(std::bind(TTF_SetFontStyle, _pFont, TTF_STYLE_BOLD));   break;
    case TTFStyle::Italic:    pushMainThreadTask(std::bind(TTF_SetFontStyle, _pFont, TTF_STYLE_ITALIC)); break;
    case TTFStyle::_BI:       pushMainThreadTask(std::bind(TTF_SetFontStyle, _pFont, TTF_STYLE_BOLD | TTF_STYLE_ITALIC)); break;
    }
}
void TTFFont::setOutline(bool enabled)
{
    assert(IsMainThread());
    if (!_loaded) return;

    pushMainThreadTask(std::bind(TTF_SetFontOutline, _pFont, enabled));
}
void TTFFont::setHinting(TTFHinting mode)
{
    assert(IsMainThread());
    if (!_loaded) return;

    switch (mode)
    {
    case TTFHinting::Normal:    TTF_SetFontHinting(_pFont, TTF_HINTING_NORMAL); break;
    case TTFHinting::Light:     TTF_SetFontHinting(_pFont, TTF_HINTING_LIGHT);  break;
    case TTFHinting::Mono:      TTF_SetFontHinting(_pFont, TTF_HINTING_MONO);   break;
    case TTFHinting::None:      TTF_SetFontHinting(_pFont, TTF_HINTING_NONE);   break;
    }
}
void TTFFont::setKerning(bool enabled)
{
    assert(IsMainThread());
    if (!_loaded) return;

    pushMainThreadTask(std::bind(TTF_SetFontKerning, _pFont, enabled));
}

std::shared_ptr<Texture> TTFFont::TextUTF8(const char* text, const Color& c)
{
    assert(IsMainThread());
    if (!_loaded) return nullptr;

    auto pSurf = TTF_RenderUTF8_Blended(_pFont, text, c);
    if (pSurf != NULL)
    {
        auto p = std::shared_ptr<SDL_Surface>(pSurf, SDL_FreeSurface);
        return std::make_shared<Texture>(&*p);
    }
    //LOG_WARNING << "[TTF] " << text << ": " << TTF_GetError();
    return nullptr;
}
std::shared_ptr<Texture> TTFFont::TextUTF8Solid(const char* text, const Color& c)
{
    assert(IsMainThread());
    if (!_loaded) return nullptr;

    auto pSurf = TTF_RenderUTF8_Solid(_pFont, text, c);
    if (pSurf != NULL)
    {
        auto p = std::shared_ptr<SDL_Surface>(pSurf, SDL_FreeSurface);
        return std::make_shared<Texture>(&*p);
    }
    //LOG_WARNING << "[TTF] " << text << ": " << TTF_GetError();
    return nullptr;
}
std::shared_ptr<Texture> TTFFont::TextUTF8Shaded(const char* text, const Color& c, const Color& bg)
{
    assert(IsMainThread());
    if (!_loaded) return nullptr;

    auto pSurf = TTF_RenderUTF8_Shaded(_pFont, text, c, bg);
    if (pSurf != NULL)
    {
        auto p = std::shared_ptr<SDL_Surface>(pSurf, SDL_FreeSurface);
        return std::make_shared<Texture>(&*p);
    }
    //LOG_WARNING << "[TTF] " << text << ": " << TTF_GetError();
    return nullptr;
}

Rect TTFFont::getRectUTF8(const char* text)
{
    assert(IsMainThread());
    Rect r{ 0, 0, 0, 0 };

    if (!_loaded) return r;
    TTF_SizeUTF8(_pFont, text, &r.w, &r.h);
    return r;
}
