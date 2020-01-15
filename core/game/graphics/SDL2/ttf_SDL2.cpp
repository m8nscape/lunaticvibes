#include "graphics_SDL2.h"
#include "plog/Log.h"

TTFFont::TTFFont(const char* filePath, int ptsize): _pFont(TTF_OpenFont(filePath, ptsize))
{
	// FIXME Maybe we need a global buffer for fonts opened...
	// Current implement may load the file EVERYTIME we declare a text instance.
    if (!_pFont)
        LOG_WARNING << "[TTF] " << filePath << ": " << TTF_GetError();
    else _loaded = true;
}

TTFFont::~TTFFont()
{
    if (!_loaded) return;
	TTF_CloseFont(_pFont);
}


void TTFFont::setStyle(TTFStyle style)
{
    if (!_loaded) return;
    switch (style)
    {
    case TTFStyle::Normal:    TTF_SetFontStyle(_pFont, TTF_STYLE_NORMAL); break;
    case TTFStyle::Bold:      TTF_SetFontStyle(_pFont, TTF_STYLE_BOLD);   break;
    case TTFStyle::Italic:    TTF_SetFontStyle(_pFont, TTF_STYLE_ITALIC); break;
    case TTFStyle::_BI:       TTF_SetFontStyle(_pFont, TTF_STYLE_BOLD | TTF_STYLE_ITALIC); break;
    }
}
void TTFFont::setOutline(bool enabled)
{
    if (!_loaded) return;
    TTF_SetFontOutline(_pFont, enabled);
}
void TTFFont::setHinting(TTFHinting mode)
{
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
    if (!_loaded) return;
    TTF_SetFontKerning(_pFont, enabled);
}

std::shared_ptr<Texture> TTFFont::TextUTF8(const char* text, const Color& c)
{
    if (!_loaded) return nullptr;
    auto pSurf = TTF_RenderUTF8_Blended(_pFont, text, c);
    if (pSurf != NULL)
    {
        auto p = std::shared_ptr<SDL_Surface>(pSurf, SDL_FreeSurface);
        return std::make_shared<Texture>(&*p);
    }
    LOG_WARNING << "[TTF] " << text << ": " << TTF_GetError();
    return nullptr;
}
std::shared_ptr<Texture> TTFFont::TextUTF8Solid(const char* text, const Color& c)
{
    if (!_loaded) return nullptr;
    auto pSurf = TTF_RenderUTF8_Solid(_pFont, text, c);
    if (pSurf != NULL)
    {
        auto p = std::shared_ptr<SDL_Surface>(pSurf, SDL_FreeSurface);
        return std::make_shared<Texture>(&*p);
    }
    LOG_WARNING << "[TTF] " << text << ": " << TTF_GetError();
    return nullptr;
}
std::shared_ptr<Texture> TTFFont::TextUTF8Shaded(const char* text, const Color& c, const Color& bg)
{
    if (!_loaded) return nullptr;
    auto pSurf = TTF_RenderUTF8_Shaded(_pFont, text, c, bg);
    if (pSurf != NULL)
    {
        auto p = std::shared_ptr<SDL_Surface>(pSurf, SDL_FreeSurface);
        return std::make_shared<Texture>(&*p);
    }
    LOG_WARNING << "[TTF] " << text << ": " << TTF_GetError();
    return nullptr;
}

Rect TTFFont::getRectUTF8(const char* text)
{
    Rect r{ 0, 0, 0, 0 };
    if (_loaded) TTF_SizeUTF8(_pFont, text, &r.w, &r.h);
    return r;
}
