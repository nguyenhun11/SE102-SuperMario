// SoundManager.h
#pragma comment(lib, "irrKlang.lib")
#include <irrKlang.h>
#include <map>
#include <string>

using namespace irrklang;

class SoundManager 
{
private:
    static SoundManager* instance;
    ISoundEngine* engine;
    std::map<std::string, std::string> soundMap;
    std::map<std::string, ISound*> playingSounds;

    /// ISOUND
    // Phát nhạc nền
    ISound* currentBGM = nullptr;
    ISound* pmeterFullSound = nullptr;
    ISound* skidSound = nullptr;
    ISound* tailSound = nullptr;

public:
    static SoundManager* GetInstance() 
    {
        if (!instance) instance = new SoundManager();
        return instance;
    }

    void Init() 
    {
        engine = createIrrKlangDevice();
        if (!engine) return;
    }

    // Load đường dẫn âm thanh
    void Load(std::string name, std::string path) 
    {
        soundMap[name] = path;
    }

    // Phát âm thanh
    void Play(std::string name, bool loop = false)
    {
        if (soundMap.count(name))
        {
            Stop(name);

            ISound* snd = engine->play2D(soundMap[name].c_str(), loop, false, true);

            if (snd)
            {
                playingSounds[name] = snd;
            }
        }
    }

    void Stop(std::string name)
    {
        if (playingSounds.count(name))
        {
            ISound* snd = playingSounds[name];
            if (snd)
            {
                if (!snd->isFinished())
                {
                    snd->stop();
                }
                snd->drop(); 
            }
            playingSounds.erase(name);
        }
    }



    void PlayBGM(std::string name) 
    {
        if (currentBGM) 
        {
            currentBGM->stop(); 
            currentBGM->drop();
        }
        if (soundMap.count(name)) 
        {
            currentBGM = engine->play2D(soundMap[name].c_str(), true, false, true);
        }
    }

    void PauseBGM()
    {
        if (currentBGM != nullptr && !currentBGM->isFinished())
        {
            currentBGM->setIsPaused(true);
        }
    }

    void ResumeBGM()
    {
        if (currentBGM != nullptr && !currentBGM->isFinished())
        {
            currentBGM->setIsPaused(false);
        }
    }

    #pragma region PLAYSFX
    void PlayPmeterFull(std::string name) 
    {
        if (soundMap.count(name)) 
        {
            if (pmeterFullSound == nullptr || pmeterFullSound->isFinished()) {
                pmeterFullSound = engine->play2D(soundMap[name].c_str(), true, false, true);
            }
        }
    }

    void StopPmeterFull() {
        if (pmeterFullSound) {
            pmeterFullSound->stop(); 
            pmeterFullSound->drop(); 
            pmeterFullSound = nullptr;
        }
    }

    

    void PlaySkid(std::string name) {
        if (soundMap.count(name)) {
            if (skidSound && !skidSound->isFinished()) {
                skidSound->stop();
                skidSound->drop();
                skidSound = nullptr;
            }

            skidSound = engine->play2D(soundMap[name].c_str(), false, false, true);
            if (skidSound) {
                skidSound->setVolume(0.8f);
            }
        }
    }

    void PlayTail(std::string name) {
        if (soundMap.count(name)) {
            if (tailSound && !tailSound->isFinished()) {
                tailSound->stop();
                tailSound->drop();
                tailSound = nullptr;
            }

            tailSound = engine->play2D(soundMap[name].c_str(), false, false, true);
            if (tailSound) {
                tailSound->setVolume(0.8f);
            }
        }
    }

    void StopAll()
    {
        engine->stopAllSounds();
    }
#pragma endregion
    ~SoundManager() 
    {
        if (engine) engine->drop();
	}
};