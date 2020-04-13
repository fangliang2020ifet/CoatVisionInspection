#pragma once


// CSetupDlg 对话框

class CSetupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetupDlg)

public:
	CSetupDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSetupDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SETUP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	HWND hMainWnd;                           //主窗口句柄

	float m_wnd1_range = 0.0f;
	float m_wnd2_range = 0.0f;

private:
	float GetWnd1DisplayRange();
	float GetWnd2DisplayRange();


public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnClose();
	virtual void OnOK();
};
