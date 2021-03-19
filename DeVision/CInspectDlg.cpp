// CInspectDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "DeVisionDlg.h"
#include "CInspectDlg.h"
#include "afxdialogex.h"

// CInspectDlg 对话框

IMPLEMENT_DYNAMIC(CInspectDlg, CDialogEx)

CInspectDlg::CInspectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_INSPECT, pParent)
{
	m_bSystemRunning = false;
	m_bTableSaving = false;

	for (int i = 0; i < 4; i++) { m_bDeffectDisplay[i] = TRUE; }

	m_hChangeIcon = AfxGetApp()->LoadIcon(IDI_ICON_CHANGE);
	m_hChangeXIcon = AfxGetApp()->LoadIcon(IDI_ICON_CHANGEX);

}

CInspectDlg::~CInspectDlg()
{

}

void CInspectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CAMERA_IMAGE1, m_ImageWnd[0]);
	DDX_Control(pDX, IDC_CAMERA_IMAGE2, m_ImageWnd[1]);
	DDX_Control(pDX, IDC_CAMERA_IMAGE3, m_ImageWnd[2]);
	DDX_Control(pDX, IDC_CAMERA_IMAGE4, m_ImageWnd[3]);
	DDX_Control(pDX, IDC_LIST_RECORD, m_listLog);
	DDX_Control(pDX, IDC_LIST_WARNNING, m_listWarning);
	DDX_Control(pDX, IDC_BUTTON_CHANGEINFO, m_btnSwitchRoll);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_NUMBER, m_eBatch);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_WIDTH, m_eName);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_MODEL, m_eSchedule);
	DDX_Control(pDX, IDC_EDIT_OPERATOR, m_eAddition);
}


BEGIN_MESSAGE_MAP(CInspectDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CHANGEINFO, &CInspectDlg::OnBnClickedButtonInfoChange)
	ON_BN_CLICKED(IDC_CHECK1, &CInspectDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &CInspectDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &CInspectDlg::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, &CInspectDlg::OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON1, &CInspectDlg::OnBnClickedMfccolorbutton1)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON2, &CInspectDlg::OnBnClickedMfccolorbutton2)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON3, &CInspectDlg::OnBnClickedMfccolorbutton3)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON4, &CInspectDlg::OnBnClickedMfccolorbutton4)
END_MESSAGE_MAP()


