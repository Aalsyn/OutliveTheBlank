#pragma once
#include "Singleton.h"

#pragma region preparation_analyse_audio

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

#pragma endregion

class SongError {};

namespace PM3D
{
    class SoundController : public CSingleton<SoundController>
    {
        IXAudio2* mpXAudio2;
        IXAudio2MasteringVoice* mpMasterVoice;
        std::map<std::string, IXAudio2SourceVoice*> mSongMap;
       


        HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
        HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);

    public:
        SoundController();
        void init();
        void createMySourceVoice(const char* path, std::string voiceName, int loop);
        void refuelSourceVoice(const char* path, std::string voiceName, int loop);
        void playMoving();
        void stopMoving();
        void exitLoopMoving();
        void playIdle();
        void stopIdle();
        void exitLoopIdle();
        void playRaceSoundtrack();
        void stopRaceSoundtrack();
        void exitLoopRaceSoundtrack();
        void playMenuSoundtrack();
        void stopMenuSoundtrack();
        void exitLoopMenuSoundtrack();
        void playFall();
        void stopFall();
        void exitLoopFall();
        void playDeath();
        void stopDeath();
        void exitLoopDeath();
        void playMonster();
        void stopMonster();
        void exitLoopMonster();
        void release();
        void changeVolume(float volume);

    };
}