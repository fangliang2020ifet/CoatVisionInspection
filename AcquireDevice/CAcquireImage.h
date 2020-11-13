#pragma once

#include <string>
#include <vector>
#include <list>
#include <memory>
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
	BOOL FREE_RUN = FALSE;                       //相机内部触发模式
	BOOL SLOW_DOWN = FALSE;
	BOOL SHOW_BUFFER = TRUE;
	int SCANE_RATE = 10000;
	float m_k_speed = 0.0f;                      //  编码器速度较正系数
	BOOL m_camera_system_initialled;
	//	CImageExWnd     m_ImageWnd[4];
	HWND m_hwnd[4] = { 0,0,0,0 };

	BOOL CameraSystemInitial();
	void Grab();
	void Snap();
	void Freeze();
	void DropAcquireSpeed(int k);                           //降低采集速度
	UINT64 GetTotalFrameCount();
	int GetTotalTrashCount();
	float CalculateEncoderSpeed();

protected:
	BOOL InitialThisClass();
	enum ServerCategory
	{
		ServerAll,
		ServerAcq,
		ServerAcqDevice
	};


private:
	HWND hMainWnd;                           //主窗口句柄

	int ScanAcqDevice();
	BOOL InitialAcqDevices();
	BOOL WriteCoefficientsToBuffer(SapBuffer& buffer);
	BOOL SetHardwareFilter();
	HObject CopyHobject(HObject ho_image);
	void GenerateHImage(SapBuffer* m_Buffers, int index, HImage &m_HImage);
	void SaveImageFromBuffer(SapBuffer* m_Buffer, int index);
	static void AcqCallback(SapXferCallbackInfo *pInfo);

	int m_serverCount = 0;
	std::vector<CString> m_vServerName;
	std::unique_ptr<SapAcquisition> m_Acq[4];
	std::unique_ptr<SapAcqDevice>   m_AcqDevice[4];
	std::unique_ptr<SapBuffer>      m_Buffer[4];
	std::unique_ptr<SapFeature>     m_Feature[4];
	std::unique_ptr<SapTransfer>    m_Xfer[4];
	std::unique_ptr<SapView>        m_View[4];

	CImageExWndEventHandler* m_pWinHandler[4];
	std::list<HObject> m_listImage1;
	std::list<HObject> m_listImage2;
	std::list<HObject> m_listImage3;
	std::list<HObject> m_listImage4;
	int m_arrayBufferIndex[4] = { 0,0,0,0 };         // 切换双缓存
	UINT m_arrayTrashCount[4] = { 0,0,0,0 };
	UINT64 m_arrayFrameCount[4] = { 0,0,0,0 };

};

