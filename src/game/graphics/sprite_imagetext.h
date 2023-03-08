#pragma once

#include "sprite.h"

#include <unordered_map>

struct CharMapping
{
    size_t textureIdx;
    Rect textureRect;
};
using CharMappingList = std::unordered_map<char32_t, CharMapping>;

class SpriteImageText : public SpriteText
{
protected:
    std::vector<pTexture> _textures;
    CharMappingList* _chrList;
    unsigned _height;
    int _margin;

private:
    std::vector<std::pair<char32_t, RectF>> _drawListOrig;
    std::vector<std::pair<char32_t, RectF>> _drawList;
    Rect _drawRect;

public:
    struct SpriteImageTextBuilder : SpriteTextBuilder
    {
        std::vector<pTexture> charTextures;
        CharMappingList* charMappingList = nullptr;
        int height = 0;
        int margin = 0;

        std::shared_ptr<SpriteImageText> build() const { return std::make_shared<SpriteImageText>(*this); }
    };
public:
    SpriteImageText() = delete;
    SpriteImageText(const SpriteImageTextBuilder& builder);
    virtual ~SpriteImageText() = default;

public:
    virtual void updateTextRect();
    virtual bool update(const Time& t);
    virtual void draw() const;

private:
    void setInputBindingText(std::string&& text);
};