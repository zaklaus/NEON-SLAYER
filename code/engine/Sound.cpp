#include "StdAfx.h"

#include "Sound.h"
#include "SoundLoader.h"

#include <string>

CSound::CSound(LPSTR wavPath): CAllocable()
{
    mBuffer = NULL;
    mIsLooping = FALSE;
    mData = NULL;
    mDataSize = 0;

    std::string path(wavPath);

    if (path.rfind(".ogg") != std::string::npos) {
        CSoundLoader::LoadOGG(wavPath, &mBuffer, &mData, &mDataSize);
    }
    else {
        CSoundLoader::LoadWAV(wavPath, &mBuffer, &mData, &mDataSize);
    }
}

VOID CSound::Release()
{
    if (DelRef())
    {
        SAFE_RELEASE(mBuffer);
        SAFE_DELETE_ARRAY(mData);
        delete this;
    }
}

VOID CSound::Play()
{
    mBuffer->Play(0, 0, mIsLooping ? DSBPLAY_LOOPING : 0);
}

VOID CSound::Pause()
{
    mBuffer->Stop();
}

VOID CSound::Stop()
{
    Pause();
    SetCurrentPosition(0);
}

VOID CSound::SetVolume(LONG vol)
{
    LONG realVol = (LONG)ScaleBetween((FLOAT)vol, DSBVOLUME_MIN, DSBVOLUME_MAX, 0, 100);
    mBuffer->SetVolume(realVol);
}

VOID CSound::SetPan(LONG pan)
{
    LONG realPan = (LONG)ScaleBetween((FLOAT)pan, DSBPAN_LEFT, DSBPAN_RIGHT, -100, 100);
    mBuffer->SetPan(realPan);
}

VOID CSound::SetFrequency(DWORD freq)
{
    if (freq == 0)
    {
        freq = DSBFREQUENCY_ORIGINAL;
    }

    mBuffer->SetFrequency(freq);
}

LONG CSound::GetVolume()
{
    LONG vol;
    mBuffer->GetVolume(&vol);
    return (LONG)ScaleBetween((FLOAT)vol, 0, 100, DSBVOLUME_MIN, DSBVOLUME_MAX);
}

LONG CSound::GetPan()
{
    LONG pan;
    mBuffer->GetPan(&pan);
    return (LONG)ScaleBetween((FLOAT)pan, -100, 100, DSBPAN_LEFT, DSBPAN_RIGHT);
}

DWORD CSound::GetFrequency()
{
    DWORD freq = 0;
    mBuffer->GetFrequency(&freq);
    return freq;
}

BOOL CSound::IsPlaying()
{
    DWORD playStatus = 0x000000;
    mBuffer->GetStatus(&playStatus);
    return playStatus & DSBSTATUS_PLAYING;
}

DWORD CSound::GetCurrentPosition()
{
    DWORD playPos = 0x000000;
    mBuffer->GetCurrentPosition(&playPos, NULL);
    return playPos;
}

VOID CSound::SetCurrentPosition(DWORD cursor)
{
    mBuffer->SetCurrentPosition(cursor);
}

VOID CSound::SetLoop(BOOL state)
{
    mIsLooping = state;

    if (IsPlaying())
    {
        Pause();
        Play();
    }
}

BOOL CSound::IsLooping()
{
    return mIsLooping;
}

UCHAR* CSound::GetData(ULONG* sizeOut)
{
    *sizeOut = mDataSize;
    return mData;
}
