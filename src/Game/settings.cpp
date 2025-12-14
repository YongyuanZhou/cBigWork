#include "stdafx.h"
#include "config.h"
#include <fstream>

int g_masterVolume = 80; // default
int g_difficultyIndex = 1; // default

void LoadSettings()
{
    std::ifstream ifs(SETTINGS_DATA_FILE);
    if (!ifs.is_open()) return;
    int vol = g_masterVolume;
    int diff = g_difficultyIndex;
    ifs >> vol;
    ifs >> diff;
    if (vol >= 0 && vol <= 100) g_masterVolume = vol;
    if (diff >= 0 && diff <= 2) g_difficultyIndex = diff;
    ifs.close();
    ApplyMasterVolume();
}

void SaveSettings()
{
    std::ofstream ofs(SETTINGS_DATA_FILE);
    if (!ofs.is_open()) return;
    ofs << g_masterVolume << std::endl;
    ofs << g_difficultyIndex << std::endl;
    ofs.close();
}

// Simple master volume apply using PlaySound cannot set global volume, but we can store for audio module
void ApplyMasterVolume()
{
    // No-op here; audio playback functions may read g_masterVolume when playing sounds
}
