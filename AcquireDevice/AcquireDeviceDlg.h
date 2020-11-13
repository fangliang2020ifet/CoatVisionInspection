
// AcquireDeviceDlg.h: 头文件
//

#pragma once

#include "CAcquireImage.h"

// CAcquireDeviceDlg 对话框
class CAcquireDeviceDlg : public CDialogEx
{
// 构造
public:
	CAcquireDeviceDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACQUIREDEVICE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:

private:
	CAcquireImage m_acquires;


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnAcq();
	afx_msg void OnBnClickedBtnStop();
};
