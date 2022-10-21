#include "graphics_SDL2.h"
#include "common/log.h"

TTFFont::TTFFont(const char* filePath, int ptsize): _filePath(filePath), _ptsize(ptsize)
{
    pushAndWaitMainThreadTask<void>([&]() { _pFont = TTF_OpenFont(_filePath.c_str(), ptsize); });
    if (!_pFont)
        LOG_WARNING << "[TTF] " << filePath << ": " << TTF_GetError();
    else _loaded = true;
}

TTFFont::TTFFont(const char* filePath, int ptsize, int faceIndex) : _filePath(filePath), _ptsize(ptsize), _faceIndex(faceIndex)
{
    pushAndWaitMainThreadTask<void>([&]() { _pFont = TTF_OpenFontIndex(_filePath.c_str(), ptsize, faceIndex); });
    if (!_pFont)
        LOG_WARNING << "[TTF] " << filePath << ": " << TTF_GetError();
    else _loaded = true;
}

TTFFont::~TTFFont()
{
    if (!_loaded) return;

    if (_pFontOutline)
        pushAndWaitMainThreadTask<void>(std::bind(TTF_CloseFont, _pFontOutline));
    if (_pFont) 
        pushAndWaitMainThreadTask<void>(std::bind(TTF_CloseFont, _pFont));
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
void TTFFont::setOutline(int width, const Color& c)
{
    assert(IsMainThread());
    if (!_loaded) return;

    if (width == 0)
    {
        if (_pFontOutline != NULL)
        {
            pushAndWaitMainThreadTask<void>(std::bind(TTF_CloseFont, _pFontOutline));
            _pFontOutline = NULL;
        }
    }
    else
    {
        if (_pFontOutline == NULL)
        {
            pushAndWaitMainThreadTask<void>([&]() 
                { 
                    if (_faceIndex >= 0)
                        _pFontOutline = TTF_OpenFontIndex(_filePath.c_str(), _ptsize, _faceIndex);
                    else
                        _pFontOutline = TTF_OpenFont(_filePath.c_str(), _ptsize);
                    TTF_SetFontOutline(_pFontOutline, width);
                });
        }
    }
    _outlineWidth = width;
    _outlineColor = c;
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

    SDL_Surface* surfaceText = TTF_RenderUTF8_Blended(_pFont, text, c);
    SDL_Rect rcText = { 0 };
    TTF_SizeUTF8(_pFont, text, &rcText.w, &rcText.h);
    if (_pFontOutline)
    {
        SDL_Surface* surfaceOutline = TTF_RenderUTF8_Blended(_pFontOutline, text, _outlineColor);

        SDL_Rect rcTextInner = rcText;
        //rcTextInner.x += (rcOutline.w - rcText.w) / 2;
        //rcTextInner.y += (rcOutline.h - rcText.h) / 2;
        rcTextInner.x += _outlineWidth;
        rcTextInner.y += _outlineWidth;
        SDL_BlitSurface(surfaceText, &rcText, surfaceOutline, &rcTextInner);

        std::shared_ptr<Texture> pTexture = std::make_shared<Texture>(surfaceOutline);
        SDL_FreeSurface(surfaceText);
        SDL_FreeSurface(surfaceOutline);
        return pTexture;
    }

    std::shared_ptr<Texture> pTexture = std::make_shared<Texture>(surfaceText);
    SDL_FreeSurface(surfaceText);
    return pTexture;
}

Rect TTFFont::getRectUTF8(const char* text)
{
    assert(IsMainThread());
    Rect r{ 0, 0, 0, 0 };

    if (!_loaded) return r;
    TTF_SizeUTF8(_pFontOutline ? _pFontOutline : _pFont, text, &r.w, &r.h);
    return r;
}
