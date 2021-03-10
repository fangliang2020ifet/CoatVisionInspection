#pragma once

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <numeric>
#include "SapClassBasic.h"
#include "SapClassGui.h"
#include "ImportHalconCpp.h"
#include "CImageProcessing.h"

#include "stdio.h"
#include "conio.h"

#include <math.h>



class CAcquireImage :	public SapManager
{

public:
	CAcquireImage();
	CAcquireImage(const CAcquireImage &acq);
	~CAcquireImage();

public:
	HWND hMainWnd;                                    //主窗口句柄
	CImageProcessing *m_pProcessing[4];
	UINT64 m_arrayFrameCount[4] = { 0,0,0,0 };

	BOOL m_bSystemPause;
	int m_nCameraNum;
	BOOL FREE_RUN = TRUE;                            //相机内部触发模式
	BOOL SLOW_DOWN = FALSE;
	BOOL SHOW_BUFFER = TRUE;                          //图像显示
	int SCANE_RATE = 10000;
	float m_k_speed = 0.0f;                           //编码器速度较正系数
	BOOL m_camera_system_initialled;
	CImageExWnd  *m_pImageWnd[4];                     //图像显示窗口
	CImageExWndEventHandler *m_pWndHandle;
	BOOL InitialThisClass();
	BOOL CameraSystemInitial();
	BOOL Grab();
	BOOL Snap();
	BOOL Freeze();
	void DropAcquireSpeed(int k);                     //降低采集速度
	UINT64 GetTotalFrameCount();
	int GetTotalTrashCount();
	float CalculateEncoderSpeed();
	void ResetAcquire();

private:
	//SapAcquisition	*m_Acq1;
	//SapAcquisition  *m_Acq2;
	//SapAcquisition  *m_Acq3;
	//SapAcquisition  *m_Acq4;
	//SapAcqDevice	*m_AcqDevice1;
	//SapAcqDevice	*m_AcqDevice2;
	//SapAcqDevice	*m_AcqDevice3;
	//SapAcqDevice	*m_AcqDevice4;
	//SapFeature      *m_Feature1;
	//SapFeature      *m_Feature2;
	//SapFeature      *m_Feature3;
	//SapFeature      *m_Feature4;
	//SapBuffer		*m_Buffers1;
	//SapBuffer		*m_Buffers2;
	//SapBuffer		*m_Buffers3;
	//SapBuffer		*m_Buffers4;
	//SapTransfer		*m_Xfer1;
	//SapTransfer		*m_Xfer2;
	//SapTransfer		*m_Xfer3;
	//SapTransfer		*m_Xfer4;
	//SapView         *m_View1;
	//SapView         *m_View2;
	//SapView         *m_View3;
	//SapView         *m_View4;
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






	std::vector<CString> m_vAcquireServerName;
	std::vector<CString> m_vAcquireDeviceServerName;
	SapAcquisition *m_Acq[4];
	SapAcqDevice   *m_AcqDevice[4];
	SapBuffer      *m_Buffer[4];
	SapFeature     *m_Feature[4];
	SapTransfer    *m_Xfer[4];
	SapView        *m_View[4];
	SapXferCallback m_Callback[4] = { AcqCallback1, AcqCallback2, AcqCallback3, AcqCallback4 };

	std::list<HObject> m_listImage1;
	std::list<HObject> m_listImage2;
	std::list<HObject> m_listImage3;
	std::list<HObject> m_listImage4;
	int m_arrayBufferIndex[4] = { 0,0,0,0 };         // 切换双缓存
	UINT m_arrayTrashCount[4] = { 0,0,0,0 };
	static void AcqCallback1(SapXferCallbackInfo *pInfo);
	static void AcqCallback2(SapXferCallbackInfo *pInfo);
	static void AcqCallback3(SapXferCallbackInfo *pInfo);
	static void AcqCallback4(SapXferCallbackInfo *pInfo);

	int ScanAcqDevice();
	int ScanAcqDevice2();	
	int InitServerOld();
	int InitialAllBoards();
	BOOL CreateObjects();
	BOOL DestroyObjects();
	std::vector<CString> m_vServerName;
	enum ServerCategory
	{
		ServerAll,
		ServerAcq,
		ServerAcqDevice
	};
	ServerCategory m_serverCategory;          // Category of servers (All, Acq or acqDevice)
	bool m_bShowCameraLinkServers = true;            // Show the camera links servers whatsoever


	int InitialAcqDevices();
	int InitialAcqDevices2();
	BOOL WriteCoefficientsToBuffer(SapBuffer& buffer);
	BOOL SetHardwareFilter();
	HObject CopyHobject(HObject ho_image);
	void GenerateHImage(SapBuffer* m_Buffers, int index, HImage &m_HImage);
	void SaveImageFromBuffer(SapBuffer* m_Buffer, int index);
};

