// CMyView.cpp: 实现文件
//

//#include "pch.h"
#include "stdafx.h"
#include "DeVision.h"
//#include "DeVisionDlg.h"
#include "CMyView.h"

IMPLEMENT_DYNCREATE(CMyView, CScrollView)

CMyView::CMyView()
{
	m_acolor[0] = RGB(255, 35, 15);
	m_acolor[1] = RGB(25, 255, 35);
	m_acolor[2] = RGB(35, 55, 225);
	m_acolor[3] = RGB(255, 255, 0);
	m_acolor[4] = RGB(55, 35, 55);
	m_acolor[5] = RGB(255, 255, 255);
	m_acolor[6] = RGB(155, 35, 155);
	m_acolor[7] = RGB(155, 155, 85);

	for (INT i = 0; i < 8; i++) {m_bFlagShow[i] = true;}

}

CMyView::~CMyView()
{
}

BEGIN_MESSAGE_MAP(CMyView, CScrollView)

	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CMyView 绘图
void CMyView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类
	m_nMapMode = MM_TEXT;   //映射模式,解决 scrollToPosition错误

	CScrollView::OnPrepareDC(pDC, pInfo);
}

void CMyView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO:  计算此视图的合计大小
	sizeTotal.cx = (LONG)(wnd_width * 0.9);
	sizeTotal.cy = wnd_height * wnd_scroll_scale_size;
	SetScrollSizes(MM_TEXT, sizeTotal);

	//垂直滚动范围为：3米
	//此处的scale 的单位为: 米/像素
	scale_x = (float)sizeTotal.cx / (IMAGE_WIDTH * 4 * HORIZON_PRECISION);
	//scale_y = (float)DISPLAY_SCOPE / sizeTotal.cy;
}

BOOL CMyView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//return CScrollView::OnEraseBkgnd(pDC);
	//直接返回 true， 解决窗口闪烁问题
	return TRUE;
}

void CMyView::OnDraw(CDC* pDC)
{
	//CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
	//CRect rc;
	//GetClientRect(&rc);
	//CPoint pt = GetScrollPosition();
	//rc.OffsetRect(pt);

	//POINT pt_scroll;
	//pt_scroll.x = 0;
	//pt_scroll.y = (int)(std::abs(wnd_height * wnd_scroll_scale_size - m_previous_position / scale_y));
	//ScrollToPosition(pt_scroll);

	CDC MemDC; // 定义一个内存显示设备对象
	CBitmap MemBitmap; // 定义一个位图对象
	//建立与屏幕显示兼容的内存显示设备
	MemDC.CreateCompatibleDC(pDC);
	//建立一个与屏幕显示兼容的位图，位图的大小可选用窗口客户区的大小
	MemBitmap.CreateCompatibleBitmap(pDC, wnd_width, wnd_height*wnd_scroll_scale_size);
	//将位图选入到内存显示设备中，只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
	MemDC.SelectObject(&MemBitmap);
	//CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
	//先用背景色将位图清除干净，否则是黑色。这里用的是白色作为背景
	MemDC.FillSolidRect(0, 0, wnd_width, wnd_height * wnd_scroll_scale_size, RGB(128, 128, 128));

	//绘图部分
	MemDC.SelectObject(m_font);
	//AddFlag(MemDC);
	AddFlag(MemDC, 0);

	//将内存中的图拷贝到屏幕上进行显示
	pDC->BitBlt(0, 0, wnd_width, wnd_height * wnd_scroll_scale_size, &MemDC, 0, 0, SRCCOPY);
	//绘图完成后的清理
	//MemDC.SelectObject(pOldBit);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}

// CMyView 诊断

#ifdef _DEBUG
void CMyView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG

// CMyView 消息处理程序

//Customer
void CMyView::UpdateScreen(CFont &mfont, float display_range)
{
	m_font = &mfont;
	m_display_range = display_range;
	scale_y = (float)display_range / (wnd_height * wnd_scroll_scale_size);

	//不自动滚屏的计算方式
	//scale_y = (float)(wnd_height * wnd_scroll_scale_size) / display_range;

	Invalidate();
}

void CMyView::CreateFlag(CDC &mDC, int x, int y, int kind)
{
	switch (kind)
	{
	case 0: {
		if (m_bFlagShow[0]) {
			mDC.SetBkColor(m_acolor[0]);
			mDC.TextOutW(x, y, _T("A"));
		}
		break;
	}
	case 1: {
		if (m_bFlagShow[1]) {
			mDC.SetBkColor(m_acolor[1]);
			mDC.TextOutW(x, y, _T("B"));
		}
		break;
	}
	case 2: {
		if (m_bFlagShow[2]) {
			mDC.SetBkColor(m_acolor[2]);
			mDC.TextOutW(x, y, _T("C"));
		}
		break;
	}
	case 3: {
		if (m_bFlagShow[3]) {
			mDC.SetBkColor(m_acolor[3]);
			mDC.TextOutW(x, y, _T("D"));
		}
		break;
	}
	case 4: {
		if (m_bFlagShow[4]) {
			mDC.SetBkColor(m_acolor[4]);
			mDC.TextOutW(x, y, _T("E"));
		}
		break;
	}
	case 5: {
		if (m_bFlagShow[5]) {
			mDC.SetBkColor(m_acolor[5]);
			mDC.TextOutW(x, y, _T("F"));
		}
		break;
	}
	case 6: {
		if (m_bFlagShow[6]) {
			mDC.SetBkColor(m_acolor[6]);
			mDC.TextOutW(x, y, _T("G"));
		}
		break;
	}
	case 7: {
		if (m_bFlagShow[7]) {
			mDC.SetBkColor(m_acolor[7]);
			mDC.TextOutW(x, y, _T("H"));
		}
		break;
	}
	default: {
		mDC.SetBkColor(RGB(0, 0, 0));
		break;
	}
	}
}

// 方法二
void CMyView::AddFlag(CDC &mDC, int test)
{
	DeffectInfo info;
	if (!m_pvecDFT->empty()) {
		int x_coord = 0, y_coord = 0;
		std::vector<DeffectInfo>::reverse_iterator rit = m_pvecDFT->rbegin();
		for (; rit != m_pvecDFT->rend(); ++rit) {
			info = *rit;
			x_coord = (int)(info.x * scale_x);
			y_coord = wnd_height * wnd_scroll_scale_size - (int)((info.y - m_previous_position) / scale_y);
			if (y_coord < 0)
				m_previous_position = info.y - m_display_range;
			if (y_coord > wnd_height * wnd_scroll_scale_size)
				break;

			CreateFlag(mDC, x_coord, y_coord, info.type);
		}
	}
}

void CMyView::Redraw()
{
	scale_y = 0.0f;
	m_display_range = 0.0f;
	m_previous_position = 0.0f;
	m_pvecDFT->clear();

}