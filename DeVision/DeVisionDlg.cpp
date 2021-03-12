// DeVisionDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "DeVisionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CDeVisionDlg 对话框

CDeVisionDlg::CDeVisionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEVISION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hOnlineIcon = AfxGetApp()->LoadIcon(IDI_ICON_ONLINE);
	m_hOfflineIcon = AfxGetApp()->LoadIcon(IDI_ICON_OFFLINE);
	m_hCameraInIcon = AfxGetApp()->LoadIcon(IDI_ICON_CAMERA_IN);
	m_hCameraOutIcon = AfxGetApp()->LoadIcon(IDI_ICON_CAMERA_OUT);
	m_hStartIcon = AfxGetApp()->LoadIcon(IDI_ICON_START);
	m_hStopIcon = AfxGetApp()->LoadIcon(IDI_ICON_STOP);
	m_hPauseIcon = AfxGetApp()->LoadIcon(IDI_ICON_PAUSE);
	m_hUpIcon = AfxGetApp()->LoadIcon(IDI_ICON_UP);
	m_hDownIcon = AfxGetApp()->LoadIcon(IDI_ICON_DOWN);
	m_hLockIcon = AfxGetApp()->LoadIcon(IDI_ICON_LOCK);
	m_hUnlockIcon = AfxGetApp()->LoadIcon(IDI_ICON_UNLOCK);
	m_hExitIcon = AfxGetApp()->LoadIconW(IDI_ICON_EXIT);

	m_system_state = SYSTEM_STATE_OFFLINE;
	m_screen_state = SCREEN_UNLOCK;

	//初始化临界区
	InitializeCriticalSection(&m_csListDftDisplay);
}

CDeVisionDlg::~CDeVisionDlg()
{
	DeleteCriticalSection(&m_csListDftDisplay);
}

void CDeVisionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TAB_DIALOG, m_tab);
	DDX_Control(pDX, IDC_MFCBUTTON_START, m_button_start);
	DDX_Control(pDX, IDC_MFCBUTTON_STOP, m_button_stop);
	DDX_Control(pDX, IDC_MFCBUTTON_PAUSE, m_button_pause);
	DDX_Control(pDX, IDC_MFCBUTTON_ONLINE, m_button_online);
	DDX_Control(pDX, IDC_PICTURE_TOTAL, m_PictureControlTotal);
	DDX_Control(pDX, IDC_PICTURE_PARTICAL, m_partical_picture);
	DDX_Control(pDX, IDC_BUTTON_LOCK, m_button_lock);
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_button_exit);
	DDX_Control(pDX, IDC_EDIT_DISPLAY_RANGE, m_edisplay_range);
	DDX_Control(pDX, IDC_EDIT_WIDTH1, m_ewidth1);
	DDX_Control(pDX, IDC_EDIT_WIDTH2, m_ewidth2);
	DDX_Control(pDX, IDC_EDIT_LONGTH1, m_elongth1);
	DDX_Control(pDX, IDC_EDIT_LONGTH2, m_elongth2);
	DDX_Control(pDX, IDC_EDIT_SELECTWIDTH, m_eselectwidth);
	DDX_Control(pDX, IDC_EDIT_SELECTLONGTH, m_eselectlongth);
	DDX_Control(pDX, IDC_EDIT_TOTAL, m_etotal);
	DDX_Control(pDX, IDC_STATIC_SYSTEM_STATUE, m_sSystem_Statue);
}

BEGIN_MESSAGE_MAP(CDeVisionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_DIALOG, &CDeVisionDlg::OnTcnSelchangeTabDialog)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CDeVisionDlg::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_FIND, &CDeVisionDlg::OnBnClickedButtonFind)
	ON_BN_CLICKED(IDC_BUTTON_GODOWN, &CDeVisionDlg::OnBnClickedButtonGodown)
	ON_BN_CLICKED(IDC_BUTTON_GOUP, &CDeVisionDlg::OnBnClickedButtonGoup)
	ON_BN_CLICKED(IDC_MFCBUTTON_START, &CDeVisionDlg::OnBnClickedMfcbuttonStart)
	ON_BN_CLICKED(IDC_MFCBUTTON_STOP, &CDeVisionDlg::OnBnClickedMfcbuttonStop)
	ON_BN_CLICKED(IDC_MFCBUTTON_PAUSE, &CDeVisionDlg::OnBnClickedMfcbuttonPause)
	ON_BN_CLICKED(IDC_MFCBUTTON_ONLINE, &CDeVisionDlg::OnBnClickedMfcbuttonOnline)
	ON_COMMAND(ID_SETUP, &CDeVisionDlg::OnSetup)
	ON_COMMAND(ID_CAMERA_SETUP, &CDeVisionDlg::OnCameraSetup)
	ON_COMMAND(ID_LED_SETUP, &CDeVisionDlg::OnLedSetup)
	ON_COMMAND(ID_TRIGGER, &CDeVisionDlg::OnTrigger)
	ON_COMMAND(ID_Save, &CDeVisionDlg::OnSave)
	ON_COMMAND(ID_EXIT, &CDeVisionDlg::OnExit)
	ON_COMMAND(ID_PRODUCT, &CDeVisionDlg::OnProduct)
	ON_COMMAND(ID_DEFFECT_TRADER, &CDeVisionDlg::OnDeffectTrader)
	ON_COMMAND(ID_DEFECT_ANALYSIS, &CDeVisionDlg::OnDefectAnalysis)
	ON_COMMAND(ID_TABLE, &CDeVisionDlg::OnTable)
	ON_COMMAND(ID_IMAGE, &CDeVisionDlg::OnImage)
	ON_COMMAND(ID_HELP, &CDeVisionDlg::OnHelp)
	ON_COMMAND(ID_RECORD, &CDeVisionDlg::OnRecord)
	ON_COMMAND(ID_ABOUT, &CDeVisionDlg::OnAbout)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_LOCK, &CDeVisionDlg::OnBnClickedButtonLock)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CDeVisionDlg::OnBnClickedButtonExit)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_COMMAND(IDOK, &CDeVisionDlg::OnIdok)
	ON_COMMAND(IDCANCEL, &CDeVisionDlg::OnIdcancel)
	ON_COMMAND(ID_REMOTE, &CDeVisionDlg::OnRemote)
	ON_MESSAGE(WM_LOGGING_MSG, &CDeVisionDlg::OnLoggingMsg)
	ON_MESSAGE(WM_WARNING_MSG, &CDeVisionDlg::OnWarningMsg)
	ON_MESSAGE(WM_UPDATE_CONTROLS, &CDeVisionDlg::OnUpdateControls)
	ON_MESSAGE(WM_UPDATE_HISTORY, &CDeVisionDlg::OnUpdateHistory)
	ON_MESSAGE(WM_UPDATE_MAINWND, &CDeVisionDlg::OnUpdateMainwnd)
END_MESSAGE_MAP()

// CDeVisionDlg 消息处理程序

//对话框初始化
BOOL CDeVisionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	hMainWnd = AfxGetMainWnd()->m_hWnd;

	//禁止关闭按钮
	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	ShowWindow(SW_MAXIMIZE);//对话框默认最大化弹出

	//创建字体
	small_flag_font.CreatePointFont(50, _T("Times New Roman"));
	loggle_font.CreatePointFont(300, _T("楷体"));
	GetDlgItem(IDC_STATIC_LOGGLE)->SetWindowPos(0, 600, 20, 400, 60, SWP_SHOWWINDOW);
	GetDlgItem(IDC_STATIC_LOGGLE)->SetWindowText(L"浙   清   柔   电");
	GetDlgItem(IDC_STATIC_LOGGLE)->SetFont(&loggle_font);
	m_sSystem_Statue.SetWindowPos(0, 350, 20, 180, 60, SWP_SHOWWINDOW);
	m_sSystem_Statue.SetFont(&loggle_font);

	// Table 页面初始化
	InitialTabDlg();
	isTabInitialized = true;

	//状态栏初始化
	InitialStateBar();

	//按钮图标初始化
	InitialBtnIcon();

	//注册表：加载默认设置
	LoadRegConfig();

	//初始化全局瑕疵显示区, 设置显示范围
	InitialTotalDefect();

	// 报表页面初始化
	pView->m_pvecDFT = &m_vecDftDisplay;
	m_tableDlg.m_pvDFT = &m_vecDftDisplay;

	//主界面信息刷新定时器
	SetTimer(1, 1000, 0);

	InitialImageAcquire();

	PostMessage(WM_UPDATE_CONTROLS, 0, 0);

	CString cstr = L"程序已启动，当前登录用户:  " + m_logo_name;
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

//退出
void CDeVisionDlg::ExitProgram()
{
	//停止界面刷新
	KillTimer(1);

	//删除已保存的参考图像
	DeleteHistoryImage();

	WriteToRegedit();

	return;
}

void CDeVisionDlg::OnIdok()
{
	// TODO: 在此添加命令处理程序代码
	// NOTHING
}

void CDeVisionDlg::OnIdcancel()
{
	// TODO: 在此添加命令处理程序代码
	//nothing
}

void CDeVisionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

//登录窗口
int CDeVisionDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	
	//登录窗口
	//if (!m_bTestModel) {
	//	CLogin loginDlg;
	//	loginDlg.DoModal();
	//	if (!loginDlg.ACCEPTED)
	//		return -1;
	//	else {
	//		m_logo_name = loginDlg.m_logo_name;
	//		return 0;
	//	}
	//}

	return 0;
}

