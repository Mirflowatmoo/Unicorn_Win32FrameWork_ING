#include "uaudio.h"
#include "unicorn.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

//std::string GetPlayCommandStr(const std::string filename) {
//	//加上路径的转义字符可以带空格的路径
//	
//	return commandStr;
//}
//内置一个音频文件管理器
//定义Open接口，每次open将音频加入管理器，close从管理器中删除

std::set<std::string> UAudio::audioSet_;

bool UAudio::LoadAudioFromFile(const std::string fileName, std::string asName) {

	if ("" == asName) {
		size_t right = fileName.find_last_of('.');
		size_t left = fileName.find_last_of('\\');

		if (std::string::npos == left) {
			left = fileName.find_last_of('/');
			if (std::string::npos == left) {
				left = -1;
			}
		}

		if (std::string::npos == right) {
			return false;
		}

		asName = fileName.substr(left + 1, right - left - 1);
	}
	
	std::string commandStr = "open \"" + fileName + "\" alias " + asName;

	MCIERROR mciErr = 0;
	mciErr = mciSendStringA(commandStr.c_str(), NULL, 0, NULL);
	if (0 == mciErr) {
		audioSet_.insert(asName);
		return true;
	}
	
	return false;
}

bool UAudio::NameCheck(const std::string name) {
	if (audioSet_.end() == audioSet_.find(name)) {
		return false;
	}

	return true;
}

void UAudio::PlayAudio(const std::string name) {
	if (!NameCheck(name)) {
		return;
	}

	std::string cmd = "play " + name + " from 0";
	mciSendStringA(cmd.c_str(), NULL, 0, NULL);
}

void UAudio::PauseAudio(const std::string name) {
	if (!NameCheck(name)) {
		return;
	}

	std::string cmd = "pause " + name;
	mciSendStringA(cmd.c_str(), NULL, 0, NULL);
}

void UAudio::ResumeAudio(const std::string name) {
	if (!NameCheck(name)) {
		return;
	}

	std::string cmd = "resume " + name;
	mciSendStringA(cmd.c_str(), NULL, 0, NULL);
}

void UAudio::CloseAudio(const std::string name) {
	if (!NameCheck(name)) {
		return;
	}

	std::string cmd = "close " + name;
	mciSendStringA(cmd.c_str(), NULL, 0, NULL);
	audioSet_.erase(name);
}