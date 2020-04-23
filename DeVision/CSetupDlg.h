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
	BOOL ACCEPTED;
	CString m_current_logged_name;

	float GetWnd1DisplayRange();
	float GetWnd2DisplayRange();

	BOOL ConnectAccess();
	_ConnectionPtr  m_pConnection;
	_RecordsetPtr   m_pRecordset;
	//_CommandPtr     m_pCommand;

public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnClose();
	virtual void OnOK();
	afx_msg void OnBnClickedButtonSetupLogin();
	afx_msg void OnBnClickedButtonAddUser();
	afx_msg void OnBnClickedButtonSetupChange();
	afx_msg void OnBnClickedButtonSetupDelete();
};
