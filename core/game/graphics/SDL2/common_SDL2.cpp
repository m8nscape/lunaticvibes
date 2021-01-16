#include "graphics_SDL2.h"
#include "window_SDL2.h"
#include "SDL2_gfxPrimitives.h"
#include <memory>
#include <map>
#include <plog/Log.h>

#define SDL_LOAD_NOAUTOFREE 0
#define SDL_LOAD_AUTOFREE 1

#ifdef _MSC_VER
#define strcpy strcpy_s
#endif

const std::map<BlendMode, SDL_BlendMode> BlendMap =
{

    { BlendMode::NONE                       , SDL_BLENDMODE_BLEND      },  // ???
    { BlendMode::ALPHA                      , SDL_BLENDMODE_BLEND      },
    { BlendMode::ADD                        , SDL_BLENDMODE_ADD        },
    { BlendMode::MULTIPLY                   , SDL_BLENDMODE_MOD        },
    { BlendMode::SUBTRACT                   , SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_SRC_COLOR, SDL_BLENDFACTOR_DST_COLOR, SDL_BLENDOPERATION_SUBTRACT, SDL_BLENDFACTOR_ZERO, SDL_BLENDFACTOR_DST_ALPHA, SDL_BLENDOPERATION_ADD)  },
    { BlendMode::ANTICOLOR                  , SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE_MINUS_SRC_COLOR, SDL_BLENDFACTOR_DST_COLOR, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDFACTOR_DST_ALPHA, SDL_BLENDOPERATION_ADD)  },
};


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

Image::Image(const std::filesystem::path& path) : Image(path.string().c_str()) {}

Image::Image(const char* filePath) : _path(filePath), _pRWop(SDL_RWFromFile(filePath, "rb"), [](SDL_RWops* s) { if (s) s->close(s); })
{
    if (!_pRWop && strlen(filePath) > 0)
    {
        LOG_WARNING << "[Image] Load image file error! " << SDL_GetError();
    }
    if (isTGA(filePath))
    {
        _pSurface = std::shared_ptr<SDL_Surface>(IMG_LoadTGA_RW(&*_pRWop), SDL_FreeSurface);
        setTransparentColorRGB(Color(0, 255, 0, -1));
    }
    else if (isPNG(filePath))
    {
        _pSurface = std::shared_ptr<SDL_Surface>(IMG_LoadPNG_RW(&*_pRWop), SDL_FreeSurface);
    }
    else if (isGIF(filePath))
    {
        _pSurface = std::shared_ptr<SDL_Surface>(IMG_LoadGIF_RW(&*_pRWop), SDL_FreeSurface);
        setTransparentColorRGB(Color(0, 255, 0, -1));
    }
    else
    {
        _pSurface = std::shared_ptr<SDL_Surface>(IMG_Load_RW(&*_pRWop, SDL_LOAD_NOAUTOFREE), SDL_FreeSurface);
    }

    if (!_pSurface)
    {
        LOG_WARNING << "[Image] Build surface object error! " << IMG_GetError();
        return;
    }

    _loaded = true;
    LOG_DEBUG << "[Image] Load image file finished. " << _path.c_str();
}

Image::~Image() 
{
}

