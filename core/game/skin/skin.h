#pragma once
#include "game/graphics/sprite.h"
#include "types.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

enum class eSkinType
{
    LR2,

};

typedef unsigned long timeMS;

typedef StringContent Token;
typedef std::vector<StringContent> Tokens;

typedef std::shared_ptr<vSprite> pSprite;
typedef std::shared_ptr<Texture> pTexture;
typedef std::shared_ptr<Image>   pImage;
typedef std::shared_ptr<TTFFont> pFont;

inline std::array<pSprite, SPRITE_GLOBAL_MAX> gSprites{ nullptr };

class vSkin
{
public:

protected:
    vSkin();
public:
    virtual ~vSkin() = default;

protected:
    bool _loaded = false;
public:
    constexpr bool isLoaded() { return _loaded; }
    virtual int setExtendedProperty(std::string& key, void* value) = 0;

////////////////////////////////////////////////////////////////////////////////
// Images
protected:
    std::map<std::string, pFont>  _fontNameMap;

////////////////////////////////////////////////////////////////////////////////
// Textures
protected:
    std::map<std::string, pTexture> _texNameMap;    // Use this to get texture instance from name

    //TextureFull _texBlack;   // _texNameMap["BlackDot"]
    //TextureFull _texWhite;   // _texNameMap["WhiteDot"]
    //TextureFull _texError;   // _texNameMap["Error"]

////////////////////////////////////////////////////////////////////////////////
// Texture Cache for resources require dynamic loading, such as BG, banner
protected:
    bool _enableTextureCache = false;
    std::map<Path, pTexture> _texDynPathMap;
public:
    void setDynamicLoadTexture(bool enabled);

////////////////////////////////////////////////////////////////////////////////
// Sprite elements
protected:
    std::list<pSprite> _sprites;                    // Push instance on parsing
    std::list<pSprite> _sprites_parent;             // sublist of _sprites, managed by impl
    std::list<pSprite> _sprites_child;              // sublist of _sprites, managed by impl

// functional support
protected:
    std::list<std::map<Rect, pSprite>> _mouseCursorAreaMap;

////////////////////////////////////////////////////////////////////////////////
public:
    virtual void update();
    virtual void draw() const;

    ///////////////////////////////////////////////////////////
    // Info defined by header
    struct skinInfo
    {
        eMode mode;
        std::string name;
        std::string maker;

        // General 
        unsigned timeIntro; // time before this cannot receive input. Implemented by derived class
        unsigned timeOutro;

        // Decide
        //unsigned startSkipTime; // handled by Intro

        // Play
        unsigned timeStartLoading;
        unsigned timeMinimumLoad;
        unsigned timeGetReady;
        unsigned timeFailed;

        // Result
        //unsigned resultStartInputTimeRank; //ignored
        //unsigned resultStartInputTimeUpdate; //ignored
        unsigned timeResultDrawing = 3000;
    } info{};

    ///////////////////////////////////////////////////////////
};