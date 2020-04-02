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
END_MESSAGE_MAP()


// CAnalysisDlg 消息处理程序
