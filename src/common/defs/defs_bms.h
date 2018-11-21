#pragma once
namespace bms
{
	const unsigned BGMCHANNELS = 32;
	const unsigned MAXSAMPLEIDX = 36 * 36;
	const unsigned MAXMEASUREIDX = 999;
	enum GameMode {
		MODE_5KEYS,
		MODE_7KEYS,
		MODE_9KEYS,
		MODE_10KEYS,
		MODE_14KEYS
	};
	enum class ErrorCode
	{
		OK = 0,
		FILE_ERROR = 1,
		ALREADY_INITIALIZED,
		VALUE_ERROR,
		TYPE_MISMATCH,
		NOTE_LINE_ERROR,
	};
	enum class ChannelCode
	{
		BGM = 0,
		BPM,
		EXBPM,
		STOP,
		BGABASE,
		BGALAYER,
		BGAPOOR,
		NOTE1,
		NOTE2,
		NOTEINV1,
		NOTEINV2,
		NOTELN1,
		NOTELN2,
		NOTEMINE1,
		NOTEMINE2,
	};
}