#include "game/graphics/graphics.h"
#include "graphics_SDL2.h"
#include "window_SDL2.h"
#include "SDL2_gfxPrimitives.h"
#include "common/log.h"
#include "common/sysutil.h"

#include <memory>
#include <map>
#include <future>

#define SDL_LOAD_NOAUTOFREE 0
#define SDL_LOAD_AUTOFREE 1

#ifdef _MSC_VER
#define strcpy strcpy_s
#endif

using namespace std::placeholders;

Color::Color(uint32_t rgba)
{
    r = (rgba & 0xff000000) >> 24;
    g = (rgba & 0x00ff0000) >> 16;
    b = (rgba & 0x0000ff00) >> 8;
    a = (rgba & 0x000000ff);
}

Color::Color(int r, int g, int b, int a)
{
    if (r < 0) r = 0; if (r > 255) r = 255;
    if (g < 0) g = 0; if (g > 255) g = 255;
    if (b < 0) b = 0; if (b > 255) b = 255;
    if (a < 0) a = 0; if (a > 255) a = 255;
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

uint32_t Color::hex() const
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
    if (rhs < 0) return Color(0);
    Color c;
    c.r = (r * rhs <= 255) ? (Uint8)(r * rhs) : 255;
    c.g = (g * rhs <= 255) ? (Uint8)(g * rhs) : 255;
    c.b = (b * rhs <= 255) ? (Uint8)(b * rhs) : 255;
    c.a = (a * rhs <= 255) ? (Uint8)(a * rhs) : 255;
    return c;
}

Color Color::operator*(const Color& rhs) const
{
    if (hex() == 0xffffffff) return rhs;

    Color c;
    c.r = Uint8(r * (rhs.r / 255.0));
    c.g = Uint8(g * (rhs.g / 255.0));
    c.b = Uint8(b * (rhs.b / 255.0));
    c.a = Uint8(a * (rhs.a / 255.0));
    return c;
}

