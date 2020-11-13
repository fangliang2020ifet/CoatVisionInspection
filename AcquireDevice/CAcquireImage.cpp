//图像获取类

#include "pch.h"
#include "AcquireDevice.h"
#include "CAcquireImage.h"


CAcquireImage::CAcquireImage()
{

}


CAcquireImage::CAcquireImage(const CAcquireImage &acq)
{
}


CAcquireImage::~CAcquireImage()
{
	for (int index = 0; index < (int)m_vServerName.size(); index++) {
		if (m_Xfer[index] && *m_Xfer[index]) m_Xfer[index]->Destroy();
		if (m_View[index] && *m_View[index]) m_View[index]->Destroy();
		if (m_Buffer[index] && *m_Buffer[index]) m_Buffer[index]->Destroy();
		if (m_Feature[index] && *m_Feature[index]) m_Feature[index]->Destroy();
		if (m_AcqDevice[index] && *m_AcqDevice[index]) m_AcqDevice[index]->Destroy();
		if (m_Acq[index] && *m_Acq[index]) m_Acq[index]->Destroy();
	}


}


BOOL CAcquireImage::InitialThisClass()
{
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	return TRUE;
}


//自动搜索采集设备
int CAcquireImage::ScanAcqDevice()
{
	ServerCategory m_serverCategory = ServerAll;          // Category of servers (All, Acq or acqDevice)
	m_vServerName.clear();
	m_serverCount = SapManager::GetServerCount();
	for (int i = 0; i < m_serverCount; i++){
		// Does this server support "Acq" (frame-grabber) or "AcqDevice" (camera)?
		bool bAcq = (m_serverCategory == ServerAcq || m_serverCategory == ServerAll) 
			&& (SapManager::GetResourceCount(i, ResourceAcq) > 0);
		// when m_serverCategory is ServerAcqDevice, show only servers that
		// have only one resource that is an acqdevice and no acq
		bool bAcqDevice = (m_serverCategory == ServerAcqDevice || m_serverCategory == ServerAll)
			&& (SapManager::GetResourceCount(i, ResourceAcqDevice) == 1) && (SapManager::GetResourceCount(i, ResourceAcq) == 0);

		if (bAcq || bAcqDevice){
			char name[128];
			if (GetServerName(i, name, sizeof(name))){
				if (strstr(name, "CameraLink_") != NULL){
					if (bAcq) m_vServerName.push_back(CString(name));
					//m_cbServer.AddString(CString(name));
				}
			}
		}
	}

	return (int)m_vServerName.size();
}

//图像采集系统初始化
BOOL CAcquireImage::CameraSystemInitial()
{
	CString cstrlog;
	if (!m_camera_system_initialled) {
		int num = ScanAcqDevice();
		if (num == 0) {
			cstrlog = L"未发现采集设备，请检查线路和电源连接";
			//::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
			return FALSE;
		}

		if (!InitialAcqDevices()) {
			cstrlog = L"资源初始化失败";
			//::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
			return FALSE;
		}
	}
	m_camera_system_initialled = TRUE;
	cstrlog = L"资源初始化成功";
	//::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
	return TRUE;
}

