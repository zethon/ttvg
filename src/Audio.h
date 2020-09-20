#pragma once

#include "ResourceManager.h"

namespace tt
{

class Audio;
using AudioPtr = std::shared_ptr<Audio>;

class Audio
{
public:
  virtual ~Audio() {}
  virtual void playSound(int soundID) = 0;
  virtual void stopSound(int soundID) = 0;
  virtual void stopAllSounds() = 0;

private:
};

class NullAudio : public Audio
{

public:
    void playSound(int soundID) { /* do nothing */ }
    void stopSound(int soundID) { /* do nothing */ }
    void stopAllSounds() { /* do nothing */ }
};

// class LoggedAudio : public Audio
// {

// };

} // namespace