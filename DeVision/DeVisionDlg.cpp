
// DeVisionDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "DeVisionDlg.h"
#include "afxdialogex.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4);

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
	m_hStartIcon = AfxGetApp()->LoadIcon(IDI_ICON_START);
	m_hStopIcon = AfxGetApp()->LoadIcon(IDI_ICON_STOP);
	m_hPauseIcon = AfxGetApp()->LoadIcon(IDI_ICON_PAUSE);
	m_hUpIcon = AfxGetApp()->LoadIcon(IDI_ICON_UP);
	m_hDownIcon = AfxGetApp()->LoadIcon(IDI_ICON_DOWN);
	m_hLockIcon = AfxGetApp()->LoadIcon(IDI_ICON_LOCK);
	m_hUnlockIcon = AfxGetApp()->LoadIcon(IDI_ICON_UNLOCK);
	m_hExitIcon = AfxGetApp()->LoadIconW(IDI_ICON_EXIT);

	//初始化算法类
	m_inspectDlg.m_pImgProc = &m_ImgProc;

	m_system_state = SYSTEM_STATE_OFFLINE;
	m_screen_state = SCREEN_UNLOCK;

	//初始化临界区
	InitializeCriticalSection(&m_csVecDFT);

}

CDeVisionDlg::~CDeVisionDlg()
{

	DeleteCriticalSection(&m_csVecDFT);
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
	ON_MESSAGE(WM_LOGGING_MSG, &CDeVisionDlg::OnLoggingMsg)
	ON_MESSAGE(WM_WARNING_MSG, &CDeVisionDlg::OnWarningMsg)
	ON_COMMAND(ID_REMOTE, &CDeVisionDlg::OnRemote)
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
	flag_font.CreatePointFont(200, _T("Times New Roman"));
	small_flag_font.CreatePointFont(50, _T("Times New Roman"));
	loggle_font.CreatePointFont(300, _T("楷体"));
	CString logge;
	logge.Format(L"浙   清   柔   电");
	GetDlgItem(IDC_STATIC_LOGGLE)->SetWindowText(logge);
	GetDlgItem(IDC_STATIC_LOGGLE)->SetFont(&loggle_font);

	// Table 页面初始化
	InitialTabDlg();
	isTabInitialized = true;

	//状态栏初始化
	InitialStateBar();

	//按钮图标初始化
	InitialBtnIcon();

	//用户数据加载
	LoadCustomerDate();

	//初始化全局瑕疵显示区, 设置显示范围
	InitialTotalDefect();
	m_display_range = 100.0f;

	// 报表页面初始化
	m_tableDlg.m_pvDFT = &m_vDFT;

	//加载默认设置
	LoadInitialInfo();

	//设置默认工作路径
	m_work_path = "D:\\history\\";
	m_fold_name = "";


	//主界面信息刷新定时器
	SetTimer(1, 1000, 0);

	m_iAllThread_stopped = 0;
	//m_inspectDlg.FREE_RUN = FALSE;
	m_ImgProc.TEST_MODEL = TRUE;

	std::string struser((LPCSTR)CW2A(m_tableDlg.m_wstr_user.c_str()));
	CString cuser = CA2W(struser.c_str());
	CString cstr = L"程序已启动，当前操作员：" + cuser;
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDeVisionDlg::ExitProgram()
{
	m_vDFT.clear();

	SaveUserInfo();

	//停止界面刷新
	KillTimer(1);

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

int CDeVisionDlg::CheckThreadStatue()
{
	if (m_ImgProc.IsThreadsAlive())
		return 1;

	BOOL camera_thread = m_inspectDlg.m_camera1_thread_alive || m_inspectDlg.m_camera2_thread_alive
			      		|| m_inspectDlg.m_camera3_thread_alive || m_inspectDlg.m_camera4_thread_alive;
	if (camera_thread)
		return 3;

	return 0;
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

int CDeVisionDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CLogin loginDlg;
	loginDlg.DoModal();

	if (!loginDlg.ACCEPTED)
		return -1;
	else
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

BOOL CDeVisionDlg::LoadInitialInfo()
{
	LPCWSTR PATH = L"inis\\SystemInfo.ini";
	LPWSTR ReturnedString = new wchar_t[STRINGLENGTH];

	GetPrivateProfileStringW(L"UserInfo", L"NUMBER", L"", ReturnedString, STRINGLENGTH, PATH);
	m_inspectDlg.m_static_number.SetWindowTextW(ReturnedString);
	m_tableDlg.m_wstr_num = ReturnedString;

	GetPrivateProfileStringW(L"UserInfo", L"WIDTH", L"", ReturnedString, STRINGLENGTH, PATH);
	m_inspectDlg.m_static_width.SetWindowTextW(ReturnedString);
	m_tableDlg.m_wstr_width = ReturnedString;

	GetPrivateProfileStringW(L"UserInfo", L"ID", L"", ReturnedString, STRINGLENGTH, PATH);
	m_inspectDlg.m_static_id.SetWindowTextW(ReturnedString);
	m_tableDlg.m_wstr_id = ReturnedString;

	GetPrivateProfileStringW(L"UserInfo", L"OPERATOR", L"", ReturnedString, STRINGLENGTH, PATH);
	m_inspectDlg.m_static_operator.SetWindowTextW(ReturnedString);
	m_tableDlg.m_wstr_user = ReturnedString;

	delete[] ReturnedString;
	return TRUE;
}

BOOL CDeVisionDlg::SaveUserInfo()
{
	LPCWSTR PATH = L"inis\\SystemInfo.ini";

	CStringW wtext;
	m_inspectDlg.m_static_number.GetWindowTextW(wtext);
	LPCWSTR WriteString = (LPCWSTR)(LPCTSTR)wtext;
	WritePrivateProfileStringW(L"UserInfo", L"NUMBER", WriteString, PATH);
	m_tableDlg.m_wstr_num = WriteString;

	m_inspectDlg.m_static_width.GetWindowTextW(wtext);
	WriteString = (LPCWSTR)(LPCTSTR)wtext;
	WritePrivateProfileStringW(L"UserInfo", L"WIDTH", WriteString, PATH);
	m_tableDlg.m_wstr_width = WriteString;

	m_inspectDlg.m_static_id.GetWindowTextW(wtext);
	WriteString = (LPCWSTR)(LPCTSTR)wtext;
	WritePrivateProfileStringW(L"UserInfo", L"ID", WriteString, PATH);
	m_tableDlg.m_wstr_id = WriteString;

	m_inspectDlg.m_static_operator.GetWindowTextW(wtext);
	WriteString = (LPCWSTR)(LPCTSTR)wtext;
	WritePrivateProfileStringW(L"UserInfo", L"OPERATOR", WriteString, PATH);
	m_tableDlg.m_wstr_user = WriteString;

	//用户信息更新完成
	m_inspectDlg.m_bUpdateUserInfo = FALSE;

	CString cstr = L"保存当前设置";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	return TRUE;
}

BOOL CDeVisionDlg::GetUserInfo(std::string &num, std::string &width, std::string &id, std::string &user)
{
	CStringW cstrnumber;
	m_inspectDlg.m_static_number.GetWindowTextW(cstrnumber);
	num = CW2A(cstrnumber);

	CStringW cstrwidth;
	m_inspectDlg.m_static_width.GetWindowTextW(cstrwidth);
	width = CW2A(cstrwidth);

	CStringW cstrid;
	m_inspectDlg.m_static_id.GetWindowTextW(cstrid);
	id = CW2A(cstrid);

	CStringW cstruser;
	m_inspectDlg.m_static_operator.GetWindowTextW(cstruser);
	user = CW2A(cstruser);

	return TRUE;
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

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

//定时器回调函数
void CDeVisionDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1: 
		//获取当前线程状态
		if(m_system_state == SYSTEM_STATE_STOP || m_system_state == SYSTEM_STATE_OFFLINE)
			m_iAllThread_stopped = CheckThreadStatue();
		//更新按钮和菜单
		UpdateSysMenuBtn();
		//更新状态栏
		UpdateSysStatus();
		//更新当前位置
		current_position = m_ImgProc.m_current_position;
		m_tableDlg.m_current_position = current_position;
		//更新刻度
		UpdateScaleValue(1650.0f, current_position);
		//保存用户信息
		if (m_inspectDlg.m_bUpdateUserInfo)
			SaveUserInfo();

		break;
	case 2:
		//更新历史图像，选中历史页面
		if (m_CurSelTab == 3 && m_system_state != SYSTEM_STATE_OFFLINE && m_historyDlg.m_pages == 0)
			m_historyDlg.LoadHistoryImage();
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CDeVisionDlg::TestLoadAndWrite()
{
	test_num += 1;

	HObject load, write;
	HTuple hv_name = "D:\\test_image.bmp";
	HalconCpp::ReadImage(&load, hv_name);

	HTuple end = ".bmp";
	HTuple save = (HTuple)m_work_path.c_str() + (HTuple)test_num + end;
	HalconCpp::WriteImage(load, "bmp", 0, save);

}

void CDeVisionDlg::LoadCustomerDate()
{
	LPWSTR ReturnedString = new wchar_t[STRINGLENGTH];
	//LPWSTR KeyName;

	//产品信息 InspectDlg
	//GetPrivateProfileStringW(L"MainForm", L"ID", L"", ReturnedString, STRINGLENGTH, CUSTOMER_FILEPATH);
	//m_inspectDlg.m_static_id.SetWindowTextW(ReturnedString);

	delete[] ReturnedString;
}

//TabControl 切换
void CDeVisionDlg::OnTcnSelchangeTabDialog(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	
	//把当前的页面隐藏起来
	pDialog[m_CurSelTab]->ShowWindow(SW_HIDE);
	//得到新的页面索引
	m_CurSelTab = m_tab.GetCurSel();
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

//状态栏初始化
void CDeVisionDlg::InitialStateBar()
{
	//添加状态栏
	static UINT indicators[] = {
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
	if (!m_StatusBar.Create(this) || !m_StatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("Can't create status bar\n");
		return;
	}
	m_StatusBar.GetClientRect(&stateRect);
	m_StatusBar.MoveWindow(0, rect.bottom - stateRect.Height(), rect.right, stateRect.Height());// 调整状态栏的位置和大小
	//int strPartDim[9] = { 150, 350, 500, 700, 850, 950, 1050, 1200, -1 }; //分割数量
	m_StatusBar.SetPaneInfo(0, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 200);//设置状态栏的宽度
	m_StatusBar.SetPaneInfo(1, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(2, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(3, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(4, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(5, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 200);
	m_StatusBar.SetPaneInfo(6, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(7, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	m_StatusBar.SetPaneInfo(8, m_StatusBar.GetDlgCtrlID(), SBPS_STRETCH, 150);
	//m_StatusBar.SetPaneBackgroundColor(8, RGB(255, 0, 0));  //设置背景色

	//m_StatusBar.SetIcon(AfxGetApp()->LoadIcon(IDR_STATEBAR), TRUE);//为分栏中加的图标

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);//显示状态栏
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
	m_button_online.SetIcon(m_hOfflineIcon);

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
	ctext.Format(_T("%.2f"), m_display_range);
	m_edisplay_range.SetWindowTextW(ctext);

	return TRUE;
}

//从瑕疵队列中取出
void CDeVisionDlg::DelQueueFromSource()
{
	//重新排序
	m_ImgProc.ReSortDefectQueue();

	EnterCriticalSection(&m_csVecDFT);
	//从原始队列中取出
	if (!m_ImgProc.m_Sorted_DFTList.empty()) {
		DefectType crt_defect;
		auto queue_length = m_ImgProc.m_Sorted_DFTList.size();
		for (auto i = 0; i < queue_length; i++)
		{
			crt_defect = m_ImgProc.m_Sorted_DFTList.front();
			m_ImgProc.m_Sorted_DFTList.pop_front();

			m_vDFT.push_back(crt_defect);
			pView->m_vDefect.push_back(crt_defect);     
			
			switch (crt_defect.type)
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

			//判断是否是严重缺陷
			if (crt_defect.area > 50 || crt_defect.contlength > 32)
				serious_def_num += 1;
		}
		total_number_def += (int)queue_length;
		total_def_length += queue_length * 0.01f;
		//TRACE("QUEUE LENGTH %d\n", queue_length);
	}
	LeaveCriticalSection(&m_csVecDFT);

}

//划分当前薄膜的等级
int CDeVisionDlg::DevideDFTRank(int num)
{
	int rank;
	if(num < 10)
		rank = RANK_COMMON;
	else if(10 <= num && num < 100)
		rank = RANK_GRADE1;
	else if (100 <= num && num < 500)
		rank = RANK_GRADE2;
	else if (500 <= num && num < 1000)
		rank = RANK_GRADE3;
	else if (1000 <= num )
		rank = RANK_SERIOUS;
	return rank;
}

//创建瑕疵标记
void CDeVisionDlg::CreateFlag(CDC &mDC, int x, int y, int kind)
{
	switch (kind)
	{
	case 0: {
		mDC.SetBkColor(pView->m_acolor[0]);
		mDC.TextOutW(x, y, _T("A"));
		break;
	}
	case 1: {
		mDC.SetBkColor(pView->m_acolor[1]);
		mDC.TextOutW(x, y, _T("B"));
		break;
	}
	case 2: {
		mDC.SetBkColor(pView->m_acolor[2]);
		mDC.TextOutW(x, y, _T("C"));
		break;
	}
	case 3: {
		mDC.SetBkColor(pView->m_acolor[3]);
		mDC.TextOutW(x, y, _T("D"));
		break;
	}
	case 4:
		mDC.SetBkColor(pView->m_acolor[4]);
		mDC.TextOutW(x, y, _T("E"));
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
	float scale_x = (float)wnd_width / (IMAGE_WIDTH * 4 * HORIZON_PRECISION);
	float scale_y = (float)(5.0f / wnd_height);
	   
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

	DefectType temp_def;
	if (!m_vDFT.empty()) {

		//方法III：反向迭代器输出
		std::vector<DefectType>::reverse_iterator it = m_vDFT.rbegin();
		for (; it != m_vDFT.rend(); ++it)
		{
			temp_def = *it;
			int itvalue = (int)(std::distance(m_vDFT.rbegin(), it));
			int x_coord = (int)(temp_def.center_x * scale_x);
			int y_coord = wnd_height - (int)((temp_def.absolute_position - m_previous_position) / scale_y);

			if (y_coord < 0)
				m_previous_position = temp_def.absolute_position - 5.0f;
			if (y_coord > wnd_height)
				break;

			CreateFlag(MemDC, x_coord, y_coord, temp_def.type);

			if (itvalue == 0) {
				CString cwidth, cposition;
				cwidth.Format(_T("%.1f 毫米"), temp_def.center_x);
				cposition.Format(_T("%.3f 米"), temp_def.absolute_position);
				m_ewidth1.SetWindowTextW(cwidth);
				m_elongth1.SetWindowTextW(cposition);
			}
			else if (itvalue == 1) {
				CString cwidth, cposition;
				cwidth.Format(_T("%.1f 毫米"), temp_def.center_x);
				cposition.Format(_T("%.3f 米"), temp_def.absolute_position);
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
		float current_pos = position - i * m_display_range / 12.0f;
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

float CDeVisionDlg::GetDisplayRange()
{
	float range;
	CString str_edit;
	m_edisplay_range.GetWindowTextW(str_edit);
	range = (float)_ttof(str_edit);
	if (range != 0 && range <= current_position) {
		return range;
	}
	else
		return 0.0f;
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

//更新菜单和按钮
void CDeVisionDlg::UpdateSysMenuBtn()
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
		m_button_online.SetWindowTextW(_T("在线"));
		if(m_iAllThread_stopped == 0)
			GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(true);
		if (pMenu){
			pMenu->EnableMenuItem(ID_SETUP, MF_ENABLED);
			//pMenu->EnableMenuItem(ID_EXIT, MF_ENABLED);
			pMenu->EnableMenuItem(ID_CAMERA_SETUP, MF_DISABLED);
			pMenu->EnableMenuItem(ID_LED_SETUP, MF_DISABLED);
			pMenu->EnableMenuItem(ID_TRIGGER, MF_DISABLED);
		}

		break;
	case SYSTEM_STATE_ONLINE:
		GetDlgItem(IDC_MFCBUTTON_START)->EnableWindow(true);
		GetDlgItem(IDC_MFCBUTTON_STOP)->EnableWindow(false);
		GetDlgItem(IDC_MFCBUTTON_PAUSE)->EnableWindow(false);
		GetDlgItem(IDC_MFCBUTTON_ONLINE)->EnableWindow(true);
		m_button_online.SetWindowTextW(_T("离线"));
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(false);
		if (pMenu){
			pMenu->EnableMenuItem(ID_SETUP, MF_DISABLED);
			//pMenu->EnableMenuItem(ID_EXIT, MF_DISABLED);
			pMenu->EnableMenuItem(ID_CAMERA_SETUP, MF_ENABLED);
			pMenu->EnableMenuItem(ID_LED_SETUP, MF_ENABLED);
			pMenu->EnableMenuItem(ID_TRIGGER, MF_ENABLED);
		}

		break;
	case SYSTEM_STATE_RUN:
		GetDlgItem(IDC_MFCBUTTON_START)->EnableWindow(false);
		if (m_ImgProc.m_referenceImage_OK) {
			GetDlgItem(IDC_MFCBUTTON_STOP)->EnableWindow(true);
			GetDlgItem(IDC_MFCBUTTON_PAUSE)->EnableWindow(true);
		}
		GetDlgItem(IDC_MFCBUTTON_ONLINE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(false);

		m_inspectDlg.m_btn_changeinfo.EnableWindow(false);
		m_tableDlg.m_open_inprogram.EnableWindow(false);
		m_historyDlg.m_btn_pre_page.EnableWindow(false);
		m_historyDlg.m_btn_next_page.EnableWindow(false);
		if (pMenu) {
			pMenu->EnableMenuItem(ID_PRODUCT, MF_DISABLED);
			pMenu->EnableMenuItem(ID_DEFFECT_TRADER, MF_DISABLED);
			pMenu->EnableMenuItem(ID_DEFECT_ANALYSIS, MF_DISABLED);
		}

		break;
	case SYSTEM_STATE_PAUSE:
		GetDlgItem(IDC_MFCBUTTON_START)->EnableWindow(true);
		if(m_ImgProc.m_total_list_size == 0)
			GetDlgItem(IDC_MFCBUTTON_STOP)->EnableWindow(true);
		GetDlgItem(IDC_MFCBUTTON_PAUSE)->EnableWindow(false);
		GetDlgItem(IDC_MFCBUTTON_ONLINE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(false);

		m_inspectDlg.m_btn_changeinfo.EnableWindow(false);
		m_historyDlg.m_btn_pre_page.EnableWindow(true);
		m_historyDlg.m_btn_next_page.EnableWindow(true);


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
}

//更新状态栏
void CDeVisionDlg::UpdateSysStatus()
{
	CString cstr_ID;
	m_inspectDlg.m_static_id.GetWindowText(cstr_ID);
	if (cstr_ID != _T(""))
	{
		CString name, cstr_unit;
		name.Format(L"产品： ");
		name += cstr_ID;
		cstr_ID = name + cstr_unit;
		m_StatusBar.SetPaneText(1, cstr_ID, 1);
	}

	CString cstr_operator;
	m_inspectDlg.m_static_operator.GetWindowText(cstr_operator);
	if (cstr_operator != _T(""))
	{
		CString name, cstr_unit;
		name.Format(L"操作员： ");
		name += cstr_operator;
		cstr_operator = name + cstr_unit;
		m_StatusBar.SetPaneText(2, cstr_operator, 1);
	}

	//if (m_system_state == SYSTEM_STATE_ONLINE || m_system_state == SYSTEM_STATE_RUN)
	if(m_system_state != SYSTEM_STATE_OFFLINE)
	{
		if (current_position > 0)
		{
			CString cst_current_pos;
			cst_current_pos.Format(L"当前位置：第 %.2f 米", current_position);
			m_StatusBar.SetPaneText(3, cst_current_pos, 1);
		}

		float speed = 0.0f;
		if(!m_ImgProc.TEST_MODEL)
			speed = m_inspectDlg.CalculateEncoderSpeed();
		if (speed > 0)
		{
			CString cstr_speed, cstr_unit;
			cstr_speed.Format(L"当前速度：%.2f 米/分钟", speed);
			m_StatusBar.SetPaneText(4, cstr_speed, 1);

			m_tableDlg.m_wstr_speed = std::to_wstring(speed);
		}

		//总处理数量
		UINT64 acquire_frame_count = m_inspectDlg.GetTotalFrameCount();
		if (acquire_frame_count >= 0)
		{
			CString cstr_frame_count, cstr_unit;
			cstr_frame_count.Format(L"总帧数：%d 帧(%d)", acquire_frame_count, m_ImgProc.m_total_list_size);
			m_StatusBar.SetPaneText(5, cstr_frame_count, 1);
		}

		int trash_count = m_inspectDlg.GetTotalTrashCount();
		if (trash_count > 0)
		{
			CString cstr_trash_count, cstr_unit;
			cstr_trash_count.Format(L"丢帧数： %d 帧", trash_count);
			m_StatusBar.SetPaneText(6, cstr_trash_count, 1);
		}
		if (total_number_def > 0)
		{
			CString cst_current_pos;
			cst_current_pos.Format(L"瑕疵数：%d / %d  ", serious_def_num, total_number_def);
			m_StatusBar.SetPaneText(7, cst_current_pos, 1);
		}
	}

	CString cstr_statue;
	switch (m_system_state)
	{
	case SYSTEM_STATE_OFFLINE:
		cstr_statue.Format(L"离线: %d", m_iAllThread_stopped);
		m_StatusBar.SetPaneText(8, cstr_statue, 1);
		break;
	case SYSTEM_STATE_ONLINE:
		cstr_statue.Format(L"在线");
		m_StatusBar.SetPaneText(8, cstr_statue, 1);
		break;
	case SYSTEM_STATE_RUN:
		cstr_statue.Format(L"运行");
		m_StatusBar.SetPaneText(8, cstr_statue, 1);
		break;
	case SYSTEM_STATE_STOP:
		cstr_statue.Format(L"停止");
		m_StatusBar.SetPaneText(8, cstr_statue, 1);
		break;
	case SYSTEM_STATE_PAUSE:
		cstr_statue.Format(L"暂停");
		m_StatusBar.SetPaneText(8, cstr_statue, 1);
		break;
	default:
		break;
	}

	//inspectDlg中更新瑕疵信息
	m_inspectDlg.UpdateDFTinformation(total_number_def, serious_def_num, total_def_length);

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
	m_fold_name = strpath;         //保存文件夹名称

	std::string work_fold = "D:\\history\\";
	path = work_fold + strpath;
	//path = m_work_path + strpath;

	//创建文件夹
	_mkdir(path.c_str());

	//创建参考图像文件夹
	std::string reference_image = path + "\\ref";
	_mkdir(reference_image.c_str());
	path += "\\";

	CString cstr = L"工作目录已创建";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

//重新启动检测
void CDeVisionDlg::ReStartPrepare()
{
	m_previous_position = 0.0f;
	current_position = 0.0f;
	total_number_def = 0;
	serious_def_num = 0;
	total_def_length = 0.0f;
	memset(m_rank, 0, sizeof(m_rank));

	m_vDFT.clear();
	m_ImgProc.RestartProcess();
	m_inspectDlg.RestartInspect();
	pView->Redraw();

	//创建工作目录
	CreateWorkPath(m_work_path);
	//获取瑕疵图像保存路径
	m_ImgProc.m_strPath = m_work_path;
	m_tableDlg.m_DFT_img_path         = m_work_path;
	m_historyDlg.m_file_path          = m_work_path;

	return;
}

//界面刷新线程函数
UINT CDeVisionDlg::RefrushWnd(LPVOID pParam)
{
	CDeVisionDlg *pDlg = (CDeVisionDlg *)pParam;
	DWORD dwStop = 0;

	CString cstr = L"启动界面刷新线程";
	::SendMessage(pDlg->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	for(;;)
	{
		//获取瑕疵信息数据
		pDlg->DelQueueFromSource();

		dwStop = WaitForSingleObject(pDlg->StopRefrush_Event, 2000);
		switch (dwStop)
		{
		case WAIT_TIMEOUT: {
			if (pDlg->m_system_state != SYSTEM_STATE_PAUSE) {
				pDlg->DrawPartial(5);
				pDlg->pView->UpdateScreen(pDlg->small_flag_font, pDlg->m_display_range);

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
			wndLoad.Create(IDB_BITMAP_WAIT);
			wndLoad.UpdateWindow();
			HWND hwnd = wndLoad.GetSafeHwnd();

			if (WAIT_OBJECT_0 == WaitForSingleObject(pDlg->m_ImgProc.AllCalculateThreadStopped_Event, INFINITE)) {
				//结束历史图像刷新定时器
				pDlg->KillTimer(2);
				//保存检测记录
				EnterCriticalSection(&pDlg->m_csVecDFT);
				pDlg->m_tableDlg.m_vecDFT = pDlg->m_vDFT;
				pDlg->m_tableDlg.m_product_rank = pDlg->DevideDFTRank(pDlg->total_number_def);
				pDlg->m_tableDlg.m_serious_num = pDlg->serious_def_num;
				pDlg->m_tableDlg.GetDetectResult(
					pDlg->m_rank[0], 
					pDlg->m_rank[1], 
					pDlg->m_rank[2], 
					pDlg->m_rank[3], 
					pDlg->m_rank[4]);

				pDlg->m_tableDlg.BeginSaveTable();
				LeaveCriticalSection(&pDlg->m_csVecDFT);
			}
			WaitForSingleObject(pDlg->m_tableDlg.TableSaved_Event, INFINITE);
			pDlg->m_system_state = SYSTEM_STATE_STOP;

			//关闭等待界面
			wndLoad.DestroyWindow();

			CString cstr = L"结束界面刷新线程";
			::SendMessage(pDlg->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
			return 0;
		}
		default:
			break;							
		}
	}
	CString cstop = L"结束界面刷新线程";
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

}

//下一页  按钮
void CDeVisionDlg::OnBnClickedButtonGodown()
{
	// TODO: 在此添加控件通知处理程序代码	

	CString cstr = L"向后翻页";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	CString warning = L"向后翻页";
	::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&warning, NULL);

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
		//if (m_inspectDlg.Grab() == 0)
		if (1) {
			if (m_ImgProc.BeginProcess()) {				
				//启动主界面刷新线程
				StopRefrush_Event.ResetEvent();
				RefrushThreadStopped_Event.ResetEvent();
				m_RefrushThread = AfxBeginThread(RefrushWnd, this);

				//启动历史页面刷新
				m_historyDlg.m_pages = 0;
				SetTimer(2, 3000, 0);
			}
			else {
				//处理线程启动失败则结束采集
				m_inspectDlg.Freeze();
				CString cstr = L"图像处理线程启动失败";
				::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
				return;
			}
		}
		else {
			//Win::log("获取图像失败");
			//RecordWarning(L"获取图像失败");
			return;
		}

		CString cstr = L"启动检测...";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}
	else {
		m_inspectDlg.m_is_system_pause = FALSE;

		CString cstr = L"结束暂停，启动检测...";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}	

	//时间记录戳
	start_time = GetTickCount();

	if(m_system_state != SYSTEM_STATE_RUN)
		m_system_state = SYSTEM_STATE_RUN;

}

//停止  按钮
void CDeVisionDlg::OnBnClickedMfcbuttonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	CWaitCursor wait;

	if (m_system_state == SYSTEM_STATE_RUN || m_system_state == SYSTEM_STATE_PAUSE) {

		//if (m_inspectDlg.Freeze() == 0) 
		if (1) {

			//等待列表中的图像都处理完成, 之后结束处理线程
			StopRefrush_Event.SetEvent();

			//结束计算线程
			m_ImgProc.StopProcess();

			m_tableDlg.TableSaved_Event.ResetEvent();

			m_inspectDlg.m_is_system_pause = FALSE;
		}
		else {
			return;
		}
	}

	CString cstr = L"停止检测";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	//m_system_state = SYSTEM_STATE_STOP;
}

//暂停  按钮
void CDeVisionDlg::OnBnClickedMfcbuttonPause()
{
	// TODO: 在此添加控件通知处理程序代码
	
	//暂停检测，相机获取的图像不放入图像列表
	m_inspectDlg.m_is_system_pause = TRUE;

	CString cstr = L"暂停检测";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	if(m_system_state != SYSTEM_STATE_PAUSE)
		m_system_state = SYSTEM_STATE_PAUSE;
}

//在线  按钮
void CDeVisionDlg::OnBnClickedMfcbuttonOnline()
{
	// TODO: 在此添加控件通知处理程序代码
	CWaitCursor wait;

	if (m_system_state == SYSTEM_STATE_OFFLINE) {

		//采集系统初始化
		//if (!m_inspectDlg.CameraSystemInitial()) {
		//	return;
		//}

		//初始化算法类
		m_ImgProc.InitialImageProcess();

		CString cstr = L"当前模式：在线，采集系统初始化成功";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

		m_system_state = SYSTEM_STATE_ONLINE;
		m_button_online.SetIcon(m_hOnlineIcon);
	}
	else {
		m_vDFT.clear();
		pView->m_vDefect.clear();

		CString cstr;
		cstr.Format(_T("当前模式：离线，当前线程：%d"), CheckThreadStatue());
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
		
		m_system_state = SYSTEM_STATE_OFFLINE;
		m_button_online.SetIcon(m_hOfflineIcon);
	}
	return;
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
	setup.DoModal();

	m_display_range = setup.m_wnd1_range;
	CString ctext;
	ctext.Format(_T("%.2f"), m_display_range);
	m_edisplay_range.SetWindowTextW(ctext);

}

//系统：保存
void CDeVisionDlg::OnSave()
{
	// TODO: 在此添加命令处理程序代码
	SaveUserInfo();

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
	CProductInfo m_productinfo;
	m_productinfo.DoModal();

}

//瑕疵检测：瑕疵趋势信息
void CDeVisionDlg::OnDeffectTrader()
{
	// TODO: 在此添加命令处理程序代码
}

//瑕疵检测：瑕疵检测算法
void CDeVisionDlg::OnDefectAnalysis()
{
	// TODO: 在此添加命令处理程序代码
	CAlgorithmDlg algorithmDlg;
	algorithmDlg.DoModal();

	//标准差倍数
	if (algorithmDlg.m_global_threshold != 0)
		m_ImgProc.m_k_normal_distribution = algorithmDlg.m_global_threshold;
	//滤波器大小
	if (algorithmDlg.m_select_threshold != 0)
		m_ImgProc.m_median_filter_size = algorithmDlg.m_select_threshold;
	//是否使用默认参考图像
	m_ImgProc.m_bLoad_Default_Ref_Dev = algorithmDlg.m_load_default;


}

//历史记录：报表
void CDeVisionDlg::OnTable()
{
	// TODO: 在此添加命令处理程序代码

	CStringW wpath = CA2W(m_work_path.c_str());

	// get root path
	ShellExecute(NULL, L"explore", L"D://report", NULL, NULL, SW_SHOW);

}

//历史记录：图像
void CDeVisionDlg::OnImage()
{
	// TODO: 在此添加命令处理程序代码
	CStringW wpath = CA2W(m_work_path.c_str());

	ShellExecute(NULL, L"explore", wpath, NULL, NULL, SW_SHOW);

}

//帮助：帮助
void CDeVisionDlg::OnHelp()
{
	// TODO: 在此添加命令处理程序代码
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

