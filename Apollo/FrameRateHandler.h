#pragma once

#include<Windows.h>
#include<iostream>

using namespace std;
//Reference: https://www.youtube.com/watch?v=jqT96a7fB7E
class FrameRateHandler {
public:
	FrameRateHandler(int frameRate) {
		LARGE_INTEGER time;
		QueryPerformanceFrequency(&time);
		_frequency = time.QuadPart;
		_frameRate = frameRate;
		Reset();
	}

	void Reset() {
		LARGE_INTEGER time;
		if(!QueryPerformanceCounter(&time)) cout<<"Error loading Frame Handler\n";
		_startTime = time.QuadPart;
		cout << _startTime << endl;
		_currentCallToUpdate = time.QuadPart;
		_lastCallToUpdate = time.QuadPart;
	}

	void Update() {
		_lastCallToUpdate = _currentCallToUpdate;
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);
		_currentCallToUpdate = time.QuadPart;
	}

	double GetTotalTime() {
		double d = (double)_currentCallToUpdate - (double)_startTime;
		return d / (double)_frequency;
	}

	double GetDeltaTime() {
		double deltaTime = (double)_currentCallToUpdate - (double)_lastCallToUpdate;
		if (deltaTime / (double)_frequency < (1 / _frameRate)) Sleep((DWORD)((1 / _frameRate) - (deltaTime / _frequency)) * 1000);
		return deltaTime / (double)_frequency;
	}

	int& getFrameRate() {
		return _frameRate;
	}

private:
	long long _startTime;
	long long _lastCallToUpdate;
	long long _currentCallToUpdate;
	long long _frequency;
	int _frameRate;
};
//End Reference