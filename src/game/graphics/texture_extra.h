#pragma once
#include <map>
#include <set>
#include <variant>
#include "common/asynclooper.h"
#include "common/beat.h"
#include "graphics.h"
#include "video.h"


#ifndef VIDEO_DISABLED

class TextureVideo : public Texture
{
protected:
	std::shared_ptr<sVideo> pVideo;
	unsigned decoded_frames = ~0;
	PixelFormat format;
	bool updated = false;

public:
	TextureVideo(std::shared_ptr<sVideo> pv);
	virtual ~TextureVideo();
	void start();
	void stop();
	void seek(int64_t sec);
	void update();
	void reset();

	virtual void draw(Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees) const;
	virtual void draw(Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees, const Point& center) const;
	virtual void draw(const Rect& srcRect, Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees) const;
	virtual void draw(const Rect& srcRect, Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees, const Point& center) const;

	void stopUpdate();

private:
	static std::shared_ptr<std::shared_mutex> texMapMutex;
	static std::shared_ptr<std::map<uintptr_t, TextureVideo*>> textures;
	std::shared_ptr<std::shared_mutex> pTexMapMutex;	// ref counter guard
	std::shared_ptr<std::map<uintptr_t, TextureVideo*>> pTextures;	// ref counter guard
public:
	static void updateAll();
};

#endif

///////////////////////////////////////////////////////////////////////////////

class ChartObjectBMS;
class TextureBmsBga: public Texture
{
protected:
	mutable std::shared_mutex idxLock;
	size_t baseIdx = INDEX_INVALID;
	size_t layerIdx = INDEX_INVALID;
	size_t poorIdx = INDEX_INVALID;

	class obj
	{
	public:
		enum class Ty{
			EMPTY,
			PIC,
			VIDEO
		} type = Ty::EMPTY;

		struct playslot {
			Time time;
			bool base, layer, poor;
		};

		std::shared_ptr<Texture> pt = nullptr;
		Time playStartTime = 0;	// video

		obj() = default;
		obj(Ty t, std::shared_ptr<Texture> pt) :type(t), pt(pt) {}
	};

protected:
	std::map<size_t, obj> objs;
	std::map<size_t, obj> objs_layer;
	std::vector<std::pair<Time, size_t>> baseSlot, layerSlot, poorSlot;
	decltype(baseSlot.begin()) baseIt;
	decltype(layerSlot.begin()) layerIt;
	decltype(poorSlot.begin()) poorIt;
	bool inPoor = false;
	
public:
	TextureBmsBga(int x = 256, int y = 256) : Texture(nullptr, x, y)
	{
		baseIt = baseSlot.begin();
		layerIt = layerSlot.begin();
		poorIt = poorSlot.begin();
	}
	virtual ~TextureBmsBga()
	{
		stopUpdate();
	}

public:
	bool addBmp(size_t idx, Path path);
	bool setSlot(size_t idx, Time time, bool base, bool layer, bool poor);
	void sortSlot();
	bool setSlotFromBMS(ChartObjectBMS& bms);
	virtual void seek(const Time& t);

	virtual void update(const Time& t, bool poor);
	virtual void draw(const Rect& srcRect, Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees) const override;
	virtual void draw(const Rect& srcRect, Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees,
		const Point& center) const override;

	void reset();
	void clear();

	void setLoaded();
	void stopUpdate();
};


///////////////////////////////////////////////////////////////////////////////

class TextureDynamic : public Texture
{
protected:
	Texture* _dynTexture = nullptr;

public:
	TextureDynamic();
	virtual ~TextureDynamic() = default;

public:
	void setPath(const Path& path);

	virtual void draw(Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees) const;
	virtual void draw(Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees, const Point& center) const;
	virtual void draw(const Rect& srcRect, Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees) const;
	virtual void draw(const Rect& srcRect, Rect dstRect,
		const Color c, const BlendMode blend, const bool filter, const double angleInDegrees, const Point& center) const;
};