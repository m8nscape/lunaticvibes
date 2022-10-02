#pragma once
#include "game/graphics/sprite.h"
#include "game/graphics/video.h"
#include "common/types.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

enum class eSkinType
{
    UNDEF,
    LR2,

};

typedef unsigned long timeMS;

//typedef StringContent Token;
//typedef std::vector<StringContent> Tokens;
typedef StringContent Token;
typedef std::vector<Token> Tokens;
typedef std::shared_ptr<vSprite> pSprite;
typedef std::shared_ptr<Texture> pTexture;
typedef std::shared_ptr<sVideo>  pVideo;
typedef std::shared_ptr<Image>   pImage;
typedef std::shared_ptr<TTFFont> pFont;

class vSkin
{
public:

protected:
    eSkinType _type;
    vSkin();
public:
	virtual ~vSkin();
    eSkinType type() const { return _type; }

protected:
    bool _loaded = false;
public:
    constexpr bool isLoaded() { return _loaded; }
    virtual int setExtendedProperty(std::string&& key, void* value) = 0;

////////////////////////////////////////////////////////////////////////////////
// Images
protected:
    std::map<std::string, pFont>  _fontNameMap;

////////////////////////////////////////////////////////////////////////////////
// Textures
protected:
    std::map<std::string, pTexture> _textureNameMap;    // Use this to get texture instance from name
	std::map<std::string, pVideo>   _vidNameMap;	// Use this to get video instance from name

////////////////////////////////////////////////////////////////////////////////
// Sprite elements
protected:
    std::list<pSprite> _sprites;                    // Push instance on parsing

// functional support
protected:
    bool _handleMouseEvents = true;
    std::list<std::map<Rect, pSprite>> _mouseCursorAreaMap;

    std::shared_ptr<iSpriteMouse> _pDragging = nullptr;  // currently (mouse) dragging element
    std::shared_ptr<SpriteText> _pEditing = nullptr;     // currently text edit element
    std::shared_ptr<iSpriteMouse> _pLastClick = nullptr; 

////////////////////////////////////////////////////////////////////////////////
public:
    virtual void update();
    virtual void update_mouse(int x, int y);
    virtual void update_mouse_click(int x, int y);
    virtual void update_mouse_drag(int x, int y);
    virtual void update_mouse_release();
    virtual void reset_bar_animation() = 0;
    virtual void start_bar_animation() = 0;
    virtual void draw() const;
    void setHandleMouseEvents(bool b) { _handleMouseEvents = b; }
    void startSpriteVideoPlayback();
    void stopSpriteVideoPlayback();

    bool textEditSpriteClicked() const;
    eText textEditType() const;
    void startTextEdit(bool clear);
    void stopTextEdit(bool modify);

    ///////////////////////////////////////////////////////////
    // Info defined by header
    struct skinInfo
    {
        eMode mode;
        std::string name;
        std::string maker;

        bool hasTransparentColor;
        Color transparentColor;

        // General 
        unsigned timeIntro; // #STARTINPUT | before this time the scene cannot receive input. Implemented by derived class
        unsigned timeOutro; // #FADEOUT

        // Decide
        unsigned timeDecideSkip; // #SKIP | after this time the scene can be skipped. Solely for decide scene
        unsigned timeDecideExpiry; // #SCENETIME | switch to play scene if not skipped

        // Play
        unsigned timeStartLoading;
        unsigned timeMinimumLoad;
        unsigned timeGetReady;
        unsigned timeFailed;
        unsigned noteLaneHeight1P = 0;
        unsigned noteLaneHeight2P = 0;

        // Result
        unsigned timeResultRank; //ignored
        unsigned timeResultRecord; //ignored
    } info{};

    ///////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// customize
public:
    struct CustomizeOption
    {
        int id;
        StringContent displayName;
        StringContent internalName;
        std::vector<StringContent> entries;
        size_t defaultEntry;
    };
    virtual size_t getCustomizeOptionCount() const = 0;
    virtual CustomizeOption getCustomizeOptionInfo(size_t idx) const = 0;

    virtual StringContent getName() const = 0;
    virtual StringContent getMaker() const = 0;
    virtual StringPath getFilePath() const = 0;

////////////////////////////////////////////////////////////////////////////////
// extended capability support
public:
    bool isSupportExHardAndAssistEasy = false;
    bool isSupportFastSlow = false;
    bool isSupportGreenNumber = false;
    bool isSupportLift = false;
};