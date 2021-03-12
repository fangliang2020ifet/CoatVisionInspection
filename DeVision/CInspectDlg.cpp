// CInspectDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "CInspectDlg.h"
#include "afxdialogex.h"

// CInspectDlg 对话框

IMPLEMENT_DYNAMIC(CInspectDlg, CDialogEx)

CInspectDlg::CInspectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_INSPECT, pParent)
{
	for (int i = 0; i < 8; i++) { m_bDeffectDisplay[i] = TRUE; }

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
	DDX_Control(pDX, IDC_BUTTON_CHANGEINFO, m_btn_changeinfo);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_NUMBER, m_eNumber);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_WIDTH, m_eWidth);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_MODEL, m_eModel);
	DDX_Control(pDX, IDC_EDIT_OPERATOR, m_eOperator);
}


BEGIN_MESSAGE_MAP(CInspectDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_CHANGEINFO, &CInspectDlg::OnBnClickedButtonInfoChange)
	ON_BN_CLICKED(IDC_CHECK1, &CInspectDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &CInspectDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &CInspectDlg::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, &CInspectDlg::OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_CHECK5, &CInspectDlg::OnBnClickedCheck5)
	ON_BN_CLICKED(IDC_CHECK6, &CInspectDlg::OnBnClickedCheck6)
	ON_BN_CLICKED(IDC_CHECK7, &CInspectDlg::OnBnClickedCheck7)
	ON_BN_CLICKED(IDC_CHECK8, &CInspectDlg::OnBnClickedCheck8)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON1, &CInspectDlg::OnBnClickedMfccolorbutton1)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON2, &CInspectDlg::OnBnClickedMfccolorbutton2)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON3, &CInspectDlg::OnBnClickedMfccolorbutton3)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON4, &CInspectDlg::OnBnClickedMfccolorbutton4)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON5, &CInspectDlg::OnBnClickedMfccolorbutton5)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON6, &CInspectDlg::OnBnClickedMfccolorbutton6)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON7, &CInspectDlg::OnBnClickedMfccolorbutton7)
	ON_BN_CLICKED(IDC_MFCCOLORBUTTON8, &CInspectDlg::OnBnClickedMfccolorbutton8)
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
	GetDlgItem(IDC_STATIC_TOTAL_DFT_NUM)->SetWindowPos(0, 580, 25, 100, 30, SWP_SHOWWINDOW);
	GetDlgItem(IDC_STATIC_GREAT_DFT_NUM)->SetWindowPos(0, 580, 70, 100, 30, SWP_SHOWWINDOW);
	GetDlgItem(IDC_STATIC_DFT_LONGTH)->SetWindowPos(0, 580, 110, 100, 30, SWP_SHOWWINDOW);

	m_btn_changeinfo.SetIcon(m_hChangeIcon);
	m_btn_changeinfo.SetFont(&m_font);
	m_btn_changeinfo.SetWindowTextW(L"切卷");

	CButton *pcheck =(CButton*)GetDlgItem(IDC_CHECK1);
	pcheck->SetCheck(TRUE);
	pcheck = (CButton*)GetDlgItem(IDC_CHECK2);
	pcheck->SetCheck(TRUE);
	pcheck = (CButton*)GetDlgItem(IDC_CHECK3);
	pcheck->SetCheck(TRUE);
	pcheck = (CButton*)GetDlgItem(IDC_CHECK4);
	pcheck->SetCheck(TRUE);
	pcheck = (CButton*)GetDlgItem(IDC_CHECK5);
	pcheck->SetCheck(TRUE);
	pcheck = (CButton*)GetDlgItem(IDC_CHECK6);
	pcheck->SetCheck(TRUE);
	pcheck = (CButton*)GetDlgItem(IDC_CHECK7);
	pcheck->SetCheck(TRUE);
	pcheck = (CButton*)GetDlgItem(IDC_CHECK8);
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
	pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON5);
	pcolorbtn->SetColor(RGB(55, 35, 55));
	m_color5 = pcolorbtn->GetColor();
	pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON6);
	pcolorbtn->SetColor(RGB(255, 255, 255));
	m_color6 = pcolorbtn->GetColor();
	pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON7);
	pcolorbtn->SetColor(RGB(155, 35, 155));
	m_color7 = pcolorbtn->GetColor();
	pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON8);
	pcolorbtn->SetColor(RGB(155, 155, 85));
	m_color8 = pcolorbtn->GetColor();

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
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_DFT_LONGTH){
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

void CInspectDlg::UpdateDFTinformation(size_t total_num,int great_dft_num, float longth)
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


	CString cstlongth;
	cstlongth.Format(L"%.2f 米", longth);
	GetDlgItem(IDC_STATIC_DFT_LONGTH)->SetWindowText(cstlongth);
	GetDlgItem(IDC_STATIC_DFT_LONGTH)->SetFont(&m_font);

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
	CProductInfo productinfo;
	productinfo.DoModal();
	if (productinfo.m_bSave_Parameter) {
		m_eNumber.SetWindowTextW(productinfo.m_ctrNUMBER);
		m_eWidth.SetWindowTextW(productinfo.m_ctrWIDTH);
		m_eModel.SetWindowTextW(productinfo.m_ctrID);
		m_eOperator.SetWindowTextW(productinfo.m_ctrOPERATOR);
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


void CInspectDlg::OnBnClickedCheck5()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton * check = (CButton*)GetDlgItem(IDC_CHECK5);
	if (check->GetCheck()) {
		m_bDeffectDisplay[4] = TRUE;
		CString cstr = L"5_1";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
	else {
		m_bDeffectDisplay[4] = FALSE;
		CString cstr = L"5_0";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
}


void CInspectDlg::OnBnClickedCheck6()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton * check = (CButton*)GetDlgItem(IDC_CHECK6);
	if (check->GetCheck()) {
		m_bDeffectDisplay[5] = TRUE;
		CString cstr = L"6_1";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
	else {
		m_bDeffectDisplay[5] = FALSE;
		CString cstr = L"6_0";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
}


void CInspectDlg::OnBnClickedCheck7()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton * check = (CButton*)GetDlgItem(IDC_CHECK7);
	if (check->GetCheck()) {
		m_bDeffectDisplay[6] = TRUE;
		CString cstr = L"7_1";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
	else {
		m_bDeffectDisplay[6] = FALSE;
		CString cstr = L"7_0";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
}


void CInspectDlg::OnBnClickedCheck8()
{
	// TODO: 在此添加控件通知处理程序代码
	CButton * check = (CButton*)GetDlgItem(IDC_CHECK8);
	if (check->GetCheck()) {
		m_bDeffectDisplay[7] = TRUE;
		CString cstr = L"8_1";
		::SendNotifyMessageW(hMainWnd, WM_UPDATE_MAINWND, (WPARAM)&cstr, 0);
	}
	else {
		m_bDeffectDisplay[7] = FALSE;
		CString cstr = L"8_0";
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

void CInspectDlg::OnBnClickedMfccolorbutton5()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON5);
	m_color5 = pcolorbtn->GetColor();

	::SendNotifyMessageW(hMainWnd, WM_UPDATE_CONTROLS, 0, 0);
}

void CInspectDlg::OnBnClickedMfccolorbutton6()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON6);
	m_color6 = pcolorbtn->GetColor();

	::SendNotifyMessageW(hMainWnd, WM_UPDATE_CONTROLS, 0, 0);
}

void CInspectDlg::OnBnClickedMfccolorbutton7()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON7);
	m_color7 = pcolorbtn->GetColor();

	::SendNotifyMessageW(hMainWnd, WM_UPDATE_CONTROLS, 0, 0);
}

void CInspectDlg::OnBnClickedMfccolorbutton8()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON8);
	m_color8 = pcolorbtn->GetColor();

	::SendNotifyMessageW(hMainWnd, WM_UPDATE_CONTROLS, 0, 0);
}
