#pragma once
#include <shared_mutex>
#include "types.h"
#include "game/graphics/graphics.h"

extern "C"
{
	struct AVFormatContext;
	struct AVCodec;
	struct AVCodecContext;
	struct AVFrame;
	struct AVPacket;
}
class vScene;
class vSkin;

// libav decoder wrap
class sVideo
{
	friend class vScene;
	friend class vSkin;
public:
	bool haveVideo = false;

private:

	// decoder params
	AVFormatContext *pFormatCtx = nullptr;
	AVCodec *pCodec = nullptr;
	AVCodecContext *pCodecCtx = nullptr;
	AVFrame *pFrame = nullptr;
	AVPacket *pPacket = nullptr;
	int videoIndex = -1;
	unsigned decoded_frames;

	// render properties
	Path filePath;
	double speed = 1.0;
	int w, h;
	bool playing = false;
	bool decoding = false;

public:
	sVideo() = default;
	sVideo(const Path& file) { setVideo(file); }
	virtual ~sVideo();
	int setVideo(const Path& file);
	int unsetVideo();
	int getW() { return w; }
	int getH() { return h; }

public:
	// properties
	Texture::PixelFormat getFormat();

public:
	// video playback control
	void startPlaying();
	void stopPlaying();
	bool isPlaying() { return playing; }
	void decodeLoop();

	int getDecodedFrames() { return decoded_frames; }
	AVFrame* getFrame() { return pFrame; }

public:
	std::shared_mutex lock;

public:
	void seek(int64_t second);
	
};