#pragma once

#include <vector>
#include "CImageProcess.h"

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
	void UpdateScreen(CFont &mfont, float display_range, float position);
	void Redraw();

	COLORREF red_color = RGB(255, 35, 15);             //红色
	COLORREF green_color = RGB(25, 255, 35);          //绿色
	COLORREF blue_color = RGB(35, 55, 225);            //蓝色
	COLORREF yellow_color = RGB(255, 255, 0);          //黄色
	std::vector<DefectType> m_vDefect;

private:
	float scale_x = 0.0f;
	float scale_y = 0.0f;

	float m_current_position = 0.0f;   //当前检测位置，单位: 米
	float m_previous_position = 0.0f;  //前次刷新位置
	float m_display_range = 0.0f;
	CFont *m_font;
	void AddFlag(CDC &mDC);
	void AddFlag(CDC &mDC, int test);
	void CreateFlag(CDC &mDC, int x, int y, int kind);

public:
	virtual void OnDraw(CDC* pDC);// overridden to draw this view
	virtual void OnInitialUpdate();// first time after construct

	DECLARE_MESSAGE_MAP()

	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
