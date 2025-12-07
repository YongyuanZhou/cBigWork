/**
 * 这个文件是预编译头文件，用来加速编译过程
 * 把库文件和不会经常修改的头文件放在这个文件中
 */

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "comctl32.lib") // 解决报错：无法解析的外部符号 __imp_InitCommonControlsEx
#pragma comment(lib, "Msimg32.lib")  // 解决报错：无法解析的外部符号 __imp_TransparentBlt
#pragma comment(lib, "Winmm.lib") // 解决报错：无法解析的外部符号 __imp_timeSetEvent
// C运行库
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
// C++运行库
#include <string>
#include <vector>
#include <deque>
#include <set>
#include <algorithm>
#include <numeric>
#include <random>
// 配置与系统信息与工具函数
#include "config.h"
#include "type.h"
#include "info.h"
#include "util.h"
