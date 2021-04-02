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
	int* m_pnSystemState;                      //系统状态

	CImageExWnd		m_ImageWnd[4];
	bool m_bTableSaving;                      // 切卷：报表保存中
	CFont m_font;
	BOOL m_bDeffectDisplay[4];
	COLORREF m_color1;
	COLORREF m_color2;
	COLORREF m_color3;
	COLORREF m_color4;

	CString m_cstrBatch;                      //  保存的为下一卷的卷号，需要从control item获取当前卷号
	int     m_nBatchIndex;
	CString m_cstrName;
	CString m_cstrSchedule;
	CString m_cstrAddition;

	void RecordWarning(CString cstr);
	void clearWarning();
	void RecordLogList(CString cstr);
	void clearLogging();
	void UpdateDFTinformation(size_t total_num, int great_dft_num, int num1, int num2, int num3, int num4);
	void ResetBatchInformation();
	void SetBatchInfoControlItem();

private:
	HICON m_hChangeIcon;
	HICON m_hChangeXIcon;

	CString GetTimeStamp();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnSwitchRoll;                        // 切卷
	CListBox m_listLog;
	CListBox m_listWarning;
	CEdit m_eBatch;
	CEdit m_eName;
	CEdit m_eSchedule;
	CEdit m_eAddition;
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonInfoChange();
	afx_msg void OnBnClickedMfccolorbutton1();
	afx_msg void OnBnClickedMfccolorbutton2();
	afx_msg void OnBnClickedMfccolorbutton3();
	afx_msg void OnBnClickedMfccolorbutton4();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
};
