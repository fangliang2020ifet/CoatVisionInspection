
// AcquireDeviceDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "AcquireDevice.h"
#include "AcquireDeviceDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAcquireDeviceDlg 对话框



CAcquireDeviceDlg::CAcquireDeviceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ACQUIREDEVICE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAcquireDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CAcquireDeviceDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_OPEN, &CAcquireDeviceDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CAcquireDeviceDlg::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_ACQ, &CAcquireDeviceDlg::OnBnClickedBtnAcq)
	ON_BN_CLICKED(IDC_BTN_STOP, &CAcquireDeviceDlg::OnBnClickedBtnStop)
END_MESSAGE_MAP()


// CAcquireDeviceDlg 消息处理程序

BOOL CAcquireDeviceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	HWND hwnd = GetDlgItem(IDC_IMAGE1)->GetSafeHwnd();
	m_acquires.m_hwnd[0] = hwnd;
	hwnd = GetDlgItem(IDC_IMAGE2)->GetSafeHwnd();
	m_acquires.m_hwnd[1] = hwnd;
	hwnd = GetDlgItem(IDC_IMAGE3)->GetSafeHwnd();
	m_acquires.m_hwnd[2] = hwnd;
	hwnd = GetDlgItem(IDC_IMAGE4)->GetSafeHwnd();
	m_acquires.m_hwnd[3] = hwnd;



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAcquireDeviceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAcquireDeviceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAcquireDeviceDlg::OnBnClickedBtnOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	m_acquires.CameraSystemInitial();


}


void CAcquireDeviceDlg::OnBnClickedBtnClose()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CAcquireDeviceDlg::OnBnClickedBtnAcq()
{
	// TODO: 在此添加控件通知处理程序代码
	m_acquires.Grab();
}


void CAcquireDeviceDlg::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
	m_acquires.Freeze();
}
