#pragma once

#include <string>
#include <vector>
#include <memory>
#include "SapClassBasic.h"
#include "SapClassGui.h"
#include "ImportHalconCpp.h"
#include "CImageProcessing.h"

class CAcquireImage :	public SapManager
{

public:
	CAcquireImage();
	CAcquireImage(const CAcquireImage &acq);
	~CAcquireImage();

public:
	HWND hMainWnd;                                    //�����ھ��
	CImageProcessing *m_pProcessing[4];
	UINT64 m_arrayFrameCount[4] = { 0,0,0,0 };

	BOOL m_bSystemPause;
	int m_nCameraNum = 0;
	BOOL SLOW_DOWN = FALSE;
	BOOL SHOW_BUFFER = TRUE;                          //ͼ����ʾ
	int SCANE_RATE = 10000;                           // 10 ms
	float m_k_speed = 0.0f;                           //�������ٶȽ���ϵ��
	BOOL m_bIsInitialized;
	CImageExWnd  *m_pImageWnd[4];                     //ͼ����ʾ����
	CImageExWndEventHandler *m_pWndHandle;
	BOOL InitialThisClass();
	BOOL CameraSystemInitial();
	bool CameraSystemInitial(int newest);
	BOOL Grab();
	BOOL Snap();
	BOOL Freeze();
	void DropAcquireSpeed(int k);                     //���Ͳɼ��ٶ�
	UINT64 GetTotalFrameCount();
	int GetTotalTrashCount();
	float CalculateEncoderSpeed();
	void ResetAcquire();

private:
	char *m_ccfFileName = nullptr;
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
	int m_arrayBufferIndex[4] = { 0,0,0,0 };         // �л�˫����
	UINT m_arrayTrashCount[4] = { 0,0,0,0 };
	static void AcqCallback1(SapXferCallbackInfo *pInfo);
	static void AcqCallback2(SapXferCallbackInfo *pInfo);
	static void AcqCallback3(SapXferCallbackInfo *pInfo);
	static void AcqCallback4(SapXferCallbackInfo *pInfo);
	BOOL AutoScanServers(std::vector<std::string> &vServerName, std::vector<std::string> &vDeviceName);
	int InitialAcqServerAndDevice(std::vector<std::string> vServerName, std::vector<std::string> vDeviceName);
	enum ServerCategory
	{
		ServerAll,
		ServerAcq,
		ServerAcqDevice
	};
	BOOL WriteCoefficientsToBuffer(SapBuffer& buffer);
	BOOL SetHardwareFilter();
	HObject CopyHobject(HObject ho_image);
	void GenerateHImage(SapBuffer* m_Buffers, int index, HImage &m_HImage);
	void SaveImageFromBuffer(SapBuffer* m_Buffer, int index);
};