bool Color::operator==(const Color& rhs) const
{
    return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
}
bool Color::operator!=(const Color& rhs) const
{
    return !(*this == rhs);
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
bool isPNG(const char* filePath)
{
    std::size_t len = strlen(filePath);
    if (len < 4) return false;

    char ext[4];
    strcpy(ext, &filePath[len - 3]);
    if (ext[0] > 'Z') ext[0] -= ('a' - 'A');
    if (ext[1] > 'Z') ext[1] -= ('a' - 'A');
    if (ext[2] > 'Z') ext[2] -= ('a' - 'A');

    return strcmp("PNG", ext) == 0;
}
bool isGIF(const char* filePath)
{
    std::size_t len = strlen(filePath);
    if (len < 4) return false;

    char ext[4];
    strcpy(ext, &filePath[len - 3]);
    if (ext[0] > 'Z') ext[0] -= ('a' - 'A');
    if (ext[1] > 'Z') ext[1] -= ('a' - 'A');
    if (ext[2] > 'Z') ext[2] -= ('a' - 'A');

    return strcmp("GIF", ext) == 0;
}

Image::Image(const std::filesystem::path& path) : Image(path.u8string().c_str()) {}

Image::Image(const char* filePath) : 
    Image(filePath, std::shared_ptr<SDL_RWops>(SDL_RWFromFile(filePath, "rb"), [](SDL_RWops* s) { if (s) s->close(s); }))
{
}

Image::Image(const char* format, void* data, size_t size): 
    Image(format, std::shared_ptr<SDL_RWops>(SDL_RWFromMem(data, size), [](SDL_RWops* s) { if (s) s->close(s); }))
{
}

Image::Image(const char* path, std::shared_ptr<SDL_RWops>&& rw): _path(path), _pRWop(rw)
{
    if (!_pRWop && strlen(path) > 0)
    {
        LOG_WARNING << "[Image] Load image file error! " << SDL_GetError();
        return;
    }
    if (isTGA(path))
    {
        _pSurface = std::shared_ptr<SDL_Surface>(
            pushAndWaitMainThreadTask<SDL_Surface*>(std::bind(IMG_LoadTGA_RW, &*_pRWop)),
            [](SDL_Surface* p) { pushMainThreadTask(std::bind(SDL_FreeSurface, p)); });
    }
    else if (isPNG(path))
    {
        _pSurface = std::shared_ptr<SDL_Surface>(
            pushAndWaitMainThreadTask<SDL_Surface*>(std::bind(IMG_LoadPNG_RW, &*_pRWop)),
            [](SDL_Surface* p) { pushMainThreadTask(std::bind(SDL_FreeSurface, p)); });
    }
    else if (isGIF(path))
    {
        _pSurface = std::shared_ptr<SDL_Surface>(
            pushAndWaitMainThreadTask<SDL_Surface*>(std::bind(IMG_LoadGIF_RW, &*_pRWop)),
            [](SDL_Surface* p) { pushMainThreadTask(std::bind(SDL_FreeSurface, p)); });
        setTransparentColorRGB(Color(0, 255, 0, -1));
    }
    else
    {
        _pSurface = std::shared_ptr<SDL_Surface>(
            pushAndWaitMainThreadTask<SDL_Surface*>(std::bind(IMG_Load_RW, &*_pRWop, SDL_LOAD_NOAUTOFREE)),
            [](SDL_Surface* p) { pushMainThreadTask(std::bind(SDL_FreeSurface, p)); });
    }

    if (!_pSurface)
    {
        LOG_WARNING << "[Image] Build surface object error! " << IMG_GetError();
        return;
    }

    _loaded = true;
    //LOG_DEBUG << "[Image] Load image file finished. " << _path.c_str();
}

Image::~Image() 
{
}

void Image::setTransparentColorRGB(Color c)
{
    if (_pSurface)
    {
        pushAndWaitMainThreadTask<void>(std::bind(
            SDL_SetColorKey, &*_pSurface, SDL_TRUE, SDL_MapRGB(_pSurface->format, c.r, c.g, c.b)));
    }
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

    _pTexture = std::shared_ptr<SDL_Texture>(
        pushAndWaitMainThreadTask<SDL_Texture*>(std::bind(SDL_CreateTextureFromSurface, gFrameRenderer, &*srcImage._pSurface)),
        [](SDL_Texture* p) { pushMainThreadTask(std::bind(SDL_DestroyTexture, p)); });
    if (_pTexture)
    {
        _texRect = srcImage.getRect();
        _loaded = true;
    }

    if (!_loaded)
    {
        LOG_WARNING << "[Texture] Build texture object error! " << srcImage._path.c_str();
        LOG_WARNING << "[Texture] ^ " << SDL_GetError();
    }
    else
    {
        //LOG_DEBUG << "[Texture] Build texture object finished. " << srcImage._path.c_str();
    }
}

Texture::Texture(const Image& srcImage, const Rect& srcRect)
{
    if (!srcImage._loaded) return;
    if (srcRect.w == 0 && srcRect.h == 0)
    {
        _loaded = true;
        return;
    }

    auto pSrcSurface = srcImage._pSurface;
    SDL_Surface* surfaceTmp = SDL_CreateRGBSurface(pSrcSurface->flags, srcRect.w, srcRect.h, pSrcSurface->format->BitsPerPixel,
        pSrcSurface->format->Rmask, pSrcSurface->format->Gmask, pSrcSurface->format->Bmask, pSrcSurface->format->Amask);
    SDL_SetSurfaceBlendMode(&*pSrcSurface, SDL_BLENDMODE_NONE);
    SDL_BlitSurface(&*pSrcSurface, &srcRect, surfaceTmp, NULL);

    _pTexture = std::shared_ptr<SDL_Texture>(
        pushAndWaitMainThreadTask<SDL_Texture*>(std::bind(SDL_CreateTextureFromSurface, gFrameRenderer, surfaceTmp)),
        [](SDL_Texture* p) { pushMainThreadTask(std::bind(SDL_DestroyTexture, p)); });
    if (_pTexture)
    {
        _texRect = srcImage.getRect();
        _loaded = true;
    }
    if (!_loaded)
    {
        LOG_WARNING << "[Texture] Build texture object error! " << srcImage._path.c_str();
        LOG_WARNING << "[Texture] ^ " << SDL_GetError();
    }
    else
    {
        //LOG_DEBUG << "[Texture] Build texture object finished. " << srcImage._path.c_str();
    }

    SDL_FreeSurface(surfaceTmp);
}

Texture::Texture(const SDL_Surface* pSurface)
{
    _pTexture = std::shared_ptr<SDL_Texture>(
        pushAndWaitMainThreadTask<SDL_Texture*>(std::bind(SDL_CreateTextureFromSurface, gFrameRenderer, const_cast<SDL_Surface*>(pSurface))),
        [](SDL_Texture* p) { pushMainThreadTask(std::bind(SDL_DestroyTexture, p)); });
    if (!_pTexture) return;
    _texRect = pSurface->clip_rect;
    _loaded = true;
}

Texture::Texture(const SDL_Texture* pTexture, int w, int h)
{
    _pTexture = std::shared_ptr<SDL_Texture>(
        const_cast<SDL_Texture*>(pTexture),
        [](SDL_Texture* p) { pushMainThreadTask(std::bind(SDL_DestroyTexture, p)); });
    if (!pTexture) return;
    _texRect = {0, 0, w, h};
    _loaded = true;
}

Texture::Texture(int w, int h, PixelFormat fmt, bool target)
{
	SDL_PixelFormatEnum sdlfmt = SDL_PIXELFORMAT_UNKNOWN;
	switch (fmt)
	{
    case PixelFormat::RGB24:
        sdlfmt = SDL_PIXELFORMAT_RGB24; break;
    case PixelFormat::BGR24:
        sdlfmt = SDL_PIXELFORMAT_BGR24; break;
	case PixelFormat::YV12:
		sdlfmt = SDL_PIXELFORMAT_YV12; break;
	case PixelFormat::IYUV:
		sdlfmt = SDL_PIXELFORMAT_IYUV; break;
	case PixelFormat::YUY2:
		sdlfmt = SDL_PIXELFORMAT_YUY2; break;
	case PixelFormat::UYVY:
		sdlfmt = SDL_PIXELFORMAT_UYVY; break;
	case PixelFormat::YVYU:
		sdlfmt = SDL_PIXELFORMAT_YVYU; break;
	default:
		sdlfmt = SDL_PIXELFORMAT_UNKNOWN; break;
	}

	if (sdlfmt != SDL_PIXELFORMAT_UNKNOWN)
	{
        _pTexture = std::shared_ptr<SDL_Texture>(
            pushAndWaitMainThreadTask<SDL_Texture*>(std::bind(SDL_CreateTexture, gFrameRenderer, sdlfmt, target ? SDL_TEXTUREACCESS_TARGET : SDL_TEXTUREACCESS_STREAMING, w, h)),
            [](SDL_Texture* p) { pushMainThreadTask(std::bind(SDL_DestroyTexture, p)); });
        if (_pTexture)
        {
            _texRect = { 0, 0, w, h };
            _loaded = true;
        }
	}
}

Texture::~Texture()
{
}

int Texture::updateYUV(uint8_t* Y, int Ypitch, uint8_t* U, int Upitch, uint8_t* V, int Vpitch)
{
    assert(IsMainThread());

    if (!_loaded) return -1;
    if (!Ypitch || !Upitch || !Vpitch) return -2;
    return SDL_UpdateYUVTexture(
        &*_pTexture, nullptr,
        Y, Ypitch,
        U, Upitch,
        V, Vpitch);
}

void Texture::_draw(std::shared_ptr<SDL_Texture> pTex, const Rect* srcRect, Rect dstRect,
	const Color c, const BlendMode b, const bool filter, const double angle, const Point* center)
{
    int flipFlags = 0;
    if (dstRect.w < 0) { dstRect.w = -dstRect.w; dstRect.x -= dstRect.w; /*flipFlags |= SDL_FLIP_HORIZONTAL;*/ }
    if (dstRect.h < 0) { dstRect.h = -dstRect.h; dstRect.y -= dstRect.h; /*flipFlags |= SDL_FLIP_VERTICAL;*/ }

	SDL_SetTextureColorMod(&*pTex, c.r, c.g, c.b);

    int ssLevel = graphics_get_supersample_level();
    dstRect.x *= ssLevel;
    dstRect.y *= ssLevel;
    dstRect.w *= ssLevel;
    dstRect.h *= ssLevel;

    SDL_Point scenter;
    if (center) scenter = { (int)center->x * ssLevel, (int)center->y * ssLevel };

    SDL_SetTextureScaleMode(&*pTex, filter ? SDL_ScaleModeBest : SDL_ScaleModeNearest);

    if (b == BlendMode::INVERT)
    {
        // ... pls help
        Rect rc = dstRect;
        rc.x = rc.y = 0;

        SDL_Texture* pTextureInverted = SDL_CreateTexture(gFrameRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, dstRect.w, dstRect.h);
        auto oldTarget = SDL_GetRenderTarget(gFrameRenderer);
        SDL_SetRenderTarget(gFrameRenderer, pTextureInverted);

        uint8_t r, g, b, a;
        SDL_GetRenderDrawColor(gFrameRenderer, &r, &g, &b, &a);
        SDL_SetRenderDrawColor(gFrameRenderer, 255, 255, 255, 255);
        SDL_RenderFillRect(gFrameRenderer, &rc);
        SDL_SetRenderDrawColor(gFrameRenderer, r, g, b, a);

        static auto blendMode = SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR, SDL_BLENDOPERATION_ADD,
            SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_ADD);
        SDL_SetTextureBlendMode(&*pTex, blendMode);
        SDL_SetTextureAlphaMod(&*pTex, c.a);
        SDL_RenderCopyEx(
            gFrameRenderer,
            &*pTex,
            srcRect, &rc,
            0, 
            NULL, SDL_FLIP_NONE
        );

        SDL_SetRenderTarget(gFrameRenderer, oldTarget);
        SDL_SetTextureBlendMode(pTextureInverted, SDL_BLENDMODE_BLEND);
        SDL_RenderCopyEx(
            gFrameRenderer,
            pTextureInverted,
            &rc, &dstRect,
            angle,
            center ? &scenter : NULL, SDL_RendererFlip(flipFlags)
        );

        SDL_DestroyTexture(pTextureInverted);
        return;
    }
    else if (b == BlendMode::MULTIPLY_INVERTED_BACKGROUND)
    {
        if (c.a <= 1) return;   // do not draw
        // FIXME lmao

        const SDL_BlendMode blendMode = SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_DST_COLOR, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD,
            SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD);

        SDL_SetTextureBlendMode(&*pTex, blendMode);
    }
    else
    {
        SDL_SetTextureAlphaMod(&*pTex, b == BlendMode::NONE ? 255 : c.a);

        static const std::map<BlendMode, SDL_BlendMode> BlendMap
        {
            { BlendMode::NONE, SDL_BLENDMODE_BLEND },  // Do not use SDL_BLENDMODE_NONE, set alpha=255 instead
            { BlendMode::ALPHA, SDL_BLENDMODE_BLEND },
            { BlendMode::ADD, SDL_BLENDMODE_ADD },
            { BlendMode::MOD, SDL_BLENDMODE_MOD },

            { BlendMode::SUBTRACT, SDL_ComposeCustomBlendMode(
                SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
                SDL_BLENDFACTOR_ONE_MINUS_DST_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD) },

            { BlendMode::INVERT, SDL_ComposeCustomBlendMode(
                SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
                SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD) },

            { BlendMode::MULTIPLY_WITH_ALPHA, SDL_ComposeCustomBlendMode(
                SDL_BLENDFACTOR_DST_COLOR, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD,
                SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD)} // FIXME this is not correct
        };

        if (BlendMap.find(b) != BlendMap.end())
        {
            SDL_SetTextureBlendMode(&*pTex, BlendMap.at(b));
        }
    }

    SDL_RenderCopyEx(
        gFrameRenderer,
        &*pTex,
        srcRect, &dstRect,
        angle,
        center ? &scenter : NULL, SDL_RendererFlip(flipFlags)
    );
}

