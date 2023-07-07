#include "AudioManager.h"
#include <cassert>

AudioManager* AudioManager::instance = nullptr;
void AudioManager::Inititalize() {
	instance = new AudioManager();
	assert(instance!=nullptr);
}
void AudioManager::Finalize() {
	delete instance;
	instance = nullptr;
}

AudioManager::AudioManager() :
	xAudio2(),
	masterVoice(nullptr)
{
	HRESULT hr = XAudio2Create(xAudio2.GetAddressOf(), 0u, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));
	hr = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(hr));
}
AudioManager::~AudioManager() {
	xAudio2.Reset();
}

void AudioManager::LoadWav(const std::string& fileName, bool flg) {
	if (audios.empty()) {
		auto audio = std::make_unique<Audio>();
		audio->Load(fileName, flg);
		audios.insert({ fileName, std::move(audio) });
	}
	else {
		auto itr = audios.find(fileName);

		if (itr == audios.end()) {
			auto audio = std::make_unique<Audio>();
			audio->Load(fileName, flg);
			audios.insert({ fileName, std::move(audio) });
		}
	}
}

void AudioManager::Start(const std::string& fileName, float volume) {
	HRESULT hr = audios[fileName]->pSourceVoice->Start();
	audios[fileName]->pSourceVoice->SetVolume(volume);
	assert(SUCCEEDED(hr));
}

void AudioManager::Stop(const std::string& fileName) {
	audios[fileName]->pSourceVoice->Stop();
}