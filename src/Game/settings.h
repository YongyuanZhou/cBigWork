#pragma once



// Initialize settings from disk
void LoadSettings();
// Save current settings to disk
void SaveSettings();
// Apply master volume to audio subsystem (implementation-dependent)
void ApplyMasterVolume();

// Getters / setters for external modules
int GetMasterVolume();
void SetMasterVolume(int volume);

int GetDifficultyIndex();
void SetDifficultyIndex(int index);

// Convenience: get difficulty base interval mapped from index
double GetDifficultyBaseInterval();