void Texture::draw(Rect dstRect,
	const Color c, const BlendMode b, const bool filter, const double angle) const
{
	_draw(_pTexture, NULL, dstRect, c, b, filter, angle, NULL);
}

void Texture::draw(Rect dstRect,
	const Color c, const BlendMode b, const bool filter, const double angle, const Point& center) const
{
	_draw(_pTexture, NULL, dstRect, c, b, filter, angle, &center);
}

void Texture::draw(const Rect& srcRect, Rect dstRect,
	const Color c, const BlendMode b, const bool filter, const double angle) const
{
    Rect srcRectTmp(srcRect);
    if (srcRectTmp.w == RECT_FULL.w) srcRectTmp.w = _texRect.w;
    if (srcRectTmp.h == RECT_FULL.h) srcRectTmp.h = _texRect.h;
	_draw(_pTexture, &srcRectTmp, dstRect, c, b, filter, angle, NULL);
}

void Texture::draw(const Rect& srcRect, Rect dstRect,
	const Color c, const BlendMode b, const bool filter, const double angle, const Point& center) const
{
    Rect srcRectTmp(srcRect);
    if (srcRectTmp.w == RECT_FULL.w) srcRectTmp.w = _texRect.w;
    if (srcRectTmp.h == RECT_FULL.h) srcRectTmp.h = _texRect.h;
	_draw(_pTexture, &srcRectTmp, dstRect, c, b, filter, angle, &center);
}