// CInspectDlg 消息处理程序
BOOL CInspectDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	m_font.CreatePointFont(200, _T("Times New Roman"));
	GetDlgItem(IDC_STATIC_TOTAL_DFT_NUM)->SetWindowPos(0, 580, 40, 100, 30, SWP_SHOWWINDOW);
	GetDlgItem(IDC_STATIC_GREAT_DFT_NUM)->SetWindowPos(0, 580, 100, 100, 30, SWP_SHOWWINDOW);

	CString ctime = GetTimeStamp();
	m_nBatchIndex = 1;
	m_cstrBatch = ctime + L"_KA_0001";
	m_cstrName = L"PET-1";
	m_cstrSchedule = L"早班";
	m_cstrAddition = L"";
	SetBatchInfoControlItem();

	m_btnSwitchRoll.SetIcon(m_hChangeIcon);
	m_btnSwitchRoll.SetFont(&m_font);
	m_btnSwitchRoll.SetWindowTextW(L"切卷");

	CButton *pcheck =(CButton*)GetDlgItem(IDC_CHECK1);
	pcheck->SetCheck(TRUE);
	pcheck = (CButton*)GetDlgItem(IDC_CHECK2);
	pcheck->SetCheck(TRUE);
	pcheck = (CButton*)GetDlgItem(IDC_CHECK3);
	pcheck->SetCheck(TRUE);
	pcheck = (CButton*)GetDlgItem(IDC_CHECK4);
	pcheck->SetCheck(TRUE);

	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON1);
	pcolorbtn->SetColor(RGB(255, 35, 15));
	m_color1 = pcolorbtn->GetColor();
	pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON2);
	pcolorbtn->SetColor(RGB(25, 255, 35));
	m_color2 = pcolorbtn->GetColor();
	pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON3);
	pcolorbtn->SetColor(RGB(35, 55, 225));
	m_color3 = pcolorbtn->GetColor();
	pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON4);
	pcolorbtn->SetColor(RGB(255, 255, 0));
	m_color4 = pcolorbtn->GetColor();
	   	 

	m_listLog.SetHorizontalExtent(1000);
	CString clog = L"***********************操作记录***********************";
	m_listLog.AddString(clog);
	m_listLog.PostMessageW(WM_VSCROLL, SB_BOTTOM, 0);

	m_listWarning.SetHorizontalExtent(1000);
	CString cwarning = L"***********************报警信息***********************";
	m_listWarning.AddString(cwarning);
	m_listWarning.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CInspectDlg::OnDestroy()
{
	__super::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

HBRUSH CInspectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_TOTAL_DFT_NUM){
		pDC->SetBkMode(TRANSPARENT);//设置背景透明
		pDC->SetTextColor(RGB(255, 0, 0)); //文字颜色  
		//pDC->SetBkColor(RGB(251, 247, 200));//背景色
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_GREAT_DFT_NUM){
		pDC->SetBkMode(TRANSPARENT);//设置背景透明
		pDC->SetTextColor(RGB(185, 55, 0)); //文字颜色  
		//pDC->SetBkColor(RGB(251, 247, 200));//背景色
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_MATTER){
		pDC->SetBkMode(TRANSPARENT);//设置背景透明
		pDC->SetTextColor(RGB(155, 0, 128)); //文字颜色  
		//pDC->SetBkColor(RGB(251, 247, 200));//背景色
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_CONVEX) {
		pDC->SetBkMode(TRANSPARENT);//设置背景透明
		pDC->SetTextColor(RGB(155, 0, 128)); //文字颜色  
		//pDC->SetBkColor(RGB(251, 247, 200));//背景色
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_BUBBLE) {
		pDC->SetBkMode(TRANSPARENT);//设置背景透明
		pDC->SetTextColor(RGB(155, 0, 128)); //文字颜色  
		//pDC->SetBkColor(RGB(251, 247, 200));//背景色
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_COATTING) {
		pDC->SetBkMode(TRANSPARENT);//设置背景透明
		pDC->SetTextColor(RGB(155, 0, 128)); //文字颜色  
		//pDC->SetBkColor(RGB(251, 247, 200));//背景色
	}
	if (pWnd->GetDlgCtrlID() == IDC_LIST_WARNNING) {
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 0, 0));
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CInspectDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
		// resize image viewer
	
	for (int i = 0; i < 4; i++) {
		if (m_ImageWnd[i].GetSafeHwnd()) {
			CRect rWnd;
			m_ImageWnd[i].GetWindowRect(rWnd);
			ScreenToClient(rWnd);
			m_ImageWnd[i].MoveWindow((rWnd));
		}
	}
}

//定时器
void CInspectDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1) {
		// 前一个报表保存后才刷新控件内容
		if (!m_bTableSaving){
			SetBatchInfoControlItem();
			KillTimer(1);
		}
	}

	__super::OnTimer(nIDEvent);
}


void CInspectDlg::UpdateDFTinformation(size_t total_num, int great_dft_num, int num1, int num2, int num3, int num4)
{
	CString csttotal_dft_num;
	csttotal_dft_num.Format(L"%d 个", total_num);
	GetDlgItem(IDC_STATIC_TOTAL_DFT_NUM)->SetWindowText(csttotal_dft_num);
	GetDlgItem(IDC_STATIC_TOTAL_DFT_NUM)->SetFont(&m_font);
	//CDC* pDC = GetDlgItem(IDC_STATIC_TOTAL_DFT_NUM)->GetDC();
	//pDC->SetBkMode(BKMODE_LAST);//透明
	//pDC->SetTextColor(RGB(0, 255, 0));
	//ReleaseDC(pDC);

	CString cstgreat_num;
	cstgreat_num.Format(L"%d 个", great_dft_num);
	GetDlgItem(IDC_STATIC_GREAT_DFT_NUM)->SetWindowText(cstgreat_num);
	GetDlgItem(IDC_STATIC_GREAT_DFT_NUM)->SetFont(&m_font);


	CString cstnum;
	cstnum.Format(L"%d 个", num1);
	GetDlgItem(IDC_STATIC_MATTER)->SetWindowText(cstnum);
	GetDlgItem(IDC_STATIC_MATTER)->SetFont(&m_font);

	cstnum.Format(L"%d 个", num2);
	GetDlgItem(IDC_STATIC_CONVEX)->SetWindowText(cstnum);
	GetDlgItem(IDC_STATIC_CONVEX)->SetFont(&m_font);

	cstnum.Format(L"%d 个", num3);
	GetDlgItem(IDC_STATIC_BUBBLE)->SetWindowText(cstnum);
	GetDlgItem(IDC_STATIC_BUBBLE)->SetFont(&m_font);

	cstnum.Format(L"%d 个", num4);
	GetDlgItem(IDC_STATIC_COATTING)->SetWindowText(cstnum);
	GetDlgItem(IDC_STATIC_COATTING)->SetFont(&m_font);

}

