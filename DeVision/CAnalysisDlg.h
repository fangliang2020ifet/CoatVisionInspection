#pragma once

#include "BarChart.h"


// CAnalysisDlg 对话框

class CAnalysisDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAnalysisDlg)

public:
	CAnalysisDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAnalysisDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ANALYSIS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	HWND hMainWnd;                           //主窗口句柄
	std::string m_strSavePath;

	enum { IDD = IDD_DIALOG_ANALYSIS };
	BOOL m_bShowGrid;
	BOOL m_bShowText;
	BOOL m_bShowLable;
	BOOL m_bShowTip;
	BOOL m_bShowPercent;
	double m_dDftNumber1;
	double m_dDftNumber2;
	double m_dDftNumber3;
	double m_dDftNumber4;

	void ClearAll();
	void UpdateChartValue();

protected:

	HICON m_hIcon;
	CBarChart m_chart;
	CBrush m_brushBK;

	void CreateCustomBarChart();


private:
	//COpenGLControl m_oglWindow;
	HBITMAP GetSrcBit(HDC hDC, LPRECT rEct);
	bool SaveBitmapToFile(HBITMAP hBitmap, const char* name);

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedCheckGrid();
	afx_msg void OnBnClickedCheckText();
	afx_msg void OnBnClickedCheckLabel();
	afx_msg void OnBnClickedCheckTooltips();
	afx_msg void OnBnClickedCheckPercent();
public:
	afx_msg void OnBnClickedButtonAnalysisSave();
};
