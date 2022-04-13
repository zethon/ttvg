#pragma once
#include <memory>
#include <map>

#include <SFML/Audio.hpp>

#include "TooterLogger.h"

namespace tt
{

class ResourceManager;

class IAudio;
using IAudioPtr = std::shared_ptr<IAudio>;

class AudioLocator
{
public:
    static const struct luaL_Reg MusicLuaMethods[];
    static const struct luaL_Reg SoundLuaMethods[];

    static IAudioPtr music() { return _music; }
    static void setMusic(IAudioPtr audio) { _music = audio; }

    static IAudioPtr sound() { return _sound; }
    static void setSound(IAudioPtr audio) { _sound = audio; }

private: 
    inline static IAudioPtr _music;
    inline static IAudioPtr _sound;
};

class IAudio
{
public:
    virtual ~IAudio() = default;

    virtual void cacheAudio(const std::string& name) = 0;

    virtual void play(const std::string& name) = 0;
    virtual void stop(const std::string& name) = 0;
    virtual void pause(const std::string& name) = 0;

    virtual void setLoop(const std::string& name, bool doLoop) = 0;

    // set overall volume for all audio
    virtual void setAllVolume(float volume) = 0;

    // set volume for one specific audio sound
    virtual void setVolume(const std::string& name, float volume) = 0;

    virtual sf::SoundSource::Status getStatus(const std::string& name) = 0;
};

class NullAudio : public IAudio
{
public:
    void cacheAudio(const std::string& name) override { }

    void play(const std::string& name) override { }
    void stop(const std::string& name) override { }
    void pause(const std::string& name) override { }

    void setLoop(const std::string&, bool) override { }

    void setAllVolume(float volume) override { };
    void setVolume(const std::string& name, float volume) override { }

    sf::SoundSource::Status getStatus(const std::string& name) override
    {
        // is this the right thing to do?
        return sf::SoundSource::Status::Stopped;
    }
};

class LoggedAudio : public IAudio
{
public:
    LoggedAudio(IAudioPtr wrapped)
        : _wrapped{ wrapped },
          _logger{ log::initializeLogger("Audio") }
    {
        // do nothing
    }

    void cacheAudio(const std::string& name) override
    {
        _logger->trace("caching audio '{}'", name);
        _wrapped->cacheAudio(name);
    }

    void play(const std::string& name) override
    {
        _logger->trace("playing audio '{}'", name);
        _wrapped->play(name);
    }

    void stop(const std::string& name) override
    {
        _logger->trace("stopping audio '{}'", name);
        _wrapped->stop(name);
    }

    void pause(const std::string& name) override
    {
        _logger->trace("pausing audio '{}'", name);
        _wrapped->pause(name);
    }

    void setLoop(const std::string& name, bool doLoop) override
    {
        _logger->trace("setting loop of '{}' to '{}'", name, doLoop);
        _wrapped->setLoop(name, doLoop);
    }

    void setAllVolume(float volume) override
    {
        _logger->trace("setting all volume to '{}'", volume);
        _wrapped->setAllVolume(volume);
    }

    void setVolume(const std::string& name, float volume) override
    {
        _logger->trace("setting volume of '{}' to '{}'", name, volume);
        _wrapped->setVolume(name, volume);
    }

    sf::SoundSource::Status getStatus(const std::string& name) override
    {
        return _wrapped->getStatus(name);
    }

private:
    IAudioPtr _wrapped;
    log::SpdLogPtr  _logger;
};

class SfxAudio : public IAudio
{
public:
    SfxAudio(tt::ResourceManager& resources)
        : _resources{resources}
    {
        // nothing to do
    }

    void cacheAudio(const std::string& name) override;

    void play(const std::string& name) override
    {
        if (name.empty()) return;
        _sounds.at(name)->play();
    }

    void stop(const std::string& name) override
    {
        if (name.empty()) return;
        _sounds.at(name)->stop();
    }

    void pause(const std::string& name) override
    {
        if (name.empty()) return;
        _sounds.at(name)->pause();
    }

    void setLoop(const std::string& name, bool doLoop) override
    {
        if (name.empty()) return;
        _sounds.at(name)->setLoop(doLoop);
    }

    void setAllVolume(float volume) override
    {
        _volume = volume;
        for(auto&[name, sound] : _sounds)
        {
            sound->setVolume(_volume);
        }
    }

    void setVolume(const std::string& name, float volume) override
    {
        if (name.empty()) return;
        _sounds.at(name)->setVolume(volume);
    }

    sf::SoundSource::Status getStatus(const std::string& name) override
    {
        if (name.empty()) return sf::SoundSource::Status::Stopped;
        return _sounds.at(name)->getStatus();
    }

private:
    ResourceManager&    _resources;
    float               _volume = 100.f;

    using SoundPtr = std::shared_ptr<sf::Sound>;
    std::map<std::string, SoundPtr> _sounds;
};

class MusicAudio: public IAudio
{
public:
    MusicAudio(tt::ResourceManager& resources)
        : _resources{resources}
    {
        // nothing to do
    }

    void cacheAudio(const std::string& name) override;

    void play(const std::string& name) override
    {
        if (name.empty()) return;
        assert(_music.find(name) != _music.end());
        _music.at(name)->play();
    }

    void stop(const std::string& name) override
    {
        if (name.empty()) return;
        assert(_music.find(name) != _music.end());
        _music.at(name)->stop();
    }

    void pause(const std::string& name) override
    {
        if (name.empty()) return;
        assert(_music.find(name) != _music.end());
        _music.at(name)->pause();
    }

    void setLoop(const std::string& name, bool doLoop) override
    {
        if (name.empty()) return;
        assert(_music.find(name) != _music.end());
        _music.at(name)->setLoop(doLoop);
    }

    void setAllVolume(float volume) override
    {
        _volume = volume;
        for(auto&[name, song] : _music)
        {
            song->setVolume(_volume);
        }
    }

    void setVolume(const std::string& name, float volume) override
    {
        if (name.empty()) return;
        assert(_music.find(name) != _music.end());
        _music.at(name)->setVolume(volume);
    }

    sf::SoundSource::Status getStatus(const std::string& name) override
    {
        if (name.empty()) return sf::SoundSource::Status::Stopped;
        assert(_music.find(name) != _music.end());
        return _music.at(name)->getStatus();
    }

private:
    ResourceManager&    _resources;
    float               _volume = 100.f;

    using MusicPtr = std::unique_ptr<sf::Music>;
    std::map<std::string, MusicPtr> _music;
};

} // namespace