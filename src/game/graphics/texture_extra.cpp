
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
	Texture(pv->getW(), pv->getH(), pv->getFormat(), false),
	format(pv->getFormat()), 
	pVideo(pv)
{
	_texRect.x = 0;
	_texRect.y = 0;
	_texRect.w = pv->getW();
	_texRect.h = pv->getH();

	if (!texMapMutex)
		texMapMutex = std::make_shared<std::shared_mutex>();
	if (!textures)
		textures = std::make_shared<std::map<uintptr_t, TextureVideo*>>();

	pTexMapMutex = texMapMutex;
	pTextures = textures;

	std::unique_lock l(*texMapMutex);
	(*textures)[(uintptr_t)this] = this;
}

TextureVideo::~TextureVideo()
{
	assert(!pVideo->isPlaying());

	std::unique_lock l(*texMapMutex);
	textures->erase((uintptr_t)this);
}

void TextureVideo::start()
{
	if (!pVideo->isPlaying())
	{
		pVideo->startPlaying();
	}
}

void TextureVideo::stop()
{
	if (pVideo->isPlaying())
	{
		pVideo->stopPlaying();
	}
}

void TextureVideo::seek(int64_t sec)
{
	pVideo->seek(sec);
}

void TextureVideo::setSpeed(double speed)
{
	pVideo->setSpeed(speed);
}

void TextureVideo::update()
{
	if (!pVideo) return;
	if (!pVideo->haveVideo) return;
	if (!pVideo->isPlaying()) return;

	auto vrfc = pVideo->getDecodedFrames();
	if (decoded_frames == vrfc) return;
	decoded_frames = vrfc;

	using namespace std::chrono_literals;
		
	std::shared_lock l(pVideo->video_frame_mutex, std::try_to_lock);
	if (l.owns_lock())
	{
		auto pf = pVideo->getFrame();
		if (!pf) return;

		switch (format)
		{
		case Texture::PixelFormat::IYUV:
			if (updateYUV(
				pf->data[0], pf->linesize[0],
				pf->data[1], pf->linesize[1],
				pf->data[2], pf->linesize[2]) == 0)
				updated = true;
			break;

		default:
			break;
		}
	}
}

void TextureVideo::stopUpdate()
{
	updated = false;
}

void TextureVideo::draw(Rect dstRect,
	const Color c, const BlendMode blend, const bool filter, const double angleInDegrees) const
{
	Texture::draw(dstRect, updated ? c : Color(0, 0, 0, c.a), blend, filter, angleInDegrees);
}
void TextureVideo::draw(Rect dstRect,
	const Color c, const BlendMode blend, const bool filter, const double angleInDegrees, const Point& center) const
{
	Texture::draw(dstRect, updated ? c : Color(0, 0, 0, c.a), blend, filter, angleInDegrees, center);
}
void TextureVideo::draw(const Rect& srcRect, Rect dstRect,
	const Color c, const BlendMode blend, const bool filter, const double angleInDegrees) const
{
	Texture::draw(srcRect, dstRect, updated ? c : Color(0, 0, 0, c.a), blend, filter, angleInDegrees);
}
void TextureVideo::draw(const Rect& srcRect, Rect dstRect,
	const Color c, const BlendMode blend, const bool filter, const double angleInDegrees, const Point& center) const
{
	Texture::draw(srcRect, dstRect, updated ? c : Color(0, 0, 0, c.a), blend, filter, angleInDegrees, center);
}


void TextureVideo::reset()
{
	seek(0);
	decoded_frames = 0;
}

std::shared_ptr<std::shared_mutex> TextureVideo::texMapMutex;
std::shared_ptr<std::map<uintptr_t, TextureVideo*>> TextureVideo::textures;

void TextureVideo::updateAll()
{
	if (texMapMutex)
	{
		std::shared_lock l(*texMapMutex);
		for (auto& t : *textures)
		{
			t.second->update();
		}
	}
}

#endif