void CInspectDlg::RecordWarning(const std::wstring& str)
{
	CString cstr_log;
	CString strSpace, strNextLine, strTime;
	strSpace.Format(_T("%s"), _T("  "));
	strNextLine.Format(_T("%s"), _T("\r\n"));

	CTime tm; tm = CTime::GetCurrentTime();
	strTime = tm.Format("%X:");
	cstr_log += strTime;
	cstr_log += strSpace;
	cstr_log += str.c_str();
	cstr_log += strNextLine;

	m_listWarning.AddString(cstr_log);
	m_listWarning.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void CInspectDlg::RecordWarning(int test, CString cstr)
{
	CString clog, ctime;
	CTime tm; tm = CTime::GetCurrentTime();
	ctime = tm.Format("[%X] ");
	clog.Format(L"\r\n");
	m_listWarning.AddString(ctime + cstr + clog);
	m_listWarning.PostMessageW(WM_VSCROLL, SB_BOTTOM, 0);
}

void CInspectDlg::RecordLogList(const std::wstring& str)
{
	CString cstr_log;
	CString strSpace, strNextLine, strTime;
	strSpace.Format(_T("%s"), _T("  "));
	strNextLine.Format(_T("%s"), _T("\r\n"));

	CTime tm; tm = CTime::GetCurrentTime();
	strTime = tm.Format("%X:");
	cstr_log += strTime;
	cstr_log += strSpace;
	cstr_log += str.c_str();
	cstr_log += strNextLine;

	m_listLog.AddString(cstr_log);
	m_listLog.PostMessageW(WM_VSCROLL, SB_BOTTOM, 0);

}

void CInspectDlg::RecordLogList(int test, CString cstr)
{
	CString clog, ctime;
	CTime tm; tm = CTime::GetCurrentTime();
	ctime = tm.Format("[%X] ");
	clog.Format(L"\r\n");
	m_listLog.AddString(ctime + cstr + clog);
	m_listLog.PostMessageW(WM_VSCROLL, SB_BOTTOM, 0);
}

// 切卷
void CInspectDlg::OnBnClickedButtonInfoChange()
{
	// TODO: 在此添加控件通知处理程序代码
	//if (AfxMessageBox(_T("确定切卷？"), MB_YESNO | MB_ICONWARNING) == IDNO)
	//	return;

	//    210316_123649_KA_0004   截取批号
	CString cbatch;
	m_eBatch.GetWindowTextW(cbatch);
	std::string strbatch = (CW2A)cbatch.GetBuffer();
	auto pos1 = strbatch.find_first_of("_");
	std::string substrbatch = strbatch.substr(pos1 + 8);
	auto pos2 = substrbatch.find_first_of("_");
	std::string substrcode = substrbatch.substr(0, pos2);
	//std::string substrindex = substrbatch.substr(pos2 + 1);
	//int n_batch_index = std::stoi(substrindex);

	CProductInfo productinfo;
	// 显示下一卷的卷号信息, 序号自动递增
	productinfo.m_nBatchIndex = m_nBatchIndex + 1;
	productinfo.m_cstrBatchCode = substrcode.c_str();
	productinfo.m_cstrName = m_cstrName;
	productinfo.m_cstrSchedule = m_cstrSchedule;
	productinfo.m_cstrAddition = m_cstrAddition;
	productinfo.DoModal();
	if (productinfo.m_bSave_Parameter) {
		m_cstrBatch = productinfo.m_cstrBatch;
		m_nBatchIndex = productinfo.m_nBatchIndex;
		m_cstrName = productinfo.m_cstrName;
		m_cstrSchedule = productinfo.m_cstrSchedule;
		m_cstrAddition = productinfo.m_cstrAddition;

		if (m_bSystemRunning) {
			m_bTableSaving = true;
			//::SendMessage(hMainWnd, WM_SWITCHROLL, 0, 0);
			::SendNotifyMessageW(hMainWnd, WM_SWITCHROLL, 0, 0);
			SetTimer(1, 200, NULL);
		}
		else {
			SetBatchInfoControlItem();
		}
	}
}

//选中类型显示
void CInspectDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton * check = (CButton*)GetDlgItem(IDC_CHECK1);
	if (check->GetCheck()) {
		m_bDeffectDisplay[0] = TRUE;
		CString cstr = L"1_1";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	} else {
		m_bDeffectDisplay[0] = FALSE;
		CString cstr = L"1_0";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
}


void CInspectDlg::OnBnClickedCheck2()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton * check = (CButton*)GetDlgItem(IDC_CHECK2);
	if (check->GetCheck()) {
		m_bDeffectDisplay[1] = TRUE;
		CString cstr = L"2_1";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
	else {
		m_bDeffectDisplay[1] = FALSE;
		CString cstr = L"2_0";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
}


void CInspectDlg::OnBnClickedCheck3()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton * check = (CButton*)GetDlgItem(IDC_CHECK3);
	if (check->GetCheck()) {
		m_bDeffectDisplay[2] = TRUE;
		CString cstr = L"3_1";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
	else {
		m_bDeffectDisplay[2] = FALSE;
		CString cstr = L"3_0";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
}


void CInspectDlg::OnBnClickedCheck4()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton * check = (CButton*)GetDlgItem(IDC_CHECK4);
	if (check->GetCheck()) {
		m_bDeffectDisplay[3] = TRUE;
		CString cstr = L"4_1";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
	else {
		m_bDeffectDisplay[3] = FALSE;
		CString cstr = L"4_0";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
}

//修改颜色
void CInspectDlg::OnBnClickedMfccolorbutton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON1);
	m_color1 = pcolorbtn->GetColor();

	::SendNotifyMessageW(hMainWnd, WM_UPDATE_CONTROLS, 0, 0);
}

void CInspectDlg::OnBnClickedMfccolorbutton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON2);
	m_color2 = pcolorbtn->GetColor();

	::SendNotifyMessageW(hMainWnd, WM_UPDATE_CONTROLS, 0, 0);
}

