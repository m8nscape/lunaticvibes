#include "graphics_SDL2.h"
#include "window_SDL2.h"
#include <plog/Log.h>
#include <memory>

#define SDL_LOAD_NOAUTOFREE 0
#define SDL_LOAD_AUTOFREE 1

Color::Color(unsigned rgba)
{
    r = rgba & 0xff000000 >> 24;
    g = rgba & 0x00ff0000 >> 16;
    b = rgba & 0x0000ff00 >> 8;
    a = rgba & 0x000000ff;
}

Color::Color(unsigned r, unsigned g, unsigned b, unsigned a)
{
    this->r = r > 0xff ? 0xff : r;
    this->g = g > 0xff ? 0xff : g;
    this->b = b > 0xff ? 0xff : b;
    this->a = a > 0xff ? 0xff : a;
}

Color::operator Uint8() const
{
    return r << 24 | g << 16 | b << 8 | a;
}

Color Color::operator+(const Color& rhs) const
{
    // always >=0
    Color c;
    c.r = (r + rhs.r <= 255) ? (r + rhs.r) : 255;
    c.g = (g + rhs.g <= 255) ? (g + rhs.g) : 255;
    c.b = (b + rhs.b <= 255) ? (b + rhs.b) : 255;
    c.a = (a + rhs.a <= 255) ? (a + rhs.a) : 255;
    return c;
}

Color Color::operator*(const double& rhs) const
{
    Color c;
    c.r = (r * rhs <= 255) ? (Uint8)(r * rhs) : 255;
    c.g = (g * rhs <= 255) ? (Uint8)(g * rhs) : 255;
    c.b = (b * rhs <= 255) ? (Uint8)(b * rhs) : 255;
    c.a = (a * rhs <= 255) ? (Uint8)(a * rhs) : 255;
    return c;
}

////////////////////////////////////////////////////////////////////////////////
// Image

bool isTGA(const char* filePath)
{
    std::size_t len = strlen(filePath);
    if (len < 4) return false;

    char ext[4];
    strcpy(ext, &filePath[len - 3]);
    if (ext[0] > 'Z') ext[0] -= ('a' - 'A');
    if (ext[1] > 'Z') ext[1] -= ('a' - 'A');
    if (ext[2] > 'Z') ext[2] -= ('a' - 'A');

    return strcmp("TGA", ext) == 0;
}

Image::Image(const char* filePath) : _path(filePath), _pRWop(SDL_RWFromFile(filePath, "rb"), [](SDL_RWops* s) { if (s) s->close(s); })
{
    if (!_pRWop && strlen(filePath) > 0)
    {
        LOG_WARNING << "[Image] Load image file error! " << filePath;
        LOG_WARNING << "[Image] " << SDL_GetError();
    }
    if (!isTGA(filePath))
    {
        // Start non-TGA type loading
        _pSurface = std::shared_ptr<SDL_Surface>(
            IMG_Load_RW(&*_pRWop, SDL_LOAD_NOAUTOFREE), SDL_FreeSurface);
        if (!_pSurface)
        {
            LOG_WARNING << "[Image] Build surface object error! " << filePath;
            LOG_WARNING << "[Image] ^ " << IMG_GetError();
            return;
        }

        _loaded = true;
        if (IMG_isGIF(&*_pRWop) || IMG_isPNG(&*_pRWop))
            _haveAlphaLayer = true;
    }
    else
    {
        // separated TGA type loading since IMG_Load_RW does not support TGA.
        _pSurface = std::shared_ptr<SDL_Surface>(
            IMG_LoadTGA_RW(&*_pRWop), SDL_FreeSurface);
        if (!_pSurface)
        {
            LOG_WARNING << "[Image] Build surface object error! " << filePath;
            LOG_WARNING << "[Image] ^ " << IMG_GetError();
            return;
        }

        _loaded = true;
        _haveAlphaLayer = true;
    }
    LOG_DEBUG << "[Image] Load image file finished. " << _path.c_str();
}

Image::~Image() 
{
}

Rect Image::getRect() const
{
    return Rect(
        0,
        0,
        _pSurface->w,
        _pSurface->h
    );
}

