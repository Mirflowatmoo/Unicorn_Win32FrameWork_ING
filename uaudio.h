#pragma once
//#include "unicorn.h"
#include <set>
#include <string>

class UAudio {

public:
	//°ó¶¨ÒôÆµÎÄ¼þ¼Ð
	//static void BindAudiosFolder(string dirPath);
	static bool LoadAudioFromFile(const std::string fileName, std::string asName = "");
	static void PlayAudio(const std::string name);
	static void PauseAudio(const std::string filename);
	static void ResumeAudio(const std::string filename);
	static void CloseAudio(const std::string filename);

protected:
	static std::set<std::string> audioSet_;

private:
	UAudio() = default;
	static bool NameCheck(const std::string name);
};

