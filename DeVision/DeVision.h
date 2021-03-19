
// DeVision.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#include "CLoad.h"

// CDeVisionApp:
// 有关此类的实现，请参阅 DeVision.cpp
//

// 自定义消息
//#define WM_LOGGING_MSG WM_USER+0x01001
//#define WM_WARNING_MSG WM_USER+0x02001
#define WM_LOGGING_MSG     (WM_USER + 1)
#define WM_WARNING_MSG     (WM_USER + 2)
#define WM_UPDATE_CONTROLS (WM_USER + 3)
#define WM_UPDATE_HISTORY  (WM_USER + 4)
#define WM_UPDATE_MAINWND  (WM_USER + 5)
#define WM_SWITCHROLL      (WM_USER + 6)

#define STRINGLENGTH 64
// 字节宽度
#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4);
#define IMAGE_WIDTH 8192
#define IMAGE_HEIGHT 8192
// 纵向的精度, 单位： mm
#define VERTICAL_PRECISION 0.035f
// 横向的精度，单位： mm
#define HORIZON_PRECISION 0.05f

class CDeVisionApp : public CWinApp
{
public:
	CDeVisionApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CDeVisionApp theApp;
