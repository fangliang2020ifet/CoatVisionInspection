#pragma once

#include <string>
#include <vector>
#include "CProductInfo.h"
#include "CImageProcess.h"
#include "SapClassBasic.h"
#include "SapClassGui.h"


// CInspectDlg 对话框

class CInspectDlg : public CDialogEx, public CImageExWndEventHandler, public SapManager
{
	DECLARE_DYNAMIC(CInspectDlg)

public:
	CInspectDlg(CWnd* pParent = nullptr);   // 标准构造函数

	virtual ~CInspectDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INSPECT };
#endif

// Enumerations
public:
	HWND hMainWnd;                           //主窗口句柄

	CImageProcess *m_pImgProc;

	enum ServerCategory
	{
		ServerAll,
		ServerAcq,
		ServerAcqDevice
	};

public:
	BOOL FREE_RUN = FALSE;           //相机内部触发模式
	int SCANE_RATE = 20000; 


	LPCWSTR FILEPATH = L"inis\\ProductInformation.ini";
	CFont m_font;
	BOOL m_bUpdateUserInfo = FALSE;
	BOOL m_is_system_pause;
	BOOL m_camera_system_initialled;
	BOOL m_camera1_thread_alive = FALSE;
	BOOL m_camera2_thread_alive = FALSE;
	BOOL m_camera3_thread_alive = FALSE;
	BOOL m_camera4_thread_alive = FALSE;


	void RecordWarning(const std::wstring& str);   //记录报警信息
	void RecordWarning(int test, CString cstr);
	void RecordLogList(const std::wstring& str);   //记录日志
	void RecordLogList(int test, CString cstr);

	void UpdateDFTinformation(size_t total_num, int great_dft_num, float longth);
	void RestartInspect();

	BOOL CameraSystemInitial();
	BOOL InitialAllBoards();
	BOOL InitialBoard1();
	BOOL InitialBoard2();
	BOOL InitialBoard3();
	BOOL InitialBoard4();
	BOOL SetCameraParemeter1();
	BOOL SetCameraParemeter2();
	BOOL SetCameraParemeter3();
	BOOL SetCameraParemeter4();
	float CalculateEncoderSpeed();
	BOOL CreateObjects();
	BOOL DestroyObjects();
	int Grab();
	void Snap();
	int Freeze();

	UINT64 GetTotalFrameCount();
	int GetTotalTrashCount();

	UINT64 total_frame_count;
	int total_trash_count;
	UINT64 camera1_frame_count, camera2_frame_count, camera3_frame_count, camera4_frame_count;
	int camera1_trash_count, camera2_trash_count, camera3_trash_count, camera4_trash_count;
	//CImageExWnd		m_ImageWnd;
	SapAcquisition	*m_Acq1;
	SapAcquisition  *m_Acq2;
	SapAcquisition  *m_Acq3;
	SapAcquisition  *m_Acq4;
	SapAcqDevice	*m_AcqDevice1;
	SapAcqDevice	*m_AcqDevice2;
	SapAcqDevice	*m_AcqDevice3;
	SapAcqDevice	*m_AcqDevice4;
	SapFeature      *m_Feature1;
	SapFeature      *m_Feature2;
	SapFeature      *m_Feature3;
	SapFeature      *m_Feature4;
	SapBuffer		*m_Buffers1;
	SapBuffer		*m_Buffers2;
	SapBuffer		*m_Buffers3;
	SapBuffer		*m_Buffers4;
	SapTransfer		*m_Xfer1;
	SapTransfer		*m_Xfer2;
	SapTransfer		*m_Xfer3;
	SapTransfer		*m_Xfer4;
	SapView         *m_View1;
	SapView         *m_View2;
	SapView         *m_View3;
	SapView         *m_View4;

	BOOL camera1_show_buffer = FALSE;
	BOOL camera2_show_buffer = FALSE;
	BOOL camera3_show_buffer = FALSE;
	BOOL camera4_show_buffer = FALSE;

//自动搜索板卡和相机
protected:
	BOOL InitServer();
	std::vector<CString> m_vServerName;
	ServerCategory m_serverCategory;          // Category of servers (All, Acq or acqDevice)
	bool m_bShowCameraLinkServers;            // Show the camera links servers whatsoever

protected:
	CImageExWnd		m_ImageWnd1;
	CImageExWnd		m_ImageWnd2;
	CImageExWnd		m_ImageWnd3;
	CImageExWnd		m_ImageWnd4;
	BOOL InitializeCWndAndView();

	// Static Functions
	static void AcqCallback1(SapXferCallbackInfo *pInfo);
	static void AcqCallback2(SapXferCallbackInfo *pInfo);
	static void AcqCallback3(SapXferCallbackInfo *pInfo);
	static void AcqCallback4(SapXferCallbackInfo *pInfo);

	BOOL isNotSupported = FALSE, status = FALSE, acquisitionCreated = TRUE, acqDeviceCreated = TRUE;
	UINT32 m_acqDeviceNumber1 = 0;
	UINT32 m_acqDeviceNumber2 = 0;
	UINT32 m_acqDeviceNumber3 = 0;
	UINT32 m_acqDeviceNumber4 = 0;
	char acqServerName1[CORSERVER_MAX_STRLEN] = "Xtium-CL_MX4_1";
	char acqServerName2[CORSERVER_MAX_STRLEN] = "Xtium-CL_MX4_2";
	char acqServerName3[CORSERVER_MAX_STRLEN] = "Xtium-CL_MX4_3";
	char acqServerName4[CORSERVER_MAX_STRLEN] = "Xtium-CL_MX4_4";
	char configFilename1[MAX_PATH];
	char configFilename2[MAX_PATH];
	char configFilename3[MAX_PATH];
	char configFilename4[MAX_PATH];
	char acqDeviceName1[CORSERVER_MAX_STRLEN] = "CameraLink_1";  
	char acqDeviceName2[CORSERVER_MAX_STRLEN] = "CameraLink_2";
	char acqDeviceName3[CORSERVER_MAX_STRLEN] = "CameraLink_3";   //CameraLink的名称有颠倒，3，4
	char acqDeviceName4[CORSERVER_MAX_STRLEN] = "CameraLink_4";

private:
	HObject CopyHobject(HObject ho_image);

public:

	void GenerateHImage(SapBuffer* m_Buffers, int index, HImage &m_HImage);
	void SaveImageFromBuffer(SapBuffer* m_Buffer, int index);
	int m_static_count1 = 0;  // 切换双缓存
	int m_static_count2 = 0;
	int m_static_count3 = 0;
	int m_static_count4 = 0;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CButton m_btn_changeinfo;
	CStatic m_static_number;
	CStatic m_static_width;
	CStatic m_static_operator;
	CStatic m_static_id;
	CListBox m_listLog;
	CListBox m_listWarning;
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