void CDeVisionDlg::OnPaint()
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
		CPaintDC dc(this);

		CDialogEx::OnPaint();
	}
}

void CDeVisionDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

//从注册表读取
void CDeVisionDlg::LoadRegConfig()
{
	float tmp = (float)AfxGetApp()->GetProfileIntW(L"System Setup", L"wnd1 range", 0);
	//判断是否已有注册表信息
	if (tmp == 0.0f) {
		//初次运行则进行默认设置
		m_wnd1_range = 100.0f;
		m_wnd2_range = 5.0f;
		m_ImgAcq.m_k_speed = 491.52f;
		m_nThreadNumbers = 1;
		m_bSaveRefImg = false;
		m_strDeffect_Path = "D:\\瑕疵检测数据记录\\2瑕疵图像记录";
		m_strTable_Path = "D:\\瑕疵检测数据记录\\1检测报表记录";
		m_nNormalDistribution = 5;
		m_nFIlterSize = 1;
		m_fRadiusMin = 0.05f;
		m_fRadiusMax = 50.0f;
		m_cProduct_Number = "20200101";
		m_inspectDlg.m_eNumber.SetWindowTextW(m_cProduct_Number);
		m_cProduct_Model = "PET01";
		m_inspectDlg.m_eModel.SetWindowTextW(m_cProduct_Model);
		m_cProduct_Width = "1650";
		m_inspectDlg.m_eWidth.SetWindowTextW(m_cProduct_Width);
		m_cOperator = "UserA";
		m_inspectDlg.m_eOperator.SetWindowTextW(m_cOperator);
	}
	else {
		ReadFromRegedit();
	}
}


void CDeVisionDlg::ReadFromRegedit()
{
	m_wnd1_range = (float)AfxGetApp()->GetProfileIntW(L"System Setup", L"wnd1 range", 0);
	m_wnd2_range = (float)AfxGetApp()->GetProfileIntW(L"System Setup", L"wnd2 range", 0);
	m_ImgAcq.m_k_speed = ((float)AfxGetApp()->GetProfileIntW(L"System Setup", L"speed revise", 0)) / 100.0f;
	m_nThreadNumbers = AfxGetApp()->GetProfileIntW(L"System Setup", L"parallel thread", 0);
	m_bSaveRefImg = (bool)AfxGetApp()->GetProfileIntW(L"System Setup", L"save reference image", 0);
	m_strDeffect_Path = (CW2A)AfxGetApp()->GetProfileStringW(L"System Setup", L"deffect path", _T("")).GetBuffer();
	m_strTable_Path = (CW2A)AfxGetApp()->GetProfileStringW(L"System Setup", L"table path", _T("")).GetBuffer();

	m_nNormalDistribution = AfxGetApp()->GetProfileIntW(L"Algorithm Parameter", L"normal distribution", 0);
	m_nFIlterSize = AfxGetApp()->GetProfileIntW(L"Algorithm Parameter", L"filter size", 0);
	m_fRadiusMin = ((float)AfxGetApp()->GetProfileIntW(L"Algorithm Parameter", L"min radius", 0)) / 100.0f;
	m_fRadiusMax = ((float)AfxGetApp()->GetProfileIntW(L"Algorithm Parameter", L"max radius", 0)) / 100.0f;

	m_cProduct_Number = AfxGetApp()->GetProfileStringW(L"User Information", L"产品批次号", _T(""));
	m_inspectDlg.m_eNumber.SetWindowTextW(m_cProduct_Number);
	m_cProduct_Model = AfxGetApp()->GetProfileStringW(L"User Information", L"产品型号", _T(""));
	m_inspectDlg.m_eModel.SetWindowTextW(m_cProduct_Model);
	m_cProduct_Width = AfxGetApp()->GetProfileStringW(L"User Information", L"薄膜宽度", _T(""));
	m_inspectDlg.m_eWidth.SetWindowTextW(m_cProduct_Width);
	m_cOperator = AfxGetApp()->GetProfileStringW(L"User Information", L"操作员", _T(""));
	m_inspectDlg.m_eOperator.SetWindowTextW(m_cOperator);
}

//写入注册表
void CDeVisionDlg::WriteToRegedit()
{
	//系统设置
	AfxGetApp()->WriteProfileInt(L"System Setup", L"wnd1 range", (int)m_wnd1_range);
	AfxGetApp()->WriteProfileInt(L"System Setup", L"wnd2 range", (int)m_wnd2_range);
	AfxGetApp()->WriteProfileInt(L"System Setup", L"speed revise", (int)(m_ImgAcq.m_k_speed * 100));
	AfxGetApp()->WriteProfileInt(L"System Setup", L"parallel thread", m_nThreadNumbers);
	AfxGetApp()->WriteProfileInt(L"System Setup", L"save reference image", m_bSaveRefImg);
	AfxGetApp()->WriteProfileStringW(L"System Setup", L"deffect path", (CA2W)m_strDeffect_Path.c_str());
	AfxGetApp()->WriteProfileStringW(L"System Setup", L"table path", (CA2W)m_strTable_Path.c_str());

	//算法参数
	AfxGetApp()->WriteProfileInt(L"Algorithm Parameter", L"normal distribution", m_nNormalDistribution);
	AfxGetApp()->WriteProfileInt(L"Algorithm Parameter", L"filter size", m_nFIlterSize);
	//由float 保存为 int， 只保留两位小数
	AfxGetApp()->WriteProfileInt(L"Algorithm Parameter", L"min radius", (int)(m_fRadiusMin * 100));
	AfxGetApp()->WriteProfileInt(L"Algorithm Parameter", L"max radius", (int)(m_fRadiusMax * 100));

	//用户信息
	CStringW wtext;
	m_inspectDlg.m_eNumber.GetWindowTextW(wtext);
	AfxGetApp()->WriteProfileStringW(L"User Information", L"产品批次号", wtext);
	m_inspectDlg.m_eModel.GetWindowTextW(wtext);
	AfxGetApp()->WriteProfileStringW(L"User Information", L"产品型号", wtext);
	m_inspectDlg.m_eWidth.GetWindowTextW(wtext);
	AfxGetApp()->WriteProfileStringW(L"User Information", L"薄膜宽度", wtext);
	m_inspectDlg.m_eOperator.GetWindowTextW(wtext);
	AfxGetApp()->WriteProfileStringW(L"User Information", L"操作员", wtext);
}

//当用户拖动最小化窗口时系统调用此函数取得光标
HCURSOR CDeVisionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HBRUSH CDeVisionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_LOGGLE)//特定的某一个标签，IDC_STATIC_FONT为标签控件ID
	{
		pDC->SetBkMode(TRANSPARENT);//设置背景透明
		pDC->SetTextColor(RGB(55, 155, 225)); //文字颜色
		//pDC->SetBkColor(RGB(251, 247, 200));//背景色
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_SYSTEM_STATUE) {
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 0, 0));
		if (m_system_state == SYSTEM_STATE_RUN || m_system_state == SYSTEM_STATE_PAUSE)
			pDC->SetBkColor(RGB(0, 230, 20));
		else if (m_system_state == SYSTEM_STATE_ONLINE)
			pDC->SetBkColor(RGB(150, 150, 150));
		else
			pDC->SetBkColor(RGB(220, 30, 0));
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