//初始化采集卡
BOOL CAcquireImage::InitialAcqDevices()
{
	CString cstrServerName, cstrDeviceName;
	for(int index = 0; index < m_serverCount; index ++){
		std::unique_ptr<char[]> up_configure_file(new char[MAX_PATH]());
		if (!FREE_RUN) {
			CString name;
			name.Format(L"system\\T_LA_CM_08K08A_00_R_External_Trigger_Board%d.ccf", index + 1);
			memcpy(&up_configure_file[0], (CW2A)name.GetBuffer(), name.GetLength());
		} else {
			CString name;
			name.Format(L"system\\T_LA_CM_08K08A_00_R_FreeRun_%d.ccf", index + 1);
			memcpy(&up_configure_file[0], (CW2A)name.GetBuffer(), name.GetLength());
		}

		//此处可以尝试通过调用Sap的api获取 ServerName 和 DeviceName
		cstrServerName.Format(L"Xtium-CL_MX4_%d", index);
		SapLocation locServer((CW2A)cstrServerName.GetBuffer(), index);
		m_Acq[index].reset(new SapAcquisition(locServer, &up_configure_file[0]));
		m_Buffer[index].reset(new SapBufferWithTrash(2, m_Acq[index].get()));
		m_Xfer[index].reset(new SapAcqToBuf(m_Acq[index].get(), m_Buffer[index].get(), AcqCallback, this));
		m_View[index].reset(new SapView(m_Buffer[index].get(), m_hwnd[index]));

		cstrDeviceName.Format(L"CameraLink_%d", index + 1);
		SapLocation locDevice((CW2A)cstrDeviceName.GetBuffer(), index);
		m_AcqDevice[index].reset(new SapAcqDevice(locDevice, FALSE));
		m_Feature[index].reset(new SapFeature(locDevice));

		if (m_Acq[index] && !*m_Acq[index]) m_Acq[index]->Create();
		if (m_AcqDevice[index] && !*m_AcqDevice[index]) m_AcqDevice[index]->Create();
		if (m_Feature[index] && !*m_Feature[index]) m_Feature[index]->Create();
		if (m_Buffer[index] && !*m_Buffer[index]) m_Buffer[index]->Create();
		if (m_View[index] && !*m_View[index]) m_View[index]->Create();
		if (m_Xfer[index] && !*m_Xfer[index]) m_Xfer[index]->Create();

		m_View[index]->SetScalingMode(SapView::ScalingNone, TRUE);

		if (!FREE_RUN) 	m_AcqDevice[index]->SetFeatureValue("TriggerMode", "On");//触发模式打开		
		else {
			m_AcqDevice[index]->SetFeatureValue("TriggerMode", "Off");//触发模式关闭
			m_AcqDevice[index]->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定触发频率
		}
	
	}

	return TRUE;
}

// 板卡：硬件滤波
typedef struct
{
	SapAcquisition::ImageFilterKernelSize param;
	int dim;
} FILTER_SIZE_PAIR;

static const FILTER_SIZE_PAIR FILTER_SIZES_PRM[] = { { SapAcquisition::ImageFilterSize1x1, 1 },
{ SapAcquisition::ImageFilterSize2x2, 2 },{ SapAcquisition::ImageFilterSize3x3, 3 },
{ SapAcquisition::ImageFilterSize4x4, 4 }, { SapAcquisition::ImageFilterSize5x5, 5 },
{ SapAcquisition::ImageFilterSize6x6, 6 }, { SapAcquisition::ImageFilterSize7x7, 7 } };

static int ConvertKernelSizeToInt(SapAcquisition::ImageFilterKernelSize ksize)
{
	for (int i = 0; i < sizeof(FILTER_SIZES_PRM) / sizeof(FILTER_SIZES_PRM[0]); i++)
	{
		if (FILTER_SIZES_PRM[i].param == ksize)
			return FILTER_SIZES_PRM[i].dim;
	}

	return 0;
}

BOOL CAcquireImage::WriteCoefficientsToBuffer(SapBuffer& buffer)
{
	int nWidth = buffer.GetWidth();
	int nHeight = buffer.GetHeight();

	if (nWidth != nHeight)
	{
		AfxMessageBox(_T("The output buffer is not a square matrix. Image Filter Editor"), MB_ICONERROR);
		return false;
	}

	bool bStatus = true;
	int nValue = 1;
	for (int iEditLine = 0; (iEditLine < nHeight) && bStatus; iEditLine++)
	{
		for (int iEditColumn = 0; iEditColumn < nWidth; iEditColumn++)
		{
			//需详细阅读帮助文档和实例程序
			// write value to the kernel
			SapDataMono data;
			data.Set(nValue);
			if (!buffer.WriteElement(iEditColumn, iEditLine, data))
			{
				AfxMessageBox(_T("Failed to write a coefficient to the buffer.Image Filter Editor"), MB_ICONERROR);
				bStatus = false;
				break;
			}
		}
	}

	return bStatus;
}

