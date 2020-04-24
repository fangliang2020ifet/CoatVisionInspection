// CAlgorithmDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "CAlgorithmDlg.h"
#include "afxdialogex.h"


// CAlgorithmDlg 对话框

IMPLEMENT_DYNAMIC(CAlgorithmDlg, CDialogEx)

CAlgorithmDlg::CAlgorithmDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ALGORITHM, pParent)
{

}

CAlgorithmDlg::~CAlgorithmDlg()
{
}

void CAlgorithmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_GLOBAL_THRESHOLD, m_combo_global_threshold);
	DDX_Control(pDX, IDC_COMBO_SELECT_THRESHOLD, m_combo_select_threshold);
	DDX_Control(pDX, IDC_CHECK_LOAD_DEFAULT, m_btn_load_default);
}


BEGIN_MESSAGE_MAP(CAlgorithmDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAlgorithmDlg 消息处理程序


BOOL CAlgorithmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	m_combo_global_threshold.AddString(L"3");
	m_combo_global_threshold.AddString(L"4");
	m_combo_global_threshold.AddString(L"5");
	m_combo_global_threshold.AddString(L"6");
	m_combo_global_threshold.AddString(L"7");
	m_combo_global_threshold.AddString(L"8");
	m_combo_global_threshold.AddString(L"9");
	m_combo_global_threshold.AddString(L"10");
	m_combo_global_threshold.AddString(L"11");
	m_combo_global_threshold.AddString(L"12");
	m_combo_global_threshold.AddString(L"13");
	m_combo_global_threshold.AddString(L"14");
	m_combo_global_threshold.AddString(L"15");
	m_combo_global_threshold.AddString(L"16");
	m_combo_global_threshold.AddString(L"17");
	m_combo_global_threshold.AddString(L"18");
	m_combo_global_threshold.AddString(L"19");
	m_combo_global_threshold.AddString(L"20");
	m_combo_global_threshold.SetCurSel(2);
	
	m_combo_select_threshold.AddString(L"1");
	m_combo_select_threshold.AddString(L"3");
	m_combo_select_threshold.AddString(L"5");
	m_combo_select_threshold.AddString(L"7");
	m_combo_select_threshold.AddString(L"9");
	m_combo_select_threshold.AddString(L"11");
	m_combo_select_threshold.AddString(L"15");
	m_combo_select_threshold.AddString(L"20");
	m_combo_select_threshold.SetCurSel(2);

	m_btn_load_default.SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CAlgorithmDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


void CAlgorithmDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类


	//CDialogEx::OnOK();
}


void CAlgorithmDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnCancel();
}


void CAlgorithmDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (AfxMessageBox(_T("是否保存？"), MB_YESNO | MB_ICONWARNING) == IDYES) {
		int index_global = m_combo_global_threshold.GetCurSel();
		m_global_threshold = index_global + 3;
		CString cstr;
		cstr.Format(_T("设置检测算法概率密度为： %d"), m_global_threshold);
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

		int index_select = m_combo_select_threshold.GetCurSel();
		switch (index_select)
		{
		case 0:
			m_select_threshold = 1;
			break;
		case 1:
			m_select_threshold = 3;
			break;
		case 2:
			m_select_threshold = 5;
			break;
		case 3:
			m_select_threshold = 7;
			break;
		case 4:
			m_select_threshold = 9;
			break;
		case 5:
			m_select_threshold = 11;
			break;
		case 6:
			m_select_threshold = 15;
			break;
		case 7:
			m_select_threshold = 20;
			break;
		default:
			break;
		}
		cstr.Format(_T("设置检测算法图像滤波器大小为： %d"), m_select_threshold);
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);


		int state = m_btn_load_default.GetCheck();
		if (state == 0)
			m_load_default = FALSE;
		else if (state == 1)
			m_load_default = TRUE;
		cstr.Format(_T("设置检测算法使用默认参考图像： %d"), (int)m_load_default);
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);


	}

	CDialogEx::OnClose();
}
