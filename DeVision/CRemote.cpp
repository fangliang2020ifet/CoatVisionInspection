// CRemote.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "CRemote.h"
#include "afxdialogex.h"


// CRemote 对话框

IMPLEMENT_DYNAMIC(CRemote, CDialogEx)

CRemote::CRemote(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REMOTE, pParent)
{

}

CRemote::~CRemote()
{
}

void CRemote::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRemote, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CRemote::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CRemote::OnBnClickedCancel)
END_MESSAGE_MAP()


// CRemote 消息处理程序



BOOL CRemote::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CRemote::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (AfxMessageBox(_T("是否保存？"), MB_YESNO | MB_ICONWARNING) == IDYES) {
		

	}

	CDialogEx::OnClose();
}


void CRemote::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
}


void CRemote::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}
