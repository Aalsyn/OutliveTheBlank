#include <stdafx.h>
#include "SoundController.h"
//using namespace std;


namespace PM3D
{
    HRESULT SoundController::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());

        DWORD dwChunkType;
        DWORD dwChunkDataSize;
        DWORD dwRIFFDataSize = 0;
        DWORD dwFileType;
        DWORD bytesRead = 0;
        DWORD dwOffset = 0;

        while (hr == S_OK)
        {
            DWORD dwRead;
            if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());

            if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());

            switch (dwChunkType)
            {
            case fourccRIFF:
                dwRIFFDataSize = dwChunkDataSize;
                dwChunkDataSize = 4;
                if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                    hr = HRESULT_FROM_WIN32(GetLastError());
                break;

            default:
                if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                    return HRESULT_FROM_WIN32(GetLastError());
            }

            dwOffset += sizeof(DWORD) * 2;

            if (dwChunkType == fourcc)
            {
                dwChunkSize = dwChunkDataSize;
                dwChunkDataPosition = dwOffset;
                return S_OK;
            }

            dwOffset += dwChunkDataSize;

            if (bytesRead >= dwRIFFDataSize) return S_FALSE;

        }

        return S_OK;

    }


    HRESULT SoundController::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
    {
        HRESULT hr = S_OK;
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
            return HRESULT_FROM_WIN32(GetLastError());
        DWORD dwRead;
        if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    SoundController::SoundController()
    {
        init();
    }



    void SoundController::init()
    {
        HRESULT hr;
        hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr))
            throw SongError{};

        //creer instance moteur Xaudio2
        if (FAILED(hr = XAudio2Create(&mpXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
            throw SongError{};

        // creer mastering
        if (FAILED(hr = mpXAudio2->CreateMasteringVoice(&mpMasterVoice)))
            throw SongError{};

        createMySourceVoice(".\\sounds\\VehiculeIdle.wav", "bikeIdle", 256);
        createMySourceVoice(".\\sounds\\Vehicule_Moving.wav", "bikeMoving", 256);
        createMySourceVoice(".\\sounds\\RaceSoundtrack.wav", "raceSoundtrack", 5);
        createMySourceVoice(".\\sounds\\MainMenuSoundtrack.wav", "mainMenuSoundtrack", 2);
        createMySourceVoice(".\\sounds\\fall_sound.wav", "fall", 0);
        createMySourceVoice(".\\sounds\\death_sound.wav", "death",0);
        createMySourceVoice(".\\sounds\\gorilla_sound.wav", "gorilla", 10);

    }

    void SoundController::createMySourceVoice(const char* path, std::string voiceName, int loop = 0) {

        HRESULT hr;

#pragma region remplissage_xaudio2_avec_segment_RIFF

        WAVEFORMATEXTENSIBLE wfx = { 0 };
        XAUDIO2_BUFFER buffer = { 0 };
        loop < 255 ? buffer.LoopCount = loop : buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

#pragma warning(disable : 4996)
        const size_t cSize = strlen(path) + 1;
        wchar_t* wcPath = new wchar_t[cSize];
        mbstowcs(wcPath, path, cSize);

        // Open the file
        HANDLE hFile = CreateFile(
            wcPath,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (INVALID_HANDLE_VALUE == hFile)
            throw SongError{};
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            throw SongError{};

#pragma endregion

#pragma region recherche_du_riff_dans_audio

        DWORD dwChunkSize;
        DWORD dwChunkPosition;
        //check the file type, should be fourccWAVE or 'XWMA'
        FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
        DWORD filetype;
        ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
        if (filetype != fourccWAVE)
            throw SongError{};


#pragma endregion

        FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
        ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

        //fill out the audio data buffer with the contents of the fourccDATA chunk
        FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
        BYTE* pDataBuffer = new BYTE[dwChunkSize];
        ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

        buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
        buffer.pAudioData = pDataBuffer;  //buffer containing audio data
        buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

        //etape 3
        IXAudio2SourceVoice* voice;
        if (FAILED(hr = mpXAudio2->CreateSourceVoice(&voice, (WAVEFORMATEX*)&wfx)))
            throw SongError{};
        
        mSongMap.emplace(voiceName, voice);
        //4
        if (FAILED(hr = mSongMap[voiceName]->SubmitSourceBuffer(&buffer)))
            throw SongError{};
    }

    void SoundController::refuelSourceVoice(const char* path, std::string voiceName, int loop = 0) {

        HRESULT hr;

#pragma region remplissage_xaudio2_avec_segment_RIFF

        WAVEFORMATEXTENSIBLE wfx = { 0 };
        XAUDIO2_BUFFER buffer = { 0 };
        loop < 255 ? buffer.LoopCount = loop : buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

#pragma warning(disable : 4996)
        const size_t cSize = strlen(path) + 1;
        wchar_t* wcPath = new wchar_t[cSize];
        mbstowcs(wcPath, path, cSize);

        // Open the file
        HANDLE hFile = CreateFile(
            wcPath,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (INVALID_HANDLE_VALUE == hFile)
            throw SongError{};
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
            throw SongError{};

#pragma endregion

#pragma region recherche_du_riff_dans_audio

        DWORD dwChunkSize;
        DWORD dwChunkPosition;
        //check the file type, should be fourccWAVE or 'XWMA'
        FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
        DWORD filetype;
        ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
        if (filetype != fourccWAVE)
            throw SongError{};


#pragma endregion

        FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
        ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

        //fill out the audio data buffer with the contents of the fourccDATA chunk
        FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
        BYTE* pDataBuffer = new BYTE[dwChunkSize];
        ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

        buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
        buffer.pAudioData = pDataBuffer;  //buffer containing audio data
        buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

        //etape 3
        IXAudio2SourceVoice* voice;
        if (FAILED(hr = mpXAudio2->CreateSourceVoice(&voice, (WAVEFORMATEX*)&wfx)))
            throw SongError{};
        mSongMap[voiceName] = voice;
        //4
        if (FAILED(hr = mSongMap[voiceName]->SubmitSourceBuffer(&buffer)))
            throw SongError{};
    }

#pragma region playMoving
    void SoundController::playMoving()
    {
        mSongMap["bikeMoving"]->Start();
        mSongMap["bikeMoving"]->SetVolume(0.75);
    }

    void SoundController::stopMoving()
    {
        mSongMap["bikeMoving"]->Stop();
    }

    void SoundController::exitLoopMoving()
    {
        mSongMap["bikeMoving"]->ExitLoop();
    }
#pragma endregion

#pragma region playIdle
    void SoundController::playIdle()
    {
        mSongMap["bikeIdle"]->Start();
    }

    void SoundController::stopIdle()
    {
        mSongMap["bikeIdle"]->Stop();
    }

    void SoundController::exitLoopIdle()
    {
        mSongMap["bikeIdle"]->ExitLoop();
    }
#pragma endregion

#pragma region playRaceSoundtrack
    void SoundController::playRaceSoundtrack()
    {
        mSongMap["raceSoundtrack"]->Start();
    }

    void SoundController::stopRaceSoundtrack()
    {
        mSongMap["raceSoundtrack"]->Stop();
    }

    void SoundController::exitLoopRaceSoundtrack()
    {
        mSongMap["raceSoundtrack"]->ExitLoop();
    }
#pragma endregion

#pragma region playMenuSoundtrack
    void SoundController::playMenuSoundtrack()
    {
        mSongMap["mainMenuSoundtrack"]->Start();
    }

    void SoundController::stopMenuSoundtrack()
    {
        mSongMap["mainMenuSoundtrack"]->Stop();
    }

    void SoundController::exitLoopMenuSoundtrack()
    {
        mSongMap["mainMenuSoundtrack"]->ExitLoop();
    }
#pragma endregion

#pragma region fall
    void SoundController::playFall()
    {
        mSongMap["fall"]->Start();
    }

    void SoundController::stopFall()
    {
        mSongMap["fall"]->Stop();
    }

    void SoundController::exitLoopFall()
    {
        mSongMap["fall"]->ExitLoop();
    }
#pragma endregion

#pragma region death
    void SoundController::playDeath()
    {
        mSongMap["death"]->Start();
    }

    void SoundController::stopDeath()
    {
        mSongMap["death"]->Stop();
    }

    void SoundController::exitLoopDeath()
    {
        mSongMap["death"]->ExitLoop();
    }
#pragma endregion

#pragma region gorilla
    void SoundController::playMonster()
    {
        mSongMap["gorilla"]->Start();
    }

    void SoundController::stopMonster()
    {
        mSongMap["gorilla"]->Stop();
    }

    void SoundController::exitLoopMonster()
    {
        mSongMap["gorilla"]->ExitLoop();
    }
#pragma endregion

    void SoundController::release()
    {
        for (auto& pair : mSongMap) {
            if (pair.second)
                delete pair.second;
        }

    }

    void SoundController::changeVolume(float volume) {
        std::map<std::string, IXAudio2SourceVoice*>::iterator it;
        for (it = mSongMap.begin(); it != mSongMap.end(); it++) {
            it->second->SetVolume(volume);
            if (it->first == "bikeMoving") it->second->SetVolume(volume * 0.75f);
        }
    }

}