//定时器回调函数
void CDeVisionDlg::OnTimer(UINT_PTR nIDEvent)
{
	float fwidth;
	if (!m_cProduct_Width.IsEmpty())
		fwidth = std::stof(m_cProduct_Width.GetBuffer());
	else fwidth = 1650.0f;

	switch (nIDEvent)
	{
	case 1: {
		UpdateSysStatus();
		m_tableDlg.m_current_position = UpdateCurrentInspectPosition();
		m_tableDlg.m_wstr_width = m_cProduct_Width;
		//更新刻度
		UpdateScaleValue(fwidth, m_fCurrentPosition);
		//控件闪烁控制
		if (m_bFlicker)  m_bFlicker = FALSE;
		else m_bFlicker = TRUE;

		break;
	}
	case 2:
		//更新历史图像，选中历史页面
		//if (m_CurSelTab == 3 && m_system_state != SYSTEM_STATE_OFFLINE && m_historyDlg.m_pages == 0)
		//	m_historyDlg.LoadHistoryImage();
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

float CDeVisionDlg::UpdateCurrentInspectPosition()
{
	float pos = 0.0f;
	UINT64 image_position = 0;
	if (m_bTestModel) {
		if (m_pImgProc[0] != NULL) {
			image_position = m_pImgProc[0]->m_nTotalListNumber;
		}
	}
	else
		image_position = m_ImgAcq.m_arrayFrameCount[0];

	pos = image_position * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
	m_fCurrentPosition = pos;

	return pos;
}

void CDeVisionDlg::TestLoadAndWrite()
{
	test_num += 1;

	HObject load, write;
	HTuple hv_name = "D:\\test_image.bmp";
	HalconCpp::ReadImage(&load, hv_name);

	HTuple end = ".bmp";
	HTuple save = (HTuple)m_strDeffect_Path.c_str() + (HTuple)test_num + end;
	HalconCpp::WriteImage(load, "bmp", 0, save);
}

//TabControl 切换
void CDeVisionDlg::OnTcnSelchangeTabDialog(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码

	//把当前的页面隐藏起来
	pDialog[m_CurSelTab]->ShowWindow(SW_HIDE);
	//得到新的页面索引
	m_CurSelTab = m_tab.GetCurSel();
	switch (m_CurSelTab)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	}
	//把新的页面显示出来
	pDialog[m_CurSelTab]->ShowWindow(SW_SHOW);

	*pResult = 0;
}

void CDeVisionDlg::InitialTabDlg()
{
	//子窗口的初始化Tab

	//为Tab Control增加页面
	m_tab.InsertItem(0, _T("检测画面"));
	m_tab.InsertItem(1, _T("趋势分析"));
	m_tab.InsertItem(2, _T("报表打印"));
	m_tab.InsertItem(3, _T("历史图像"));
	// 设置 tab 按钮大小
	m_tab.SetItemSize(CSize(150, 30));
	//m_tab.SetPadding(CSize(10, 10));

	//创建对话框
	m_inspectDlg.Create(IDD_DIALOG_INSPECT, &m_tab);
	m_analysisDlg.Create(IDD_DIALOG_ANALYSIS, &m_tab);
	m_tableDlg.Create(IDD_DIALOG_TABLE, &m_tab);
	m_historyDlg.Create(IDD_DIALOG_HISTORY, &m_tab);
	//设定在Tab内显示的范围
	CRect rc;
	m_tab.GetClientRect(rc);
	rc.top += 30;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	m_inspectDlg.MoveWindow(&rc);
	m_analysisDlg.MoveWindow(&rc);
	m_tableDlg.MoveWindow(&rc);
	m_historyDlg.MoveWindow(&rc);
	//把对话框对象指针保存起来
	pDialog[0] = &m_inspectDlg;
	pDialog[1] = &m_analysisDlg;
	pDialog[2] = &m_tableDlg;
	pDialog[3] = &m_historyDlg;
	//显示初始页面
	pDialog[0]->ShowWindow(SW_SHOW);
	pDialog[1]->ShowWindow(SW_HIDE);
	pDialog[2]->ShowWindow(SW_HIDE);
	pDialog[3]->ShowWindow(SW_HIDE);
	m_CurSelTab = 0;
}

void CDeVisionDlg::TabDlgResize()
{
	CRect rc;
	m_tab.GetClientRect(rc);
	rc.top += 30;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	m_inspectDlg.MoveWindow(&rc);
	m_analysisDlg.MoveWindow(&rc);
	m_tableDlg.MoveWindow(&rc);
	m_historyDlg.MoveWindow(&rc);
}

//图像采集系统初始化
void CDeVisionDlg::InitialImageAcquire()
{
	m_ImgAcq.m_pWndHandle = &m_inspectDlg;
	for (int i = 0; i < 4; i++) {
		m_ImgAcq.m_pImageWnd[i] = &m_inspectDlg.m_ImageWnd[i];
	}

}

//状态栏初始化
void CDeVisionDlg::InitialStateBar()
{
	//添加状态栏
	static UINT BASED_CODE indicators[] = {
	IDS_STATESTRING1,
	IDS_STATESTRING2,
	IDS_STATESTRING3,
	IDS_STATESTRING4,
	IDS_STATESTRING5,
	IDS_STATESTRING6,
	IDS_STATESTRING7,
	IDS_STATESTRING8,
	IDS_STATESTRING9 };

	CRect rect, stateRect;
	GetClientRect(rect);
	m_StatusBar.Create(this);
	m_StatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	m_StatusBar.GetClientRect(&stateRect);
	// 调整状态栏的位置和大小
	m_StatusBar.MoveWindow(0, rect.bottom - stateRect.Height(), rect.right, stateRect.Height());
	//设置状态栏的宽度
	m_StatusBar.SetPaneInfo(0, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 200);
	m_StatusBar.SetPaneInfo(1, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(2, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(3, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(4, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(5, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 200);
	m_StatusBar.SetPaneInfo(6, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(7, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(8, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);

	//显示状态栏
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	//设置背景颜色
	m_StatusBar.GetStatusBarCtrl().SetBkColor(RGB(0, 0, 0));
}

//按钮图标
void CDeVisionDlg::InitialBtnIcon()
{
	//HICON hIcon;
	//hIcon = AfxGetApp()->LoadIconW(IDI_ICON_START);
	//((CButton*)GetDlgItem(IDC_BUTTON_START))->SetIcon(hIcon);

	//m_button_start.m_bDontUseWinXPTheme = TRUE;
	//m_button_start.SetFaceColor(RGB(0, 150, 50));
	m_button_start.SetIcon(m_hStartIcon);

	//m_button_stop.m_bDontUseWinXPTheme = TRUE;
	//m_button_stop.SetFaceColor(RGB(0, 150, 50));
	m_button_stop.SetIcon(m_hStopIcon);

	//m_button_pause.m_bDontUseWinXPTheme = TRUE;
	//m_button_pause.SetFaceColor(RGB(0, 150, 50));
	m_button_pause.SetIcon(m_hPauseIcon);

	//m_button_online.m_bDontUseWinXPTheme = TRUE;
	//m_button_online.SetFaceColor(RGB(150, 50, 150));
	//m_button_online.SetIcon(m_hOfflineIcon);
	m_button_online.SetIcon(m_hCameraOutIcon);

	m_button_lock.SetIcon(m_hUnlockIcon);

	m_button_exit.SetIcon(m_hExitIcon);
}

//窗口大小调整
void CDeVisionDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码

	//根据窗口的大小自动调整工具栏、状态栏的大小
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	//根据窗口的大小自动调整 Tab页的大小
	if (isTabInitialized)
		TabDlgResize();

	//判断窗口是不是最小化了，因为窗口最小化之后 ，窗口的长和宽会变成0，当前一次变化的时就会出现除以0的错误操作
	if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED)
	{
	}
}

//全局瑕疵显示区初始化
BOOL CDeVisionDlg::InitialTotalDefect()
{
	//   3
	CCreateContext pContext;
	CWnd* pFrameWnd = this->GetDlgItem(IDC_PICTURE_TOTAL);
	CRect rectWindow;
	pFrameWnd->GetWindowRect(rectWindow);

	pContext.m_pCurrentDoc = new CMyDocument;
	pContext.m_pNewViewClass = RUNTIME_CLASS(CMyView);
	pView = (CMyView *)((CFrameWnd*)pFrameWnd)->CreateView(&pContext);
	ASSERT(pView);
	pView->wnd_width = rectWindow.Width();
	pView->wnd_height = rectWindow.Height();
	pView->wnd_scroll_scale_size = 10;
	pView->OnInitialUpdate();
	//pView->m_nMapMode = MM_TEXT;   //映射模式
	pView->ShowWindow(SW_NORMAL);
	rectWindow.top -= 135;
	rectWindow.left -= 20;
	rectWindow.right -= 5;
	rectWindow.bottom -= 100;
	pView->MoveWindow(rectWindow);

	//刻度显示初始化
	InitialScaleFlag();

	CString ctext;
	ctext.Format(_T("%.2f"), m_wnd1_range);
	m_edisplay_range.SetWindowTextW(ctext);

	return TRUE;
}

//从瑕疵队列中取出
void CDeVisionDlg::DelQueueFromSource()
{
	// 瑕疵显示范围，图像数量为单位
	unsigned display_range = (int)(m_wnd1_range * 1000 / (IMAGE_HEIGHT * VERTICAL_PRECISION)); 
	pView->m_unDisplayRangeIndex = display_range;
	DeffectInfo info;
	EnterCriticalSection(&m_csListDftDisplay);
	for (int index = 0; index < m_nConnectedCameras; index++) {
		if (!m_listDftInfo[index].empty()) {
			//unsigned newest_image_index = m_listDftInfo[index][m_listDftInfo[index].size() - 1].image_index;
			//std::vector<DeffectInfo>::reverse_iterator rit = m_listDftInfo[index].rbegin();
			//for (; rit != m_listDftInfo[index].rend(); ++rit) {
			for (int i = 0; i < (int)m_listDftInfo[index].size(); i++) {
				info = m_listDftInfo[index].front();
				m_listDftInfo[index].pop_front();
				//把像素数据转换为毫米/米, 存入vector
				info.x = (info.x + index * IMAGE_WIDTH) * HORIZON_PRECISION;
				info.y = (info.y + (info.image_index - 1) * IMAGE_HEIGHT) * VERTICAL_PRECISION / 1000.0f;
				info.area = info.area * HORIZON_PRECISION * VERTICAL_PRECISION;
				m_vecDftDisplay.push_back(info);
				m_nTotalDeffects += 1;
				//判断是否是严重缺陷
				if (info.area > 5.0f || info.contlength > 32)
					m_nSeriousDeffects += 1;

				//瑕疵类型统计
				switch (info.type)
				{
				case 0:
					m_rank[0] += 1;
					break;
				case 1:
					m_rank[1] += 1;
					break;
				case 2:
					m_rank[2] += 1;
					break;
				case 3:
					m_rank[3] += 1;
					break;
				case 4:
					m_rank[4] += 1;
					break;
				}
			}
		}
	}
	LeaveCriticalSection(&m_csListDftDisplay);

	m_fTotalDeffectsLength = m_nTotalDeffects * 0.01f;
	m_inspectDlg.UpdateDFTinformation(m_nTotalDeffects, m_nSeriousDeffects, m_fTotalDeffectsLength);
	m_analysisDlg.m_dDftNumber1 = (double)m_rank[0];
	m_analysisDlg.m_dDftNumber2 = (double)m_rank[1];
	m_analysisDlg.m_dDftNumber3 = (double)m_rank[2];
	m_analysisDlg.m_dDftNumber4 = (double)m_rank[3];
	m_analysisDlg.m_dDftNumber5 = (double)m_rank[4];
	// ......
	if (m_CurSelTab == 3 && m_system_state != SYSTEM_STATE_OFFLINE && m_historyDlg.m_pages == 0)
		PostMessage(WM_UPDATE_HISTORY, 0, 0);
}

//划分当前薄膜的等级
int CDeVisionDlg::DevideDFTRank(int num)
{
	int rank;
	if (num < 10)
		rank = RANK_COMMON;
	else if (10 <= num && num < 100)
		rank = RANK_GRADE1;
	else if (100 <= num && num < 500)
		rank = RANK_GRADE2;
	else if (500 <= num && num < 1000)
		rank = RANK_GRADE3;
	else if (1000 <= num)
		rank = RANK_SERIOUS;
	return rank;
}

//创建瑕疵标记
void CDeVisionDlg::CreateFlag(CDC &mDC, int x, int y, int kind)
{
	switch (kind)
	{
	case 0: {
		if (pView->m_bFlagShow[0]) {
			mDC.SetBkColor(pView->m_acolor[0]);
			mDC.TextOutW(x, y, _T("A"));
		}
		break;
	}
	case 1: {
		if (pView->m_bFlagShow[1]) {
			mDC.SetBkColor(pView->m_acolor[1]);
			mDC.TextOutW(x, y, _T("B"));
		}
		break;
	}
	case 2: {
		if (pView->m_bFlagShow[2]) {
			mDC.SetBkColor(pView->m_acolor[2]);
			mDC.TextOutW(x, y, _T("C"));
		}
		break;
	}
	case 3: {
		if (pView->m_bFlagShow[3]) {
			mDC.SetBkColor(pView->m_acolor[3]);
			mDC.TextOutW(x, y, _T("D"));
		}
		break;
	}
	case 4:
		if (pView->m_bFlagShow[4]) {
			mDC.SetBkColor(pView->m_acolor[4]);
			mDC.TextOutW(x, y, _T("E"));
		}
		break;
	case 5:
		if (pView->m_bFlagShow[5]) {
			mDC.SetBkColor(pView->m_acolor[5]);
			mDC.TextOutW(x, y, _T("F"));
		}
		break;
	case 6:
		if (pView->m_bFlagShow[6]) {
			mDC.SetBkColor(pView->m_acolor[6]);
			mDC.TextOutW(x, y, _T("G"));
		}
		break;
	case 7:
		if (pView->m_bFlagShow[7]) {
			mDC.SetBkColor(pView->m_acolor[7]);
			mDC.TextOutW(x, y, _T("H"));
		}
		break;
	default: {
		mDC.SetBkColor(RGB(0, 0, 0));
		break;
	}
	}
}

//绘制局部瑕疵图
void CDeVisionDlg::DrawPartial(int test)
{
	CRect rect;
	m_partical_picture.GetClientRect(&rect);
	//获取窗口DC
	CDC* p_screenDC = m_partical_picture.GetDC();
	int wnd_width = rect.Width();
	int wnd_height = rect.Height();
	//窗口 / 图像 缩放因子
	float scale_x = (float)wnd_width / (IMAGE_WIDTH * m_nConnectedCameras * HORIZON_PRECISION);
	//float scale_y = (float)wnd_height / (IMAGE_HEIGHT * m_wnd2_range * VERTICAL_PRECISION);
	float scale_y = (float)(m_wnd2_range / wnd_height);

	CDC MemDC; // 定义一个内存显示设备对象
	CBitmap MemBitmap; // 定义一个位图对象
	//建立与屏幕显示兼容的内存显示设备
	MemDC.CreateCompatibleDC(p_screenDC);
	//建立一个与屏幕显示兼容的位图，位图的大小可选用窗口客户区的大小
	MemBitmap.CreateCompatibleBitmap(p_screenDC, rect.Width(), rect.Height());
	//将位图选入到内存显示设备中，只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
	//先用背景色将位图清除干净，否则是黑色。这里用的是白色作为背景
	MemDC.FillSolidRect(5, 0, rect.Width() - 10, rect.Height(), RGB(128, 128, 128));

	DeffectInfo info;
	int x_coord = 0, y_coord = 0;
	if (!m_vecDftDisplay.empty()) {
		std::vector<DeffectInfo>::reverse_iterator rit = m_vecDftDisplay.rbegin();
		for (; rit != m_vecDftDisplay.rend(); ++rit) {
			info = *rit;
			x_coord = (int)(info.x * scale_x);
			y_coord = wnd_height - (int)((info.y - m_previous_position) / scale_y);
			if (y_coord < 0)
				m_previous_position = info.y - m_wnd2_range;
			if (y_coord > wnd_height)
				break;

			//创建标记
			CreateFlag(MemDC, x_coord, y_coord, info.type);
			TRACE("type = %d\n", info.type);
			// 刷新控件显示
			if (std::distance(m_vecDftDisplay.rbegin(), rit) == 0) {
				CString cwidth, cposition;
				cwidth.Format(_T("%.1f 毫米"), info.x * HORIZON_PRECISION);
				cposition.Format(_T("%.3f 米"), info.y * VERTICAL_PRECISION / 1000);
				m_ewidth1.SetWindowTextW(cwidth);
				m_elongth1.SetWindowTextW(cposition);
			}
			else if (std::distance(m_vecDftDisplay.rbegin(), rit) == 1) {
				CString cwidth, cposition;
				cwidth.Format(_T("%.1f 毫米"), info.x * HORIZON_PRECISION);
				cposition.Format(_T("%.3f 米"), info.y * VERTICAL_PRECISION / 1000);
				m_ewidth2.SetWindowTextW(cwidth);
				m_elongth2.SetWindowTextW(cposition);
			}
		}
	}

	//将内存中的图拷贝到屏幕上进行显示
	p_screenDC->BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	//绘图完成后的清理
	MemDC.SelectObject(pOldBit);
	MemBitmap.DeleteObject();
	m_partical_picture.ReleaseDC(p_screenDC);
}

void CDeVisionDlg::InitialScaleFlag()
{
	//初始化 瑕疵标记 显示区域
	CString yscale("--\r\n-\r\n-\r\n-\r\n-\r\n" \
		"--\r\n-\r\n-\r\n-\r\n-\r\n" \
		"--\r\n-\r\n-\r\n-\r\n-\r\n" \
		"--\r\n-\r\n-\r\n-\r\n-\r\n" \
		"--\r\n-\r\n-\r\n-\r\n-\r\n" \
		"--\r\n-\r\n-\r\n-\r\n-\r\n" \
		"--\r\n-\r\n-\r\n-\r\n-\r\n" \
		"--\r\n-\r\n-\r\n-\r\n-\r\n" \
		"--\r\n-\r\n-\r\n-\r\n-\r\n" \
		"--\r\n-\r\n-\r\n-\r\n-\r\n" \
		"--\r\n-\r\n-\r\n-\r\n-\r\n" \
		"--\r\n-\r\n-\r\n-\r\n-\r\n--");
	GetDlgItem(IDC_STATIC_YSCALE)->SetWindowText(yscale);
	CString xscale("l                                                   "\
		"l                                                   "\
		"l                                                   "\
		"l                                                   l");
	GetDlgItem(IDC_STATIC_XSCALE)->SetWindowTextW(xscale);

	return;
}

//刻度更新
void CDeVisionDlg::UpdateScaleValue(float x, float y)
{
	//设置检测宽度
	SetXScalePos(x);
	//设置当前检测位置
	SetYScalePos(y);

	return;
}

void CDeVisionDlg::SetXScalePos(float width)
{
	CString pos1 = L"                                               ";
	CString pos2 = L"                                           ";
	CString unit = L"mm";

	CString xpos, num;
	for (int i = 0; i < 5; i++)
	{
		int current_pos = (int)width * i / 4;
		num.Format(L"%d", current_pos);
		xpos = xpos + num;
		if (i % 2 == 0)
			xpos += pos1;
		else
			xpos += pos2;
	}
	xpos += unit;

	GetDlgItem(IDC_STATIC_XPOS)->SetWindowTextW(xpos);

	return;
}

void CDeVisionDlg::SetYScalePos(float position)
{
	CString pos = L"\r\n\r\n\r\n\r\n\r\n";
	CString unit = L"m";

	CString ypos, num;
	for (int i = 0; i < 13; i++)
	{
		//float current_pos = position * (12 - i) / 12 ;
		float current_pos = position - i * m_wnd1_range / 12.0f;
		if (current_pos <= 0)
			current_pos = position * (12 - i) / 12;
		num.Format(L"%.2f", current_pos);
		ypos += num;
		ypos += unit;
		ypos += pos;
	}

	GetDlgItem(IDC_STATIC_YPOS)->SetWindowText(ypos);

	return;
}

//获取运行时间
float CDeVisionDlg::GetRunTime()
{
	long current_time = GetTickCount();
	float duration = (float)(current_time - start_time);
	//将毫秒转为秒
	duration = duration / 1000;
	//将秒转为分钟
	duration = duration / 60;

	return duration;
}

//更新状态栏
void CDeVisionDlg::UpdateSysStatus()
{
	CString cstr_ID;
	m_inspectDlg.m_eModel.GetWindowText(cstr_ID);
	if (cstr_ID != _T("")) {
		cstr_ID = L"产品： " + cstr_ID;
		m_StatusBar.SetPaneText(1, cstr_ID, 1);
	}

	CString cstr_operator;
	m_inspectDlg.m_eOperator.GetWindowText(cstr_operator);
	if (cstr_operator != _T("")) {
		cstr_operator = L"操作员： " + cstr_operator;
		m_StatusBar.SetPaneText(2, cstr_operator, 1);
	}

	if (m_system_state != SYSTEM_STATE_OFFLINE) {
		if (m_fCurrentPosition > 0) {
			CString cst_current_pos;
			cst_current_pos.Format(L"当前位置：第 %.2f 米", m_fCurrentPosition);
			m_StatusBar.SetPaneText(3, cst_current_pos, 1);
		}

		if (!m_bTestModel)
			m_speed = m_ImgAcq.CalculateEncoderSpeed();
		if (m_speed > 0) {
			CString cstr_speed;
			cstr_speed.Format(L"当前速度：%.2f 米/分钟", m_speed);
			m_StatusBar.SetPaneText(4, cstr_speed, 1);
		}
			   		 
		//总处理数量
		UINT64 frame_count = 0;
		if (m_bTestModel) {
			if (m_pImgProc[0] != NULL) 
				frame_count = m_pImgProc[0]->m_nTotalListNumber;			
		}
		else
			frame_count = m_ImgAcq.m_arrayFrameCount[0];            //  1#相机采集的图像总数
		UINT64 frame_processed = 0;
		if (m_pImgProc[0] != NULL)
			frame_processed = m_pImgProc[0]->m_unImageIndex;        //  多个线程合计处理的总数
		int frame_over = (int)(frame_count - frame_processed);
		if (frame_over < 0)
			frame_over = 0;
		CString cstr_frame_count;
		cstr_frame_count.Format(L"总帧数：%d 帧(待处理：%d)", frame_count, frame_over);
		m_StatusBar.SetPaneText(5, cstr_frame_count, 1);
		if (frame_over > 50 && frame_over < 100) {
			if (!m_ImgAcq.SLOW_DOWN) {
				m_ImgAcq.DropAcquireSpeed(1);
				m_ImgAcq.SLOW_DOWN = TRUE;
				CString warning = L"车速过快，已降低纵向检测精度";
				::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&warning, NULL);
			}
		}
		else if (frame_over >= 100) {
			AutoStop();
			CString warning = L"车速过快，已停止检测";
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&warning, NULL);
		}

		int trash_count = m_ImgAcq.GetTotalTrashCount();
		if (trash_count > 0) {
			CString cstr_trash_count;
			cstr_trash_count.Format(L"丢帧数： %d 帧", trash_count);
			m_StatusBar.SetPaneText(6, cstr_trash_count, 1);
		}
		if (m_nTotalDeffects > 0) {
			CString cst_current_pos;
			cst_current_pos.Format(L"瑕疵数：%d / %d  ", m_nSeriousDeffects, m_nTotalDeffects);
			m_StatusBar.SetPaneText(7, cst_current_pos, 1);
		}
	}

	float run_time = GetRunTime();
	CString cstr_statue;
	switch (m_system_state)
	{
	case SYSTEM_STATE_OFFLINE:
		cstr_statue.Format(L"相机离线");
		m_StatusBar.SetPaneText(8, cstr_statue, 1);
		m_sSystem_Statue.SetWindowTextW(cstr_statue);
		break;
	case SYSTEM_STATE_ONLINE:
		cstr_statue.Format(L"相机在线");
		m_StatusBar.SetPaneText(8, cstr_statue, 1);
		m_sSystem_Statue.SetWindowTextW(cstr_statue);
		break;
	case SYSTEM_STATE_RUN:
		cstr_statue.Format(L"运行中   已运行：%.1f分钟", run_time);
		m_StatusBar.SetPaneText(8, cstr_statue, 1);
		m_sSystem_Statue.SetWindowTextW(L"运行中");
		break;
	case SYSTEM_STATE_STOP:
		cstr_statue.Format(L"停止");
		m_StatusBar.SetPaneText(8, cstr_statue, 1);
		m_sSystem_Statue.SetWindowTextW(cstr_statue);
		break;
	case SYSTEM_STATE_PAUSE:
		cstr_statue.Format(L"暂停");
		m_StatusBar.SetPaneText(8, cstr_statue, 1);
		m_sSystem_Statue.SetWindowTextW(cstr_statue);
		m_sSystem_Statue.ShowWindow(m_bFlicker);
		break;
	default:
		break;
	}
}

//更新颜色选项
void CDeVisionDlg::UpdateSysColor()
{
	pView->m_acolor[0] = m_inspectDlg.m_color1;
	pView->m_acolor[1] = m_inspectDlg.m_color2;
	pView->m_acolor[2] = m_inspectDlg.m_color3;
	pView->m_acolor[3] = m_inspectDlg.m_color4;
	pView->m_acolor[4] = m_inspectDlg.m_color5;
	pView->m_acolor[5] = m_inspectDlg.m_color6;
	pView->m_acolor[6] = m_inspectDlg.m_color7;
	pView->m_acolor[7] = m_inspectDlg.m_color8;

}

//创建工作目录
void CDeVisionDlg::CreateWorkPath(std::string &path)
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
	std::string strpath = name;
	std::string work_fold = m_strDeffect_Path + "\\";
	path = work_fold + strpath;
	//创建文件夹
	_mkdir(path.c_str());

	//创建参考图像文件夹
	std::string reference_image = path + "\\参考图像";
	_mkdir(reference_image.c_str());
	std::wstring wref = (CA2W)reference_image.c_str();
	m_vec_refpath.push_back(wref);

	path += "\\";

	CString cstr = L"工作目录已创建";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

void CDeVisionDlg::DeleteHistoryImage()
{
	std::wstring wstrpath;
	if (!m_vec_refpath.empty()) {
		for (int i = 0; i < m_vec_refpath.size(); i++)
		{
			wstrpath = m_vec_refpath.front();
			m_vec_refpath.pop_back();
			RemoveAll(wstrpath);
		}
	}
}

void CDeVisionDlg::RemoveAll(std::wstring wst)
{
	std::wstring wstCurrentFindPath;
	wstCurrentFindPath.assign(wst);
	wstCurrentFindPath.append(L"\\*.*");

	std::wstring wstCurrentFile;
	WIN32_FIND_DATAW wfd;
	HANDLE h = FindFirstFileW(wstCurrentFindPath.c_str(), &wfd);
	if (h == INVALID_HANDLE_VALUE) {
		return;
	}

	do
	{
		if (lstrcmpW(wfd.cFileName, L".") == 0 ||
			lstrcmpW(wfd.cFileName, L"..") == 0) {
			continue;
		}
		wstCurrentFile.assign(wst);
		wstCurrentFile.append(L"\\");
		wstCurrentFile.append(wfd.cFileName);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			RemoveAll(wstCurrentFile);
		}
		else {
			DeleteFileW(wstCurrentFile.c_str());
		}
	} while (FindNextFileW(h, &wfd));
	FindClose(h);

	RemoveDirectoryW(wst.c_str());
}

//重新启动检测
void CDeVisionDlg::ReStartPrepare()
{
	m_previous_position = 0.0f;
	//current_position = 0.0f;
	m_nTotalDeffects = 0;
	m_nSeriousDeffects = 0;
	m_fTotalDeffectsLength = 0.0f;
	memset(m_rank, 0, sizeof(m_rank));

	for (int i = 0; i < m_nConnectedCameras; i++) {
		m_listDftInfo[i].clear();
	}
	m_vecDftDisplay.clear();
	m_ImgAcq.ResetAcquire();
	pView->Redraw();
	
	//创建工作目录
	std::string new_path;
	CreateWorkPath(new_path);
	//获取瑕疵图像保存路径
	m_strDeffectImgSavePath = new_path;
	m_tableDlg.m_DFT_img_path = new_path;
	m_historyDlg.m_file_path = new_path;

	return;
}

//自动停机
void CDeVisionDlg::AutoStop()
{
	CWaitCursor wait;

	if (m_system_state == SYSTEM_STATE_RUN || m_system_state == SYSTEM_STATE_PAUSE) {
		if (!m_bTestModel) {
			if (!m_ImgAcq.Freeze()) return;
		}

		//等待列表中的图像都处理完成, 之后结束处理线程
		StopRefrush_Event.SetEvent();

		m_tableDlg.TableSaved_Event.ResetEvent();
	}

	CString cstr = L"保护性停机";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

//保存图像
void CDeVisionDlg::SaveDeffectImage(int acquire_index, HObject ho_img, DeffectInfo information)
{
	//图像保存，格式化文件名
	HTuple hv_path = (HTuple)((CA2W)(m_strDeffectImgSavePath.c_str()));
	float absolute_position = ((information.image_index - 1) * IMAGE_HEIGHT + information.y) * VERTICAL_PRECISION / 1000.0f;
	char cpos[16];
	sprintf_s(cpos, "%.3f", absolute_position);
	HTuple hv_position = (HTuple)cpos;
	char cX[16];
	sprintf_s(cX, "%.3f", (acquire_index * IMAGE_WIDTH + information.x) * HORIZON_PRECISION);
	HTuple hv_X = (HTuple)cX;
	char cradius[16];
	sprintf_s(cradius, "%.3f", information.radius * HORIZON_PRECISION);
	HTuple hv_radius = (HTuple)cradius;
	char carea[16];
	sprintf_s(carea, "%.3f", information.area * HORIZON_PRECISION * VERTICAL_PRECISION);
	HTuple hv_harea = (HTuple)carea;
	HTuple hv_kind = (HTuple)information.type;
	HTuple hv_img_name = hv_path + "P" + hv_position
		+ "_X" + hv_X
		+ "_R" + hv_radius
		+ "_A" + hv_harea
		+ "_K" + hv_kind;
	HalconCpp::WriteImage(ho_img, "bmp", 0, hv_img_name);
}

void CDeVisionDlg::SaveImages(int index, unsigned &numbers)
{
	int dft_img_num = (int)m_pImgProc[index]->m_listDftImg.size();
	int imagenums = 0;
	if (dft_img_num > 20)
		imagenums = 20;
	else
		imagenums = dft_img_num;

	HObject img;
	HTuple  name;
	DeffectInfo info;
	for (int i = 0; i < imagenums; i++) {
		img = m_pImgProc[index]->m_listDftImg.front();
		m_pImgProc[index]->m_listDftImg.pop_front();
		info = m_pImgProc[index]->m_listDftInfo.front();
		m_pImgProc[index]->m_listDftInfo.pop_front();
		SaveDeffectImage(index, img, info);
		//将瑕疵信息导出
		m_listDftInfo[index].push_back(info);
	}
	numbers += imagenums;
}

//界面刷新与保存
UINT CDeVisionDlg::RefrushWnd(LPVOID pParam)
{
	CDeVisionDlg *pDlg = (CDeVisionDlg *)pParam;
	DWORD dwStop = 0;
	//保存的瑕疵图像数量
	unsigned int save_index[4] = {0};

	CString cstr = L"启动界面刷新...";
	::SendMessage(pDlg->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	for (;;) {
		//获取瑕疵信息数据
		pDlg->DelQueueFromSource();

		//设置为 2s 刷新
		dwStop = WaitForSingleObject(pDlg->StopRefrush_Event, 2000);
		switch (dwStop)
		{
		case WAIT_TIMEOUT: {
			for (int index = 0; index < pDlg->m_nConnectedCameras; index++) {
				pDlg->SaveImages(index, save_index[index]);
			}			
			if (pDlg->m_system_state != SYSTEM_STATE_PAUSE) {
				pDlg->DrawPartial(5);
				pDlg->pView->UpdateScreen(pDlg->small_flag_font, pDlg->m_wnd1_range);
				//pDlg->m_inspectDlg.UpdateDFTinformation(pDlg->total_number_def,
				//	pDlg->serious_def_num,
				//	pDlg->total_def_length);
				//更新系统状态信息
				pDlg->m_tableDlg.m_iSystemState = pDlg->m_system_state;
				CString cwidth;
				cwidth.Format(_T("当前选择：***米"));
				pDlg->m_etotal.SetWindowTextW(cwidth);
			}
			break;
		}
		case WAIT_FAILED:
			return -1;
		case WAIT_OBJECT_0: {
			//显示等待界面
			CLoad  wndLoad;
			wndLoad.Create(IDB_BITMAP_WAIT0);
			wndLoad.UpdateWindow();
			HWND hwnd = wndLoad.GetSafeHwnd();
			int nIndex = WaitForMultipleObjects(pDlg->m_nConnectedCameras, *pDlg->m_phFinishProcessEvent, TRUE, INFINITE);
			if (WAIT_OBJECT_0 + 3 <= nIndex <= WAIT_OBJECT_0 + 2) {
				for (int index = 0; index < pDlg->m_nConnectedCameras; index++) {
					pDlg->SaveImages(index, save_index[index]);
				}
				//保存检测记录
				EnterCriticalSection(&pDlg->m_csListDftDisplay);
				//报表保存路径
				std::string table_path = pDlg->m_strTable_Path + "\\";
				pDlg->m_tableDlg.m_save_path = table_path.c_str();
				pDlg->m_tableDlg.m_vecDFT = pDlg->m_vecDftDisplay;
				//产品评级
				pDlg->m_tableDlg.m_product_rank = pDlg->DevideDFTRank(pDlg->m_nTotalDeffects);
				//严重缺陷个数
				pDlg->m_tableDlg.m_serious_num = pDlg->m_nSeriousDeffects;
				//型号
				CString ctext;
				pDlg->m_inspectDlg.m_eModel.GetWindowTextW(ctext);
				pDlg->m_tableDlg.m_wstr_id = ctext;
				//宽度
				pDlg->m_inspectDlg.m_eWidth.GetWindowTextW(ctext);
				pDlg->m_tableDlg.m_wstr_width = ctext;
				//批号
				pDlg->m_inspectDlg.m_eNumber.GetWindowTextW(ctext);
				pDlg->m_tableDlg.m_wstr_num = ctext;
				//操作员
				pDlg->m_inspectDlg.m_eOperator.GetWindowTextW(ctext);
				pDlg->m_tableDlg.m_wstr_user = ctext;
				//平均速度
				pDlg->m_tableDlg.m_wstr_speed = std::to_wstring(pDlg->m_speed);
				//瑕疵类型个数统计
				pDlg->m_tableDlg.GetDetectResult(
					pDlg->m_rank[0],
					pDlg->m_rank[1],
					pDlg->m_rank[2],
					pDlg->m_rank[3],
					pDlg->m_rank[4]);

				pDlg->m_tableDlg.BeginSaveTable();
				LeaveCriticalSection(&pDlg->m_csListDftDisplay);
			}

			WaitForSingleObject(pDlg->m_tableDlg.TableSaved_Event, INFINITE);
			pDlg->m_system_state = SYSTEM_STATE_STOP;

			//关闭等待界面
			wndLoad.DestroyWindow();

			CString cstr = L"检测数据保存成功";
			::SendMessage(pDlg->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
			::SendMessage(pDlg->hMainWnd, WM_UPDATE_CONTROLS, 0, 0);

			return 0;
		}
		default:
			break;
		}
	}
	CString cstop = L"结束界面刷新";
	::SendMessage(pDlg->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstop, NULL);

	return 0;
}

//******************************************************按钮*****************************//

//选择  按钮
void CDeVisionDlg::OnBnClickedButtonSelect()
{
	// TODO: 在此添加控件通知处理程序代码
	CWaitCursor wait;

	CString cstr = L"选择瑕疵，位置：";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

}

//查找  按钮
void CDeVisionDlg::OnBnClickedButtonFind()
{
	// TODO: 在此添加控件通知处理程序代码

	CString cstr = L"已找到瑕疵数： 个";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

//前一页  按钮
void CDeVisionDlg::OnBnClickedButtonGoup()
{
	// TODO: 在此添加控件通知处理程序代码

	CString cstr = L"向前翻页";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	PostMessage(WM_UPDATE_CONTROLS, 0, 0);
}

//下一页  按钮
void CDeVisionDlg::OnBnClickedButtonGodown()
{
	// TODO: 在此添加控件通知处理程序代码

	CString cstr = L"向后翻页";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	CString warning = L"向后翻页";
	::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&warning, NULL);

	//PostMessage(WM_UPDATE_HISTORY, 0, 0);
}

//开始  按钮
void CDeVisionDlg::OnBnClickedMfcbuttonStart()
{
	// TODO: 在此添加控件通知处理程序代码
	CWaitCursor wait;
	if (m_system_state != SYSTEM_STATE_PAUSE) {
		//开始运行前的准备
		ReStartPrepare();
		//启动图像采集
		if (!m_bTestModel) {
			if (!m_ImgAcq.Grab()) return;
		}
		bool state = false;
		for (int index = 0; index < m_nConnectedCameras; index++) {
			state = m_pImgProc[index]->BeginProcess();
			if (!state) {
				cstrlog.Format(_T("相机 #%d 处理线程初始化：失败，%d"), index + 1, GetLastError());
				::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
				return;
			}
			else {
				cstrlog.Format(_T("相机 #%d 处理线程初始化：成功"), index + 1);
				::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
			}
		}

		//启动主界面刷新线程
		StopRefrush_Event.ResetEvent();
		RefrushThreadStopped_Event.ResetEvent();
		m_RefrushThread = AfxBeginThread(RefrushWnd, this);

		//启动历史页面刷新
		m_historyDlg.m_pages = 0;

		CString cstr = L"启动检测...";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}
	else {
		CString cstr = L"结束暂停，启动检测...";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}

	//时间记录戳
	start_time = GetTickCount();

	if (m_system_state != SYSTEM_STATE_RUN)
		m_system_state = SYSTEM_STATE_RUN;

	PostMessage(WM_UPDATE_CONTROLS, 0, 0);
}

//停止  按钮
void CDeVisionDlg::OnBnClickedMfcbuttonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	CWaitCursor wait;

	if (m_system_state == SYSTEM_STATE_RUN || m_system_state == SYSTEM_STATE_PAUSE) {
		if(!m_bTestModel)
			m_ImgAcq.Freeze();
		for (int index = 0; index < m_nConnectedCameras; index++) {
			if (m_bTestModel) {
				m_pImgProc[index]->StopManageThread();
			}
			else
				m_pImgProc[index]->StopCalculateThreads();
		}

		//等待列表中的图像都处理完成, 之后结束处理线程
		StopRefrush_Event.SetEvent();

		m_tableDlg.TableSaved_Event.ResetEvent();
	}

	CString cstr = L"停止检测";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	//m_system_state = SYSTEM_STATE_STOP;

	PostMessage(WM_UPDATE_CONTROLS, 0, 0);
}

//暂停  按钮
void CDeVisionDlg::OnBnClickedMfcbuttonPause()
{
	//暂停检测，相机获取的图像不放入图像列表

	if (m_system_state != SYSTEM_STATE_PAUSE)
		m_system_state = SYSTEM_STATE_PAUSE;

	CString cstr = L"暂停检测";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	PostMessage(WM_UPDATE_CONTROLS, 0, 0);
}

//在线  按钮     连接相机 / 断开相机
void CDeVisionDlg::OnBnClickedMfcbuttonOnline()
{
	// TODO: 在此添加控件通知处理程序代码
	CWaitCursor wait;
	bool state;
	if (m_system_state == SYSTEM_STATE_OFFLINE) {
		if (!m_bTestModel) {
			//采集系统初始化
			state = m_ImgAcq.CameraSystemInitial();
			if (!state) {
				cstrlog.Format(_T("图像采集系统初始化：失败，%d"), GetLastError());
				::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
				return;
			}
			else {
				cstrlog.Format(_T("图像采集系统初始化：成功"));
				::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
			}
			m_nConnectedCameras = m_ImgAcq.m_nCameraNum;
		}
		else
			m_nConnectedCameras = 4;

		//启动图像处理线程
		for (int index = 0; index < m_nConnectedCameras; index++) {
			m_pImgProc[index] = new CImageProcessing(m_nThreadNumbers, m_nNormalDistribution,
				m_nFIlterSize, m_fRadiusMin, m_fRadiusMax);
			m_ImgAcq.m_pProcessing[index] = m_pImgProc[index];
			m_pImgProc[index]->TEST_MODEL = m_bTestModel;
			m_pImgProc[index]->m_threadnum = m_nThreadNumbers;
			m_pImgProc[index]->SAVE_REFERENCE_IMAGE = m_bSaveRefImg;
			m_phFinishProcessEvent[index] = &m_pImgProc[index]->m_hFinishedProcess;
		}		
		m_system_state = SYSTEM_STATE_ONLINE;
		//m_button_online.SetIcon(m_hOnlineIcon);
		m_button_online.SetIcon(m_hCameraInIcon);
	}
	else {		
		//删除图像处理线程
		for (int index = 0; index < m_nConnectedCameras; index++) {
			//应当在数据保存之后删除
			m_pImgProc[index]->ClearThisClass();
			delete m_pImgProc[index];
			m_pImgProc[index] = NULL;
		}

		m_vecDftDisplay.clear();
		cstrlog.Format(_T("当前模式：离线"));
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
		m_system_state = SYSTEM_STATE_OFFLINE;
		//m_button_online.SetIcon(m_hOfflineIcon);
		m_button_online.SetIcon(m_hCameraOutIcon);
	}

	PostMessage(WM_UPDATE_CONTROLS, 0, 0);
}

//锁定按钮
void CDeVisionDlg::OnBnClickedButtonLock()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_screen_state == SCREEN_UNLOCK) {
		m_button_lock.SetIcon(m_hLockIcon);
		m_screen_state = SCREEN_LOCK;

		CWnd *pwnd = this->GetDlgItem(IDC_BUTTON_LOCK);
		CRect rect;
		pwnd->GetWindowRect(&rect);
		ClipCursor(rect);
		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, true, 0, SPIF_UPDATEINIFILE);
		::ShowWindow(::FindWindow(L"Shell_TrayWnd", NULL), SW_HIDE);

		CString cstr = L"系统已锁定";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}
	else if (m_screen_state == SCREEN_LOCK) {
		ClipCursor(NULL);
		CLogin loginDlg;
		loginDlg.DoModal();

		//密码输入正确则解锁
		if (!loginDlg.ACCEPTED) {
			CWnd *pwnd = this->GetDlgItem(IDC_BUTTON_LOCK);
			CRect rect;
			pwnd->GetWindowRect(&rect);
			ClipCursor(rect);
			return;
		}
		ClipCursor(NULL);
		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, false, 0, SPIF_UPDATEINIFILE);
		::ShowWindow(::FindWindow(L"Shell_TrayWnd", NULL), SW_SHOW);

		m_button_lock.SetIcon(m_hUnlockIcon);
		m_screen_state = SCREEN_UNLOCK;

		CString cstr = L"系统解除锁定";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}

	PostMessage(WM_UPDATE_CONTROLS, 0, 0);
}

