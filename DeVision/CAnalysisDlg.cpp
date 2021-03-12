// CAnalysisDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "DeVisionDlg.h"
#include "CAnalysisDlg.h"
#include "afxdialogex.h"
#include <string>



// CAnalysisDlg 对话框

IMPLEMENT_DYNAMIC(CAnalysisDlg, CDialogEx)

CAnalysisDlg::CAnalysisDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ANALYSIS, pParent)
{
	m_bShowGrid = FALSE;
	m_bShowText = FALSE;
	m_bShowLable = FALSE;
	m_bShowTip = FALSE;
	m_bShowPercent = FALSE;
	m_dDftNumber1 = 0.0;
	m_dDftNumber2 = 0.0;
	m_dDftNumber3 = 0.0;
	m_dDftNumber4 = 0.0;
	m_dDftNumber5 = 0.0;
	m_dDftNumber6 = 0.0;
	m_dDftNumber7 = 0.0;
	m_dDftNumber8 = 0.0;


}

CAnalysisDlg::~CAnalysisDlg()
{
}

void CAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_GRID, m_bShowGrid);
	DDX_Check(pDX, IDC_CHECK_TEXT, m_bShowText);
	DDX_Check(pDX, IDC_CHECK_LABEL, m_bShowLable);
	DDX_Check(pDX, IDC_CHECK_TOOLTIPS, m_bShowTip);
	DDX_Check(pDX, IDC_CHECK_PERCENT, m_bShowPercent);
}

BEGIN_MESSAGE_MAP(CAnalysisDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CAnalysisDlg::OnBnClickedButtonReset)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_GRID, &CAnalysisDlg::OnBnClickedCheckGrid)
	ON_BN_CLICKED(IDC_CHECK_TEXT, &CAnalysisDlg::OnBnClickedCheckText)
	ON_BN_CLICKED(IDC_CHECK_LABEL, &CAnalysisDlg::OnBnClickedCheckLabel)
	ON_BN_CLICKED(IDC_CHECK_TOOLTIPS, &CAnalysisDlg::OnBnClickedCheckTooltips)
	ON_BN_CLICKED(IDC_CHECK_PERCENT, &CAnalysisDlg::OnBnClickedCheckPercent)
	ON_BN_CLICKED(IDC_BUTTON_ANALYSIS_SAVE, &CAnalysisDlg::OnBnClickedButtonAnalysisSave)
END_MESSAGE_MAP()

// CAnalysisDlg 消息处理程序

BOOL CAnalysisDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	m_strSavePath = "D:\\瑕疵检测数据记录\\3瑕疵柱状图记录\\";

	m_bShowGrid = TRUE;
	m_bShowLable = TRUE;
	m_bShowText = TRUE;
	m_bShowTip = TRUE;
	m_bShowPercent = FALSE;
	m_dDftNumber1 = 0.0;
	m_dDftNumber2 = 0.0;
	m_dDftNumber3 = 0.0;
	m_dDftNumber4 = 0.0;
	m_dDftNumber5 = 0.0;
	m_dDftNumber6 = 0.0;
	m_dDftNumber7 = 0.0;
	m_dDftNumber8 = 0.0;


	CreateCustomBarChart();

	UpdateData(FALSE);

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

		break;
	}
	case SIZE_MAXIMIZED:
	{
		break;
	}

	}
}

void CAnalysisDlg::CreateCustomBarChart()
{
	CWnd *pwnd = GetDlgItem(IDC_STATIC_BARCHART);
	CRect rcClient;
	pwnd->GetClientRect(&rcClient);

	if (!m_chart.Create(CRect(10, 10, rcClient.Width() - 10, rcClient.Height() - 20), this, 0)){
		if (!m_chart.GetSafeHwnd())	{
			CString cstr = L"无法创建柱状图";
			::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
			return;
		}
		m_chart.Reset();
		m_chart.SetAutoScale(FALSE);
	}

	m_chart.SetTitle(L"瑕疵类型与数量分布柱状图");
	m_chart.SetBKColor(RGB(238, 238, 238));
	m_chart.ShowTooltip(TRUE);
	//m_chart.SetAutoScale(TRUE);


	m_chart.AddBar(m_dDftNumber1, L"异物", RGB(255, 35, 15));
	m_chart.AddBar(m_dDftNumber2, L"擦伤", RGB(25, 255, 35));
	m_chart.AddBar(m_dDftNumber3, L"气泡", RGB(35, 55, 225));
	m_chart.AddBar(m_dDftNumber4, L"黑点", RGB(255, 255, 0));
	m_chart.AddBar(m_dDftNumber5, L"斑块", RGB(55, 35, 55));
	m_chart.AddBar(m_dDftNumber6, L"凹凸", RGB(255, 255, 255));
	m_chart.AddBar(m_dDftNumber7, L"色差", RGB(155, 35, 155));
	m_chart.AddBar(m_dDftNumber8, L"均匀度", RGB(155, 155, 85));

}

void CAnalysisDlg::ClearAll()
{
	m_chart.RemoveAll();
}

