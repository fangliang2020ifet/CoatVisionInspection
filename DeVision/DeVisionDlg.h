// DeVisionDlg.h: 头文件
//

#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <memory>
#include <cmath>

#include "CLogin.h"
#include "CAcquireImage.h"
#include "CImageProcessing.h"
#include "CInspectDlg.h"
#include "CAnalysisDlg.h"
#include "CRemote.h"
#include "CTableDlg.h"
#include "CHistoryDlg.h"
#include "CSetupDlg.h"
#include "CCameraDlg.h"
#include "CAlgorithmDlg.h"
#include "CMyView.h"
#include "CMyDocument.h"
#include "Log.h"


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
	DECLARE_MESSAGE_MAP()

public:
	bool m_bTestModel = false;
	bool m_bSaveRefImg;

	CAcquireImage   m_ImgAcq;               //图像获取
	int m_nConnectedCameras = 0;                //已连接相机数
	CImageProcessing   *m_pImgProc[4];

	CMyView* pView;                                   //全局瑕疵滚动显示区域
	CDialog         *pDialog[4];                       //用来保存对话框对象指针
	CTabCtrl        m_tab;
	CInspectDlg     m_inspectDlg;
	CAnalysisDlg    m_analysisDlg;
	CTableDlg       m_tableDlg;
	CHistoryDlg     m_historyDlg;
	CCameraDlg      *m_pCamera;

	void ExitProgram();
	int m_CurSelTab;                              //标记当前选择的页面
	void InitialTabDlg();                         //初始化 tab control
	void TabDlgResize();
	bool isTabInitialized = false;
	void InitialImageAcquire();
	void InitialStateBar();                       //初始化状态栏
	void InitialBtnIcon();
	BOOL InitialTotalDefect();                    //全部瑕疵显示区
	void DelQueueFromSource();
	int DevideDFTRank(int num);                   //定义产品等级
	void CreateFlag(CDC &mDC, int x, int y, int kind);
	void DrawPartial(int test);
	void InitialScaleFlag();
	void UpdateScaleValue(float x, float y);       //更新刻度
	void SetYScalePos(float position);
	void SetXScalePos(float width);

	void CreateWorkPath(std::string &path);
	void DeleteHistoryImage();
	void RemoveAll(std::wstring wst);
	void UpdateSysStatus();
	void UpdateSysColor();
	float GetRunTime();
	float UpdateCurrentInspectPosition();
	void ReStartPrepare();
	void AutoStop();

	long start_time;

	CFont small_flag_font;                             //小字体
	CFont loggle_font;
	BOOL m_bFlicker = FALSE;                           //控件闪烁
	CStatusBar     m_StatusBar;                        //状态栏

	void TestLoadAndWrite();
	BOOL test_clicked = FALSE;
	int test_num = 0;

private:
	CString cstrlog;
	HICON m_hIcon;
	HICON m_hOnlineIcon;
	HICON m_hOfflineIcon;
	HICON m_hCameraInIcon;
	HICON m_hCameraOutIcon;
	HICON m_hStartIcon;
	HICON m_hStopIcon;
	HICON m_hPauseIcon;
	HICON m_hUpIcon;
	HICON m_hDownIcon;
	HICON m_hLockIcon;
	HICON m_hUnlockIcon;
	HICON m_hExitIcon;

	int m_nThreadNumbers;
	int m_nNormalDistribution;
	int m_nFIlterSize;
	float m_fRadiusMin;
	float m_fRadiusMax;


	CEvent StopRefrush_Event;
	CEvent RefrushThreadStopped_Event;
	HANDLE *m_phFinishProcessEvent[4];              //处理完成事件

	CRITICAL_SECTION m_csListDftDisplay;                  //定义一个临界区
	CWinThread *m_RefrushThread;
	static UINT RefrushWnd(LPVOID pParam);

	CString m_logo_name;
	CString m_cProduct_Model;                     //产品型号
	CString m_cProduct_Width;                     //宽度
	CString m_cOperator;                          //操作员
	CString m_cProduct_Number;                    //生产批号

	void LoadRegConfig();
	void ReadFromRegedit();
	void WriteToRegedit();
	void SaveDeffectImage(int acquire_index, HObject ho_img, DeffectInfo information);
	void SaveImages(int index, unsigned &numbers);

public:
	std::string m_strDeffect_Path;                             //工作路径
	std::string m_strTable_Path;
	std::string m_strDeffectImgSavePath;
	std::vector<std::wstring> m_vec_refpath;

	std::list<DeffectInfo> m_listDftInfo[4];                   //瑕疵信息
	//std::list<HalconCpp::HObject> m_listDftImg[4];              //瑕疵图像队列
	//std::vector<DefectType> m_vDFT;
	std::vector<DeffectInfo> m_vecDftDisplay;            //x,y的单位已转为毫米/米

	//离线， 在线， 运行， 停止， 暂停
	enum { SYSTEM_STATE_OFFLINE = 0, SYSTEM_STATE_ONLINE, SYSTEM_STATE_RUN, SYSTEM_STATE_STOP, SYSTEM_STATE_PAUSE };
	int m_system_state;                                  //系统状态
	//普通， 一级， 二级， 三级， 严重
	enum { RANK_COMMON = 0, RANK_GRADE1, RANK_GRADE2, RANK_GRADE3, RANK_SERIOUS };
	int m_rank[5] = { 0 };
	enum { SCREEN_UNLOCK = 0, SCREEN_LOCK };
	int m_screen_state;                                  //屏幕状态
	float m_speed = 0.0f;                             //当前车速
	float m_fCurrentPosition = 0.0f;
	float m_previous_position = 0.0;
	float m_wnd1_range = 0.0f;                   //全局瑕疵显示窗口显示范围：米
	float m_wnd2_range = 0.0f;
	//int total_number_def = 0;                             //当前检测到的瑕疵总数
	int m_nTotalDeffects = 0;
	//int serious_def_num = 0;                              //严重瑕疵个数
	int m_nSeriousDeffects = 0;
	//float total_def_length = 0.0f;							  //瑕疵总米数
	float m_fTotalDeffectsLength = 0.0f;
	float m_width;

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
	CStatic m_sSystem_Statue;
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnLoggingMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWarningMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnTcnSelchangeTabDialog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnIdok();
	afx_msg void OnIdcancel();
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
	afx_msg void OnRemote();
	afx_msg LRESULT OnUpdateControls(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateHistory(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateMainwnd(WPARAM wParam, LPARAM lParam);
};
