#pragma once

#include <set>

inline const std::set<std::string> video_file_extensions =
{
	".mpg",
	".mp2",
	".mpeg",
	".mpeg2",
	".mpe",
	".mpv",
	".flv",
	".mp4",
	".m4p",
	".m4v",
	".f4v",
	".avi",
	".wmv",
	".mkv",
	".webm",
	".mov",
	".m1v",
};

#ifndef VIDEO_DISABLED

#include <shared_mutex>
#include <future>
#include "common/types.h"
#include "graphics.h"

extern "C"
{
	struct AVFormatContext;
	struct AVCodec;
	struct AVCodecContext;
	struct AVFrame;
	struct AVPacket;
}
class SceneBase;
class SkinBase;

void video_init();

// libav decoder wrap
class sVideo
{
	friend class SceneBase;
	friend class SkinBase;
public:
	Path file;
	bool haveVideo = false;

private:

	// decoder params
	AVFormatContext *pFormatCtx = nullptr;
	const AVCodec *pCodec = nullptr;
	AVCodecContext *pCodecCtx = nullptr;
	AVFrame *pFrame = nullptr;
	AVPacket *pPacket = nullptr;
	int videoIndex = -1;
	unsigned decoded_frames = 0;
	std::chrono::time_point<std::chrono::system_clock> startTime;
	std::future<void> decodeEnd;

	// render properties
	Path filePath;
	double speed = 1.0;
	int w = -1, h = -1;		// set in setVideo()
	bool playing = false;
	bool finished = false;
	bool decoding = false;
	bool firstFrame = true;
	bool valid = false;
	bool loop_playback = false;

public:
	sVideo() = default;
	sVideo(const Path& file, double speed = 1.0, bool loop = false) { setVideo(file, speed, loop); }
	virtual ~sVideo();
	int setVideo(const Path& file, double speed, bool loop = false);
	int unsetVideo();
	int getW() { return w; }
	int getH() { return h; }
	bool isPlaying() const { return playing; }

public:
	// properties
	Texture::PixelFormat getFormat();

public:
	// video playback control
	void startPlaying();
	void stopPlaying();
	void decodeLoop();

	int getDecodedFrames() { return decoded_frames; }
	AVFrame* getFrame() { return valid ? pFrame : NULL; }

public:
	std::shared_mutex video_frame_mutex;

public:
	void setSpeed(double speed) { this->speed = speed; }
	void seek(int64_t second, bool backwards = false);
	
};

#else

class sVideo
{
public:
	sVideo() = default;
	virtual ~sVideo() = default;
};

#endif