// CSetupDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "CSetupDlg.h"
#include "afxdialogex.h"


// CSetupDlg 对话框

IMPLEMENT_DYNAMIC(CSetupDlg, CDialogEx)

CSetupDlg::CSetupDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SETUP, pParent)
{

}

CSetupDlg::~CSetupDlg()
{
}

void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetupDlg, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSetupDlg 消息处理程序


BOOL CSetupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_wnd1_range = 20.0f;
	CString ctext1, ctext2;
	ctext1.Format(_T("%.2f"), m_wnd1_range);
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_WND1_RANGE);
	pedit->SetWindowTextW(ctext1);

	m_wnd2_range = 3.0f;
	ctext2.Format(_T("%.2f"), m_wnd2_range);
	pedit = (CEdit*)GetDlgItem(IDC_EDIT_WND2_RANGE);
	pedit->SetWindowTextW(ctext2);




	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CSetupDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类




	return CDialogEx::DestroyWindow();
}


void CSetupDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (MessageBox(_T("是否放弃保存"), _T("提示"), MB_YESNO | MB_ICONWARNING) == IDNO)
		return;

	CDialogEx::OnClose();
}


void CSetupDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	m_wnd1_range = GetWnd1DisplayRange();
	m_wnd2_range = GetWnd2DisplayRange();

	CDialogEx::OnOK();
}


float CSetupDlg::GetWnd1DisplayRange()
{
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_WND1_RANGE);
	float range;
	CString str_edit;
	pedit->GetWindowTextW(str_edit);
	range = (float)_ttof(str_edit);

	return range;
}

float CSetupDlg::GetWnd2DisplayRange()
{
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_WND2_RANGE);
	float range;
	CString str_edit;
	pedit->GetWindowTextW(str_edit);
	range = (float)_ttof(str_edit);

	return range;
}
