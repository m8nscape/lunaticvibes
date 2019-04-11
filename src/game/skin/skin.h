#pragma once
#include "game/graphics/sprite.h"
#include "defs/defs_scene.h"
#include "types.h"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <mutex>

enum class eSkinType
{
    LR2,

};

typedef unsigned long timeMS;

typedef StringContent Token;
typedef std::vector<Token> Tokens;

class vSkin
{
public:
    typedef std::shared_ptr<vSprite> pSprite;
    typedef std::shared_ptr<Texture> pTexture;
    typedef std::shared_ptr<Image>   pImage;
    typedef std::shared_ptr<TTFFont> pFont;

protected:
    vSkin();
public:
    virtual ~vSkin() = default;

protected:
    bool _loaded = false;
	std::mutex _mutex;
public:
    constexpr bool isLoaded() { return _loaded; }

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

// functional support
protected:
    std::list<std::map<Rect, pSprite>> _mouseCursorAreaMap;

////////////////////////////////////////////////////////////////////////////////
// Input wrapping
protected:
    bool acceptInputGame = false;   // currently accept input from keyboard/joystick/etc
    bool acceptInputFunc = false;   // currently accept FN keys
    timeMS timeStartAcceptInput = 0;    // initialize in parsing

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
        unsigned timeIntro;
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
    } info{};

    ///////////////////////////////////////////////////////////
};