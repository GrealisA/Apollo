#include "Sound.h"

//Reference: https://www.codeguru.com/cpp/g-m/directx/directshow/article.php/c19079/Simple-C-MP3-Player-Class.htm
Sound::Sound(string filename, float volume, bool loop)
{
	_pGraph = NULL;
	_pMediaControl = NULL;
	_pEvent = NULL;
	_pAudio = NULL;
	_pMediaSeeker = NULL;
	_ready = false;
	_filePath = filename;
	changeSound(this, _filePath, volume, false, loop);
}

Sound::~Sound()
{
	Stop();
	Cleanup();
}

bool Sound::Load(string filename) {
	_stopped = false;
	_loop = false;
	_duration = 0;
	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	_filePath = NPath + filename;
	wstring szFile(_filePath.begin(), _filePath.end());
	Cleanup();
	if (SUCCEEDED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&this->_pGraph)))
	{
		_pGraph->QueryInterface(IID_IMediaControl, (void **)&_pMediaControl);
		_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&_pEvent);
		_pGraph->QueryInterface(IID_IBasicAudio, (void**)&_pAudio);
		_pGraph->QueryInterface(IID_IMediaSeeking, (void**)&_pMediaSeeker);

		HRESULT hr = _pGraph->RenderFile(szFile.c_str(), NULL);
		if (SUCCEEDED(hr))
		{
			_ready = true;
			if (_pMediaSeeker)
			{
				_pMediaSeeker->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
				_pMediaSeeker->GetDuration(&_duration); // returns 10,000,000 for a second.
				_duration = _duration/10000;
			}
		}
	}
	return _ready;
}

void Sound::Cleanup()
{
	if(_pGraph)
	{
		_pGraph->Release();
		_pGraph = NULL;
	}

	if(_pMediaControl)
	{
		_pMediaControl->Release();
		_pMediaControl = NULL;
	}

	if(_pEvent)
	{
		_pEvent->Release();
		_pEvent = NULL;
	}

	if(_pAudio)
	{
		_pAudio->Release();
		_pAudio = NULL;
	}

	if(_pMediaSeeker)
	{
		_pMediaSeeker->Release();
		_pMediaSeeker = NULL;
	}
	_ready = false;
}

bool Sound::Play()
{
	if (_ready&&_pMediaControl)
	{
		HRESULT hr = _pMediaControl->Run();
		_stopped = false;
		return SUCCEEDED(hr);
	}
	return false;
}

bool Sound::Pause()
{
	if (_ready&&_pMediaControl)
	{
		HRESULT hr = _pMediaControl->Pause();
		return SUCCEEDED(hr);
	}
	return false;
}

bool Sound::Stop()
{
	if (_ready&&_pMediaControl)
	{
		HRESULT hr = _pMediaControl->Stop();
		_stopped = true;
		return SUCCEEDED(hr);
	}
	
	return false;
}

bool Sound::WaitForCompletion(long msTimeout)
{
	long* EvCode = new long();
	if (_ready&&_pEvent)
	{
		HRESULT hr = _pEvent->WaitForCompletion(msTimeout, EvCode);
		return *EvCode > 0;
	}

	return false;
}

bool Sound::SetVolume(long vol)
{
	if (_ready&&_pAudio)
	{
		HRESULT hr = _pAudio->put_Volume(vol);
		return SUCCEEDED(hr);
	}
	return false;
}

long Sound::GetVolume()
{
	if (_ready&&_pAudio)
	{
		long vol = -1;
		HRESULT hr = _pAudio->get_Volume(&vol);

		if(SUCCEEDED(hr))
			return vol;
	}

	return -1;
}

__int64 Sound::GetDuration()
{
	return _duration;
}

__int64 Sound::GetCurrentPosition()
{
	if (_ready&&_pMediaSeeker)
	{
		__int64 curpos = -1;
		HRESULT hr = _pMediaSeeker->GetCurrentPosition(&curpos);

		if(SUCCEEDED(hr))
			return curpos;
	}

	return -1;
}

bool Sound::SetPositions(__int64 pCurrent, __int64 pStop, bool bAbsolutePositioning)
{
	if (_ready&&_pMediaSeeker)
	{
		DWORD flags = 0;
		if(bAbsolutePositioning)
			flags = AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame;
		else
			flags = AM_SEEKING_RelativePositioning | AM_SEEKING_SeekToKeyFrame;

		HRESULT hr = _pMediaSeeker->SetPositions(&pCurrent, flags, &pStop, flags);
		if(SUCCEEDED(hr))
			return true;
	}

	return false;
}
//End Reference

void Sound::MusicLoop(Sound* sfx) {
	do {		
		//Restarts the sound in the event of it finishing and loop being selected
		if (sfx->GetCurrentPosition() >= (sfx->_duration * 10000) - 190000) {
			sfx->SetPositions(0, sfx->_duration * 10000, true);
		}			
		Sleep(10);
	} while (sfx->_loop && !sfx->_stopped);
}

void Sound::changeSound(Sound* sfx, string newMusicFile, float volume, bool fade, bool loop) {
	//Fades the previous sound out then plays the new one
	if (sfx != nullptr && sfx->GetCurrentPosition() != -1 && fade) {
		for (int i = sfx->GetVolume(); i > -10000; i -= 10) {
			sfx->SetVolume(i);
			Sleep(1);
		}
	}
	sfx->Stop();
	sfx->Load(newMusicFile);	
	sfx->_loop = loop;
	if (sfx->_ready) std::cout << "Loaded " << newMusicFile << endl;
	else std::cout << "ERROR LOADING " << newMusicFile << endl;
	if (sfx->Play()) std::cout << "PLAYING " << newMusicFile << endl;
	else std::cout << "ERROR PLAYING " << newMusicFile << endl;
	if (fade) {
		for (int i = -10000; i < volume; i += 5) {
			sfx->SetVolume(i);
			Sleep(1);
		}
	}
	else sfx->SetVolume(long(volume));
	thread sfxThread;
	sfxThread = thread(Sound::MusicLoop, sfx);
	sfxThread.detach();
}