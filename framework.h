#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容

// Windows 头文件
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <objbase.h> 

// Direct2D 和 DirectWrite
#include <dwrite.h>
#include <d2d1.h>
#include <d2d1_3.h>
#include <d2d1svg.h>
#include <wrl.h>
#include <comdef.h>

// 输入法
#include <imm.h>
#include <dwmapi.h>
#include <shlwapi.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <string>
#include <algorithm>
#include <vector>
#include <functional>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include <fstream>