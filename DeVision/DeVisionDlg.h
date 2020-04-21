
// DeVisionDlg.h: 头文件
//

#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "CImageProcess.h"
#include "CInspectDlg.h"
#include "CAnalysisDlg.h"
#include "CTableDlg.h"
#include "CHistoryDlg.h"
#include "CSetupDlg.h"
#include "CCameraDlg.h"
#include "CAlgorithmDlg.h"
#include "CMyView.h"
#include "CMyDocument.h"
#include "Log.h"


#define IMAGE_WIDTH 8192
#define IMAGE_HEIGHT 8192
// 纵向的精度, 单位： mm
#define VERTICAL_PRECISION 0.035f
// 横向的精度，单位： mm
#define HORIZON_PRECISION 0.05f


// CDeVisionDlg 对话框
class CDeVisionDlg : public CDialogEx
{
// 构造
public:
	CDeVisionDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CDeVisionDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVISION_DIALOG };
#endif

protected:
	HWND hMainWnd;                           //主窗口句柄

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
		// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	BOOL LoadInitialInfo();
	void ExitProgram();
	int CheckThreadStatue();

	int m_CurSelTab;                              //标记当前选择的页面
	void InitialTabDlg();                         //初始化 tab control
	void TabDlgResize();
	bool isTabInitialized = false;
	void InitialStateBar();                       //初始化状态栏
	void InitialBtnIcon();
	void LoadCustomerDate();
	BOOL InitialTotalDefect();                    //全部瑕疵显示区
	void DelQueueFromSource();
	int DevideDFTRank(int num);                   //定义产品等级
	void CreateFlag(CDC &mDC, int x, int y, int kind);
	void DrawPartial(int test);
	void InitialScaleFlag();
	void UpdateScaleValue(float x, float y);       //更新刻度
	void SetYScalePos(float position);
	void SetXScalePos(float width);
	float GetDisplayRange();

	void CreateWorkPath(std::string &path);
	void UpdateSysMenuBtn();
	void UpdateSysStatus();
	float GetRunTime();
	void ReStartPrepare();

	long start_time;

	CMyView* pView;                                   //全局瑕疵滚动显示区域

	CFont flag_font;                                   //大字体
	CFont small_flag_font;                             //小字体
	CFont loggle_font;

	void TestLoadAndWrite();
	BOOL test_clicked = FALSE;
	int test_num = 0;

private:
	LPCWSTR CUSTOMER_FILEPATH = L"inis\\system.ini";
	
	CEvent StopRefrush_Event;
	CEvent RefrushThreadStopped_Event;

	int m_iAllThread_stopped;
	CRITICAL_SECTION m_csVecDFT;                  //定义一个临界区
	CWinThread *m_RefrushThread;
	static UINT RefrushWnd(LPVOID pParam);

	HICON m_hIcon;
	HICON m_hOnlineIcon;
	HICON m_hOfflineIcon;
	HICON m_hStartIcon;
	HICON m_hStopIcon;
	HICON m_hPauseIcon;
	HICON m_hUpIcon;
	HICON m_hDownIcon;
	HICON m_hLockIcon;
	HICON m_hUnlockIcon;
	HICON m_hExitIcon;

public:
	BOOL SaveUserInfo();
	BOOL GetUserInfo(std::string &num, std::string &width, std::string &id, std::string &user);
public:
	std::string m_work_path;                             //工作路径
	std::string m_fold_name;                             //文件夹名称
	//离线， 在线， 运行， 停止， 暂停
	enum { SYSTEM_STATE_OFFLINE = 0, SYSTEM_STATE_ONLINE, SYSTEM_STATE_RUN, SYSTEM_STATE_STOP, SYSTEM_STATE_PAUSE }; 
	int m_system_state;                                  //系统状态
	//普通， 一级， 二级， 三级， 严重
	enum{RANK_COMMON = 0, RANK_GRADE1, RANK_GRADE2, RANK_GRADE3, RANK_SERIOUS};
	int m_rank[5] = {0};
	enum{ SCREEN_UNLOCK = 0, SCREEN_LOCK };		
	int m_screen_state;                                  //屏幕状态

	CImageProcess  m_ImgProc;

	CTabCtrl        m_tab;
	CInspectDlg     m_inspectDlg;
	CAnalysisDlg    m_analysisDlg;
	CTableDlg       m_tableDlg;

	CHistoryDlg     m_historyDlg;
	CDialog         *pDialog[4];  //用来保存对话框对象指针
	CStatusBar     m_StatusBar;     //状态栏
	CCameraDlg      *m_pCamera;

	int online_camera_num = 1;                        //在线相机数量
	float current_speed = 5;                          //当前车速
	float current_position = 0;                       //当前检测位置
	float m_previous_position = 0.0;
	float m_display_range = 100.0f;                   //全局瑕疵显示窗口显示范围：米
	std::vector<DefectType> m_vDFT;	
	int total_number_def = 0;                             //当前检测到的瑕疵总数
	int serious_def_num = 0;                              //严重瑕疵个数
	float total_def_length = 0.0f;							  //瑕疵总米数

protected:
	CEdit m_edisplay_range;
	CEdit m_eselectwidth;
	CEdit m_eselectlongth;
	CEdit m_etotal;
	CEdit m_ewidth1;
	CEdit m_ewidth2;
	CEdit m_elongth1;
	CEdit m_elongth2;

	CMFCButton m_button_start;
	CMFCButton m_button_stop;
	CMFCButton m_button_pause;
	CMFCButton m_button_online;
	CButton m_button_lock;
	CButton m_button_exit;
	CStatic m_partical_picture;
	CStatic m_PictureControlTotal;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnTcnSelchangeTabDialog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedButtonGodown();
	afx_msg void OnBnClickedButtonGoup();
	afx_msg void OnBnClickedMfcbuttonStart();
	afx_msg void OnBnClickedMfcbuttonStop();
	afx_msg void OnBnClickedMfcbuttonPause();
	afx_msg void OnBnClickedMfcbuttonOnline();
	afx_msg void OnSetup();
	afx_msg void OnCameraSetup();
	afx_msg void OnLedSetup();
	afx_msg void OnTrigger();
	afx_msg void OnError();
	afx_msg void OnSave();
	afx_msg void OnExit();
	afx_msg void OnProduct();
	afx_msg void OnDeffectTrader();
	afx_msg void OnDefectAnalysis();
	afx_msg void OnTable();
	afx_msg void OnImage();
	afx_msg void OnHelp();
	afx_msg void OnRecord();
	afx_msg void OnAbout();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonLock();
	afx_msg void OnBnClickedButtonExit();


public:
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnIdok();
	afx_msg void OnIdcancel();
protected:
	afx_msg LRESULT OnLoggingMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWarningMsg(WPARAM wParam, LPARAM lParam);
};
