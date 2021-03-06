#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC

#define STRICT									// 侏チェックを��畜に佩なう
#define WIN32_LEAN_AND_MEAN	// ヘッダ�`からあまり聞われない�v方を福く
#define WINVER					0x501	// Windows XP參週
#define _WIN32_WINNT		0x501 

#define D3D_DEBUG_INFO				// Direct3Dデバッグ秤�鵑林��浸�

#define SAFE_RELEASE(x)  { if(x) { (x)->Release(); (x)=NULL; } }

#define	 USE_LIVE2D						1	// Live2Dの聞喘フラグ�┘妊丱奪尉達�


#pragma warning(disable:4244)
#pragma warning(disable:4018)
#pragma warning(disable:4996)

#include <stdlib.h>
#include <crtdbg.h>
#include <tchar.h>
#include<fstream>
#include<iostream>
#include <crtdbg.h>
#include <windows.h>
#include <Windowsx.h>
#include <mmsystem.h> 
#include<Tlhelp32.h>

//DX
#include <d3dx9.h>
#include <dsound.h>
#include <dxerr.h>

//others
#include "resource.h"
#include "uImageDC.h"
#include "Included.hpp"


// Live2D
#include "Live2D.h"
#include "util/UtSystem.h"
#include "Live2DModelD3D.h"
#include "LAppDefine.h"

// Live2D Sample
#include "LAppRenderer.h"
#include "LAppLive2DManager.h"
#include "L2DViewMatrix.h"
#include "LAppModel.h"
#include "LAppSoundMananger.h"
#include "LAppHookMananger.h"
#include "LAppFontMananger.h"

using namespace std;