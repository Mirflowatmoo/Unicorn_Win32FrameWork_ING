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

	//��֡��ʼ��ʱ��
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	currTime_ = currTime;

	//��֡����һ֡��ʱ���(��λ:��)
	deltaTime_ = (currTime_ - prevTime_) * secondsPerCount_;

	//��֡ʱ��������prevTime_,Ϊ��һ֡��׼��
	prevTime_ = currTime_;

	//ʹʱ���Ϊ��ֵ��������������ڽ���ģʽ�������ڼ�����֡ʱ���Ĺ������л�����һ��������ʱ
	//(��QueryPerformanceCounter���������ڲ�ͬ�Ĵ������ϵ���ʱ), currTime_����Ϊ��
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

	//��ֹͣ״̬�ָ���ʱ
	if (isStopped_) {
		//�ۼ�ֹͣʱ��
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
	//�������ֹͣ״̬������Ա���ֹͣʱ������ǰʱ�̵����ʱ�䡣���⣬���֮ǰ�Ѿ��й���ͣ�����
	//��ôҲ��Ӧͳ��stopTime_ - baseTime_���ʱ���ڵ���ͣʱ��
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
		float fps = (float)frameCnt; //֡����
		float mspf = 1000.0f / fps; //ƽ��ÿ֡��ʱ(ms)

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
	__int64 elapse = static_cast<__int64>(elapseSec * 10000000); //����������ת��������ʮ��֮һ΢��(������)�� */

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