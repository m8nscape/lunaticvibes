#ifndef VIDEO_DISABLED

#include <thread>
#include <chrono>
#include "sprite_video.h"
#include "common/utils.h"
extern "C"
{
#include "libavutil/frame.h"
}
#include "game/graphics/video.h"

SpriteVideo::SpriteVideo(int w, int h, std::shared_ptr<sVideo> pVid) :
	SpriteStatic(std::make_shared<TextureVideo>(pVid), { 0, 0, w, h }),
	w(0), h(0), format(Texture::PixelFormat::UNKNOWN)
{
	_type = SpriteTypes::VIDEO;

	if (pVid)
	{
		w = pVid->getW();
		h = pVid->getH();
		format = pVid->getFormat();
	}
}

void SpriteVideo::startPlaying()
{
	auto pVid = std::reinterpret_pointer_cast<TextureVideo>(_pTexture);
	if (!pVid) return;
	pVid->start();
}

void SpriteVideo::stopPlaying()
{
	auto pVid = std::reinterpret_pointer_cast<TextureVideo>(_pTexture);
	if (!pVid) return;
	pVid->stop();
}

#endif