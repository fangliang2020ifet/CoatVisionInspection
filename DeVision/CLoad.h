#pragma once


// CLoad

class CLoad : public CWnd
{
	DECLARE_DYNAMIC(CLoad)

public:
	CLoad();
	virtual ~CLoad();

protected:
	DECLARE_MESSAGE_MAP()
public:

	CBitmap m_bitmap;

	virtual BOOL Create(void);
	virtual BOOL Create(UINT nBitmapID);
//	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
};


