#pragma once

#ifndef VIDEO_DISABLED
#include "SDL_video.h"
#endif

#include "SDL_render.h"
#include "SDL_image.h"
#include "SDL_filesystem.h"
#include "SDL_ttf.h"
#include <vector>
#include <memory>
#include <string>
#include <filesystem>
#include <shared_mutex>

// global control pointer, do not modify
inline SDL_Renderer* gFrameRenderer;
inline SDL_Texture* gInternalRenderTarget;


////////////////////////////////////////////////////////////////////////////////


class Color : public SDL_Color
{
public:
    Color(uint32_t rgba = 0xffffffff);
    Color(int r, int g, int b, int a);
    uint32_t hex() const;
    Color operator+ (const Color& rhs) const;
    Color operator* (const double& rhs) const;
    Color operator* (const Color& rhs) const;
    bool operator== (const Color& rhs) const;
    bool operator!= (const Color& rhs) const;
};

////////////////////////////////////////////////////////////////////////////////
// Enums

enum class TTFStyle
{
    Normal,
    Bold,
    Italic,
    _BI,
};

enum class TTFHinting
{
    Normal,
    Light,
    Mono,
    None,
};

// Used by SDL_RenderCopy().
// Other blend modes should use SDL_ComposeCustomBlendMode(6).
enum class BlendMode
{
    NONE,
    ALPHA,
    ADD,
    MULTIPLY,
    SUBTRACT,
    ANTICOLOR,
    //MULTIPLY_ANTI_BACKGROUND,
    //MULTIPLY_WITH_ALPHA,
    //XOR,
};

////////////////////////////////////////////////////////////////////////////////

// Point: x, y
class Point
{
public:
	double x = 0;
	double y = 0;
public:
	constexpr Point(int zero = 0) {}
	constexpr Point(double x, double y) : x(x), y(y) {}
	constexpr Point operator+ (const Point& rhs) const { return Point(x + rhs.x, y + rhs.y); }
	constexpr Point operator- (const Point& rhs) const { return Point(x - rhs.x, y - rhs.y); }
	constexpr Point operator* (const double& rhs) const { return Point(x * rhs, y * rhs); }
	constexpr bool  operator== (const Point& rhs) const { return x == rhs.x && y == rhs.y; }
};

class Image;

// Rect: x, y, w, h
class Rect: public SDL_Rect
{
public:
    Rect(int zero = 0);
    Rect(int w, int h);
    Rect(int x, int y, int w, int h);
    Rect(const SDL_Rect& rect);
    ~Rect() = default;
public:
    Rect operator+ (const Rect& rhs) const;
    Rect operator* (const double& rhs) const;
    bool operator== (const Rect& rhs) const;
    bool operator!= (const Rect& rhs) const;
};
inline static Rect RECT_FULL = Rect(0, 0, 0xDEADBEEF, 0xDEADBEEF);

////////////////////////////////////////////////////////////////////////////////
// SDL_Image loads pictures into SDL_Surface instances
// Run IMG_Init outside.
class Image
{
    friend class Texture;
    friend class TextureDynamic;

private:
    std::string _path;
    std::shared_ptr<SDL_RWops> _pRWop;
    std::shared_ptr<SDL_Surface> _pSurface;
    bool _loaded = false;
    //bool _haveAlphaLayer = false;
private:
    Image(const char* path, std::shared_ptr<SDL_RWops>&& rw);
public:
	Image(const std::filesystem::path& path);
    Image(const char* filePath);
    Image(const char* format, void* bmp, size_t size);
    ~Image();
    void setTransparentColorRGB(Color c);
public:
    Rect getRect() const;
};


////////////////////////////////////////////////////////////////////////////////
// Convert SDL_Surface into SDL_Texture with subarea specified.
class Texture
{
	friend class vSprite;
	friend class SpriteStatic;
	friend class SpriteSelection;
	friend class SpriteAnimated;
	friend class SpriteText;
	friend class SpriteNumber;

    friend class SpriteLaneVertical;
    friend class SpriteLaneVerticalLN;
	friend class SpriteVideo;

protected:
	std::shared_ptr<SDL_Texture> _pTexture = nullptr;
	bool _loaded = false;
	Rect _texRect;

protected:
	void static _draw(std::shared_ptr<SDL_Texture> pTex, const Rect* srcRect, Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees, const Point* center = NULL);

public:
	// Inner draw function.
	virtual void draw(Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees) const;
	virtual void draw(Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees, const Point& center) const;
	virtual void draw(const Rect& srcRect, Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees) const;
	virtual void draw(const Rect& srcRect, Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees, const Point& center) const;

public:
	enum class PixelFormat
	{
		UNKNOWN, 
		UNSUPPORTED,

		RGB24,
		BGR24,

		YV12,		// 4:2:0 Y + V + U
		IYUV,		// 4:2:0 Y + U + V
		I420 = IYUV,// 4:2:0 Y + U + V
		YUY2,		// Y0 + U0 + Y1 + V0
		YUYV = YUY2,// Y0 + U0 + Y1 + V0
		UYVY,		// U0 + Y0 + V0 + Y1
		YVYU,		// Y0 + V0 + Y1 + U0

	};

public:
	Texture(const Image& srcImage);
	Texture(const SDL_Surface* pSurface);
	Texture(const SDL_Texture* pTexture, int w, int h);
	Texture(int w, int h, PixelFormat fmt);	// for streaming texture
	virtual ~Texture();
public:
	Rect getRect() const { return _texRect; }
	bool isLoaded() const { return _loaded; }
    int updateYUV(uint8_t* Y, int Ypitch, uint8_t* U, int Upitch, uint8_t* V, int Vpitch);
};


// Special texture class that always uses full texture size as output rect.
// That is, srcRect is ignored and replaced with _texRect.
// Useful when rendering BGs and Error-texture.
class TextureFull: public Texture
{
private:
    virtual void draw(const Rect& srcRect, Rect dstRect, 
        const Color c, const BlendMode blend, const bool filter, const double angleInDegrees) const override;
public:
    TextureFull(const Color& srcColor);
    TextureFull(const Image& srcImage);
    TextureFull(const SDL_Surface* pSurface);
    TextureFull(const SDL_Texture* pTexture, int w, int h);
    virtual ~TextureFull();
};

////////////////////////////////////////////////////////////////////////////////
// SDL_ttf encapsulation. Mostly as same as Image
// Run TTF_Init outside.
class TTFFont
{
    friend class SpriteText;

protected:
    TTF_Font* _pFont = NULL;
    std::string _filePath;
    bool _loaded = false;

public:
    TTFFont(const char* filePath, int ptsize);
    ~TTFFont();

public:
    // Attributes Settings
    void setStyle(TTFStyle style);
    void setOutline(bool enabled);
    void setHinting(TTFHinting mode);
    void setKerning(bool enabled);
    
    // Rendering Interfaces
    std::shared_ptr<Texture> TextUTF8(const char* text, const Color& c);
    std::shared_ptr<Texture> TextUTF8Solid(const char* text, const Color& c);
    std::shared_ptr<Texture> TextUTF8Shaded(const char* text, const Color& c, const Color& bg);
    Rect getRectUTF8(const char* text);
    //Rect getRectUTF16(const char* text);
};

////////////////////////////////////////////////////////////////////////////////
// Thick line wrapper
class GraphLine
{
public:
    int _width = 1;
    GraphLine(int width = 1) : _width(width) {}
public:
	void draw(Point p1, Point p2, Color c = 0xffffffff) const;
};