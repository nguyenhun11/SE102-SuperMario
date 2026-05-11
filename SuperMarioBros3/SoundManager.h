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
        if (soundMap.count(name)) {
            engine->play2D(soundMap[name].c_str(), loop);
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