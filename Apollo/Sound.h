#include <string>
#include <strmif.h>
#include <control.h>
#include <iostream>
#include <uuids.h>
#include <thread>

#pragma comment(lib, "strmiids.lib")

using namespace std;

class Sound
{
public:
	//Reference: https://www.codeguru.com/cpp/g-m/directx/directshow/article.php/c19079/Simple-C-MP3-Player-Class.htm
	Sound(string filename, float volume, bool loop);
	~Sound();

	void Cleanup();

	bool Load(string filename);
	bool Play();
	bool Pause();
	bool Stop();
	bool WaitForCompletion(long msTimeout);
	// -10000 is lowest volume and 0 is highest volume, positive value > 0 will fail
	bool SetVolume(long vol);
	
	// -10000 is lowest volume and 0 is highest volume
	long GetVolume();
	
	__int64 GetDuration();
	
	__int64 GetCurrentPosition();

	// Seek to position
	bool SetPositions(__int64 pCurrent, __int64 pStop, bool bAbsolutePositioning);	
	//End Reference

	static void MusicLoop(Sound* sfx);
	static void changeSound(Sound* sfx, string newMusicFile, float volume, bool fade, bool loop);

	//Reference: https://www.codeguru.com/cpp/g-m/directx/directshow/article.php/c19079/Simple-C-MP3-Player-Class.html
private:
	IGraphBuilder* _pGraph;
	IMediaControl* _pMediaControl;
	IMediaEventEx* _pEvent;
	IBasicAudio* _pAudio;
	IMediaSeeking* _pMediaSeeker;
	bool _ready;
	__int64 _duration;
	string _filePath;
	bool _loop;
	bool _stopped;
	//End Reference
};


