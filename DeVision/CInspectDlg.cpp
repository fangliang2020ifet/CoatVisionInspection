// CInspectDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "DeVisionDlg.h"
#include "CInspectDlg.h"
#include "afxdialogex.h"

// CInspectDlg 对话框

//图像采集类

IMPLEMENT_DYNAMIC(CInspectDlg, CDialogEx)

CInspectDlg::CInspectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_INSPECT, pParent)
{

	m_Acq1 = NULL;
	m_Acq2 = NULL;
	m_Acq3 = NULL;
	m_Acq4 = NULL;
	m_AcqDevice1 = NULL;
	m_AcqDevice2 = NULL;
	m_AcqDevice3 = NULL;
	m_AcqDevice4 = NULL;
	m_Buffers1 = NULL;
	m_Buffers2 = NULL;
	m_Buffers3 = NULL;
	m_Buffers4 = NULL;
	m_Xfer1 = NULL;
	m_Xfer2 = NULL;
	m_Xfer3 = NULL;
	m_Xfer4 = NULL;
	m_View1 = NULL;
	m_View2 = NULL;
	m_View3 = NULL;
	m_View4 = NULL;

	m_camera_system_initialled = FALSE;
	m_bShowCameraLinkServers = TRUE;
	m_is_system_pause = FALSE;

	camera1_frame_count = 0;
	camera2_frame_count = 0;
	camera3_frame_count = 0;
	camera4_frame_count = 0;
	camera1_trash_count = 0;
	camera2_trash_count = 0;
	camera3_trash_count = 0;
	camera4_trash_count = 0;
	total_frame_count = 0;
	total_trash_count = 0;
}

CInspectDlg::~CInspectDlg()
{

}

void CInspectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_CAMERA_IMAGE1, m_ImageWnd);
	DDX_Control(pDX, IDC_CAMERA_IMAGE1, m_ImageWnd1);
	DDX_Control(pDX, IDC_CAMERA_IMAGE2, m_ImageWnd2);
	DDX_Control(pDX, IDC_CAMERA_IMAGE3, m_ImageWnd3);
	DDX_Control(pDX, IDC_CAMERA_IMAGE4, m_ImageWnd4);
	DDX_Control(pDX, IDC_LIST_RECORD, m_listLog);
	DDX_Control(pDX, IDC_LIST_WARNNING, m_listWarning);
	DDX_Control(pDX, IDC_BUTTON_CHANGEINFO, m_btn_changeinfo);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_NUMBER, m_eNumber);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_WIDTH, m_eWidth);
	DDX_Control(pDX, IDC_EDIT_PRODUCT_MODEL, m_eModel);
	DDX_Control(pDX, IDC_EDIT_OPERATOR, m_eOperator);
}


BEGIN_MESSAGE_MAP(CInspectDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEINFO, &CInspectDlg::OnBnClickedButton1)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
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

	camera1_show_buffer = TRUE;
	camera2_show_buffer = TRUE;
	camera3_show_buffer = TRUE;
	camera4_show_buffer = TRUE;

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
	DestroyObjects();
	// Delete all objects
	if (m_Xfer1)			delete m_Xfer1;
	if (m_Xfer2)			delete m_Xfer2;
	if (m_Xfer3)			delete m_Xfer3;
	if (m_Xfer4)			delete m_Xfer4;
	if (m_View1)			delete m_View1;
	if (m_View2)			delete m_View2;
	if (m_View3)			delete m_View3;
	if (m_View4)			delete m_View4;
	if (m_Buffers1)		delete m_Buffers1;
	if (m_Buffers2)		delete m_Buffers2;
	if (m_Buffers3)		delete m_Buffers3;
	if (m_Buffers4)		delete m_Buffers4;
	if (m_Feature1)     delete m_Feature1;
	if (m_Feature2)     delete m_Feature2;
	if (m_Feature3)     delete m_Feature3;
	if (m_Feature4)     delete m_Feature4;
	if (m_AcqDevice1)    delete m_AcqDevice1;
	if (m_AcqDevice2)    delete m_AcqDevice2;
	if (m_AcqDevice3)    delete m_AcqDevice3;
	if (m_AcqDevice4)    delete m_AcqDevice4;
	if (m_Acq1)			delete m_Acq1;
	if (m_Acq2)			delete m_Acq2;
	if (m_Acq3)			delete m_Acq3;
	if (m_Acq4)			delete m_Acq4;


	while (m_pImgProc->IsThreadsAlive())
	{
		m_pImgProc->StopProcess();
		Sleep(50);
	}
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
	if (m_ImageWnd1.GetSafeHwnd())
	{
		CRect rWnd;
		m_ImageWnd1.GetWindowRect(rWnd);
		ScreenToClient(rWnd);
		//rWnd.right = rClient.right - 5;
		//rWnd.bottom = rClient.bottom - 5;
		m_ImageWnd1.MoveWindow(rWnd);
	}
	if (m_ImageWnd2.GetSafeHwnd())
	{
		CRect rWnd;
		m_ImageWnd2.GetWindowRect(rWnd);
		ScreenToClient(rWnd);
		//rWnd.right = rClient.right - 5;
		//rWnd.bottom = rClient.bottom - 5;
		m_ImageWnd2.MoveWindow(rWnd);
	}
	if (m_ImageWnd3.GetSafeHwnd())
	{
		CRect rWnd;
		m_ImageWnd3.GetWindowRect(rWnd);
		ScreenToClient(rWnd);
		//rWnd.right = rClient.right - 5;
		//rWnd.bottom = rClient.bottom - 5;
		m_ImageWnd3.MoveWindow(rWnd);
	}
	if (m_ImageWnd4.GetSafeHwnd())
	{
		CRect rWnd;
		m_ImageWnd4.GetWindowRect(rWnd);
		ScreenToClient(rWnd);
		//rWnd.right = rClient.right - 5;
		//rWnd.bottom = rClient.bottom - 5;
		m_ImageWnd4.MoveWindow(rWnd);
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
	CString cstr_log;
	CString strSpace, strNextLine, strTime;
	strSpace.Format(_T("%s"), _T("  "));
	strNextLine.Format(_T("%s"), _T("\r\n"));

	CTime tm; tm = CTime::GetCurrentTime();
	strTime = tm.Format("%X:");
	cstr_log += strTime;
	cstr_log += strSpace;
	cstr_log += cstr;
	cstr_log += strNextLine;

	m_listWarning.AddString(cstr_log);
	m_listWarning.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
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
	CString cstr_log;
	CString strSpace, strNextLine, strTime;
	strSpace.Format(_T("%s"), _T("  "));
	strNextLine.Format(_T("%s"), _T("\r\n"));

	CTime tm; tm = CTime::GetCurrentTime();
	strTime = tm.Format("%X:");
	cstr_log += strTime;
	cstr_log += strSpace;
	cstr_log += cstr;
	cstr_log += strNextLine;

	m_listLog.AddString(cstr_log);
	m_listLog.PostMessageW(WM_VSCROLL, SB_BOTTOM, 0);
}

