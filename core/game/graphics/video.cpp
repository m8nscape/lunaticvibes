#include "video.h"

#include <thread>
#include <chrono>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
}

#include "common/utils.h"
#include "plog/Log.h"

sVideo::~sVideo()
{
	if (haveVideo)
	{
		if (playing) stopPlaying();
		unsetVideo();
	}
}

int sVideo::setVideo(const Path& file)
{
	if (!fs::exists(file)) return -1;
	if (!fs::is_regular_file(file)) return -2;

	if (haveVideo) unsetVideo();

	if (int ret; (ret = avformat_open_input(&pFormatCtx, file.string().c_str(), NULL, NULL)) != 0)
	{
		char buf[256];
		av_strerror(ret, buf, 256);
		LOG_WARNING << "[Video] Could not open input stream: " << fs::absolute(file).string() << " (" << buf << ")";
		return -1;
	}

	if (int ret; (ret = avformat_find_stream_info(pFormatCtx, NULL)) < 0)
	{
		char buf[256];
		av_strerror(ret, buf, 256);
		LOG_WARNING << "[Video] Could not find stream info of " << fs::absolute(file).string() << " (" << buf << ")";
		return -2;
	}

	if ((videoIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0)) < 0)
	{
		LOG_WARNING << "[Video] Could not find video stream of " << fs::absolute(file).string();
		return -3;
	}

	if ((pCodecCtx = avcodec_alloc_context3(pCodec)) == nullptr)
	{
		LOG_WARNING << "[Video] Could not alloc codec context of " << fs::absolute(file).string();
		return -4;
	}

	if (int ret; (ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoIndex]->codecpar)) < 0)
	{
		char buf[256];
		av_strerror(ret, buf, 256);
		LOG_WARNING << "[Video] Could not convert codec parameters of " << fs::absolute(file).string() << " (" << buf << ")";
		return -5;
	}

	if ((pFrame = av_frame_alloc()) == nullptr)
	{
		LOG_WARNING << "[Video] Could not alloc frame object of " << fs::absolute(file).string();
		return -6;
	}

	if ((pPacket = av_packet_alloc()) == nullptr)
	{
		LOG_WARNING << "[Video] Could not alloc packet object of " << fs::absolute(file).string();
		return -7;
	}

	if (int ret; (ret = avcodec_open2(pCodecCtx, pCodec, NULL)) < 0)
	{
		char buf[256];
		av_strerror(ret, buf, 256);
		LOG_WARNING << "[Video] Could not open codec of " << fs::absolute(file).string() << " (" << buf << ")";
		return -8;
	}

	haveVideo = true;
	return 0;
}

int sVideo::unsetVideo()
{
	haveVideo = false;
	if (pPacket) av_packet_free(&pPacket);
	if (pFrame) av_frame_free(&pFrame);
	if (pCodecCtx) avcodec_free_context(&pCodecCtx);
	if (pFormatCtx) avformat_free_context(pFormatCtx);
	return 0;
}

Texture::PixelFormat sVideo::getFormat()
{
	using fmt = Texture::PixelFormat;
	if (!haveVideo) return fmt::UNKNOWN;
	switch (pCodecCtx->pix_fmt)
	{ 
	case AV_PIX_FMT_RGB24: return fmt::RGB24;
	case AV_PIX_FMT_BGR24: return fmt::BGR24;
	case AV_PIX_FMT_YUV420P: return fmt::IYUV;
	case AV_PIX_FMT_YUYV422: return fmt::YUY2;
	case AV_PIX_FMT_UYVY422: return fmt::UYVY;
	case AV_PIX_FMT_YVYU422: return fmt::YVYU;

	default: return fmt::UNSUPPORTED;
	}
}

void sVideo::startPlaying()
{
	if (playing) return;

	std::thread([this]() { decodeLoop(); }).detach();
	playing = true;
}

void sVideo::stopPlaying()
{
	if (!playing) return;
	using namespace std::chrono_literals;
	while (decoding) std::this_thread::sleep_for(500ms);
	playing = false;
}

void sVideo::decodeLoop()
{
	if (!haveVideo) return;
	decoding = true;

	while (av_read_frame(pFormatCtx, pPacket) == 0 && pPacket)
	{
		avcodec_send_packet(pCodecCtx, pPacket);

		int ret = 0;
		{
			std::unique_lock<decltype(lock)> l(lock);
			ret = avcodec_receive_frame(pCodecCtx, pFrame);
		}

		if (ret < 0)
		{
			if (ret == AVERROR(EAGAIN)) continue;
			else
			{
				char buf[128];
				av_strerror(ret, buf, 128);
				LOG_ERROR << "[Video] playback error: " << buf;
				decoding = false;
				return;
			}
		}
		decoded_frames = pCodecCtx->frame_number;
	}

	// drain
	avcodec_send_packet(pCodecCtx, NULL);
	{
		std::unique_lock<decltype(lock)> l(lock);
		if (avcodec_receive_frame(pCodecCtx, pFrame) != 0)
		{
			LOG_ERROR << "[Video] playback drain error";
		}
	}
	decoded_frames = pCodecCtx->frame_number;

	decoding = false;
}

void sVideo::seek(int64_t second)
{
	if (!haveVideo) return;
	double tps = pFormatCtx->streams[videoIndex]->time_base.num == 0 ? 
		AV_TIME_BASE : av_q2d(pFormatCtx->streams[videoIndex]->time_base);

	std::unique_lock<decltype(lock)> l(lock);
	av_seek_frame(pFormatCtx, videoIndex, int64_t(std::round(second / tps)), 0);
}