void Image::setTransparentColorRGB(Color c)
{
    if (_pSurface)
    {
        SDL_SetColorKey(&*_pSurface, SDL_TRUE, SDL_MapRGB(_pSurface->format, c.r, c.g, c.b));
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

        Image tmpImage = srcImage;

        _pTexture = std::shared_ptr<SDL_Texture>(
            SDL_CreateTextureFromSurface(_frame_renderer, &*tmpImage._pSurface),
            [](SDL_Texture* p) {if (p) SDL_DestroyTexture(p); });
        if (_pTexture)
        {
            // TODO set transparent color
            SDL_SetColorKey(&*srcImage._pSurface, SDL_RLEACCEL, SDL_MapRGB(srcImage._pSurface->format, 0, 255, 0));
            _texRect = srcImage.getRect();
            _loaded = true;
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
    _pTexture(SDL_CreateTextureFromSurface(_frame_renderer, const_cast<SDL_Surface*>(pSurface)), SDL_DestroyTexture)
{
    if (!_pTexture) return;
    _texRect = pSurface->clip_rect;
    _loaded = true;
}

Texture::Texture(const SDL_Texture* pTexture, int w, int h):
    _pTexture(const_cast<SDL_Texture*>(pTexture), SDL_DestroyTexture)
{
    if (!pTexture) return;
    _texRect = {0, 0, w, h};
    _loaded = true;
}

Texture::Texture(int w, int h, PixelFormat fmt)
{
	SDL_PixelFormatEnum sdlfmt = SDL_PIXELFORMAT_UNKNOWN;
	switch (fmt)
	{
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
            SDL_CreateTexture(_frame_renderer, sdlfmt, SDL_TEXTUREACCESS_STREAMING, w, h), SDL_DestroyTexture);
		if (_pTexture) _loaded = true;
	}
}

Texture::~Texture()
{
}

void Texture::_draw(std::shared_ptr<SDL_Texture> pTex, const Rect* srcRect, Rect dstRect,
	const Color c, const BlendMode b, const bool filter, const double angle, const Point* center)
{
    int flipFlags = 0;
    //if (dstRect.w < 0) { dstRect.w = -dstRect.w; dstRect.x -= dstRect.w; flipFlags |= SDL_FLIP_HORIZONTAL; }
    //if (dstRect.h < 0) { dstRect.h = -dstRect.h; dstRect.y -= dstRect.h; flipFlags |= SDL_FLIP_VERTICAL; }

	SDL_SetTextureColorMod(&*pTex, c.r, c.g, c.b);
	SDL_SetTextureAlphaMod(&*pTex, c.a);

    SDL_BlendMode sb = SDL_BLENDMODE_INVALID;
    if (BlendMap.find(b) != BlendMap.end()) SDL_SetTextureBlendMode(&*pTex, BlendMap.at(b));

    SDL_Point scenter;
    if (center) scenter = { (int)center->x, (int)center->y };

    SDL_RenderCopyEx(
        _frame_renderer,
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
	_draw(_pTexture, &srcRect, dstRect, c, b, filter, angle, NULL);
}

void Texture::draw(const Rect& srcRect, Rect dstRect,
	const Color c, const BlendMode b, const bool filter, const double angle, const Point& center) const
{
	_draw(_pTexture, &srcRect, dstRect, c, b, filter, angle, &center);
}

////////////////////////////////////////////////////////////////////////////////
// TextureFull

TextureFull::TextureFull(const Color& c): Texture(nullptr)
{
    auto surface = SDL_CreateRGBSurface(0, 1, 1, 24, 0xff, 0xff, 0xff, 0xff);   // needs to be 24bit
    _texRect = { 0,0,1,1 };
    SDL_FillRect(&*surface, &_texRect, SDL_MapRGBA(surface->format, c.r, c.g, c.b, c.a));
    _pTexture = std::shared_ptr<SDL_Texture>(
        SDL_CreateTextureFromSurface(_frame_renderer, surface),
        [](SDL_Texture* p) {if (p) SDL_DestroyTexture(p); });
    SDL_FreeSurface(surface);
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
	SDL_SetTextureAlphaMod(&*_pTexture, c.a);

    SDL_BlendMode sb = SDL_BLENDMODE_INVALID;
    if (BlendMap.find(b) != BlendMap.end()) SDL_SetTextureBlendMode(&*_pTexture, BlendMap.at(b));

    SDL_RenderCopyEx(
        _frame_renderer,
        &*_pTexture,
        &_texRect, &dstRect,
        angle,
        NULL, SDL_FLIP_NONE
    );
}

void GraphLine::draw(Point p1, Point p2, Color c) const
{
	thickLineColor(
		_frame_renderer,
		p1.x, p1.y,
		p2.x, p2.y,
		_width,
		c.hex()
	);
}