//修改产品信息
void CInspectDlg::OnBnClickedButton1()
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

//修改颜色
void CInspectDlg::OnBnClickedMfccolorbutton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON1);
	m_color1 = pcolorbtn->GetColor();
}

void CInspectDlg::OnBnClickedMfccolorbutton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON2);
	m_color2 = pcolorbtn->GetColor();
}

void CInspectDlg::OnBnClickedMfccolorbutton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON3);
	m_color3 = pcolorbtn->GetColor();
}

void CInspectDlg::OnBnClickedMfccolorbutton4()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON4);
	m_color4 = pcolorbtn->GetColor();
}

void CInspectDlg::OnBnClickedMfccolorbutton5()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON5);
	m_color5 = pcolorbtn->GetColor();
}

void CInspectDlg::OnBnClickedMfccolorbutton6()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON6);
	m_color6 = pcolorbtn->GetColor();
}

void CInspectDlg::OnBnClickedMfccolorbutton7()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON7);
	m_color7 = pcolorbtn->GetColor();
}

void CInspectDlg::OnBnClickedMfccolorbutton8()
{
	// TODO: 在此添加控件通知处理程序代码
	CMFCColorButton *pcolorbtn = (CMFCColorButton*)GetDlgItem(IDC_MFCCOLORBUTTON8);
	m_color8 = pcolorbtn->GetColor();
}

/************************************相机处理部分**************************************/
//初始化图像采集卡
BOOL CInspectDlg::InitServer()
{
	m_vServerName.clear();

	for (int i = 0; i < GetServerCount(); i++)
	{
		// Does this server support "Acq" (frame-grabber) or "AcqDevice" (camera)?

		bool bAcq = (m_serverCategory == ServerAcq || m_serverCategory == ServerAll)
			&& (GetResourceCount(i, ResourceAcq) > 0);

		// when m_serverCategory is ServerAcqDevice, show only servers that have only one resource that is an acqdevice and no acq
		bool bAcqDevice = (m_serverCategory == ServerAcqDevice || m_serverCategory == ServerAll)
			&& (GetResourceCount(i, ResourceAcqDevice) == 1) && (GetResourceCount(i, ResourceAcq) == 0);

		if (bAcq || bAcqDevice || m_bShowCameraLinkServers)
		{
			char name[128];
			if (GetServerName(i, name, sizeof(name)))
			{
				if (strstr(name, "CameraLink_") != NULL)
				{
					if (bAcq || m_bShowCameraLinkServers)
						m_vServerName.push_back(CString(name));
						//m_cbServer.AddString(CString(name));
				}
			}
		}
	}

	// At least one server must be available
	if (m_vServerName.size() <= 0)
	{
		CString cstr = L"未发现采集设备，请检查线路和电源连接";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}
	if (m_vServerName.size() < 4)
	{
		CString cstr = L"采集设备数量少于 4，请检查线路和电源连接";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}

	return TRUE;
}

BOOL CInspectDlg::CameraSystemInitial()
{
	if (!m_camera_system_initialled) {
		if (!InitServer()) {
			return FALSE;
		}

		/*
		if (!InitialBoard1())
		{
			Win::log("1#相机初始化失败");
			RecordLogList(L"1#相机初始化失败");
			return FALSE;
		}
		if (!InitialBoard2())
		{
			Win::log("2#相机初始化失败");
			RecordLogList(L"2#相机初始化失败");
			return FALSE;
		}
		if (!InitialBoard3())
		{
			Win::log("13相机初始化失败");
			RecordLogList(L"3#相机初始化失败");
			return FALSE;
		}
		if (!InitialBoard4())
		{
			Win::log("4#相机初始化失败");
			RecordLogList(L"4#相机初始化失败");
			return FALSE;
		}

		if (!CreateObjects()) {
			Win::log("相机初始化：对象创建失败");
			RecordLogList(L"相机初始化：对象创建失败");
			return FALSE;
		}
		InitializeCWndAndView();
		*/

		InitialAllBoards();
	}
	m_camera_system_initialled = TRUE;

	CString cstr = L"采集系统初始化完成";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	return TRUE;
}

