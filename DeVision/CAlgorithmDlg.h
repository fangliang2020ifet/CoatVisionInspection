#pragma once

#include <string>

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
	HWND hMainWnd;                           //主窗口句柄
	BOOL m_bSave_Parameter = FALSE;          //是否保存
	int m_normal_distribution = 0;
	int m_filter_size = 0;
	float m_min_radius = 0.0f;
	float m_max_radius = 0.0f;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	float GetSelectAreaValueMin();
	float GetSelectAreaValueMax();

public:


	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	CComboBox m_combo_global_threshold;
	CComboBox m_combo_select_threshold;
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonReset();
};
