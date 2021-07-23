#pragma once
#include "sprite.h"
#include "game/graphics/texture_extra.h"
#include "common/types.h"
#include <memory>

class sVideo;
struct AVFrame;

class SpriteVideo : public SpriteStatic
{
protected:
	int w, h;
	Texture::PixelFormat format;

public:
	SpriteVideo(int w, int h, std::shared_ptr<sVideo> pVid);
	virtual ~SpriteVideo() = default;
	
public:
	void startPlaying();
	void stopPlaying();
};

/*
class SpriteBmsBga : public SpriteStatic
{
protected:
	int w, h;
	size_t prevIdx = INDEX_INVALID;
	unsigned prevDecodedFrames;

public:
	SpriteBmsBga(int w, int h, std::shared_ptr<TextureBmsBga> pt) : SpriteStatic(pt, { 0, 0, w, h }), w(w), h(h) { _type = SpriteTypes::BMS_BGA; }
	virtual ~SpriteBmsBga() = default;

public:
	//virtual bool update(timestamp t) override;
	//virtual void draw() const override;
};
*/