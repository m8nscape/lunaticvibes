#include <thread>
#include <chrono>
#include "sprite_video.h"
#include "common/utils.h"
#include "plog/Logger.h"
extern "C"
{
#include "libavutil/frame.h"
}
#include "game/graphics/video.h"

SpriteVideo::SpriteVideo(int w, int h, Texture::PixelFormat fmt) : SpriteStatic(nullptr, { 0, 0, w, h }), format(fmt)
{
	_type = SpriteTypes::VIDEO;
	_pTexture = std::make_shared<Texture>(w, h, fmt);
}

int SpriteVideo::bindVideo(std::shared_ptr<sVideo> pVid)
{
	if (pVid->haveVideo) return -1;
	this->pVid = pVid;
	return 0;
}

bool SpriteVideo::update(Time t)
{
	if (!pVid) return false;
	if (!pVid->haveVideo) return false;
	if (!SpriteStatic::update(t)) return false;

	auto vrfc = pVid->getDecodedFrames();
	if (decoded_frames == vrfc) return true;
	decoded_frames = vrfc;

	auto pf = pVid->getFrame();
	if (!pf) return false;

	{
		// read lock
		std::shared_lock<std::shared_mutex> l(pVid->lock);

		switch (format)
		{
		case Texture::PixelFormat::IYUV:
			_pTexture->updateYUV(
				pf->data[0], pf->linesize[0],
				pf->data[1], pf->linesize[1],
				pf->data[2], pf->linesize[2]);
			break;

		default:
			break;
		}
	}
	return true;
}
