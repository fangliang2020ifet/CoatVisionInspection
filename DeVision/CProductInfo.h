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

	void LoadInifile();

private:
	//LPWSTR KeyName;
	//LPWSTR ReturnedString;
	LPCWSTR FILEPATH = L"inis\\ProductInformation.ini";


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonIdChange();
	afx_msg void OnBnClickedButtonIdInsert();
	afx_msg void OnBnClickedButtonIdSave();
	afx_msg void OnBnClickedButtonWidthChange();
	afx_msg void OnBnClickedButtonWidthInsert();
	afx_msg void OnBnClickedButtonWidthSave();
	afx_msg void OnBnClickedButtonBaseChange();
	afx_msg void OnBnClickedButtonBaseInsert();
	afx_msg void OnBnClickedButtonBaseSave();
	afx_msg void OnBnClickedButtonCoatingChange();
	afx_msg void OnBnClickedButtonCoatingInsert();
	afx_msg void OnBnClickedButtonCoatingSave();
	afx_msg void OnBnClickedButtonNumberSave();
	afx_msg void OnBnClickedButtonUserSave();
	afx_msg void OnBnClickedButtonAdditionSave();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboId();
	afx_msg void OnCbnSelchangeComboWidth();
	afx_msg void OnCbnSelchangeComboBase();
	afx_msg void OnCbnSelchangeComboCoating();
	afx_msg void OnCbnSelchangeComboOperator();
	afx_msg void OnBnClickedButtonOperatorChange();
	afx_msg void OnBnClickedButtonOperatorInsert();
	afx_msg void OnBnClickedButtonOperatorSave();
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
};