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
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CAnalysisDlg 消息处理程序

BOOL CAnalysisDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	CRect rect;

	// Get size and position of the picture control
	GetDlgItem(IDC_OPENGL)->GetWindowRect(rect);

	// Convert screen coordinates to client coordinates
	ScreenToClient(rect);

	// Create OpenGL Control window
	m_oglWindow.oglCreate(rect, this);

	// Setup the OpenGL Window's timer to render
	m_oglWindow.m_unpTimer = m_oglWindow.SetTimer(1, 1, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CAnalysisDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	switch (nType)
	{
	case SIZE_RESTORED:
	{
		if (m_oglWindow.m_bIsMaximized) {
			m_oglWindow.OnSize(nType, cx, cy);
			m_oglWindow.m_bIsMaximized = false;
		}

		break;
	}

	case SIZE_MAXIMIZED:
	{
		m_oglWindow.OnSize(nType, cx, cy);
		m_oglWindow.m_bIsMaximized = true;

		break;
	}

	}
}

void CAnalysisDlg::OnBnClickedButtonReset()
{
	// TODO: 在此添加控件通知处理程序代码

	CString cstr = L"重置分析结果";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}