////////////////////////////////////////////////////////////////////////////////
// TextureFull

TextureFull::TextureFull(const Color& c): Texture(nullptr)
{
    pushAndWaitMainThreadTask<void>([this, &c]()
        {
            auto surface = SDL_CreateRGBSurfaceWithFormat(0, 1, 1, 24, SDL_PIXELFORMAT_RGB24);
            _texRect = { 0,0,1,1 };
            SDL_FillRect(&*surface, &_texRect, SDL_MapRGBA(surface->format, c.r, c.g, c.b, c.a));
            _pTexture = std::shared_ptr<SDL_Texture>(
                pushAndWaitMainThreadTask<SDL_Texture*>(std::bind(SDL_CreateTextureFromSurface, gFrameRenderer, surface)),
                std::bind(pushAndWaitMainThreadTask<void, SDL_Texture*>, SDL_DestroyTexture, _1));
            SDL_FreeSurface(surface);
        });
    _loaded = true;
}

TextureFull::TextureFull(const Image& srcImage) : Texture(srcImage) {}

TextureFull::TextureFull(const SDL_Surface* pSurface): Texture(pSurface) {}

TextureFull::TextureFull(const SDL_Texture* pTexture, int w, int h): Texture(pTexture, w, h) {}

TextureFull::~TextureFull() {}

