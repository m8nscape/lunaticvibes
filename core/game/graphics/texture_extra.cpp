
#include <execution>
#include <algorithm>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
}

#include <plog/Log.h>

#include "game/scroll/scroll_bms.h"
#include "texture_extra.h"
#include "types.h"
#include "utils.h"

const std::set<std::string> video_file_extensions =
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
	loopStart();
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

	auto pf = pVideo->getFrame();
	if (!pf) return;

	{
		// read lock
		std::shared_lock<std::shared_mutex> l(pVideo->lock);

		switch (format)
		{
		case Texture::PixelFormat::IYUV:
			updateYUV(
				pf->data[0], pf->linesize[0],
				pf->data[1], pf->linesize[1],
				pf->data[2], pf->linesize[2]);
			break;

		default:
			break;
		}
	}
}


bool TextureBmsBga::addBmp(size_t idx, const Path& pBmp)
{
	if (idx == -1u) return false;
	if (fs::exists(pBmp) && fs::is_regular_file(pBmp) && pBmp.has_extension())
	{
		if (video_file_extensions.find(toLower(pBmp.extension().string())) != video_file_extensions.end())
		{
			objs[idx].type = obj::Ty::VIDEO;
			objs[idx].pt = std::make_shared<TextureVideo>(std::make_shared<sVideo>(pBmp));
			LOG_DEBUG << "[TextureBmsBga] added video: " << pBmp.string();
			return true;
		}
		else
		{
			objs[idx].type = obj::Ty::PIC;
			objs[idx].pt = std::make_shared<Texture>(Image(pBmp));
			LOG_DEBUG << "[TextureBmsBga] added pic: " << pBmp.string();
			return true;
		}
	}
	LOG_DEBUG << "[TextureBmsBga] file not found: " << pBmp.string();
	return false;
}

bool TextureBmsBga::setSlot(size_t idx, timestamp time, bool base, bool layer, bool poor)
{
	if (objs.find(idx) == objs.end()) return false;
	if (base) baseSlot.emplace_back(time, idx);
	if (layer) layerSlot.emplace_back(time, idx);
	if (poor) poorSlot.emplace_back(time, idx);
	return true;
}

void TextureBmsBga::sortSlot()
{
	auto less = [](const std::pair<timestamp, size_t>& l, const std::pair<timestamp, size_t>& r)
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

bool TextureBmsBga::setSlotFromBMS(ScrollBMS& bms)
{
	baseSlot.clear();
	layerSlot.clear();
	poorSlot.clear();
	const auto lBase = bms.getBgaBase();
	const auto lLayer = bms.getBgaLayer();
	const auto lPoor = bms.getBgaPoor();
	for (const auto& l : lBase) setSlot(std::get<long long>(l.value), l.time, true, false, false);
	for (const auto& l : lLayer) setSlot(std::get<long long>(l.value), l.time, false, true, false);
	for (const auto& l : lPoor) setSlot(std::get<long long>(l.value), l.time, false, false, true);
	sortSlot();
	return true;
}

void TextureBmsBga::seek(timestamp t)
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
					auto pt = std::reinterpret_pointer_cast<TextureVideo>(objs[idx].pt);
					pt->seek((t - time).norm() / 1000);
					pt->update();
				}
				return;
			}
		}
		slotIt = slot.end();	// not found
	};

	seekSub(baseSlot, baseIdx, baseIt);
	seekSub(layerSlot, layerIdx, layerIt);
	seekSub(poorSlot, poorIdx, poorIt);
	inPoor = false;
}

void TextureBmsBga::update(timestamp t, bool poor)
{
	auto seekSub = [&t, this](decltype(baseSlot)& slot, size_t& slotIdx, decltype(baseSlot.begin())& slotIt)
	{
		for (auto it = slotIt; it != slot.end(); ++it)	// start from cached iterator
		{
			auto[time, idx] = *it;
			if (time <= t)
			{
				slotIdx = idx;
				slotIt = it;
				if (objs[idx].type == obj::Ty::VIDEO)
				{
					auto pt = std::reinterpret_pointer_cast<TextureVideo>(objs[idx].pt);
					pt->seek((t - time).norm() / 1000);
					pt->update();
				}
				return;
			}
		}
		slotIt = slot.end();	// no bga
	};

	seekSub(baseSlot, baseIdx, baseIt);
	seekSub(layerSlot, layerIdx, layerIt);
	seekSub(poorSlot, poorIdx, poorIt);
	inPoor = poor;
}

void TextureBmsBga::draw(const Rect& sr, Rect dr,
	const Color c, const BlendMode b, const bool f, const double a) const
{
	if (inPoor && poorIdx != -1u && objs.at(poorIdx).type != obj::Ty::EMPTY) objs.at(poorIdx).pt->draw(sr, dr, c, b, f, a);
	else
	{
		if (baseIdx != -1u && objs.at(baseIdx).type != obj::Ty::EMPTY) objs.at(baseIdx).pt->draw(sr, dr, c, b, f, a);
		if (layerIdx != -1u && objs.at(layerIdx).type != obj::Ty::EMPTY) objs.at(layerIdx).pt->draw(sr, dr, c, b, f, a);
	}
}

void TextureBmsBga::draw(const Rect& sr, Rect dr,
	const Color c, const BlendMode b, const bool f, const double a, const Point& ct) const
{
	if (inPoor && poorIdx != -1u && objs.at(poorIdx).type != obj::Ty::EMPTY) objs.at(poorIdx).pt->draw(sr, dr, c, b, f, a, ct);
	else
	{
		if (baseIdx != -1u && objs.at(baseIdx).type != obj::Ty::EMPTY) objs.at(baseIdx).pt->draw(sr, dr, c, b, f, a, ct);
		if (layerIdx != -1u && objs.at(layerIdx).type != obj::Ty::EMPTY) objs.at(layerIdx).pt->draw(sr, dr, c, b, f, a, ct);
	}
}

