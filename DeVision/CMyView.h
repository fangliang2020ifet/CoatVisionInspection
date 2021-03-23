#pragma once

#include <vector>
#include "CImageProcessing.h"

// CMyView view
class CDialogView;

class CMyView : public CScrollView
{
	//DECLARE_DYNCREATE(CMyView)
	friend class CDialogView;
protected:
	CMyView();// protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyView)
	virtual ~CMyView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	void UpdateScreen(CFont &mfont, float display_range);
	void Redraw();

	int wnd_width;
	int wnd_height;
	int wnd_scroll_scale_size;                     //窗口滚动范围
	unsigned m_unDisplayRangeIndex;

	std::vector<DeffectInfo> *m_pvecDFT;
	COLORREF m_acolor[4];
	bool m_bFlagShow[4];

private:
	float scale_x = 0.0f;
	float scale_y = 0.0f;

	//float m_current_position = 0.0f;   //当前检测位置，单位: 米
	float m_previous_position = 0.0f;  //前次刷新位置
	float m_display_range = 0.0f;
	CFont *m_font;
	void AddFlag(CDC &mDC, int test);
	void CreateFlag(CDC &mDC, int x, int y, int kind);

public:
	virtual void OnDraw(CDC* pDC);// overridden to draw this view
	virtual void OnInitialUpdate();// first time after construct

	DECLARE_MESSAGE_MAP()

	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