void TextureFull::draw(const Rect& ignored, Rect dstRect,
    const Color c, const BlendMode b, const bool filter, const double angle) const
{
	SDL_SetTextureColorMod(&*_pTexture, c.r, c.g, c.b);

    int ssLevel = graphics_get_supersample_level();
    dstRect.x *= ssLevel;
    dstRect.y *= ssLevel;
    dstRect.w *= ssLevel;
    dstRect.h *= ssLevel;

    SDL_SetTextureScaleMode(&*_pTexture, filter ? SDL_ScaleModeBest : SDL_ScaleModeNearest);

    if (b == BlendMode::MULTIPLY_INVERTED_BACKGROUND)
    {
        if (c.a <= 1) return;   // do not draw

        const SDL_BlendMode blendMode = SDL_ComposeCustomBlendMode(
            SDL_BLENDFACTOR_DST_COLOR, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD,
            SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD);

        SDL_SetTextureBlendMode(&*_pTexture, blendMode);
    }
    else
    {
        SDL_SetTextureAlphaMod(&*_pTexture, b == BlendMode::NONE ? 255 : c.a);

        static const std::map<BlendMode, SDL_BlendMode> BlendMap
        {
            { BlendMode::NONE, SDL_BLENDMODE_BLEND },  // Do not use SDL_BLENDMODE_NONE, set alpha=255 instead
            { BlendMode::ALPHA, SDL_BLENDMODE_BLEND },
            { BlendMode::ADD, SDL_BLENDMODE_ADD },
            { BlendMode::MOD, SDL_BLENDMODE_MOD },

            { BlendMode::SUBTRACT, SDL_ComposeCustomBlendMode(
                SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
                SDL_BLENDFACTOR_ONE_MINUS_DST_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_ADD) },

            { BlendMode::INVERT, SDL_ComposeCustomBlendMode(
                SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_REV_SUBTRACT,
                SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD) },

            { BlendMode::MULTIPLY_WITH_ALPHA, SDL_ComposeCustomBlendMode(
                SDL_BLENDFACTOR_DST_COLOR, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD,
                SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD)}
        };

        if (BlendMap.find(b) != BlendMap.end())
        {
            SDL_SetTextureBlendMode(&*_pTexture, BlendMap.at(b));
        }
    }

    SDL_RenderCopyEx(
        gFrameRenderer,
        &*_pTexture,
        &_texRect, &dstRect,
        angle,
        NULL, SDL_FLIP_NONE
    );
}

void GraphLine::draw(Point p1, Point p2, Color c) const
{
    int ss = graphics_get_supersample_level();
	thickLineRGBA(
		gFrameRenderer,
		(Sint16)p1.x * ss, (Sint16)p1.y * ss,
        (Sint16)p2.x * ss, (Sint16)p2.y * ss,
		_width * ss,
		c.r, c.g, c.b, c.a
	);
    SDL_SetRenderDrawColor(gFrameRenderer, 0, 0, 0, 255);
}