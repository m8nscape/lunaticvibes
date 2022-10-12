#pragma once

#include "sprite.h"

#include <map>

struct CharMapping
{
    size_t textureIdx;
    Rect textureRect;
};
using CharMappingList = std::map<char32_t, CharMapping>;

class SpriteImageText : public SpriteText
{
protected:
    std::vector<pTexture> _textures;
    CharMappingList* _chrList;
    unsigned _height;
    int _margin;

private:
    std::vector<std::pair<char32_t, Rect>> _drawListOrig;
    std::vector<std::pair<char32_t, Rect>> _drawList;
    Rect _drawRect;

public:
    SpriteImageText() = delete;
    SpriteImageText(std::vector<pTexture>& textures, CharMappingList* chrList, IndexText textInd = IndexText::INVALID, TextAlign align = TEXT_ALIGN_LEFT, unsigned height = 72, int margin = 0);
    //SpriteText(pFont f, Rect rect, IndexText textInd = IndexText::INVALID, TextAlign align = TEXT_ALIGN_LEFT, unsigned ptsize = 72, Color c = 0xffffffff);
    virtual ~SpriteImageText() = default;

public:
    virtual void updateTextRect();
    virtual bool update(const Time& t);
    virtual void draw() const;

private:
    void setInputBindingText(std::string&& text);
};