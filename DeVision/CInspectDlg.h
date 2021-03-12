#pragma once

#include <string>
#include <vector>
//#include "CImageProcess.h"
#include "CProductInfo.h"
#include "SapClassBasic.h"
#include "SapClassGui.h"

// CInspectDlg 对话框

class CInspectDlg : public CDialogEx, public CImageExWndEventHandler, public SapManager
{
	DECLARE_DYNAMIC(CInspectDlg)

public:
	CInspectDlg(CWnd* pParent = nullptr);   // 标准构造函数

	virtual ~CInspectDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INSPECT };
#endif

public:
	HWND hMainWnd;                           //主窗口句柄

	CImageExWnd		m_ImageWnd[4];

	CFont m_font;
	BOOL m_bDeffectDisplay[8];
	COLORREF m_color1;
	COLORREF m_color2;
	COLORREF m_color3;
	COLORREF m_color4;
	COLORREF m_color5;
	COLORREF m_color6;
	COLORREF m_color7;
	COLORREF m_color8;
	void RecordWarning(const std::wstring& str);   //记录报警信息
	void RecordWarning(int test, CString cstr);
	void RecordLogList(const std::wstring& str);   //记录日志
	void RecordLogList(int test, CString cstr);
	void UpdateDFTinformation(size_t total_num, int great_dft_num, float longth);

private:
	HICON m_hChangeIcon;
	HICON m_hChangeXIcon;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_btn_changeinfo;
	CListBox m_listLog;
	CListBox m_listWarning;
	CEdit m_eNumber;
	CEdit m_eWidth;
	CEdit m_eModel;
	CEdit m_eOperator;
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonInfoChange();
	afx_msg void OnBnClickedMfccolorbutton1();
	afx_msg void OnBnClickedMfccolorbutton2();
	afx_msg void OnBnClickedMfccolorbutton3();
	afx_msg void OnBnClickedMfccolorbutton4();
	afx_msg void OnBnClickedMfccolorbutton5();
	afx_msg void OnBnClickedMfccolorbutton6();
	afx_msg void OnBnClickedMfccolorbutton7();
	afx_msg void OnBnClickedMfccolorbutton8();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnBnClickedCheck5();
	afx_msg void OnBnClickedCheck6();
	afx_msg void OnBnClickedCheck7();
	afx_msg void OnBnClickedCheck8();
};
