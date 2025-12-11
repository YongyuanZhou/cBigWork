#include "stdafx.h"
#include "audio.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// filename 应为文件路径（相对于可执行或绝对路径），例如: TEXT("sounds\\shoot.wav")
void PlaySoundEffect(LPCWSTR filename, bool loop)
{
    if (!filename) return;
    UINT flags = SND_FILENAME | SND_ASYNC | SND_NODEFAULT;
    if (loop) flags |= SND_LOOP;
    PlaySoundW(filename, NULL, flags);
}

// 停止所有使用 PlaySound 播放的声音（包括循环声）
void StopAllSounds()
{
    PlaySoundW(NULL, NULL, 0);
}