void CInspectDlg::OnBnClickedMfccolorbutton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON3);
	m_color3 = pcolorbtn->GetColor();

	::SendNotifyMessageW(hMainWnd, WM_UPDATE_CONTROLS, 0, 0);
}

void CInspectDlg::OnBnClickedMfccolorbutton4()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON4);
	m_color4 = pcolorbtn->GetColor();

	::SendNotifyMessageW(hMainWnd, WM_UPDATE_CONTROLS, 0, 0);
}


CString CInspectDlg::GetTimeStamp()
{
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
	std::string str_time = name;
	std::string str_subtime = str_time.substr(2);
	CString time = (CA2W)str_subtime.c_str();

	return time;
}

//  重新开始前需要刷新 control information
void CInspectDlg::ResetBatchInformation()
{
	CString ctime = GetTimeStamp();
	CString cindex;
	cindex.Format(_T("%.4d"), m_nBatchIndex);

	std::string strbatch = (CW2A)m_cstrBatch.GetBuffer();
	auto pos1 = strbatch.find_first_of("_");
	std::string substrbatch = strbatch.substr(pos1 + 8);
	auto pos2 = substrbatch.find_first_of("_");
	std::string substrcode = substrbatch.substr(0, pos2);
	CString ccode = (CA2W)substrcode.c_str();

	m_cstrBatch = ctime + L"_" + ccode + L"_" + cindex;

	SetBatchInfoControlItem();

	UpdateData(false);
	return;
}


void CInspectDlg::SetBatchInfoControlItem()
{
	//等待报表保存完成，修改卷信息的显示
	m_eBatch.SetWindowTextW(m_cstrBatch);
	m_eName.SetWindowTextW(m_cstrName);
	m_eSchedule.SetWindowTextW(m_cstrSchedule);
	m_eAddition.SetWindowTextW(m_cstrAddition);

	UpdateData(false);
}
