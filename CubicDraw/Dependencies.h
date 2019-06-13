#pragma once

// target Win7 or later versions
#define _WIN32_WINNT 0x0601

// disable default win configurations and objects
/*
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define STRICT
*/
#define STRICT

// include resource files
#include "resource.h"

// include the system header files
#include <Windows.h>
#include <sdkddkver.h>
#include <minwindef.h>
#include <assert.h>
#include <Wininet.h>
#include <wincodec.h>
#include <ShlObj.h>
#include <stdio.h>
#include <sal.h>
#include <wrl.h>

#include <exception>
#include <algorithm>
#include <iostream>
#include <optional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <memory>
#include <chrono>
#include <vector>
#include <random>
#include <queue>
#include <ctime>
#include <list>
#include <map>

// include intrinsics to accelerate
// SSE3 or early SIMD version, adapted to Pentium 4 or later CPU version
#include <mmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>

// choose directx 11 platform
#define __USE_DIRECTX11_SDK__

// DirectX 9 SDK include and library files
#ifdef __USE_DIRECTX9_SDK__

// include the DirectX9 auxiliary files
#include "..//..//Dependencies//Include//d3d9.h"
#include "..//..//Dependencies//Include//d3dx9.h"
#include "..//..//Dependencies//Include//dinput.h"
#include "..//..//Dependencies//Include//XInput.h"
#include "..//..//Dependencies//Include//dsound.h"

// load the DirectX9 auxiliary statc-linked libraries

#if defined (_WIN64)
#pragma comment(lib, "..//..//Dependencies//Lib//x64//dxguid.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x64//d3d9.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x64//d3dx9.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x64//dinput8.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x64//xinput.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x64//dsound.lib")
#elif defined (_WIN32)
#pragma comment(lib, "..//..//Dependencies//Lib//x86//dxguid.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x86//d3d9.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x86//d3dx9.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x86//dinput8.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x86//xinput.lib")
#pragma comment(lib, "..//..//Dependencies//Lib//x86//dsound.lib")
#endif

#endif

// compiled resources
#include "resource.h"

// DirectX 11 SDK include and library files
#ifdef __USE_DIRECTX11_SDK__

// include the DirectX11 auxiliary files
#include <d3d11.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Dependencies//Include//D3DX11.h"

// include libraries of DirectX11
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")

#if defined (_WIN64)
#pragma comment(lib, "Dependencies//Lib//x64//d3dx11.lib")
#elif defined (_WIN32)
#pragma comment(lib, "Dependencies//Lib//x86//d3dx11.lib")
#endif

#endif

// load the system static-linked libraries
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Kernel32.lib")


#define DISPLAY_SCREENWIDTH 1100
#define DISPLAY_SCREENHEIGHT 800