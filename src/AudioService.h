#pragma once
#include <memory>
#include <map>

#include <SFML/Audio.hpp>

#include "TooterLogger.h"

namespace tt
{

class ResourceManager;

class IAudio;
using AudioPtr = std::shared_ptr<IAudio>;

class AudioLocator
{
public:
    static AudioPtr music() { return _music; }
    static void setMusic(AudioPtr audio) { _music = audio; }

    static AudioPtr sound() { return _sound; }
    static void setSound(AudioPtr audio) { _sound = audio; }

private: 
    inline static AudioPtr _music;
    inline static AudioPtr _sound;
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
    LoggedAudio(IAudio& wrapped)
        : _wrapped{ wrapped },
          _logger { log::initializeLogger("Audio") }
    {
        // do nothing
    }

    void cacheAudio(const std::string& name) override
    {
        _logger->trace("caching audio '{}'", name);
        _wrapped.cacheAudio(name);
    }

    void play(const std::string& name) override
    {
        _logger->trace("playing audio '{}'", name);
        _wrapped.play(name);
    }

    void stop(const std::string& name) override
    {
        _logger->trace("stopping audio '{}'", name);
        _wrapped.stop(name);
    }

    void pause(const std::string& name) override
    {
        _logger->trace("pausing audio '{}'", name);
        _wrapped.pause(name);
    }

    void setLoop(const std::string& name, bool doLoop) override
    {
        _logger->trace("setting loop of '{}' to '{}'", name, doLoop);
        _wrapped.setLoop(name, doLoop);
    }

    void setVolume(const std::string& name, float volume) override
    {
        _logger->trace("setting volume of '{}' to '{}'", name, volume);
        _wrapped.setVolume(name, volume);
    }

    sf::SoundSource::Status getStatus(const std::string& name) override
    {
        _logger->trace("getting status of '{}'", name);
        return _wrapped.getStatus(name);
    }

private:
    IAudio& _wrapped;
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

    void cacheAudio(const std::string &name) override;

    void play(const std::string &name) override
    {
        _sounds.at(name)->play();
    }

    void stop(const std::string &name) override
    {
        _sounds.at(name)->stop();
    }

    void pause(const std::string &name) override
    {
        _sounds.at(name)->pause();
    }

    void setLoop(const std::string &name, bool doLoop) override
    {
        _sounds.at(name)->setLoop(doLoop);
    }

    void setVolume(const std::string &name, float volume) override
    {
        _sounds.at(name)->setVolume(volume);
    }

    sf::SoundSource::Status getStatus(const std::string &name) override
    {
        return _sounds.at(name)->getStatus();
    }

private:
    ResourceManager& _resources;

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

    void cacheAudio(const std::string &name) override;

    void play(const std::string &name) override
    {
        _music.at(name)->play();
    }

    void stop(const std::string &name) override
    {
        _music.at(name)->stop();
    }

    void pause(const std::string &name) override
    {
        _music.at(name)->pause();
    }

    void setLoop(const std::string &name, bool doLoop) override
    {
        _music.at(name)->setLoop(doLoop);
    }

    void setVolume(const std::string &name, float volume) override
    {
        _music.at(name)->setVolume(volume);
    }

    sf::SoundSource::Status getStatus(const std::string &name) override
    {
        return _music.at(name)->getStatus();
    }

private:
    ResourceManager& _resources;

    using MusicPtr = std::unique_ptr<sf::Music>;
    std::map<std::string, MusicPtr> _music;
};

} // namespace