//退出按钮
void CDeVisionDlg::OnBnClickedButtonExit()
{
	// TODO: 在此添加控件通知处理程序代码

	ExitProgram();

	CString cstr = L"退出...";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	EndDialog(TRUE);
}

//*********************************************菜单****************************************//

//系统：设置
void CDeVisionDlg::OnSetup()
{
	// TODO: 在此添加命令处理程序代码
	CSetupDlg setup;
	setup.m_wnd1_range = m_wnd1_range;
	setup.m_wnd2_range = m_wnd2_range;
	setup.m_k_speed = m_ImgAcq.m_k_speed;
	setup.m_threadnum = m_nThreadNumbers;
	setup.m_bSaveRefImg = m_bSaveRefImg;
	setup.m_strDeffect_Path = m_strDeffect_Path;
	setup.m_strTable_Path = m_strTable_Path;
	setup.DoModal();
	if (setup.m_bSave_Parameter) {
		m_wnd1_range = setup.m_wnd1_range;
		CString ctext;
		ctext.Format(_T("%.2f"), m_wnd1_range);
		m_edisplay_range.SetWindowTextW(ctext);
		m_wnd2_range = setup.m_wnd2_range;

		m_ImgAcq.m_k_speed = setup.m_k_speed;

		m_nThreadNumbers = setup.m_threadnum;
		m_bSaveRefImg = setup.m_bSaveRefImg;
		m_strDeffect_Path = setup.m_strDeffect_Path;
		m_strTable_Path = setup.m_strTable_Path;
	}
}

