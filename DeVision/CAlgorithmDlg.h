#pragma once


// CAlgorithmDlg 对话框

class CAlgorithmDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAlgorithmDlg)

public:
	CAlgorithmDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAlgorithmDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ALGORITHM };
#endif

public:
	int m_global_threshold = 0;
	int m_select_threshold = 0;
	BOOL m_load_default = TRUE;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	CComboBox m_combo_global_threshold;
	CComboBox m_combo_select_threshold;
	CButton m_btn_load_default;
};
