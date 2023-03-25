#pragma once
#include "game/graphics/sprite.h"
#include "game/graphics/video.h"
#include "common/types.h"
#include <list>
#include <vector>
#include <map>
#include <string>
#include <memory>

enum class SkinVersion
{
    UNDEF,
    LR2beta3,

};

//typedef StringContent Token;
typedef std::vector<StringContent> Tokens;

class SkinBase
{
protected:
    static std::map<std::string, std::shared_ptr<Texture>> preDefinedTextures;

protected:
    SkinVersion _version;
    SkinBase();
public:
	virtual ~SkinBase();
    SkinVersion version() const { return _version; }

protected:
    bool loaded = false;
public:
    constexpr bool isLoaded() { return loaded; }
    virtual int setExtendedProperty(std::string&& key, void* value) = 0;

////////////////////////////////////////////////////////////////////////////////
// Images
protected:
    std::map<std::string, std::shared_ptr<TTFFont>>  fontNameMap;

////////////////////////////////////////////////////////////////////////////////
// Textures
protected:
    static std::map<std::string, std::shared_ptr<Texture>> textureNameMap;    // Use this to get texture instance from name
	std::map<std::string, std::shared_ptr<sVideo>>  videoNameMap;	// Use this to get video instance from name

////////////////////////////////////////////////////////////////////////////////
// Sprite elements
protected:
    std::list<std::shared_ptr<SpriteBase>> _sprites;                    // Push instance on parsing

// functional support
protected:
    bool handleMouseEvents = true;

    std::shared_ptr<iSpriteMouse> pSpriteDragging = nullptr;  // currently (mouse) dragging element
    std::shared_ptr<SpriteText> pSpriteTextEditing = nullptr;     // currently text edit element
    std::shared_ptr<iSpriteMouse> pSpriteLastClicked = nullptr; 

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
    void setHandleMouseEvents(bool b) { handleMouseEvents = b; }
    void startSpriteVideoPlayback();
    void stopSpriteVideoPlayback();

    bool textEditSpriteClicked() const;
    IndexText textEditType() const;
    void startTextEdit(bool clear);
    void stopTextEdit(bool modify);

    std::shared_ptr<Texture> getTextureCustomizeThumbnail();

    ///////////////////////////////////////////////////////////
    // Info defined by header
    struct skinInfo
    {
        SkinType mode = SkinType::PLAY7;
        std::string name;
        std::string maker;
        int resolution = -1;     // #RESOLUTION | 0:480p / 1:720p / 2:1080p

        bool hasTransparentColor = false;
        Color transparentColor;

        // General 
        unsigned timeIntro = 200; // #STARTINPUT | before this time the scene cannot receive input. Implemented by derived class
        unsigned timeOutro = 200; // #FADEOUT

        // Decide
        unsigned timeDecideSkip = 200; // #SKIP | after this time the scene can be skipped. Solely for decide scene
        unsigned timeDecideExpiry = 200; // #SCENETIME | switch to play scene if not skipped

        // Play
        unsigned timeStartLoading = 200;
        unsigned timeMinimumLoad = 200;
        unsigned timeGetReady = 200;
        unsigned timeFailed = 200;
        unsigned noteLaneHeight1P = 0;
        unsigned noteLaneHeight2P = 0;
        unsigned noteLaneHeight1PSub = 0;
        unsigned noteLaneHeight2PSub = 0;
        
        // Result
        unsigned timeResultRank = 200; //ignored
        unsigned timeResultRecord = 200; //ignored

        unsigned scratchSide1P = 0;
        unsigned scratchSide2P = 0;
    } info;

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
    bool isSupportNewRandom = false;
    bool isSupportKeyConfigAbsAxis = false;
    bool isSupportHsFixInitialAndMain = false;
};