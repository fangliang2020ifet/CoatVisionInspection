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

	int m_nRankMethod;
	float m_fRankValue1;
	float m_fRankValue2;
	float m_fRankValue3;
	int m_nAlarmA;
	int m_nAlarmB;
	int m_nAlarmC;
	int m_nAlarmD;

private:
	LPCWSTR APPNAME = L"Algorithm";
	LPCWSTR FILEPATH = L"inis\\SystemInfo.ini";

	void loadInitialParameters();
	void saveParameters();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	CComboBox m_combo_global_threshold;
	CComboBox m_combo_select_threshold;
	afx_msg void OnClose();
	afx_msg void OnEnKillfocusEditSelectAreaMin();
	afx_msg void OnEnKillfocusEditSelectAreaMax();
	afx_msg void OnCbnSelchangeComboGlobalThreshold();
	afx_msg void OnCbnSelchangeComboSelectThreshold();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedRadioArea();
	afx_msg void OnBnClickedRadioRadius();
	afx_msg void OnBnClickedRadioContlength();
	afx_msg void OnEnKillfocusEditAreaValue1();
	afx_msg void OnEnKillfocusEditAreaValue2();
	afx_msg void OnEnKillfocusEditAreaValue3();
	afx_msg void OnEnKillfocusEditRadiusValue1();
	afx_msg void OnEnKillfocusEditRadiusValue2();
	afx_msg void OnEnKillfocusEditRadiusValue3();
	afx_msg void OnEnKillfocusEditContValue1();
	afx_msg void OnEnKillfocusEditContValue2();
	afx_msg void OnEnKillfocusEditContValue3();
};
