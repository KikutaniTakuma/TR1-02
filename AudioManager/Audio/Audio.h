#pragma once
#include <array>
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <string>

class Audio {
	friend class AudioManager;

private:
	struct ChunkHeader {
		std::array<char,4> id;
		int32_t size;
	};
	struct RiffHeader {
		ChunkHeader chunk;
		std::array<char, 4> type;
	};
	struct FormatChunk
	{
		ChunkHeader chunk;
		WAVEFORMATEX fmt;
	};

public:
	Audio();
	~Audio();

private:
	void Load(const std::string& fileName, bool loopFlg);

private:
	WAVEFORMATEX wfet;
	BYTE* pBuffer;
	uint32_t bufferSize;
	IXAudio2SourceVoice* pSourceVoice;
};