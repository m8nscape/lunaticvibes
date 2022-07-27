
#include <execution>
#include <algorithm>

#include "game/chart/chart_bms.h"
#include "texture_extra.h"
#include "game/scene/scene_context.h"
#include "common/types.h"
#include "common/utils.h"
#include "common/log.h"

#ifndef VIDEO_DISABLED

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
}

TextureVideo::TextureVideo(std::shared_ptr<sVideo> pv) :
	Texture(pv->getW(), pv->getH(), pv->getFormat()),
	AsyncLooper(std::bind(&TextureVideo::update, this), 60, true),
	format(pv->getFormat()), 
	pVideo(pv) {}

TextureVideo::~TextureVideo()
{
	loopEnd();
}

void TextureVideo::start()
{
	if (_running) return;
	//pVideo->seek(0);
	pVideo->startPlaying();
	loopStart();
}

void TextureVideo::stop()
{
	if (!_running) return;
	pVideo->stopPlaying();
	loopEnd();
}

void TextureVideo::seek(int64_t sec)
{
	pVideo->seek(sec);
}

void TextureVideo::update()
{
	if (!pVideo) return;
	if (!pVideo->haveVideo) return;

	auto vrfc = pVideo->getDecodedFrames();
	if (decoded_frames == vrfc) return;
	decoded_frames = vrfc;

	{
		using namespace std::chrono_literals;
		std::shared_lock l(pVideo->video_frame_mutex);

		auto pf = pVideo->getFrame();
		if (!pf) return;

		switch (format)
		{
		case Texture::PixelFormat::IYUV:
			pushMainThreadTask([this]() {
				auto pf = pVideo->getFrame();
				if (!pf) return;

				updateYUV(
					pf->data[0], pf->linesize[0],
					pf->data[1], pf->linesize[1],
					pf->data[2], pf->linesize[2]);
			});
			break;

		default:
			break;
		}
	}
}


void TextureVideo::reset()
{
	seek(0);
	decoded_frames = 0;
}

#endif

bool TextureBmsBga::addBmp(size_t idx, const Path& pBmp)
{
	if (idx == size_t(-1)) return false;

	static const std::set<std::string> video_file_extensions =
	{
		".mpg",
		".flv",
		".mp4",
		".m4p",
		".m4v",
		".f4v",
		".avi",
		".wmv",
		".mpeg",
		".mpeg2",
		".mkv",
		".webm",
	};

	if (fs::exists(pBmp) && fs::is_regular_file(pBmp) && pBmp.has_extension())
	{
		if (video_file_extensions.find(toLower(pBmp.extension().u8string())) != video_file_extensions.end())
		{
#ifndef VIDEO_DISABLED
			objs[idx].type = obj::Ty::VIDEO;
			objs[idx].pt = std::make_shared<TextureVideo>(std::make_shared<sVideo>(pBmp));
			LOG_DEBUG << "[TextureBmsBga] added video: " << pBmp.u8string();
			return true;
#else
			LOG_DEBUG << "[TextureBmsBga] video support is disabled: " << pBmp.u8string();
			return false;
#endif
		}
		else
		{
			objs[idx].type = obj::Ty::PIC;
			objs[idx].pt = std::make_shared<Texture>(Image(pBmp));

			objs_layer[idx].type = obj::Ty::PIC;
			Image layerImg(pBmp);
			layerImg.setTransparentColorRGB(Color(0, 0, 0, 255));
			objs_layer[idx].pt = std::make_shared<Texture>(layerImg);

			LOG_DEBUG << "[TextureBmsBga] added pic: " << pBmp.u8string();
			return true;
		}
	}
	LOG_DEBUG << "[TextureBmsBga] file not found: " << pBmp.u8string();
	return false;
}

bool TextureBmsBga::setSlot(size_t idx, Time time, bool base, bool layer, bool poor)
{
	if (objs.find(idx) == objs.end()) return false;
	if (base) baseSlot.emplace_back(time, idx);
	if (layer) layerSlot.emplace_back(time, idx);
	if (poor) poorSlot.emplace_back(time, idx);
	return true;
}