//初始化采集卡
BOOL CInspectDlg::InitialAllBoards()
{
	if ( FREE_RUN ) {
		char free_run_1[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_1.ccf";
		char free_run_2[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_2.ccf";
		char free_run_3[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_3.ccf";
		char free_run_4[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_4.ccf";
		memcpy(configFilename1, free_run_1, sizeof(free_run_1));
		memcpy(configFilename2, free_run_2, sizeof(free_run_2));
		memcpy(configFilename3, free_run_3, sizeof(free_run_3));
		memcpy(configFilename4, free_run_4, sizeof(free_run_4));
		CString cstr = L"已设为内部触发模式";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}
	else {
		char encode_trigger_1[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_External_Trigger_Board1.ccf";
		char encode_trigger_2[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_External_Trigger_Board2.ccf";
		char encode_trigger_3[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_External_Trigger_Board3.ccf";
		char encode_trigger_4[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_External_Trigger_Board4.ccf";
		memcpy(configFilename1, encode_trigger_1, sizeof(encode_trigger_1));
		memcpy(configFilename2, encode_trigger_2, sizeof(encode_trigger_2));
		memcpy(configFilename3, encode_trigger_3, sizeof(encode_trigger_3));
		memcpy(configFilename4, encode_trigger_4, sizeof(encode_trigger_4));
		CString cstr = L"已设为外部触发模式";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}

	if (1)
	{
		//板卡
		SapLocation loc1(acqServerName1, m_acqDeviceNumber1);
		m_Acq1 = new SapAcquisition(loc1, configFilename1);
		m_Buffers1 = new SapBufferWithTrash(2, m_Acq1);  // 双缓存
		m_Xfer1 = new SapAcqToBuf(m_Acq1, m_Buffers1, AcqCallback1, this);
		m_View1 = new SapView(m_Buffers1);

		SapLocation loc2(acqServerName2, m_acqDeviceNumber2);
		m_Acq2 = new SapAcquisition(loc2, configFilename2);
		m_Buffers2 = new SapBufferWithTrash(2, m_Acq2);  // 双缓存
		m_Xfer2 = new SapAcqToBuf(m_Acq2, m_Buffers2, AcqCallback2, this);
		m_View2 = new SapView(m_Buffers2);

		SapLocation loc3(acqServerName3, m_acqDeviceNumber3);
		m_Acq3 = new SapAcquisition(loc3, configFilename3);
		m_Buffers3 = new SapBufferWithTrash(2, m_Acq3);  // 双缓存
		m_Xfer3 = new SapAcqToBuf(m_Acq3, m_Buffers3, AcqCallback3, this);
		m_View3 = new SapView(m_Buffers3);

		SapLocation loc4(acqServerName4, m_acqDeviceNumber4);
		m_Acq4 = new SapAcquisition(loc4, configFilename4);
		m_Buffers4 = new SapBufferWithTrash(2, m_Acq4);  // 双缓存
		m_Xfer4 = new SapAcqToBuf(m_Acq4, m_Buffers4, AcqCallback4, this);
		m_View4 = new SapView(m_Buffers4);
	}
	if (1)
	{
		//相机
		SapLocation loc1(acqDeviceName1, m_acqDeviceNumber1);
		m_AcqDevice1 = new SapAcqDevice(loc1, FALSE);

		SapLocation loc2(acqDeviceName2, m_acqDeviceNumber2);
		m_AcqDevice2 = new SapAcqDevice(loc2, FALSE);

		SapLocation loc3(acqDeviceName3, m_acqDeviceNumber3);
		m_AcqDevice3 = new SapAcqDevice(loc3, FALSE);

		SapLocation loc4(acqDeviceName4, m_acqDeviceNumber4);
		m_AcqDevice4 = new SapAcqDevice(loc4, FALSE);

		m_Feature1 = new SapFeature(acqDeviceName1);
		m_Feature2 = new SapFeature(acqDeviceName2);
		m_Feature3 = new SapFeature(acqDeviceName3);
		m_Feature4 = new SapFeature(acqDeviceName4);
	}
	// Attach sapview to image viewer
	m_ImageWnd1.AttachSapView(m_View1);
	m_ImageWnd2.AttachSapView(m_View2);
	m_ImageWnd3.AttachSapView(m_View3);
	m_ImageWnd4.AttachSapView(m_View4);

	// Create all objects
	if (!CreateObjects()) { EndDialog(TRUE); return FALSE; }

	//外部触发时无需设置以下参数
	if (FREE_RUN) {
		m_AcqDevice1->SetFeatureValue("TriggerMode", "Off");//触发模式关闭
		m_AcqDevice1->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定触发频率
		m_AcqDevice2->SetFeatureValue("TriggerMode", "Off");//触发模式打开
		m_AcqDevice2->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定曝光时间
		m_AcqDevice3->SetFeatureValue("TriggerMode", "Off");//触发模式打开
		m_AcqDevice3->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定曝光时间
		m_AcqDevice4->SetFeatureValue("TriggerMode", "Off");//触发模式打开
		m_AcqDevice4->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定曝光时间
	}
	else {
		m_AcqDevice1->SetFeatureValue("TriggerMode", "On");//触发模式打开
		m_AcqDevice2->SetFeatureValue("TriggerMode", "On");//触发模式打开
		m_AcqDevice3->SetFeatureValue("TriggerMode", "On");//触发模式打开
		m_AcqDevice4->SetFeatureValue("TriggerMode", "On");//触发模式打开
	}

	m_ImageWnd1.AttachEventHandler(this);
	m_ImageWnd1.CenterImage(true);
	m_ImageWnd1.Reset();

	m_ImageWnd2.AttachEventHandler(this);
	m_ImageWnd2.CenterImage(true);
	m_ImageWnd2.Reset();

	m_ImageWnd3.AttachEventHandler(this);
	m_ImageWnd3.CenterImage(true);
	m_ImageWnd3.Reset();

	m_ImageWnd4.AttachEventHandler(this);
	m_ImageWnd4.CenterImage(true);
	m_ImageWnd4.Reset();

	return TRUE;
}

BOOL CInspectDlg::InitialBoard1()
{

	if (1)
	{
		// Define on-line objects
		SapLocation loc(acqServerName1, m_acqDeviceNumber1);
		m_Acq1 = new SapAcquisition(loc, configFilename1);
		m_Buffers1 = new SapBufferWithTrash(2, m_Acq1);  // 双缓存
		m_Xfer1 = new SapAcqToBuf(m_Acq1, m_Buffers1, AcqCallback1, this);
	}

	//初始化相机参数
	if (!SetCameraParemeter1()) {
		Win::log("1#相机设置失败，请检查线路和电源连接");
		RecordWarning(L"1#相机设置失败，请检查线路和电源连接");
		return FALSE;
	}

	// Define other objects
	m_View1 = new SapView(m_Buffers1);


	Win::log("1#相机初始化完成");
	RecordLogList(L"1#相机初始化");

	return TRUE;
}

BOOL CInspectDlg::InitialBoard2()
{

	if (1)
	{
		// Define on-line objects
		SapLocation loc(acqServerName2, m_acqDeviceNumber2);
		m_Acq2 = new SapAcquisition(loc, configFilename2);
		m_Buffers2 = new SapBufferWithTrash(2, m_Acq2);  //缓存的数量
		m_Xfer2 = new SapAcqToBuf(m_Acq2, m_Buffers2, AcqCallback2, this);
	}

	//初始化相机参数
	if (!SetCameraParemeter2()) {
		Win::log("2#相机设置失败，请检查线路和电源连接");
		RecordWarning(L"2#相机设置失败，请检查线路和电源连接");
		return FALSE;
	}

	// Define other objects
	m_View2 = new SapView(m_Buffers2);

	Win::log("2#相机初始化完成");
	RecordLogList(L"2#相机初始化");

	return TRUE;
}

BOOL CInspectDlg::InitialBoard3()
{
	if (1)
	{
		// Define on-line objects
		SapLocation loc(acqServerName3, m_acqDeviceNumber3);
		m_Acq3 = new SapAcquisition(loc, configFilename3);
		m_Buffers3 = new SapBufferWithTrash(2, m_Acq3);  //缓存的数量
		m_Xfer3 = new SapAcqToBuf(m_Acq3, m_Buffers3, AcqCallback3, this);
	}

	//初始化相机参数
	if (!SetCameraParemeter3()) {
		Win::log("3#相机设置失败，请检查线路和电源连接");
		RecordWarning(L"3#相机设置失败，请检查线路和电源连接");
		return FALSE;
	}

	// Define other objects
	m_View3 = new SapView(m_Buffers3);

	Win::log("3#相机初始化完成");
	RecordLogList(L"3#相机初始化");

	return TRUE;
}

BOOL CInspectDlg::InitialBoard4()
{
	if (1)
	{
		// Define on-line objects
		SapLocation loc(acqServerName4, m_acqDeviceNumber4);
		m_Acq4 = new SapAcquisition(loc, configFilename4);
		m_Buffers4 = new SapBufferWithTrash(2, m_Acq4);  //缓存的数量
		m_Xfer4 = new SapAcqToBuf(m_Acq4, m_Buffers4, AcqCallback4, this);
	}

	//初始化相机参数
	if (!SetCameraParemeter4()) {
		Win::log("4#相机设置失败，请检查线路和电源连接");
		RecordWarning(L"4#相机设置失败，请检查线路和电源连接");
		return FALSE;
	}

	// Define other objects
	m_View4 = new SapView(m_Buffers4);

	Win::log("4#相机初始化完成");
	RecordLogList(L"4#相机初始化");

	return TRUE;
}

BOOL CInspectDlg::SetCameraParemeter1()
{
	SapLocation loc2(acqDeviceName1, m_acqDeviceNumber1);
	m_AcqDevice1 = new SapAcqDevice(loc2, FALSE);
	//create SapAcqDevice
	if (!m_AcqDevice1->Create())
		return FALSE;

	m_Feature1 = new SapFeature(acqDeviceName1);
	if (!m_Feature1->Create())
		return FALSE;



	m_AcqDevice1->SetFeatureValue("TriggerMode", "On");//触发模式打开
	m_AcqDevice1->SetFeatureValue("ExposureTime", 15.0);//设定曝光时间

	return TRUE;
}

BOOL CInspectDlg::SetCameraParemeter2()
{
	SapLocation loc2(acqDeviceName2, m_acqDeviceNumber2);
	m_AcqDevice2 = new SapAcqDevice(loc2, FALSE);
	//create SapAcqDevice
	if (!m_AcqDevice2->Create())
		return FALSE;

	//m_Feature2 = new SapFeature(acqDeviceName2);
	//if(!m_Feature2->Create())
	//	return FALSE;

	//m_Feature->GetDescription();


	//m_AcqDevice2->SetFeatureValue("TriggerMode", "On");//触发模式打开
	//m_AcqDevice2->SetFeatureValue("ExposureTime", 15.0);//设定曝光时间

	return TRUE;
}

BOOL CInspectDlg::SetCameraParemeter3()
{
	SapLocation loc2(acqDeviceName3, m_acqDeviceNumber3);
	m_AcqDevice3 = new SapAcqDevice(loc2, FALSE);

	//create SapAcqDevice
	if (!m_AcqDevice3->Create())
		return FALSE;

	//m_Feature3 = new SapFeature(acqDeviceName3);
	//if(!m_Feature3->Create())
	//	return FALSE;

	//m_Feature->GetDescription();


	//m_AcqDevice3->SetFeatureValue("TriggerMode", "On");//触发模式打开
	//m_AcqDevice3->SetFeatureValue("ExposureTime", 15.0);//设定曝光时间

	return TRUE;
}

BOOL CInspectDlg::SetCameraParemeter4()
{
	SapLocation loc2(acqDeviceName4, m_acqDeviceNumber4);
	m_AcqDevice4 = new SapAcqDevice(loc2, FALSE);
	//create SapAcqDevice
	if (!m_AcqDevice4->Create())
		return FALSE;

	//m_Feature4 = new SapFeature(acqDeviceName4);
	//if(!m_Feature4->Create())
	//	return FALSE;

	//m_Feature->GetDescription();


	//m_AcqDevice4->SetFeatureValue("TriggerMode", "On");//触发模式打开
	//m_AcqDevice4->SetFeatureValue("ExposureTime", 15.0);//设定曝光时间

	return TRUE;
}

//显示图像
BOOL CInspectDlg::InitializeCWndAndView()
{
	// Attach sapview to image viewer
	if (m_View1)
		m_ImageWnd1.AttachSapView(m_View1);
	if (m_View2)
		m_ImageWnd2.AttachSapView(m_View2);
	if (m_View3)
		m_ImageWnd3.AttachSapView(m_View3);
	if (m_View4)
		m_ImageWnd4.AttachSapView(m_View4);


	// Create view object
	if (m_View1)
	{
		m_View1->Create();
	}
	if (m_View2)
	{
		m_View2->Create();
	}
	if (m_View3)
	{
		m_View3->Create();
	}
	if (m_View4)
	{
		m_View4->Create();
	}

	//m_ImageWnd1.AttachEventHandler(this);
	m_ImageWnd1.CenterImage(true);
	m_ImageWnd1.Reset();

	m_ImageWnd2.CenterImage(true);
	m_ImageWnd2.Reset();

	m_ImageWnd3.AttachEventHandler(this);
	m_ImageWnd3.CenterImage(true);
	m_ImageWnd3.Reset();

	m_ImageWnd4.AttachEventHandler(this);
	m_ImageWnd4.CenterImage(true);
	m_ImageWnd4.Reset();

	camera1_show_buffer = TRUE;
	camera2_show_buffer = TRUE;
	camera3_show_buffer = TRUE;
	camera4_show_buffer = TRUE;

	return TRUE;
}

BOOL CInspectDlg::CreateObjects()
{
	CWaitCursor wait;

	// Create acquisition object
	if (m_Acq1 && !*m_Acq1 && !m_Acq1->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Acq2 && !*m_Acq2 && !m_Acq2->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Acq3 && !*m_Acq3 && !m_Acq3->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Acq4 && !*m_Acq4 && !m_Acq4->Create())
	{
		DestroyObjects();
		return FALSE;
	}

	//AcqDevice
	if (m_AcqDevice1 && !*m_AcqDevice1 && !m_AcqDevice1->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_AcqDevice2 && !*m_AcqDevice2 && !m_AcqDevice2->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_AcqDevice3 && !*m_AcqDevice3 && !m_AcqDevice3->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_AcqDevice4 && !*m_AcqDevice4 && !m_AcqDevice4->Create())
	{
		DestroyObjects();
		return FALSE;
	}

	//Feature
	if (m_Feature1 && !*m_Feature1 && !m_Feature1->Create()) {
		DestroyObjects();
		return FALSE;
	}
	if (m_Feature2 && !*m_Feature2 && !m_Feature2->Create()) {
		DestroyObjects();
		return FALSE;
	}
	if (m_Feature3 && !*m_Feature3 && !m_Feature3->Create()) {
		DestroyObjects();
		return FALSE;
	}
	if (m_Feature4 && !*m_Feature4 && !m_Feature4->Create()) {
		DestroyObjects();
		return FALSE;
	}


	// Create buffer object
	if (m_Buffers1 && !*m_Buffers1)
	{
		if (!m_Buffers1->Create())
		{
			DestroyObjects();
			return FALSE;
		}
		// Clear all buffers
		m_Buffers1->Clear();
	}
	if (m_Buffers2 && !*m_Buffers2)
	{
		if (!m_Buffers2->Create())
		{
			DestroyObjects();
			return FALSE;
		}
		// Clear all buffers
		m_Buffers2->Clear();
	}
	if (m_Buffers3 && !*m_Buffers3)
	{
		if (!m_Buffers3->Create())
		{
			DestroyObjects();
			return FALSE;
		}
		// Clear all buffers
		m_Buffers3->Clear();
	}
	if (m_Buffers4 && !*m_Buffers4)
	{
		if (!m_Buffers4->Create())
		{
			DestroyObjects();
			return FALSE;
		}
		// Clear all buffers
		m_Buffers4->Clear();
	}


	// Create view object
	if (m_View1 && !*m_View1 && !m_View1->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_View2 && !*m_View2 && !m_View2->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_View3 && !*m_View3 && !m_View3->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_View4 && !*m_View4 && !m_View4->Create())
	{
		DestroyObjects();
		return FALSE;
	}


	// Create transfer object
	if (m_Xfer1 && !*m_Xfer1 && !m_Xfer1->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Xfer2 && !*m_Xfer2 && !m_Xfer2->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Xfer3 && !*m_Xfer3 && !m_Xfer3->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Xfer4 && !*m_Xfer4 && !m_Xfer4->Create())
	{
		DestroyObjects();
		return FALSE;
	}

	Win::log("创建缓存对象成功");
	RecordLogList(L"创建缓存对象成功");

	return TRUE;

}

BOOL CInspectDlg::DestroyObjects()
{
	m_camera1_thread_alive = FALSE;
	m_camera2_thread_alive = FALSE;
	m_camera3_thread_alive = FALSE;
	m_camera4_thread_alive = FALSE;

	// Destroy transfer object
	if (m_Xfer1 && *m_Xfer1) m_Xfer1->Destroy();
	if (m_Xfer2 && *m_Xfer2) m_Xfer2->Destroy();
	if (m_Xfer3 && *m_Xfer3) m_Xfer3->Destroy();
	if (m_Xfer4 && *m_Xfer4) m_Xfer4->Destroy();

	// Destroy view object
	if (m_View1 && *m_View1) m_View1->Destroy();
	if (m_View2 && *m_View2) m_View2->Destroy();
	if (m_View3 && *m_View3) m_View3->Destroy();
	if (m_View4 && *m_View4) m_View4->Destroy();

	// Destroy buffer object
	if (m_Buffers1 && *m_Buffers1) m_Buffers1->Destroy();
	if (m_Buffers2 && *m_Buffers2) m_Buffers2->Destroy();
	if (m_Buffers3 && *m_Buffers3) m_Buffers3->Destroy();
	if (m_Buffers4 && *m_Buffers4) m_Buffers4->Destroy();

	if (m_Feature1 && *m_Feature1) m_Feature1->Destroy();
	if (m_Feature2 && *m_Feature2) m_Feature2->Destroy();
	if (m_Feature3 && *m_Feature3) m_Feature3->Destroy();
	if (m_Feature4 && *m_Feature4) m_Feature4->Destroy();

	if (m_AcqDevice1 && *m_AcqDevice1) m_AcqDevice1->Destroy();
	if (m_AcqDevice2 && *m_AcqDevice2) m_AcqDevice2->Destroy();
	if (m_AcqDevice3 && *m_AcqDevice3) m_AcqDevice3->Destroy();
	if (m_AcqDevice4 && *m_AcqDevice4) m_AcqDevice4->Destroy();

	// Destroy acquisition object
	if (m_Acq1 && *m_Acq1) m_Acq1->Destroy();
	if (m_Acq2 && *m_Acq2) m_Acq2->Destroy();
	if (m_Acq3 && *m_Acq3) m_Acq3->Destroy();
	if (m_Acq4 && *m_Acq4) m_Acq4->Destroy();


	Win::log("释放缓存对象成功");
	RecordLogList(L"释放缓存对象成功");

	return TRUE;
}

//开始连续采集
int CInspectDlg::Grab()
{
	//读取测试图像
	if (m_pImgProc->TEST_MODEL) {
		std::string test_name = "E:/DeVisionProject/OneCamera_0417/test1";
		m_pImgProc->LoadSingleImage(test_name);
		CString cpath = CA2W(test_name.c_str());
		CString cstr = L"已加载测试图像: " + cpath;
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}

	if (m_Xfer1->Grab() && m_Xfer2->Grab() && m_Xfer3->Grab() && m_Xfer4->Grab()) {
		m_camera1_thread_alive = TRUE;
		m_camera2_thread_alive = TRUE;
		m_camera3_thread_alive = TRUE;
		m_camera4_thread_alive = TRUE;

		CString cstr = L"开始采集图像";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
		return 0;
	}
	else {
		CString cstr = L"图像采集失败";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return -1;
	}
}

//采集一帧
void CInspectDlg::Snap()
{
	if (!m_Xfer1->Snap()) {
		Win::log("1#相机获取图像失败");
		RecordWarning(L"1#相机获取图像失败");
		return;
	}
	if (!m_Xfer2->Snap()) {
		Win::log("2#相机获取图像失败");
		RecordWarning(L"2#相机获取图像失败");
		return;
	}
	if (!m_Xfer3->Snap()) {
		Win::log("3#相机获取图像失败");
		RecordWarning(L"3#相机获取图像失败");
		return;
	}
	if (!m_Xfer4->Snap()) {
		Win::log("4#相机获取图像失败");
		RecordWarning(L"4#相机获取图像失败");
		return;
	}

	CString cstr = L"采集一张图像";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	return;
}

//停止采集
int CInspectDlg::Freeze()
{
	if (m_Xfer1->Freeze() && m_Xfer2->Freeze() && m_Xfer3->Freeze() && m_Xfer4->Freeze()) {
		if (CAbortDlg(this, m_Xfer1).DoModal() != IDOK)
			m_Xfer1->Abort();
		if (CAbortDlg(this, m_Xfer2).DoModal() != IDOK)
			m_Xfer2->Abort();
		if (CAbortDlg(this, m_Xfer3).DoModal() != IDOK)
			m_Xfer3->Abort();
		if (CAbortDlg(this, m_Xfer4).DoModal() != IDOK)
			m_Xfer4->Abort();

		m_camera1_thread_alive = FALSE;
		m_camera2_thread_alive = FALSE;
		m_camera3_thread_alive = FALSE;
		m_camera4_thread_alive = FALSE;

		CString cstr = L"停止采集";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

		return 0;
	}
	else {
		CString cstr = L"停止图像采集失败";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return -1;
	}
}

//重启检测
void CInspectDlg::RestartInspect()
{
	total_frame_count = 0;
	camera1_frame_count = 0;
	camera2_frame_count = 0;
	camera3_frame_count = 0;
	camera4_frame_count = 0;

	total_trash_count = 0;
	camera1_trash_count = 0;
	camera2_trash_count = 0;
	camera3_trash_count = 0;
	camera4_trash_count = 0;

	return;
}

//计算编码器速度
float CInspectDlg::CalculateEncoderSpeed()
{
	SapXferFrameRateInfo* pStats = m_Xfer1->GetFrameRateStatistics();

	if (pStats->IsLiveFrameRateAvailable() && !pStats->IsLiveFrameRateStalled())
	{
		float speed = pStats->GetLiveFrameRate() * 491.52f * VERTICAL_PRECISION;//8192 * 60 / 1000      0.05
		//TRACE("frame_rate = %.1f\n", pStats->GetLiveFrameRate());

		return speed;
	}
	else
	{
		return 0.0;
	}
}

//计算总帧数
UINT64 CInspectDlg::GetTotalFrameCount()
{
	total_frame_count = camera1_frame_count + camera2_frame_count +
						camera3_frame_count + camera4_frame_count;
	
	return total_frame_count;
}

int CInspectDlg::GetTotalTrashCount()
{
	total_trash_count = camera1_trash_count + camera2_trash_count +
						camera3_trash_count + camera4_trash_count;

	return total_trash_count;
}

void CInspectDlg::GenerateHImage(SapBuffer* m_Buffer, int index, HImage &m_HImage)
{
	BYTE pData;
	void* pDataAddr = &pData;

	m_Buffer->GetAddress(index, &pDataAddr);
	int width = m_Buffer->GetWidth();
	int height = m_Buffer->GetHeight();
	m_HImage.GenImage1("byte", width, height, pDataAddr);
	m_Buffer->ReleaseAddress(pDataAddr);
}

void CInspectDlg::SaveImageFromBuffer(SapBuffer* m_Buffer, int index)
{
	BYTE pData;
	void* pDataAddr = &pData;
	m_Buffer->GetAddress(index, &pDataAddr);
	int width = m_Buffer->GetWidth();
	int height = m_Buffer->GetHeight();
	HImage m_HImage;
	m_HImage.GenImage1("byte", width, height, pDataAddr);

	long t = GetTickCount();
	CString cfilename;
	cfilename.Format(_T("%d"), t);   //格式化日期时间
	const wchar_t* filename = (LPCTSTR)cfilename;
	m_HImage.WriteImage("bmp", 0, filename);

	m_Buffer->ReleaseAddress(pDataAddr);
}

HObject CInspectDlg::CopyHobject(HObject ho_image)
{
	HObject copy;
	CopyImage(ho_image, &copy);

	return copy;
}

//图像处理入口
void CInspectDlg::AcqCallback1(SapXferCallbackInfo *pInfo)
{
	CInspectDlg *pDlg = (CInspectDlg*)pInfo->GetContext();
	int static_count = pDlg->m_static_count1;
	if (pInfo->IsTrash())
	{
		pDlg->camera1_trash_count += 1;
		CString cstr = L"1#相机丢帧";
		::SendNotifyMessageW(pDlg->hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
	}
	else
	{
		if (pDlg->camera1_show_buffer)
		{
			pDlg->m_View1->Show();
		}

		HImage ho_image;
		//切换双缓存
		pDlg->GenerateHImage(pDlg->m_Buffers1, static_count,ho_image);
		
		//是否需要增加判断 camera1  List 的大小总计不超过 100帧
		if (!pDlg->m_is_system_pause) {
			pDlg->camera1_frame_count += 1;

			//总帧数 / 每个相机的线程数
			int change_index = pDlg->camera1_frame_count % pDlg->m_pImgProc->m_threadnum;
			switch (change_index)
			{
			case 0:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList1_1.push_back(pDlg->m_pImgProc->m_hi_test1);
				else
					pDlg->m_pImgProc->m_ImgList1_1.push_back(ho_image);
				break;
			case 1:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList1_2.push_back(pDlg->m_pImgProc->m_hi_test1);
				else
					pDlg->m_pImgProc->m_ImgList1_2.push_back(ho_image);
				break;
			case 2:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList1_3.push_back(pDlg->m_pImgProc->m_hi_test1);
				else
					pDlg->m_pImgProc->m_ImgList1_3.push_back(ho_image);
				break;
			case 3:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList1_4.push_back(pDlg->m_pImgProc->m_hi_test1);
				else
					pDlg->m_pImgProc->m_ImgList1_4.push_back(ho_image);
				break;
			case 4:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList1_5.push_back(pDlg->m_pImgProc->m_hi_test1);
				else
					pDlg->m_pImgProc->m_ImgList1_5.push_back(ho_image);
				break;
			default:
				break;
			}
		}
		
		if (static_count == 0) {
			pDlg->m_static_count1 = 1;
			//HalconCpp::WriteImage(ho_image, "bmp", 0, "D:/SaveImage/save1.bmp");
		}
		else if(static_count == 1){
			pDlg->m_static_count1 = 0;
			//HalconCpp::WriteImage(ho_image, "bmp", 0, "D:/SaveImage/save2.bmp");

		}
	}
}

void CInspectDlg::AcqCallback2(SapXferCallbackInfo *pInfo)
{
	CInspectDlg *pDlg = (CInspectDlg*)pInfo->GetContext();
	int static_count = pDlg->m_static_count2;
	if (pInfo->IsTrash())
	{
		pDlg->camera2_trash_count += 1;
		CString cstr = L"2#相机丢帧";
		::SendNotifyMessageW(pDlg->hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
	}
	else
	{

		if (pDlg->camera2_show_buffer)
		{
			pDlg->m_View2->Show();
		}
		HImage ho_image;
		pDlg->GenerateHImage(pDlg->m_Buffers2, static_count, ho_image);

		if (!pDlg->m_is_system_pause) {
			pDlg->camera2_frame_count += 1;

			int change_index = pDlg->camera2_frame_count % pDlg->m_pImgProc->m_threadnum;
			switch (change_index)
			{
			case 0:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList2_1.push_back(pDlg->m_pImgProc->m_hi_test2);
				else
					pDlg->m_pImgProc->m_ImgList2_1.push_back(ho_image);
				break;
			case 1:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList2_2.push_back(pDlg->m_pImgProc->m_hi_test2);
				else
					pDlg->m_pImgProc->m_ImgList2_2.push_back(ho_image);
				break;
			case 2:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList2_3.push_back(pDlg->m_pImgProc->m_hi_test2);
				else
					pDlg->m_pImgProc->m_ImgList2_3.push_back(ho_image);
				break;
			case 3:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList2_4.push_back(pDlg->m_pImgProc->m_hi_test2);
				else
					pDlg->m_pImgProc->m_ImgList2_4.push_back(ho_image);
				break;
			case 4:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList2_5.push_back(pDlg->m_pImgProc->m_hi_test2);
				else
					pDlg->m_pImgProc->m_ImgList2_5.push_back(ho_image);
				break;
			default:
				break;
			}
		}
		
		if (static_count == 0) {
			pDlg->m_static_count2 = 1;
		}
		else if (static_count == 1) {
			pDlg->m_static_count2 = 0;
		}
	}
}

void CInspectDlg::AcqCallback3(SapXferCallbackInfo *pInfo)
{
	CInspectDlg *pDlg = (CInspectDlg*)pInfo->GetContext();
	int static_count = pDlg->m_static_count3;
	if (pInfo->IsTrash())
	{
		pDlg->camera3_trash_count += 1;
		CString cstr = L"3#相机丢帧";
		::SendNotifyMessageW(pDlg->hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
	}
	else
	{

		if (pDlg->camera3_show_buffer)
		{
			pDlg->m_View3->Show();
		}
		HImage ho_image;
		pDlg->GenerateHImage(pDlg->m_Buffers3, static_count, ho_image);

		if (!pDlg->m_is_system_pause) {
			pDlg->camera3_frame_count += 1;

			int change_index = pDlg->camera3_frame_count % pDlg->m_pImgProc->m_threadnum;
			switch (change_index)
			{
			case 0:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList3_1.push_back(pDlg->m_pImgProc->m_hi_test3);
				else
					pDlg->m_pImgProc->m_ImgList3_1.push_back(ho_image);
				break;
			case 1:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList3_2.push_back(pDlg->m_pImgProc->m_hi_test3);
				else
					pDlg->m_pImgProc->m_ImgList3_2.push_back(ho_image);
				break;
			case 2:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList3_3.push_back(pDlg->m_pImgProc->m_hi_test3);
				else
					pDlg->m_pImgProc->m_ImgList3_3.push_back(ho_image);
				break;
			case 3:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList3_4.push_back(pDlg->m_pImgProc->m_hi_test3);
				else
					pDlg->m_pImgProc->m_ImgList3_4.push_back(ho_image);
				break;
			case 4:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList3_5.push_back(pDlg->m_pImgProc->m_hi_test3);
				else
					pDlg->m_pImgProc->m_ImgList3_5.push_back(ho_image);
				break;
			default:
				break;
			}
		}

		if (static_count == 0) {
			pDlg->m_static_count3 = 1;
		}
		else if (static_count == 1) {
			pDlg->m_static_count3 = 0;
		}
	}

}

void CInspectDlg::AcqCallback4(SapXferCallbackInfo *pInfo)
{
	CInspectDlg *pDlg = (CInspectDlg*)pInfo->GetContext();
	int static_count = pDlg->m_static_count4;
	if (pInfo->IsTrash())
	{
		pDlg->camera4_trash_count += 1;
		CString cstr = L"4#相机丢帧";
		::SendNotifyMessageW(pDlg->hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
	}
	else
	{

		if (pDlg->camera4_show_buffer)
		{
			pDlg->m_View4->Show();
		}
		HImage ho_image;
		pDlg->GenerateHImage(pDlg->m_Buffers4, static_count, ho_image);
		
		if (!pDlg->m_is_system_pause) {
			pDlg->camera4_frame_count += 1;

			int change_index = pDlg->camera4_frame_count % pDlg->m_pImgProc->m_threadnum;
			switch (change_index)
			{
			case 0:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList4_1.push_back(pDlg->m_pImgProc->m_hi_test4);
				else
					pDlg->m_pImgProc->m_ImgList4_1.push_back(ho_image);
				break;
			case 1:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList4_2.push_back(pDlg->m_pImgProc->m_hi_test4);
				else
					pDlg->m_pImgProc->m_ImgList4_2.push_back(ho_image);
				break;
			case 2:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList4_3.push_back(pDlg->m_pImgProc->m_hi_test4);
				else
					pDlg->m_pImgProc->m_ImgList4_3.push_back(ho_image);
				break;
			case 3:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList4_4.push_back(pDlg->m_pImgProc->m_hi_test4);
				else
					pDlg->m_pImgProc->m_ImgList4_4.push_back(ho_image);
				break;
			case 4:
				if (pDlg->m_pImgProc->TEST_MODEL)
					pDlg->m_pImgProc->m_ImgList4_5.push_back(pDlg->m_pImgProc->m_hi_test4);
				else
					pDlg->m_pImgProc->m_ImgList4_5.push_back(ho_image);
				break;
			default:
				break;
			}
		}

		//切换双缓存
		if (static_count == 0) {
			pDlg->m_static_count4 = 1;
		}
		else if (static_count == 1) {
			pDlg->m_static_count4 = 0;
		}
	}
}