BOOL CAcquireImage::SetHardwareFilter()
{
	bool bStatus = true; // to prevent leaking resources

	//滤波器大小  3*3
	int nSize = ConvertKernelSizeToInt((SapAcquisition::ImageFilterKernelSize)3);

	SapBuffer buffer;
	buffer.SetFormat(SapFormatInt32);
	buffer.SetHeight(nSize);
	buffer.SetWidth(nSize);

	if (!buffer.Create())
	{
		AfxMessageBox(_T("Failed to create the buffer for the kernel coefficients.Image Filter Editor"), MB_ICONERROR);
		return false;
	}

	if (!WriteCoefficientsToBuffer(buffer))
	{
		AfxMessageBox(_T("Failed to write the coefficients to the buffer.Image Filter Editor"), MB_ICONERROR);
		bStatus = false;
	}

	//板卡 一
	if (bStatus && m_Acq[0]->IsImageFilterAvailable()) {
		if (!m_Acq[0]->IsImageFilterEnabled()) {
			m_Acq[0]->EnableImageFilter(1);
		}
		if (!m_Acq[0]->SetImageFilter(0, &buffer))
			bStatus = false;
	}

	//板卡 二
	if (bStatus && m_Acq[1]->IsImageFilterAvailable()) {
		if (!m_Acq[1]->IsImageFilterEnabled()) {
			m_Acq[1]->EnableImageFilter(1);
		}
		if (!m_Acq[1]->SetImageFilter(0, &buffer))
			bStatus = false;
	}

	//板卡 三
	if (bStatus && m_Acq[2]->IsImageFilterAvailable()) {
		if (!m_Acq[2]->IsImageFilterEnabled()) {
			m_Acq[2]->EnableImageFilter(1);
		}
		if (!m_Acq[2]->SetImageFilter(0, &buffer))
			bStatus = false;
	}

	//板卡 四
	if (bStatus && m_Acq[3]->IsImageFilterAvailable()) {
		if (!m_Acq[3]->IsImageFilterEnabled()) {
			m_Acq[3]->EnableImageFilter(1);
		}
		if (!m_Acq[3]->SetImageFilter(0, &buffer))
			bStatus = false;
	}

	buffer.Destroy();
	return bStatus;
}