void CAnalysisDlg::UpdateChartValue()
{
	ClearAll();

	CWnd *pwnd = GetDlgItem(IDC_STATIC_BARCHART);
	CRect rcClient;
	pwnd->GetClientRect(&rcClient);

	if (!m_chart.GetSafeHwnd()){
		m_chart.Create(CRect(10, 10, rcClient.Width() - 10, rcClient.Height() - 20), this, 0);
	}
	else{
		m_chart.Reset();
	}

	m_chart.SetTitle(L"瑕疵类型与数量分布柱状图");
	m_chart.SetBKColor(RGB(238, 238, 238));
	m_chart.AddBar(m_dDftNumber1, L"异物", RGB(255, 35, 15));
	m_chart.AddBar(m_dDftNumber2, L"擦伤", RGB(25, 255, 35));
	m_chart.AddBar(m_dDftNumber3, L"气泡", RGB(35, 55, 225));
	m_chart.AddBar(m_dDftNumber4, L"黑点", RGB(255, 255, 0));
	m_chart.AddBar(m_dDftNumber5, L"斑块", RGB(55, 35, 55));
	m_chart.AddBar(m_dDftNumber6, L"凹凸", RGB(255, 255, 255));
	m_chart.AddBar(m_dDftNumber7, L"色差", RGB(155, 35, 155));
	m_chart.AddBar(m_dDftNumber8, L"均匀度", RGB(155, 155, 85));

	m_chart.Refresh();
}

// 刷新
void CAnalysisDlg::OnBnClickedButtonReset()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_dDftNumber1 = 55.0;
	UpdateChartValue();

	CString cstr = L"重置分析结果";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

// 保存
void CAnalysisDlg::OnBnClickedButtonAnalysisSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CWnd *pwnd = GetDlgItem(IDC_STATIC_BARCHART);
	CRect rect;
	pwnd->GetClientRect(&rect);
	CDC *dc = pwnd->GetDC();

	LPRECT prect = rect;
	HBITMAP m_hbitmap;
	m_hbitmap = GetSrcBit(*dc, prect);

	//获取日期
	std::wstringstream date;
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	date << sysTime.wYear << std::setw(2) << std::setfill(L'0')
		<< sysTime.wMonth << std::setw(2) << std::setfill(L'0')
		<< sysTime.wDay << L"_" << std::setw(2) << std::setfill(L'0')
		<< sysTime.wHour << std::setw(2) << std::setfill(L'0')
		<< sysTime.wMinute << std::setw(2) << std::setfill(L'0')
		<< sysTime.wSecond;
	const std::wstring wdate = date.str();
	const wchar_t* wname = wdate.c_str();
	_bstr_t name(wname);
	std::string strname = name + ".bmp";
	std::string strfilename = m_strSavePath;
	strfilename += strname;	

	BOOL saved = SaveBitmapToFile(m_hbitmap, strfilename.c_str());

	ReleaseDC(dc);
}

HBITMAP CAnalysisDlg::GetSrcBit(HDC hDC, LPRECT rEct)
{
	HDC hBufDC;
	HBITMAP hBitmap, hBitTemp;
	//创建设备上下文(HDC)
	hBufDC = CreateCompatibleDC(hDC);
	//创建HBITMAP
	hBitmap = CreateCompatibleBitmap(hDC, abs(rEct->right - rEct->left), abs(rEct->bottom - rEct->top));
	hBitTemp = (HBITMAP)SelectObject(hBufDC, hBitmap);
	//得到位图缓冲区
	StretchBlt(hBufDC, 0, 0, abs(rEct->right - rEct->left), abs(rEct->bottom - rEct->top),
		hDC, rEct->left, rEct->top, abs(rEct->right - rEct->left), abs(rEct->bottom - rEct->top), SRCCOPY);
	//得到最终的位图信息
	hBitmap = (HBITMAP)SelectObject(hBufDC, hBitTemp);
	//释放内存
	DeleteObject(hBitTemp);
	::DeleteDC(hBufDC);
	return hBitmap;
}

bool CAnalysisDlg::SaveBitmapToFile(HBITMAP hBitmap, const char* name)
{
	CImage img;
	img.Attach(hBitmap);
	//HRESULT hResult = img.Save((LPCWSTR)lpFileName);
	HRESULT hResult = img.Save((CA2W)name);

	//其它图片格式同理
	DeleteObject(hBitmap);

	if (FAILED(hResult))
	{
		return false;
	}
	else
	{
		return true;
	}
}


void CAnalysisDlg::OnBnClickedCheckGrid()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	m_chart.ShowGrid(m_bShowGrid, TRUE);

}


void CAnalysisDlg::OnBnClickedCheckText()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	m_chart.ShowBarText(1, m_bShowText, TRUE);

}


void CAnalysisDlg::OnBnClickedCheckLabel()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	m_chart.ShowLabel(m_bShowLable, TRUE);

}


void CAnalysisDlg::OnBnClickedCheckTooltips()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_chart.ShowTooltip(m_bShowTip);

}


void CAnalysisDlg::OnBnClickedCheckPercent()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (m_bShowPercent) 
		m_chart.ShowBarText(0, m_bShowText);
	else
		m_chart.ShowBarText(1, m_bShowText);
	
	m_chart.Refresh();

}

