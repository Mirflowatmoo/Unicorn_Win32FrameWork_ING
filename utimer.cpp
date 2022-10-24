#include "Utimer.h"
#include <iostream>
#include <string>
#include <Windows.h>

UTimer::UTimer() : secondsPerCount_(0.0), deltaTime_(-1.0), baseTime_(0), prevTime_(0), pausedTime_(0), currTime_(0), isStopped_(false) {
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);

	secondsPerCount_ = 1.0 / static_cast<double>(countsPerSec);
}

void UTimer::Tick() {
	if (isStopped_) {
		deltaTime_ = 0.0;
		return;
	}

	//本帧开始的时刻
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	currTime_ = currTime;

	//本帧与上一帧的时间差(单位:秒)
	deltaTime_ = (currTime_ - prevTime_) * secondsPerCount_;

	//本帧时间戳保存给prevTime_,为下一帧作准备
	prevTime_ = currTime_;

	//使时间差为负值。如果处理器处于节能模式，或者在计算两帧时间差的过程中切换到另一个处理器时
	//(即QueryPerformanceCounter函数两次在不同的处理器上调用时), currTime_可能为负
	if (deltaTime_ < 0.0) {
		deltaTime_ = 0.0;
	}
}

void UTimer::Reset() {
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	baseTime_ = currTime;
	prevTime_ = currTime;
	stopTime_ = 0;
	isStopped_ = false;
}

void UTimer::Stop() {
	if (!isStopped_) {
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		stopTime_ = currTime;
		isStopped_ = true;
	}
}

void UTimer::Start() {
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	//          |<------d----->|
	//----------*--------------*-----------------> t
	//       stopTime_      startTime  

	//从停止状态恢复计时
	if (isStopped_) {
		//累加停止时间
		pausedTime_ += (startTime - stopTime_);
		prevTime_ = startTime;
		stopTime_ = 0;
		isStopped_ = false;
	}
}

float UTimer::DeltaTime() const {
	return static_cast<float>(deltaTime_);
}

float UTimer::TotalTime() const {
	//如果处于停止状态，则忽略本次停止时刻至当前时刻的这段时间。此外，如果之前已经有过暂停的情况
	//那么也不应统计stopTime_ - baseTime_这段时间内的暂停时间
	if (isStopped_) {
		return static_cast<float>((stopTime_ - baseTime_ - pausedTime_) * secondsPerCount_);
	}
	else {
		return static_cast<float>((currTime_ - pausedTime_ - baseTime_) * secondsPerCount_);
	}
}

void UTimer::CalculateFramesPerSec(HWND hWnd) {
	static int frameCnt = 0;
	static float timeElapsed = TotalTime();

	frameCnt++;

	//             |<----1s---->|
	// ------------*------------*-----------
	//          timeElapsed   TotalTime 
	if ((TotalTime() - timeElapsed) >= 1.0f) {
		float fps = (float)frameCnt; //帧计数
		float mspf = 1000.0f / fps; //平均每帧用时(ms)

		std::string strFps(std::to_string(fps));
		std::string strMspf(std::to_string(mspf));

		std::string windowText = "FPS: " + strFps + " MSPF: " + strMspf;
		SetWindowTextA(hWnd, windowText.c_str());
		InvalidateRect(hWnd, NULL, TRUE);
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void TimerProc(unsigned int elapse, UTimerProc timerproc) {
	/*__int64 startTime = 0;
	__int64 currentTime = 0;
	__int64 elapse = static_cast<__int64>(elapseSec * 10000000); //将浮点秒数转换成整型十分之一微秒(百纳秒)数 */

	/*QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	do {
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	} while (currentTime - startTime < elapse);*/
	Sleep(elapse);
	timerproc();
}

void UTimer::SetTimer(unsigned int elapse, UTimerProc timerproc) {
	std::thread ttimer(TimerProc, elapse, timerproc);
	ttimer.detach();
}