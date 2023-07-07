#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <wrl.h>
#include <memory>
#include <unordered_map>
#include "Audio/Audio.h"

class AudioManager {
	friend Audio;

private:
	AudioManager();
	~AudioManager();

public:
	static void Inititalize();
	static void Finalize();
	static inline AudioManager* GetInstance() {
		return instance;
	}

private:
	static AudioManager* instance;

public:
	void LoadWav(const std::string& fileName, bool flg);

	void Start(const std::string& fileName, float volume);

	void Stop(const std::string& fileName);

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;

	std::unordered_map<std::string, std::unique_ptr<class Audio>> audios;
};