#include "Audio.h"
#include <fstream>
#include <cassert>
#include "AudioManager/AudioManager.h"

Audio::Audio():
	wfet(),
	pBuffer(nullptr),
	bufferSize(0u),
	pSourceVoice(nullptr)
{}

Audio::~Audio() {
	delete[] pBuffer;

	pBuffer = nullptr;
	bufferSize = 0;
	wfet = {};
}

void Audio::Load(const std::string& fileName, bool loopFlg) {
	std::ifstream file(fileName, std::ios::binary);
	assert(file);

	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	if (strncmp(riff.chunk.id.data(), "RIFF", 4) != 0) {
		assert(!"Not RIFF");
	}
	if (strncmp(riff.type.data(), "WAVE", 4) != 0) {
		assert(!"Not WAVE");
	}

	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id.data(), "fmt ", 4) != 0) {
		assert(!"Not fmt");
	}

	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	if (strncmp(data.id.data(), "JUNK", 4) == 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id.data(), "data", 4) != 0) {
		assert(!"Not data");
	}

	char* pBufferLocal = new char[data.size];
	file.read(pBufferLocal, data.size);

	file.close();

	wfet = format.fmt;
	pBuffer = reinterpret_cast<BYTE*>(pBufferLocal);
	bufferSize = data.size;

	HRESULT hr = AudioManager::GetInstance()->xAudio2->CreateSourceVoice(&pSourceVoice, &wfet);
	assert(SUCCEEDED(hr));

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = pBuffer;
	buf.AudioBytes = bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopFlg ? XAUDIO2_LOOP_INFINITE : 0;

	hr = pSourceVoice->SubmitSourceBuffer(&buf);
}