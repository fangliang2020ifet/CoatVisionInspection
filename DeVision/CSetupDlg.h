#pragma once

#include <string>
#include <algorithm>

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

	BOOL m_bSave_Parameter;          //是否保存
	float m_wnd1_range;
	float m_wnd2_range;
	float m_k_speed;                  //速度修正系数
	int m_threadnum;
	BOOL m_bSaveRefImg;

	std::string m_strDeffect_Path;
	std::string m_strTable_Path;

private:
	LPCWSTR APPNAME = L"System";
	LPCWSTR FILEPATH = L"inis\\SystemInfo.ini";

	BOOL ACCEPTED;
	CString m_current_logged_name;

	void loadInitialParameters();
	void saveParameters();

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
	CComboBox m_combo_threadnum;
	CButton m_save_reference_image;
	afx_msg void OnBnClickedButtonSelectDeffectPath();
	afx_msg void OnBnClickedButtonTablePath();
	afx_msg void OnBnClickedButtonSystemReset();
	afx_msg void OnEnKillfocusEditWnd1Range();
	afx_msg void OnEnKillfocusEditWnd2Range();
	afx_msg void OnEnKillfocusEditKSpeed();
	afx_msg void OnCbnSelchangeComboThreadnum();
	afx_msg void OnBnClickedCheckSaveRef();
};
