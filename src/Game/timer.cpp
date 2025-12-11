/**
 * 这个文件是游戏的定时器头文件
 * 除非你想要自己使用新的计时器，否则不需要改这个文件
 */

#include "stdafx.h"

#include "config.h"
#include "timer.h"

#include "info.h"
#include "core.h"

#pragma region TQTIMER

static HANDLE hTimer = NULL;
static HANDLE hTimerQueue = NULL;

static VOID CALLBACK OnTQTimer(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
    _CRT_UNUSED(TimerOrWaitFired);
    PostMessage((HWND)lpParam, WM_TIMER, MAIN_TIMER_ID, 0);
}

void InitTQTimer(HWND hWnd)
{
    hTimerQueue = CreateTimerQueue();
    if (NULL == hTimerQueue)
    {
        Log(2, "CreateTimerQueue failed (%d)\n");
        return;
    }

    if (!CreateTimerQueueTimer(&hTimer, hTimerQueue,
                               (WAITORTIMERCALLBACK)OnTQTimer, (PVOID)hWnd, 0, (DWORD)(1000 / FPS), // timeout只接受整数
                               WT_EXECUTEINTIMERTHREAD | WT_EXECUTELONGFUNCTION))
    {
        Log(2, "CreateTimerQueueTimer failed (%d)\n");
        return;
    }
}

void DeleteTQTimer()
{
    if (!DeleteTimerQueue(hTimerQueue))
    {
        Log(2, "DeleteTimerQueue failed (%d)\n");
        return;
    }
}

#pragma endregion

#pragma region MMTIMER

static UINT mmTimerID;
static UINT wTimerRes;

static void CALLBACK OnMMTimer(UINT wTimerID, UINT msg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    _CRT_UNUSED(wTimerID);
    _CRT_UNUSED(msg);
    _CRT_UNUSED(dw1);
    _CRT_UNUSED(dw2);
    PostMessage((HWND)dwUser, WM_TIMER, MAIN_TIMER_ID, 0);
}

void InitMMTimer(HWND hWnd)
{
    TIMECAPS tc;
    timeGetDevCaps(&tc, sizeof(TIMECAPS));

    const UINT target_resolution = 1000 / FPS;
    wTimerRes = min(max(tc.wPeriodMin, target_resolution), tc.wPeriodMax);
    timeBeginPeriod(wTimerRes);

    mmTimerID = timeSetEvent(
        wTimerRes, // delay
        wTimerRes, // resolution
        (LPTIMECALLBACK)OnMMTimer,
        (DWORD_PTR)hWnd,
        TIME_PERIODIC);
}

void DeleteMMTimer()
{
    timeKillEvent(mmTimerID);
    timeEndPeriod(wTimerRes);
}

#pragma endregion
