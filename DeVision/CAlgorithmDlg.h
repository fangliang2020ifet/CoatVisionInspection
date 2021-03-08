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
	float m_fPlanValue[5][6] = {0.0f};

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	CComboBox m_combo_global_threshold;
	CComboBox m_combo_select_threshold;
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnCbnSelchangeCombo2();

private:
	CComboBox m_comboDFTselect;

	CString n11;
	CString n12;
	CString n13;
	CString n14;
	CString n15;
	CString n16;
	CString n21;
	CString n22;
	CString n23;
	CString n24;
	CString n25;
	CString n26;
	CString n31;
	CString n32;
	CString n33;
	CString n34;
	CString n35;
	CString n36;
	CString n41;
	CString n42;
	CString n43;
	CString n44;
	CString n45;
	CString n46;
	CString n51;
	CString n52;
	CString n53;
	CString n54;
	CString n55;
	CString n56;
	afx_msg void OnEnKillfocusEdit11();
	afx_msg void OnEnKillfocusEdit12();
	afx_msg void OnEnKillfocusEdit13();
	afx_msg void OnEnKillfocusEdit14();
	afx_msg void OnEnKillfocusEdit15();
	afx_msg void OnEnKillfocusEdit16();
	afx_msg void OnEnKillfocusEdit21();
	afx_msg void OnEnKillfocusEdit22();
	afx_msg void OnEnKillfocusEdit23();
	afx_msg void OnEnKillfocusEdit24();
	afx_msg void OnEnKillfocusEdit25();
	afx_msg void OnEnKillfocusEdit26();
	afx_msg void OnEnKillfocusEdit31();
	afx_msg void OnEnKillfocusEdit32();
	afx_msg void OnEnKillfocusEdit33();
	afx_msg void OnEnKillfocusEdit34();
	afx_msg void OnEnKillfocusEdit35();
	afx_msg void OnEnKillfocusEdit36();
	afx_msg void OnEnKillfocusEdit41();
	afx_msg void OnEnKillfocusEdit42();
	afx_msg void OnEnKillfocusEdit43();
	afx_msg void OnEnKillfocusEdit44();
	afx_msg void OnEnKillfocusEdit45();
	afx_msg void OnEnKillfocusEdit46();
	afx_msg void OnEnKillfocusEdit51();
	afx_msg void OnEnKillfocusEdit52();
	afx_msg void OnEnKillfocusEdit53();
	afx_msg void OnEnKillfocusEdit54();
	afx_msg void OnEnKillfocusEdit55();
	afx_msg void OnEnKillfocusEdit56();
public:
};
