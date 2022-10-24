#pragma once
#include <Windows.h>
#include <thread>

typedef void (*UTimerProc)();

class UTimer {
private:
	double secondsPerCount_;
	double deltaTime_;

	__int64 baseTime_;
	__int64 pausedTime_;
	__int64 stopTime_;
	__int64 prevTime_;
	__int64 currTime_;

	bool isStopped_;

public:
	UTimer();

	float TotalTime() const;
	float DeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

	void CalculateFramesPerSec(HWND hWnd);

	static void SetTimer(unsigned int elapse, UTimerProc timerproc);
};