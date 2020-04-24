#pragma once


// CRemote 对话框

class CRemote : public CDialogEx
{
	DECLARE_DYNAMIC(CRemote)

public:
	CRemote(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRemote();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REMOTE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