void TextureBmsBga::sortSlot()
{
	auto less = [](const std::pair<Time, size_t>& l, const std::pair<Time, size_t>& r)
	{
		if (l.first < r.first) return true;
		else if (l.first == r.first && l.second < r.second) return true;
		return false;
	};
	std::sort(baseSlot.begin(), baseSlot.end(), less);
	std::sort(layerSlot.begin(), layerSlot.end(), less);
	std::sort(poorSlot.begin(), poorSlot.end(), less);
	baseIt = baseSlot.begin();
	layerIt = layerSlot.begin();
	poorIt = poorSlot.begin();
}

bool TextureBmsBga::setSlotFromBMS(chartBMS& bms)
{
	baseSlot.clear();
	layerSlot.clear();
	poorSlot.clear();
	const auto lBase = bms.getBgaBase();
	const auto lLayer = bms.getBgaLayer();
	const auto lPoor = bms.getBgaPoor();
	for (const auto& l : lBase) setSlot(l.dvalue, l.time, true, false, false);
	for (const auto& l : lLayer) setSlot(l.dvalue, l.time, false, true, false);
	for (const auto& l : lPoor) setSlot(l.dvalue, l.time, false, false, true);
	sortSlot();
	_loaded = true;
	return true;
}

void TextureBmsBga::seek(const Time& t)
{
	auto seekSub = [&t, this](decltype(baseSlot)& slot, size_t& slotIdx, decltype(baseSlot.begin())& slotIt)
	{
		for (auto it = slot.begin(); it != slot.end(); ++it)	// search from beginning
		{
			auto[time, idx] = *it;
			if (time <= t)
			{
				slotIdx = idx;
				slotIt = it;
				if (objs[idx].type == obj::Ty::VIDEO)
				{
#ifndef VIDEO_DISABLED
					auto pt = std::reinterpret_pointer_cast<TextureVideo>(objs[idx].pt);
					pt->seek((t - time).norm() / 1000);
					pt->update();
#endif
				}
				return;
			}
		}
		//slotIt = slot.end();	// not found
	};

	seekSub(baseSlot, baseIdx, baseIt);
	seekSub(layerSlot, layerIdx, layerIt);
	seekSub(poorSlot, poorIdx, poorIt);
	inPoor = false;
}

void TextureBmsBga::update(const Time& t, bool poor)
{
	auto seekSub = [&t, this](decltype(baseSlot)& slot, size_t& slotIdx, decltype(baseSlot.begin())& slotIt)
	{
		auto it = slotIt;
		for (; it != slot.end(); ++it)
		{
			auto [time, idx] = *it;
			if (time <= t)
			{
				slotIdx = idx;
				slotIt = it;

#ifndef VIDEO_DISABLED
				if (it != slot.end() && slotIdx != INDEX_INVALID && objs[slotIdx].type == obj::Ty::VIDEO)
				{
					auto pt = std::reinterpret_pointer_cast<TextureVideo>(objs[it->second].pt);
					pt->start();
					pt->update();
				}
#endif
			}
		}

	};

	seekSub(baseSlot, baseIdx, baseIt);
	seekSub(layerSlot, layerIdx, layerIt);
	seekSub(poorSlot, poorIdx, poorIt);
	inPoor = poor;
}

void TextureBmsBga::draw(const Rect& sr, Rect dr,
	const Color c, const BlendMode b, const bool f, const double a) const
{
	if (inPoor && poorIdx != INDEX_INVALID && objs.at(poorIdx).type != obj::Ty::EMPTY)
	{
		objs.at(poorIdx).pt->draw(dr, c, b, f, a);
	}
	else
	{
		if (baseIdx != INDEX_INVALID && objs.at(baseIdx).type != obj::Ty::EMPTY)
			objs.at(baseIdx).pt->draw(dr, c, b, f, a);

		if (layerIdx != INDEX_INVALID && objs.at(layerIdx).type != obj::Ty::EMPTY)
		{
			if (objs.at(layerIdx).type == obj::Ty::PIC && objs_layer.at(layerIdx).pt != nullptr)
				objs_layer.at(layerIdx).pt->draw(dr, c, b, f, a);
			else
				objs.at(layerIdx).pt->draw(dr, c, b, f, a);
		}
	}
}

