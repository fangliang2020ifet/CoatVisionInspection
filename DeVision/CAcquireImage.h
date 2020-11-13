#pragma once

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <numeric>
#include "SapClassBasic.h"
#include "SapClassGui.h"
#include "ImportHalconCpp.h"

class CAcquireImage :	public SapManager
{

public:
	CAcquireImage();
	CAcquireImage(const CAcquireImage &acq);
	~CAcquireImage();

public:
	HWND hMainWnd;                                    //主窗口句柄

	BOOL m_bSystemPause;
	BOOL FREE_RUN = FALSE;                            //相机内部触发模式
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
	UINT64 m_arrayFrameCount[4] = { 0,0,0,0 };
	static void AcqCallback1(SapXferCallbackInfo *pInfo);
	static void AcqCallback2(SapXferCallbackInfo *pInfo);
	static void AcqCallback3(SapXferCallbackInfo *pInfo);
	static void AcqCallback4(SapXferCallbackInfo *pInfo);

	int ScanAcqDevice();
	int InitialAcqDevices();
	BOOL WriteCoefficientsToBuffer(SapBuffer& buffer);
	BOOL SetHardwareFilter();
	HObject CopyHobject(HObject ho_image);
	void GenerateHImage(SapBuffer* m_Buffers, int index, HImage &m_HImage);
	void SaveImageFromBuffer(SapBuffer* m_Buffer, int index);
};