////////////////////////////////////////////////////////////////////////////////
// Texture

Texture::Texture(const Image& srcImage)
{
    if (!srcImage._loaded) return;

    if (!srcImage._haveAlphaLayer)
    {
        Image tmpImage = srcImage;
        // TODO remove pixels that are equal to TRANSPARENT color
        // Requiring get full pixel data from instance, leading to long load time
        _pTexture = SDL_CreateTextureFromSurface(_frame_renderer, &*tmpImage._pSurface);
        if (_pTexture)
        {
            _texRect = srcImage.getRect();
            _loaded = true;
        }
    }
    else
    {
        _pTexture = SDL_CreateTextureFromSurface(_frame_renderer, &*srcImage._pSurface);
        if (_pTexture)
        {
            _texRect = srcImage.getRect();
            _loaded = true;
        }
    }

    if (!_loaded)
    {
        LOG_WARNING << "[Texture] Build texture object error! " << srcImage._path.c_str();
        LOG_WARNING << "[Texture] ^ " << SDL_GetError();
    }
    else
        LOG_DEBUG << "[Texture] Build texture object finished. " << srcImage._path.c_str();
}

Texture::Texture(const SDL_Surface* pSurface):
    _pTexture(SDL_CreateTextureFromSurface(_frame_renderer, const_cast<SDL_Surface*>(pSurface)))
{
    if (!_pTexture) return;
    _texRect = pSurface->clip_rect;
    _loaded = true;
}

Texture::Texture(const SDL_Texture* pTexture, int w, int h):
    _pTexture(const_cast<SDL_Texture*>(pTexture))
{
    if (!pTexture) return;
    _texRect = {0, 0, w, h};
    _loaded = true;
}

Texture::~Texture()
{
    if (_loaded && _pTexture)
        SDL_DestroyTexture(_pTexture);
}

Rect Texture::getRect() const
{
    return _texRect;
}

void Texture::_draw(const Rect& srcRect, Rect dstRect,
    const Color c, const BlendMode b, const bool filter, const double angle) const
{
	if (dstRect.w < 0) { dstRect.w = -dstRect.w; dstRect.x -= dstRect.w; }
	if (dstRect.h < 0) { dstRect.h = -dstRect.h; dstRect.y -= dstRect.h; }
	SDL_SetTextureColorMod(&*_pTexture, c.r, c.g, c.b);
	SDL_SetTextureAlphaMod(&*_pTexture, c.a);
	SDL_SetTextureBlendMode(&*_pTexture, (SDL_BlendMode)b);
    SDL_RenderCopyEx(
        _frame_renderer,
        &*_pTexture,
        &srcRect, &dstRect,
        angle,
        NULL, SDL_FLIP_NONE
    );
}


////////////////////////////////////////////////////////////////////////////////
// TextureFull

TextureFull::TextureFull(const Color& c): Texture(nullptr)
{
    auto surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0xff, 0xff, 0xff, 0xff);
    SDL_Rect r{ 0, 0, 1, 1 };
    SDL_FillRect(&*surface, &r, c);
    _pTexture = SDL_CreateTextureFromSurface(_frame_renderer, surface);
    SDL_FreeSurface(surface);
    _texRect = { 0,0,1,1 };
    _loaded = true;
}

TextureFull::TextureFull(const Image& srcImage) : Texture(srcImage) {}

TextureFull::TextureFull(const SDL_Surface* pSurface): Texture(pSurface) {}

TextureFull::TextureFull(const SDL_Texture* pTexture, int w, int h): Texture(pTexture, w, h) {}

TextureFull::~TextureFull() {}

void TextureFull::_draw(const Rect& ignored, const Rect& dstRect,
    const Color c, const double angle) const
{
	SDL_SetTextureColorMod(&*_pTexture, c.r, c.g, c.b);
	SDL_SetTextureAlphaMod(&*_pTexture, c.a);
    SDL_RenderCopyEx(
        _frame_renderer,
        &*_pTexture,
        &_texRect, &dstRect,
        angle,
        NULL, SDL_FLIP_NONE
    );
}
