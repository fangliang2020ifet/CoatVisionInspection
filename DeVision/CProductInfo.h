#pragma once

#include <string>


// CProductInfo 对话框

class CProductInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CProductInfo)

public:
	CProductInfo(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CProductInfo();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PRODUCTINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


private:
	DECLARE_MESSAGE_MAP()

public:
	HWND hMainWnd;                           //主窗口句柄
	BOOL m_bSave_Parameter = FALSE;          //是否保存

	LPCWSTR APPNAME = L"ProductInfo";
	LPCWSTR FILEPATH = L"inis\\ProductInformation.ini";


	void UpdateControls();
	void LoadInifile();
	void SaveAll();

public:
	int     m_nBatchIndex;
	CString m_cstrBatchTime;
	CString m_cstrBatchCode;
	CString m_cstrBatch;
	CString m_cstrName;
	CString m_cstrSchedule;
	CString m_cstrAddition;

	CString GetTimeStamp();
	CString GenerateBatchNumber();


	afx_msg BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnIndexChange();
	afx_msg void OnEnKillfocusEditInfoName();
	afx_msg void OnEnKillfocusEditSchedule();
	afx_msg void OnEnKillfocusEditInfoNo();
	afx_msg void OnEnKillfocusEditAddition();
	afx_msg void OnEnKillfocusEditNoIndex();
};