//开始连续采集
void CAcquireImage::Grab()
{
	for (int index = 0; index < (int)m_vServerName.size(); index++) {
		if (!m_Xfer[index]->Grab()) {
			CString cstrlog;
			cstrlog.Format(L"设备 %d 采集失败：Grab Error %d", index + 1, GetLastError());
			//::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		}
	}
}

//采集一帧
void CAcquireImage::Snap()
{
	for (int index = 0; index < (int)m_vServerName.size(); index++) {
		if (!m_Xfer[index]->Snap()) {
			CString cstrlog;
			cstrlog.Format(L"设备 %d 采集失败：Snap Error %d", index + 1, GetLastError());
			//::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		}
	}
}

//停止采集
void CAcquireImage::Freeze()
{
	for (int index = 0; index < (int)m_vServerName.size(); index++) {
		if (m_Xfer[index]->Freeze()) {
			//if (CAbortDlg(AfxGetApp()->m_pMainWnd, m_Xfer[index].get()).DoModal() != IDOK)
			//	m_Xfer[index]->Abort();
		}
	}
}


//降低采集卡触发速率
void CAcquireImage::DropAcquireSpeed(int k)
{
	for (int index = 0; index < (int)m_vServerName.size(); index++) {
		if (m_Acq[index]->SetParameter(CORACQ_PRM_SHAFT_ENCODER_DROP, k, TRUE)) {
			CString cstrlog;
			cstrlog.Format(L"设备 %d 已降低采集频率: %d", index + 1, k);
			//::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
		}
	}
}

//计算编码器速度
float CAcquireImage::CalculateEncoderSpeed()
{
	//设置时间戳    
	//SapAcquisition::TimeStampShaftEncoder.
	//The time base is in external line trigger or shaft encoder pulse(before drop / multiply operation).
	//m_Acq1->SetTimeStampBase(CORACQ_VAL_TIME_BASE_SHAFT_ENCODER);

	float speed = 0.0f;
	SapXferFrameRateInfo* pStats = NULL;
	if(!m_vServerName.empty())	 pStats = m_Xfer[0]->GetFrameRateStatistics();
	if (pStats->IsLiveFrameRateAvailable() && !pStats->IsLiveFrameRateStalled()) {
		//      8192 * 60 / 1000      0.05
		//speed = pStats->GetLiveFrameRate() * m_k_speed * VERTICAL_PRECISION;
		
	}
	return speed;
}

//计算总帧数
UINT64 CAcquireImage::GetTotalFrameCount()
{
	UINT64 total = m_arrayFrameCount[0] + m_arrayFrameCount[1] +
		m_arrayFrameCount[2] + m_arrayFrameCount[3];

	return total;
}

int CAcquireImage::GetTotalTrashCount()
{
	int total = m_arrayTrashCount[0] + m_arrayTrashCount[1] +
		m_arrayTrashCount[2] + m_arrayTrashCount[3];

	return total;
}

void CAcquireImage::GenerateHImage(SapBuffer* m_Buffer, int index, HImage &m_HImage)
{
	BYTE pData;
	void* pDataAddr = &pData;
	m_Buffer->GetAddress(index, &pDataAddr);
	int width = m_Buffer->GetWidth();
	int height = m_Buffer->GetHeight();
	m_HImage.GenImage1("byte", width, height, pDataAddr);
	m_Buffer->ReleaseAddress(pDataAddr);
}

void CAcquireImage::SaveImageFromBuffer(SapBuffer* m_Buffer, int index)
{
	BYTE pData;
	void* pDataAddr = &pData;
	m_Buffer->GetAddress(index, &pDataAddr);
	int width = m_Buffer->GetWidth();
	int height = m_Buffer->GetHeight();
	HImage m_HImage;
	m_HImage.GenImage1("byte", width, height, pDataAddr);

	long t = GetTickCount();
	CString cfilename;
	cfilename.Format(_T("%d"), t);   //格式化日期时间
	const wchar_t* filename = (LPCTSTR)cfilename;
	m_HImage.WriteImage("bmp", 0, filename);

	m_Buffer->ReleaseAddress(pDataAddr);
}

HObject CAcquireImage::CopyHobject(HObject ho_image)
{
	HObject copy;
	CopyImage(ho_image, &copy);

	return copy;
}


//采集卡回调函数
void CAcquireImage::AcqCallback(SapXferCallbackInfo *pInfo)
{
	CAcquireImage *pThis = (CAcquireImage*)pInfo->GetContext();
	SapTransfer* xfer = pInfo->GetTransfer();
	int INDEX = SapManager::GetServerIndex(xfer->GetLocation());
	int buffer_index = pThis->m_arrayBufferIndex[INDEX];
	if (pInfo->IsTrash())
		pThis->m_arrayTrashCount[INDEX] += 1;
	else {
		if (pThis->SHOW_BUFFER) pThis->m_View[INDEX]->Show();
		HImage ho_image;
		pThis->GenerateHImage(pThis->m_Buffer[INDEX].get(), buffer_index, ho_image);
		switch (INDEX)
		{
		case 0:
			pThis->m_listImage1.push_back(ho_image);
			break;
		case 1:
			pThis->m_listImage2.push_back(ho_image);
			break;
		case 2:
			pThis->m_listImage3.push_back(ho_image);
			break;
		case 3:
			pThis->m_listImage4.push_back(ho_image);
			break;
		default:
			break;
		}
		pThis->m_arrayFrameCount[INDEX] += 1;
		if (buffer_index == 0)      pThis->m_arrayBufferIndex[INDEX] = 1;
		else if (buffer_index == 1) pThis->m_arrayBufferIndex[INDEX] = 0;
	}
}