//系统：保存
void CDeVisionDlg::OnSave()
{
	// TODO: 在此添加命令处理程序代码
	//SaveUserInfo();
	WriteToRegedit();

	CString cstr = L"保存当前设置";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

//系统：退出
void CDeVisionDlg::OnExit()
{
	// TODO: 在此添加命令处理程序代码
	EndDialog(TRUE);
}

//控制：相机设置
void CDeVisionDlg::OnCameraSetup()
{
	// TODO: 在此添加命令处理程序代码
	CCameraDlg cameraDlg;
	m_pCamera = &cameraDlg;
	cameraDlg.DoModal();
}

//控制：光源设置
void CDeVisionDlg::OnLedSetup()
{
	// TODO: 在此添加命令处理程序代码
	CCameraDlg camera;
	camera.DoModal();
}

//控制：触发设置
void CDeVisionDlg::OnTrigger()
{
	// TODO: 在此添加命令处理程序代码
	CCameraDlg camera;
	camera.DoModal();
}

//远程连接
void CDeVisionDlg::OnRemote()
{
	// TODO: 在此添加命令处理程序代码
	CRemote remoteDlg;
	remoteDlg.DoModal();
}

//瑕疵检测：产品信息
void CDeVisionDlg::OnProduct()
{
	// TODO: 在此添加命令处理程序代码
	CProductInfo productinfo;
	productinfo.DoModal();
	if (productinfo.m_bSave_Parameter) {
		m_inspectDlg.m_eNumber.SetWindowTextW(productinfo.m_ctrNUMBER);
		m_inspectDlg.m_eWidth.SetWindowTextW(productinfo.m_ctrWIDTH);
		m_inspectDlg.m_eModel.SetWindowTextW(productinfo.m_ctrID);
		m_inspectDlg.m_eOperator.SetWindowTextW(productinfo.m_ctrOPERATOR);
	}
}

//瑕疵检测：瑕疵趋势信息
void CDeVisionDlg::OnDeffectTrader()
{
	// TODO: 在此添加命令处理程序代码
	if (m_CurSelTab != 1) {
		m_tab.SetCurSel(1);
		pDialog[m_CurSelTab]->ShowWindow(SW_HIDE);
		pDialog[1]->ShowWindow(SW_SHOW);
	}
}

//瑕疵检测：瑕疵检测算法
void CDeVisionDlg::OnDefectAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CAlgorithmDlg algorithmDlg;
	if (m_pImgProc[0] != NULL) {
		algorithmDlg.m_normal_distribution = m_pImgProc[0]->m_k_normal_distribution;
		algorithmDlg.m_filter_size = m_pImgProc[0]->m_median_filter_size;
		algorithmDlg.m_min_radius = m_pImgProc[0]->m_fMin_Radius;
		algorithmDlg.m_max_radius = m_pImgProc[0]->m_fMax_Radius;
	}
	algorithmDlg.DoModal();
	if (algorithmDlg.m_bSave_Parameter) {
		if (m_nConnectedCameras > 0) {
			for (int index = 0; index < m_nConnectedCameras; index++) {
				if (m_pImgProc[index] != NULL) {
					m_pImgProc[index]->m_k_normal_distribution = algorithmDlg.m_normal_distribution;
					m_pImgProc[index]->m_median_filter_size = algorithmDlg.m_filter_size;
					m_pImgProc[index]->m_fMin_Radius = algorithmDlg.m_min_radius;
					m_pImgProc[index]->m_fMax_Radius = algorithmDlg.m_max_radius;
				}
			}
		}
	}
}

