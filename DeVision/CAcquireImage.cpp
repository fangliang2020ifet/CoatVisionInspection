//图像获取类

#include "stdafx.h"
#include "DeVision.h"
#include "CAcquireImage.h"



#define CAMERA_LINK_SERVER_NAME_PREFIX "CameraLink_"


CAcquireImage::CAcquireImage()
{
	m_bSystemPause = FALSE;
	m_nCameraNum = 0;
	m_bIsInitialized = FALSE;
	for (int i = 0; i < 4; i++) { 
		m_pProcessing[i] = NULL;
		m_pImageWnd[i] = NULL;
		m_Acq[i] = NULL;
		m_AcqDevice[i] = NULL;
		m_Buffer[i] = NULL;
		m_Feature[i] = NULL;
		m_Xfer[i] = NULL;
		m_View[i] = NULL;
	}
	m_pWndHandle = NULL;

#ifdef FREERUN
	char Filename[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun.ccf";
	m_ccfFileName = new char[MAX_PATH]();
	memcpy(m_ccfFileName, Filename, sizeof(Filename));
#else
	char Filename[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_External.ccf";
	m_ccfFileName = new char[MAX_PATH]();
	memcpy(m_ccfFileName, Filename, sizeof(Filename));
#endif

}


CAcquireImage::CAcquireImage(const CAcquireImage &acq)
{

}


CAcquireImage::~CAcquireImage()
{
	for (int index = 0; index < m_nCameraNum; index++) {
		if (m_Xfer[index] && *m_Xfer[index]) m_Xfer[index]->Destroy();
		if (m_View[index] && *m_View[index]) m_View[index]->Destroy();
		if (m_Buffer[index] && *m_Buffer[index]) m_Buffer[index]->Destroy();
		if (m_Feature[index] && *m_Feature[index]) m_Feature[index]->Destroy();
		if (m_AcqDevice[index] && *m_AcqDevice[index]) m_AcqDevice[index]->Destroy();
		if (m_Acq[index] && *m_Acq[index]) m_Acq[index]->Destroy();

		if (m_Xfer[index]) delete m_Xfer[index];
		if (m_View[index]) delete m_View[index];
		if (m_Buffer[index]) delete m_Buffer[index];
		if (m_Feature[index]) delete m_Feature[index];
		if (m_AcqDevice[index]) delete m_AcqDevice[index];
		if (m_Acq[index]) delete m_Acq[index];
	}

	delete m_ccfFileName;
}


BOOL CAcquireImage::InitialThisClass()
{
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	return TRUE;
}

//图像采集系统初始化
BOOL CAcquireImage::CameraSystemInitial()
{
	InitialThisClass();

	CString cstrlog;
	int acquire_num = 0;
	if (!m_bIsInitialized) {
		std::vector<std::string> vServerName;
		std::vector<std::string> vDeviceName;
		if (AutoScanServers(vServerName, vDeviceName)) {
			acquire_num = InitialAcqServerAndDevice(vServerName, vDeviceName);
			if (acquire_num == 0) {
				cstrlog.Format(_T("图像采集系统初始化失败: %d"), GetLastError());
				::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
				return FALSE;
			}
			else {
				m_bIsInitialized = TRUE;
				cstrlog.Format(_T("图像采集系统初始化成功: 数量 %d"), acquire_num);
				::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
			}
		}
		return TRUE;
	}
	else
		return FALSE;
}

bool CAcquireImage::CameraSystemInitial(int newest)
{
	if (m_bIsInitialized)
		return true;

	bool state = false;

	InitialThisClass();

	CString cstrlog;
	std::vector<std::string> vServerName;
	std::vector<std::string> vDeviceName;
	if (AutoScanServers(vServerName, vDeviceName)) {
		int acquire_num = 0;
		acquire_num = InitialAcqServerAndDevice(vServerName, vDeviceName);
		if (acquire_num == 0) {
			state = false;
			cstrlog.Format(_T("图像采集系统初始化失败: %d"), GetLastError());
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		}
		else {
			state = true;
			cstrlog.Format(_T("图像采集系统初始化成功: 数量 %d"), acquire_num);
			::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
		}
	}
	else
		state = false;

	m_bIsInitialized = state;
	return state;
}


//自动搜索采集设备
BOOL CAcquireImage::AutoScanServers(std::vector<std::string> &vServerName
	, std::vector<std::string> &vDeviceName)
{
	//////// Ask questions to user to select acquisition board/device and config file ////////
	CString cstrlog;

	// Get total number of boards in the system
	int serverCount = SapManager::GetServerCount();
	if (serverCount == 0){
		cstrlog.Format(_T("未发现采集设备，请检查相机线路和电源连接: %d"), GetLastError());
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		return FALSE;
	}

	// Scan the boards to find those that support acquisition
	BOOL serverFound = FALSE;
	BOOL cameraFound = FALSE;

	for (int serverIndex = 0; serverIndex < serverCount; serverIndex++){
		// ResourceAcq 表示：采集卡的连接
		if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcq) != 0){
			char serverName[CORSERVER_MAX_STRLEN];
			SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
			vServerName.push_back(std::string(serverName));
			//printf("%d: %s\n", serverIndex, serverName);
			serverFound = TRUE;
			CStringA nameA = serverName;			
			cstrlog.Format(_T("连接采集卡:"));
			cstrlog = cstrlog + (CA2W)nameA;
			::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
		}
		//ResourceAcqDevice 表示：相机的连接
		else if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0){
			char serverName[CORSERVER_MAX_STRLEN];
			SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
			if (strstr(serverName, CAMERA_LINK_SERVER_NAME_PREFIX) > 0) {
				vDeviceName.push_back(std::string(serverName));
				CStringA nameA = serverName;
				cstrlog.Format(_T("连接相机:"));
				cstrlog = cstrlog + (CA2W)nameA;
				::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
				continue;
			}
			//printf("%d: %s\n", serverIndex, serverName);
			cameraFound = TRUE;
		}
	}

	// At least one acquisition server must be available
	if (!serverFound && !cameraFound){
		cstrlog.Format(_T("无匹配的采集设备: %d"), GetLastError());
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		return FALSE;
	}

	// Scan all the acquisition devices on that server and show menu to user
	// 获取Server支持的Resources，本相机CameraLink Full Mono默认为 AcqDeviceIndex = 0

	// List all files in the config directory
	// 获取采集卡配置文件，ccf文件
	return TRUE;
}