bool TextureBmsBga::addBmp(size_t idx, Path pBmp)
{
	if (idx == size_t(-1)) return false;

	if (!fs::exists(pBmp) && pBmp.has_extension() && toLower(pBmp.extension().string()) == ".bmp")
	{
		pBmp = pBmp.parent_path() / PathFromUTF8(pBmp.filename().stem().u8string() + ".jpg");

		if (!fs::exists(pBmp))
		{
			pBmp = pBmp.parent_path() / PathFromUTF8(pBmp.filename().stem().u8string() + ".png");
		}
	}
	if (fs::exists(pBmp) && fs::is_regular_file(pBmp) && pBmp.has_extension())
	{
		if (video_file_extensions.find(toLower(pBmp.extension().u8string())) != video_file_extensions.end())
		{
#ifndef VIDEO_DISABLED
			objs[idx].type = obj::Ty::VIDEO;
			objs[idx].pt = std::make_shared<TextureVideo>(std::make_shared<sVideo>(pBmp, gSelectContext.pitchSpeed, false));
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
	else
	{
		objs[idx].type = obj::Ty::EMPTY;

		objs_layer[idx].type = obj::Ty::EMPTY;

		LOG_DEBUG << "[TextureBmsBga] file not found, added dummy: " << pBmp.u8string();
	}
	return false;
}

bool TextureBmsBga::setSlot(size_t idx, Time time, bool base, bool layer, bool poor)
{
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

bool TextureBmsBga::setSlotFromBMS(ChartObjectBMS& bms)
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
	if (!isLoaded()) return;

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

	std::unique_lock l(idxLock);
	seekSub(baseSlot, baseIdx, baseIt);
	seekSub(layerSlot, layerIdx, layerIt);
	seekSub(poorSlot, poorIdx, poorIt);
	inPoor = false;
}

void TextureBmsBga::update(const Time& t, bool poor)
{
	if (!isLoaded()) return;

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
					//pt->update();	// Do NOT call update here; videos are updated in main thread with TextureVideo::updateAll()
				}
#endif
			}
		}

	};

	std::unique_lock l(idxLock);
	seekSub(baseSlot, baseIdx, baseIt);
	seekSub(layerSlot, layerIdx, layerIt);
	seekSub(poorSlot, poorIdx, poorIt);
	inPoor = poor;
}

void TextureBmsBga::draw(const Rect& sr, Rect dr,
	const Color c, const BlendMode b, const bool f, const double a) const
{
	std::shared_lock l(idxLock);
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
	std::shared_lock l(idxLock);
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

	std::unique_lock l(idxLock);
	{
		baseIdx = INDEX_INVALID;
		layerIdx = INDEX_INVALID;
		poorIdx = INDEX_INVALID;
	}

	auto resetSub = [this](decltype(baseSlot)& slot)
	{
		for (auto it = slot.begin(); it != slot.end(); ++it)	// search from beginning
		{
			auto[time, idx] = *it;
			if (objs[idx].type == obj::Ty::VIDEO)
			{
#ifndef VIDEO_DISABLED
				auto pt = std::reinterpret_pointer_cast<TextureVideo>(objs[idx].pt);
				pt->stopUpdate();
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

void TextureBmsBga::stopUpdate()
{
#ifndef VIDEO_DISABLED
	auto resetSub = [this](decltype(baseSlot)& slot)
	{
		for (auto it = slot.begin(); it != slot.end(); ++it)	// search from beginning
		{
			auto [time, idx] = *it;
			if (objs[idx].type == obj::Ty::VIDEO)
			{
				auto pt = std::reinterpret_pointer_cast<TextureVideo>(objs[idx].pt);
				pt->stopUpdate();
			}
		}
		//slotIt = slot.end();	// not found
	};

	resetSub(baseSlot);
	resetSub(layerSlot);
	resetSub(poorSlot);
#endif
}

void TextureBmsBga::setVideoSpeed()
{
#ifndef VIDEO_DISABLED
	auto setSpeed = [this](decltype(baseSlot)& slot)
	{
		for (auto it = slot.begin(); it != slot.end(); ++it)	// search from beginning
		{
			auto [time, idx] = *it;
			if (objs[idx].type == obj::Ty::VIDEO)
			{
				auto pt = std::reinterpret_pointer_cast<TextureVideo>(objs[idx].pt);
				pt->setSpeed(gSelectContext.pitchSpeed);
			}
		}
		//slotIt = slot.end();	// not found
	};

	setSpeed(baseSlot);
	setSpeed(layerSlot);
	setSpeed(poorSlot);
#endif
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