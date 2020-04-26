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

	LPCWSTR APPNAME = L"ProductInfo";
	LPCWSTR FILEPATH = L"inis\\ProductInformation.ini";

	CString m_ctrID;
	CString m_ctrWIDTH;
	CString m_ctrBASE;
	CString m_ctrCOATING;
	CString m_ctrOPERATOR;
	CString m_ctrNUMBER;
	CString m_ctrUSER;
	CString m_ctrADDITION;

	void LoadInifile();
	void SaveAll();

public:
	afx_msg BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	CComboBox m_combox_id;
	CComboBox m_combox_width;
	CComboBox m_combox_base;
	CComboBox m_combox_coating;
	CComboBox m_combox_operator;
};