//初始化采集卡
int CAcquireImage::InitialAcqServerAndDevice(std::vector<std::string> vServerName
	, std::vector<std::string> vDeviceName)
{
	int initial_num = 0;
	if (vServerName.empty() || vDeviceName.empty()) {
		return 0;
	}

	for (int index = 0; index < (int)vServerName.size(); index++) {			   		 
		// Loc(serverName, ResourceIndex)    resourceIndex = 0
		SapLocation locServer(vServerName.at(index).c_str(), 0);
		m_Acq[index] = new SapAcquisition(locServer, m_ccfFileName);
		m_Buffer[index] = new SapBufferWithTrash(2, m_Acq[index]);
		m_Xfer[index] = new SapAcqToBuf(m_Acq[index], m_Buffer[index], m_Callback[index], this);
		m_View[index] = new SapView(m_Buffer[index]);

		// LocDevice(deviceName, DeviceIndex)  deviceIndex = 0  
		SapLocation locDevice(vDeviceName.at(index).c_str(), 0);
		m_AcqDevice[index] = new SapAcqDevice(locDevice, FALSE);
		m_Feature[index] = new SapFeature(locDevice);

		//  m_ImageWnd 需要和控件绑定
		m_pImageWnd[index]->AttachSapView(m_View[index]);

		if (m_Acq[index] && !*m_Acq[index]) m_Acq[index]->Create();
		if (m_AcqDevice[index] && !*m_AcqDevice[index]) m_AcqDevice[index]->Create();
		if (m_Feature[index] && !*m_Feature[index]) m_Feature[index]->Create();
		if (m_Buffer[index] && !*m_Buffer[index]) m_Buffer[index]->Create();
		if (m_View[index] && !*m_View[index]) m_View[index]->Create();
		if (m_Xfer[index] && !*m_Xfer[index]) m_Xfer[index]->Create();

#ifdef FREERUN
		m_AcqDevice[index]->SetFeatureValue("TriggerMode", "Off");//触发模式关闭
		m_AcqDevice[index]->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定触发频率
#else
		m_AcqDevice[index]->SetFeatureValue("TriggerMode", "On");//触发模式打开		
#endif

		//  设置显示窗口属性
		m_pImageWnd[index]->AttachEventHandler(m_pWndHandle);
		m_pImageWnd[index]->CenterImage(true);
		m_pImageWnd[index]->Reset();

		initial_num += 1;
	}
	m_nCameraNum = initial_num;
	return initial_num;
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
BOOL CAcquireImage::Grab()
{
	for (int index = 0; index < m_nCameraNum; index++) {
		if (!m_Xfer[index]->Grab()) {
			CString cstrlog;
			cstrlog.Format(L"设备 %d 采集失败：Grab Error %d", index + 1, GetLastError());
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
			return FALSE;
		}
	}
	return TRUE;
}

//采集一帧
BOOL CAcquireImage::Snap()
{
	for (int index = 0; index < m_nCameraNum; index++) {
		if (!m_Xfer[index]->Snap()) {
			CString cstrlog;
			cstrlog.Format(L"设备 %d 采集失败：Snap Error %d", index + 1, GetLastError());
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
			return FALSE;
		}
	}
	return TRUE;
}

//停止采集
BOOL CAcquireImage::Freeze()
{
	for (int index = 0; index < m_nCameraNum; index++) {
		if (!m_Xfer[index]->Freeze()) {
			if (CAbortDlg(AfxGetApp()->m_pMainWnd, m_Xfer[index]).DoModal() != IDOK)
				m_Xfer[index]->Abort();
			return FALSE;
		}
	}
	return TRUE;
}

//采集复位
void CAcquireImage::ResetAcquire()
{
	for (int i = 0; i < 4; i++) {
		m_arrayFrameCount[i] = 0;
		m_arrayTrashCount[i] = 0;
		m_arrayBufferIndex[i] = 0;
	}

	return;
}

//降低采集卡触发速率
void CAcquireImage::DropAcquireSpeed(int k)
{
	for (int index = 0; index < m_nCameraNum; index++) {
		if (m_Acq[index]->SetParameter(CORACQ_PRM_SHAFT_ENCODER_DROP, k, TRUE)) {
			CString cstrlog;
			cstrlog.Format(L"设备 %d 已降低采集频率: %d", index + 1, k);
			::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
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
	SapXferFrameRateInfo* pStats;
	if (m_nCameraNum > 0) {
		pStats = m_Xfer[0]->GetFrameRateStatistics();
		if (pStats->IsLiveFrameRateAvailable() && !pStats->IsLiveFrameRateStalled())
			//      8192 * 60 / 1000      0.05
			speed = pStats->GetLiveFrameRate() * m_k_speed * VERTICAL_PRECISION;
	}

	return speed;
}

//计算总帧数
UINT64 CAcquireImage::GetTotalFrameCount()
{
	//m_unTotalFrameCount = std::accumulate(m_arrayFrameCount, m_arrayFrameCount + 4, 0);
	UINT64 total = m_arrayFrameCount[0] + m_arrayFrameCount[1] + m_arrayFrameCount[2] + m_arrayFrameCount[3];

	return total;
}


int CAcquireImage::GetTotalTrashCount()
{
	//m_total_trash_count = std::accumulate(m_arrayTrashCount, m_arrayTrashCount + 4, 0);
	int total = m_arrayTrashCount[0] + m_arrayTrashCount[1] + m_arrayTrashCount[2] + m_arrayTrashCount[3];

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
void CAcquireImage::AcqCallback1(SapXferCallbackInfo *pInfo)
{
	CAcquireImage *pThis = (CAcquireImage*)pInfo->GetContext();
	//考虑如何获取本采集设备的序号或名称
	int INDEX = 0;
	int buffer_index = pThis->m_arrayBufferIndex[INDEX];
	if (pInfo->IsTrash())
		pThis->m_arrayTrashCount[INDEX] += 1;
	else {
		//CString cstrlog;
		//cstrlog.Format(_T("1# camera get buffer"));
		//::SendNotifyMessageW(pThis->hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);


		if (pThis->SHOW_BUFFER) pThis->m_View[INDEX]->Show();
		if (!pThis->m_bSystemPause) {
			HImage ho_image;
			pThis->GenerateHImage(pThis->m_Buffer[INDEX], buffer_index, ho_image);
			if (ho_image.IsInitialized() && pThis->m_pProcessing[INDEX] != NULL) {
				pThis->m_pProcessing[INDEX]->m_listAcquiredImage.push_back(ho_image);
				pThis->m_arrayFrameCount[INDEX] += 1;
			}			
		}
		if (buffer_index == 0)      pThis->m_arrayBufferIndex[INDEX] = 1;
		else if (buffer_index == 1) pThis->m_arrayBufferIndex[INDEX] = 0;
	}
}


void CAcquireImage::AcqCallback2(SapXferCallbackInfo *pInfo)
{
	CAcquireImage *pThis = (CAcquireImage*)pInfo->GetContext();
	//考虑如何获取本采集设备的序号或名称
	int INDEX = 1;
	int buffer_index = pThis->m_arrayBufferIndex[INDEX];
	if (pInfo->IsTrash())
		pThis->m_arrayTrashCount[INDEX] += 1;
	else {
		if (pThis->SHOW_BUFFER) pThis->m_View[INDEX]->Show();
		if (!pThis->m_bSystemPause) {
			HImage ho_image;
			pThis->GenerateHImage(pThis->m_Buffer[INDEX], buffer_index, ho_image);
			if (ho_image.IsInitialized() && pThis->m_pProcessing[INDEX] != NULL) {
				pThis->m_pProcessing[INDEX]->m_listAcquiredImage.push_back(ho_image);
				pThis->m_arrayFrameCount[INDEX] += 1;
			}
		}
		if (buffer_index == 0)      pThis->m_arrayBufferIndex[INDEX] = 1;
		else if (buffer_index == 1) pThis->m_arrayBufferIndex[INDEX] = 0;
	}
}


void CAcquireImage::AcqCallback3(SapXferCallbackInfo *pInfo)
{
	CAcquireImage *pThis = (CAcquireImage*)pInfo->GetContext();
	//考虑如何获取本采集设备的序号或名称
	int INDEX = 2;
	int buffer_index = pThis->m_arrayBufferIndex[INDEX];
	if (pInfo->IsTrash())
		pThis->m_arrayTrashCount[INDEX] += 1;
	else {
		if (pThis->SHOW_BUFFER) pThis->m_View[INDEX]->Show();
		if (!pThis->m_bSystemPause) {
			HImage ho_image;
			pThis->GenerateHImage(pThis->m_Buffer[INDEX], buffer_index, ho_image);
			if (ho_image.IsInitialized() && pThis->m_pProcessing[INDEX] != NULL) {
				pThis->m_pProcessing[INDEX]->m_listAcquiredImage.push_back(ho_image);
				pThis->m_arrayFrameCount[INDEX] += 1;
			}
		}
		if (buffer_index == 0)      pThis->m_arrayBufferIndex[INDEX] = 1;
		else if (buffer_index == 1) pThis->m_arrayBufferIndex[INDEX] = 0;
	}
}


void CAcquireImage::AcqCallback4(SapXferCallbackInfo *pInfo)
{
	CAcquireImage *pThis = (CAcquireImage*)pInfo->GetContext();
	//考虑如何获取本采集设备的序号或名称
	int INDEX = 3;
	int buffer_index = pThis->m_arrayBufferIndex[INDEX];
	if (pInfo->IsTrash())
		pThis->m_arrayTrashCount[INDEX] += 1;
	else {
		if (pThis->SHOW_BUFFER) pThis->m_View[INDEX]->Show();
		if (!pThis->m_bSystemPause) {
			HImage ho_image;
			pThis->GenerateHImage(pThis->m_Buffer[INDEX], buffer_index, ho_image);
			if (ho_image.IsInitialized() && pThis->m_pProcessing[INDEX] != NULL) {
				pThis->m_pProcessing[INDEX]->m_listAcquiredImage.push_back(ho_image);
				pThis->m_arrayFrameCount[INDEX] += 1;
			}
		}
		if (buffer_index == 0)      pThis->m_arrayBufferIndex[INDEX] = 1;
		else if (buffer_index == 1) pThis->m_arrayBufferIndex[INDEX] = 0;
	}
}
