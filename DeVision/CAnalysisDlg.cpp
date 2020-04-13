// CAnalysisDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "CAnalysisDlg.h"
#include "afxdialogex.h"


// CAnalysisDlg 对话框

IMPLEMENT_DYNAMIC(CAnalysisDlg, CDialogEx)

CAnalysisDlg::CAnalysisDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ANALYSIS, pParent)
{

}

CAnalysisDlg::~CAnalysisDlg()
{
}

void CAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAnalysisDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CAnalysisDlg::OnBnClickedButtonReset)
END_MESSAGE_MAP()


// CAnalysisDlg 消息处理程序


BOOL CAnalysisDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;



	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CAnalysisDlg::OnBnClickedButtonReset()
{
	// TODO: 在此添加控件通知处理程序代码

	CString cstr = L"重置分析结果";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

}
