#include "stdafx.h"
#include "config.h"
#include "settings.h"
#include <fstream>

int g_masterVolume = 80; //默认值
int g_difficultyIndex = 1; //默认值，中等难度
void LoadSettings()
{
	std::ifstream ifs(SETTINGS_DATA_FILE);//打开文件
    if (!ifs.is_open()) return;
    int vol = g_masterVolume;
    int diff = g_difficultyIndex;
	ifs >> vol;//读取音量
	ifs >> diff;//读取难度
	if (vol >= 0 && vol <= 100) g_masterVolume = vol;//确保音量在0-100之间
	if (diff >= 0 && diff <= 2) g_difficultyIndex = diff;//确保难度索引在0-2之间
    ifs.close();
	ApplyMasterVolume();//应用音量设置
}

void SaveSettings()
{
    std::ofstream ofs(SETTINGS_DATA_FILE);
    if (!ofs.is_open()) return;
    ofs << g_masterVolume << std::endl;
    ofs << g_difficultyIndex << std::endl;
    ofs.close();
}

void ApplyMasterVolume()
{


}

int GetMasterVolume() { return g_masterVolume; }
void SetMasterVolume(int volume) { if (volume < 0) volume = 0; if (volume > 100) volume = 100; g_masterVolume = volume; ApplyMasterVolume(); }

int GetDifficultyIndex() { return g_difficultyIndex; }
void SetDifficultyIndex(int index) { if (index < 0) index = 0; if (index > 2) index = 2; g_difficultyIndex = index; }

double GetDifficultyBaseInterval() { return DIFFICULTY_BASE_INTERVALS[g_difficultyIndex]; }