void TextureBmsBga::draw(const Rect& sr, Rect dr,
	const Color c, const BlendMode b, const bool f, const double a, const Point& ct) const
{
	if (inPoor && poorIdx != INDEX_INVALID && objs.at(poorIdx).type != obj::Ty::EMPTY)
	{
		objs.at(poorIdx).pt->draw(dr, c, b, f, a, ct);
	}
	else
	{
		if (baseIdx != INDEX_INVALID && objs.at(baseIdx).type != obj::Ty::EMPTY) 
			objs.at(baseIdx).pt->draw(dr, c, b, f, a, ct);

		if (layerIdx != INDEX_INVALID && objs.at(layerIdx).type != obj::Ty::EMPTY)
		{
			if (objs.at(layerIdx).type == obj::Ty::PIC && objs_layer.at(layerIdx).pt != nullptr)
				objs_layer.at(layerIdx).pt->draw(dr, c, b, f, a, ct);
			else
				objs.at(layerIdx).pt->draw(dr, c, b, f, a, ct);
		}
	}
}

void TextureBmsBga::reset()
{
	baseIt = baseSlot.begin();
	layerIt = layerSlot.begin();
	poorIt = poorSlot.begin();
	baseIdx = INDEX_INVALID;
	layerIdx = INDEX_INVALID;
	poorIdx = INDEX_INVALID;

	auto resetSub = [this](decltype(baseSlot)& slot)
	{
		for (auto it = slot.begin(); it != slot.end(); ++it)	// search from beginning
		{
			auto[time, idx] = *it;
			if (objs[idx].type == obj::Ty::VIDEO)
			{
#ifndef VIDEO_DISABLED
				auto pt = std::reinterpret_pointer_cast<TextureVideo>(objs[idx].pt);
				pt->stop();
#endif
			}
		}
		//slotIt = slot.end();	// not found
	};

	resetSub(baseSlot);
	resetSub(layerSlot);
	resetSub(poorSlot);
}

void TextureBmsBga::clear()
{
	_loaded = false;
	_texRect = Rect();
	baseSlot.clear();
	layerSlot.clear();
	poorSlot.clear();
	objs_layer.clear();
	objs.clear();
	inPoor = false;
	reset();
}

void TextureBmsBga::setLoaded()
{
	_loaded = true;	
}

TextureDynamic::TextureDynamic() : Texture(nullptr, 0, 0)
{
}

void TextureDynamic::setPath(const Path& path)
{
	_loaded = false;

	if (path.empty())
	{
		return;
	}

	static std::map<Path, Texture> dynTexCache;
	if (dynTexCache.find(path) == dynTexCache.end())
	{
		Image tmp(path);
		if (!tmp._loaded)
		{
			dynTexCache.emplace(std::piecewise_construct, std::forward_as_tuple(path), std::forward_as_tuple(nullptr, 0, 0));
			_loaded = false;
			return;
		}
		dynTexCache.emplace(path, tmp);
	}

	_dynTexture = &dynTexCache.at(path);
	if (_dynTexture->isLoaded())
	{
		_loaded = true;
		_texRect = _dynTexture->getRect();
	}
}

void TextureDynamic::draw(Rect dstRect,
	const Color c, const BlendMode b, const bool filter, const double angle) const
{
	_dynTexture->draw(dstRect, c, b, filter, angle);
}

void TextureDynamic::draw(Rect dstRect,
	const Color c, const BlendMode b, const bool filter, const double angle, const Point& center) const
{
	_dynTexture->draw(dstRect, c, b, filter, angle, center);
}

void TextureDynamic::draw(const Rect& srcRect, Rect dstRect,
	const Color c, const BlendMode b, const bool filter, const double angle) const
{
	_dynTexture->draw(srcRect, dstRect, c, b, filter, angle);
}

void TextureDynamic::draw(const Rect& srcRect, Rect dstRect,
	const Color c, const BlendMode b, const bool filter, const double angle, const Point& center) const
{
	_dynTexture->draw(srcRect, dstRect, c, b, filter, angle, center);
}