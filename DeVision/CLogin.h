#pragma once

#include <string>

// CLogin 对话框

class CLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CLogin)

public:
	CLogin(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CLogin();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
	BOOL ACCEPTED;

private:
	_ConnectionPtr  m_pConnection;
	_RecordsetPtr   m_pRecordset;
	_CommandPtr     m_pCommand;

	std::string m_strname;
	std::string m_strpassword;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CEdit m_edit_name;
	CEdit m_edit_password;
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};