//历史记录：报表
void CDeVisionDlg::OnTable()
{
	// TODO: 在此添加命令处理程序代码

	CStringW wpath = CA2W(m_strTable_Path.c_str());

	// get root path
	ShellExecute(NULL, L"explore", wpath, NULL, NULL, SW_SHOW);
}

//历史记录：图像
void CDeVisionDlg::OnImage()
{
	// TODO: 在此添加命令处理程序代码
	CStringW wpath = CA2W(m_strDeffect_Path.c_str());

	ShellExecute(NULL, L"explore", wpath, NULL, NULL, SW_SHOW);
}

//帮助：帮助
void CDeVisionDlg::OnHelp()
{
	// TODO: 在此添加命令处理程序代码
	TCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	CString cpath = path;
	cpath = cpath + L"\\help\\UserGuide.CHM";
	ShellExecute(NULL, L"open", cpath, NULL, NULL, SW_SHOWMAXIMIZED);
}

//帮助：日志
void CDeVisionDlg::OnRecord()
{
	// TODO: 在此添加命令处理程序代码
	TCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	CString cpath = path;
	cpath = cpath + L"\\log";
	ShellExecute(NULL, L"explore", cpath, NULL, NULL, SW_SHOW);
}

//帮助：关于
void CDeVisionDlg::OnAbout()
{
	// TODO: 在此添加命令处理程序代码
	CAboutDlg about;
	about.DoModal();
}

