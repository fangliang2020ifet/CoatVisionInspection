// CLoad.cpp: 实现文件
//
//启动界面加载位图

#include "stdafx.h"
#include "DeVision.h"
#include "CLoad.h"


// CLoad

IMPLEMENT_DYNAMIC(CLoad, CWnd)

CLoad::CLoad()
{

}

CLoad::~CLoad()
{
}


BEGIN_MESSAGE_MAP(CLoad, CWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()



// CLoad 消息处理程序


BOOL CLoad::Create()
{
	m_bitmap.LoadBitmapW(IDB_BITMAP_LOADDING);
	BITMAP bitmap;
	m_bitmap.GetBitmap(&bitmap);
	int x = (::GetSystemMetrics(SM_CXSCREEN) - bitmap.bmWidth) / 3;
	int y = (::GetSystemMetrics(SM_CXSCREEN) - bitmap.bmHeight) / 3;
	CRect rect(x, y, x + bitmap.bmWidth, y + bitmap.bmHeight);
	CreateEx(0, AfxRegisterWndClass(0), L"", WS_POPUP | WS_VISIBLE | WS_BORDER, rect, NULL, 0);

	return Create();
}

BOOL CLoad::Create(UINT nBitmapID)
{
	// TODO: 在此添加专用代码和/或调用基类
	m_bitmap.LoadBitmapW(nBitmapID);
	BITMAP bitmap;
	m_bitmap.GetBitmap(&bitmap);
	int x = (::GetSystemMetrics(SM_CXSCREEN) - bitmap.bmWidth) / 2;
	int y = (::GetSystemMetrics(SM_CYSCREEN) - bitmap.bmHeight) / 2;
	CRect rect(x, y, x + bitmap.bmWidth, y + bitmap.bmHeight);
	CreateEx(0, AfxRegisterWndClass(0), L"", WS_POPUP | WS_VISIBLE | WS_BORDER, rect, NULL, 0);

	return TRUE;
}


//BOOL CLoad::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	// TODO: 在此添加专用代码和/或调用基类
//
//	return CWnd::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpParam);
//}


void CLoad::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CWnd::OnPaint()
	BITMAP bitmap;
	m_bitmap.GetBitmap(&bitmap);
	CDC dcComp;
	dcComp.CreateCompatibleDC(&dc);
	dcComp.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, &dcComp, 0, 0, SRCCOPY);

}


void CLoad::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


void CLoad::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CWnd::OnClose();
}

