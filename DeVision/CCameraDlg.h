#pragma once


// CCameraDlg 对话框
#include "SerialPort.h"
#include "iUart.h"
#include "Text.h"
#include "SapClassBasic.h"
#include "SapClassGui.h"
#include "CInspectDlg.h"

class CCameraDlg : public CDialogEx, public CImageExWndEventHandler
{
	DECLARE_DYNAMIC(CCameraDlg)

public:
	CCameraDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCameraDlg();

public:
	HWND hMainWnd;                           //主窗口句柄

	//串口类
	bool isOpened = false;
	iUart mUart;
	void WriteRegData(void);
	void GetRegData(CString &comName, CString &dcbConfig);
	void LoadRegConfig();
	bool OpenUart();
	bool IsUartPortAvailable(void);
	DCB GetUartConfigDCB(void);
	HANDLE hRxThread;	// receive thread HANDLE
	TextBank DataRx, DataTx;
	void OnChangeEditTxData(CString strtmp);
	int AsyncSend(const CString& dataStr) {
		return mUart.UnblockSend(dataStr);
	}
	int AsyncRead(CString& dataStr) {
		return mUart.UnblockRead(dataStr);
	}
	void InitialUartPort();

	//相机
public:

	double exposure_time;
	int select_camera_number = 0;
private:
	//CImageExWnd		m_ImageWnd1;
	//CImageExWnd		m_ImageWnd2;
	//CImageExWnd		m_ImageWnd3;
	//CImageExWnd		m_ImageWnd4;

	CInspectDlg    *pInspectDlg;
	
	BOOL m_Image_Wnd_Initialized;

	UINT_PTR mTimer;
	BOOL InitializeCWndAndView();
	void UpdateCameraParameter();
	void GetFrameRate(SapTransfer *m_Xfer);
	void GetBufferSize(SapBuffer * m_buffer);
	void GetCameraParameter(SapAcqDevice* m_AcqDevice);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CAMERA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonOpenuart();
	afx_msg void OnBnClickedButtonCloseuart();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_camera1_selected;
	CButton m_camera2_selected;
	CButton m_camera3_selected;
	CButton m_camera4_selected;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_camera_name;
	CStatic m_camera_temperature;
	CEdit m_exposure_time;
	afx_msg void OnBnClickedButtonSetExposure();
	CStatic m_current_frame;
	CStatic m_total_frame;
	CStatic m_exposuretime_display;
	CStatic m_buffer_size;
};