//写入日志
afx_msg LRESULT CDeVisionDlg::OnLoggingMsg(WPARAM wParam, LPARAM lParam)
{
	CString *strMsg = (CString*)wParam;
	m_inspectDlg.RecordLogList(0, *strMsg);
	Win::log(CW2A(*strMsg));

	return 0;
}

//写入报警
afx_msg LRESULT CDeVisionDlg::OnWarningMsg(WPARAM wParam, LPARAM lParam)
{
	CString *strMsg = (CString*)wParam;
	m_inspectDlg.RecordWarning(0, *strMsg);

	CString warning;
	warning.Format(_T("ERROR: "));
	warning = warning + *strMsg;
	Win::log(CW2A(warning));

	return 0;
}

//控件更新
afx_msg LRESULT CDeVisionDlg::OnUpdateControls(WPARAM wParam, LPARAM lParam)
{
	CMenu *pSysMenu = GetSystemMenu(FALSE);
	ASSERT(pSysMenu != NULL);

	CMenu *pMenu = GetMenu();

	switch (m_system_state)
	{
	case SYSTEM_STATE_OFFLINE:
		GetDlgItem(IDC_MFCBUTTON_START)->EnableWindow(false);
		GetDlgItem(IDC_MFCBUTTON_STOP)->EnableWindow(false);
		GetDlgItem(IDC_MFCBUTTON_PAUSE)->EnableWindow(false);
		GetDlgItem(IDC_MFCBUTTON_ONLINE)->EnableWindow(true);
		m_button_online.SetWindowTextW(_T("连接\n\n相机"));
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(true);
		if (pMenu) {
			pMenu->EnableMenuItem(ID_SETUP, MF_ENABLED);
			//pMenu->EnableMenuItem(ID_EXIT, MF_ENABLED);
			pMenu->EnableMenuItem(ID_CAMERA_SETUP, MF_DISABLED);
			pMenu->EnableMenuItem(ID_LED_SETUP, MF_DISABLED);
			pMenu->EnableMenuItem(ID_TRIGGER, MF_DISABLED);
			pMenu->EnableMenuItem(ID_DEFECT_ANALYSIS, MF_DISABLED);
		}

		break;
	case SYSTEM_STATE_ONLINE:
		GetDlgItem(IDC_MFCBUTTON_START)->EnableWindow(true);
		GetDlgItem(IDC_MFCBUTTON_STOP)->EnableWindow(false);
		GetDlgItem(IDC_MFCBUTTON_PAUSE)->EnableWindow(false);
		GetDlgItem(IDC_MFCBUTTON_ONLINE)->EnableWindow(true);
		m_button_online.SetWindowTextW(_T("断开\n\n相机"));
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(false);
		if (pMenu) {
			pMenu->EnableMenuItem(ID_SETUP, MF_DISABLED);
			//pMenu->EnableMenuItem(ID_EXIT, MF_DISABLED);
			pMenu->EnableMenuItem(ID_CAMERA_SETUP, MF_ENABLED);
			pMenu->EnableMenuItem(ID_LED_SETUP, MF_ENABLED);
			pMenu->EnableMenuItem(ID_TRIGGER, MF_ENABLED);
			pMenu->EnableMenuItem(ID_DEFECT_ANALYSIS, MF_ENABLED);
		}

		break;
	case SYSTEM_STATE_RUN:
		GetDlgItem(IDC_MFCBUTTON_START)->EnableWindow(false);
		if (m_pImgProc[0] != NULL) {
			if (m_pImgProc[0]->CheckReferenceImageAvilable()) {
				GetDlgItem(IDC_MFCBUTTON_STOP)->EnableWindow(true);
				GetDlgItem(IDC_MFCBUTTON_PAUSE)->EnableWindow(true);
			}
		}
		GetDlgItem(IDC_MFCBUTTON_ONLINE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(false);

		m_inspectDlg.m_btn_changeinfo.EnableWindow(false);
		m_tableDlg.m_open_inprogram.EnableWindow(false);
		m_historyDlg.m_btn_pre_page.EnableWindow(false);
		m_historyDlg.m_btn_next_page.EnableWindow(false);

		m_ImgAcq.m_bSystemPause = FALSE;
		if (m_nConnectedCameras > 0) {
			for (int index = 0; index < m_nConnectedCameras; index++) {
				if (m_pImgProc[index] != NULL) m_pImgProc[index]->SYSTEM_PAUSE = FALSE;				
			}
		}

		if (pMenu) {
			pMenu->EnableMenuItem(ID_PRODUCT, MF_DISABLED);
			pMenu->EnableMenuItem(ID_DEFFECT_TRADER, MF_DISABLED);
			pMenu->EnableMenuItem(ID_DEFECT_ANALYSIS, MF_DISABLED);
		}

		break;
	case SYSTEM_STATE_PAUSE:
		GetDlgItem(IDC_MFCBUTTON_START)->EnableWindow(true);
		GetDlgItem(IDC_MFCBUTTON_STOP)->EnableWindow(true);
		//if (m_ImgProc.m_total_list_size == 0)
		//	GetDlgItem(IDC_MFCBUTTON_STOP)->EnableWindow(true);
		GetDlgItem(IDC_MFCBUTTON_PAUSE)->EnableWindow(false);
		GetDlgItem(IDC_MFCBUTTON_ONLINE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(false);

		m_inspectDlg.m_btn_changeinfo.EnableWindow(false);
		m_historyDlg.m_btn_pre_page.EnableWindow(true);
		m_historyDlg.m_btn_next_page.EnableWindow(true);

		m_ImgAcq.m_bSystemPause = TRUE;
		if (m_nConnectedCameras > 0) {
			for (int index = 0; index < m_nConnectedCameras; index++) {
				if (m_pImgProc[index] != NULL) m_pImgProc[index]->SYSTEM_PAUSE = TRUE;
			}
		}
		break;
	case SYSTEM_STATE_STOP:
		GetDlgItem(IDC_MFCBUTTON_START)->EnableWindow(true);
		GetDlgItem(IDC_MFCBUTTON_STOP)->EnableWindow(false);
		GetDlgItem(IDC_MFCBUTTON_PAUSE)->EnableWindow(false);
		GetDlgItem(IDC_MFCBUTTON_ONLINE)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(false);

		m_inspectDlg.m_btn_changeinfo.EnableWindow(true);
		m_tableDlg.m_open_inprogram.EnableWindow(true);
		m_historyDlg.m_btn_pre_page.EnableWindow(true);
		m_historyDlg.m_btn_next_page.EnableWindow(true);

		m_ImgAcq.m_bSystemPause = FALSE;
		if (m_nConnectedCameras > 0) {
			for (int index = 0; index < m_nConnectedCameras; index++) {
				if (m_pImgProc[index] != NULL) m_pImgProc[index]->SYSTEM_PAUSE = FALSE;
			}
		}

		if (pMenu) {
			pMenu->EnableMenuItem(ID_PRODUCT, MF_ENABLED);
			pMenu->EnableMenuItem(ID_DEFFECT_TRADER, MF_ENABLED);
			pMenu->EnableMenuItem(ID_DEFECT_ANALYSIS, MF_ENABLED);
		}

		break;
	default:
		break;
	}

	switch (m_screen_state)
	{
	case SCREEN_UNLOCK:

		break;
	case SCREEN_LOCK:

		break;
	default:
		break;
	}

	//更新颜色选项
	UpdateSysColor();

	return 0;
}

//历史图像更新
afx_msg LRESULT CDeVisionDlg::OnUpdateHistory(WPARAM wParam, LPARAM lParam)
{
	m_historyDlg.LoadHistoryImage();

	return 0;
}

//主窗口更新
afx_msg LRESULT CDeVisionDlg::OnUpdateMainwnd(WPARAM wParam, LPARAM lParam)
{
	CString *cstrMsg = (CString*)wParam;
	std::string strmsg = (CW2A)cstrMsg->GetBuffer();
	std::string strdeffect_kind = strmsg.substr(0, 1);
	std::string strdisplay = strmsg.substr(2, 1);
	int kind = std::stoi(strdeffect_kind);
	bool show = (bool)std::stoi(strdisplay);
	pView->m_bFlagShow[kind - 1] = show;

	return 0;
}
