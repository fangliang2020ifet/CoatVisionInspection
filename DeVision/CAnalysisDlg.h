#pragma once


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


	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonReset();
};
