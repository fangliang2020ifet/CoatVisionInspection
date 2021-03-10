//图像获取类

#include "stdafx.h"
#include "DeVision.h"
#include "CAcquireImage.h"


// Static Variables
#define GAMMA_FACTOR			10000
#define MAX_CONFIG_FILES   36       // 10 numbers + 26 lowercase letters
#define STRING_LENGTH		256 
static char configFileNames[MAX_CONFIG_FILES][MAX_PATH] = { 0 };

#define GVSP_PIX_BAYRG8 (0x01000000 | 0x00080000 | 0x0009)

#define CAMERA_LINK_SERVER_NAME_PREFIX "CameraLink_"



CAcquireImage::CAcquireImage()
{
	m_bSystemPause = FALSE;
	m_nCameraNum = 0;
	m_camera_system_initialled = FALSE;
	for (int i = 0; i < 4; i++) { 
		m_pImageWnd[i] = NULL;
		m_Acq[i] = NULL;
		m_AcqDevice[i] = NULL;
		m_Buffer[i] = NULL;
		m_Feature[i] = NULL;
		m_Xfer[i] = NULL;
		m_View[i] = NULL;
	}
	m_pWndHandle = NULL;

}


CAcquireImage::CAcquireImage(const CAcquireImage &acq)
{

}


CAcquireImage::~CAcquireImage()
{
	for (int index = 0; index < (int)m_vAcquireServerName.size(); index++) {
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
	//for (int i = 0; i < GetServerCount(); i++) {
	//	char name[128];
	//	GetServerName(i, name, sizeof(name));
	//	int acq_num = GetResourceCount(i, ResourceAcq);
	//	int acq_dev_num = GetResourceCount(i, ResourceAcqDevice);
	//	int temp = 0;
	//
	//}


	//int numb = GetServerCount();
	//for (int i = 0; i < GetServerCount(); i++){
	//	if (GetResourceCount(i, ResourceAcq) > 0 && GetResourceCount(i, ResourceAcqDevice) == 0) {
	//		char name[128];
	//		if (GetServerName(i, name, sizeof(name))) {
	//			if (strstr(name, "Xtium-CL_MX4_") != NULL) {
	//				m_vAcquireServerName.push_back(CString(name));
	//			}
	//		}
	//	}
	//	if (GetResourceCount(i, ResourceAcq) == 0 && GetResourceCount(i, ResourceAcqDevice) == 1) {
	//		char name[128];
	//		if (GetServerName(i, name, sizeof(name))) {
	//			if (strstr(name, "CameraLink_") != NULL) {
	//				m_vAcquireDeviceServerName.push_back(CString(name));
	//			}
	//		}
	//	}
	//}
	CString cname1;
	cname1.Format(L"Xtium-CL_MX4_1");
	m_vAcquireServerName.push_back(cname1);
	cname1.Format(L"Xtium-CL_MX4_2");
	m_vAcquireServerName.push_back(cname1);
	cname1.Format(L"Xtium-CL_MX4_3");
	m_vAcquireServerName.push_back(cname1);
	cname1.Format(L"Xtium-CL_MX4_4");
	m_vAcquireServerName.push_back(cname1);
	CString cname2;
	cname2.Format(L"CameraLink_1");
	m_vAcquireDeviceServerName.push_back(cname2);
	cname2.Format(L"CameraLink_2");
	m_vAcquireDeviceServerName.push_back(cname2);
	cname2.Format(L"CameraLink_3");
	m_vAcquireDeviceServerName.push_back(cname2);
	cname2.Format(L"CameraLink_4");
	m_vAcquireDeviceServerName.push_back(cname2);



	//for (int i = 0; i < 4; i++) {
	//	CString servername;
	//	servername.Format(_T("Xtium-CL_MX4_%d", i+ 1));
	//	m_vAcquireServerName.push_back(servername);
	//	CString devicename;
	//	//devicename.Format(_T("CameraLink_%d", i + 1));
	//	//m_vAcquireDeviceServerName.push_back(devicename);
	//}

	return (int)m_vAcquireServerName.size();
}


int CAcquireImage::ScanAcqDevice2()
{
	int serverCount = SapManager::GetServerCount();
	int acqDeviceCount = 0;
	int deviceCount = 0;
	int GenieIndex = 0;
	int deviceIndex = 0;
	std::vector<std::string> v_ServerNames;
	if (serverCount == 0){
		//printf("No device found!\n");
		return 0;
	}

	char serverName[CORSERVER_MAX_STRLEN];
	for (int serverAcqIndex = 0; serverAcqIndex < serverCount; serverAcqIndex++){
		if (SapManager::GetResourceCount(serverAcqIndex, SapManager::ResourceAcqDevice) != 0){
			SapManager::GetServerName(serverAcqIndex, serverName, sizeof(serverName));
			acqDeviceCount++;

			if (strstr(serverName, CAMERA_LINK_SERVER_NAME_PREFIX) == 0 && SapManager::GetResourceCount(serverAcqIndex, SapManager::ResourceAcq) == 0)
				acqDeviceCount--;

		}
	}

	BOOL serverFound = FALSE;
	for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)	{
		if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0){
			SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));

			if (strstr(serverName, CAMERA_LINK_SERVER_NAME_PREFIX) == 0 && SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcq) == 0)
				continue;

			printf("%d: %s\n", GenieIndex + 1, serverName);
			GenieIndex++;
			serverFound = TRUE;
			deviceCount = GenieIndex;

			char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
			deviceIndex = 0;
			SapManager::GetResourceName(serverName, SapManager::ResourceAcqDevice, deviceIndex, deviceName, sizeof(deviceName));
			printf("    %s%s\n", "User defined Name : ", deviceName);
			printf("........................................\n");
			v_ServerNames.push_back(serverName);


			//std::string sServerName = v_ServerNames[serverNum - 1];
			// Get the Acquisition Server name selected.
			//CorStrncpy(acqServerName, sServerName.c_str(), sServerName.size() + 1);


		}
	}

	// At least one acquisition server must be available
	if (!serverFound){
		//printf("No camera found!\n");
		return 0;
	}
	return serverCount;
}


int CAcquireImage::InitServerOld()
{
	m_vServerName.clear();

	//为何debug下等于9， release下等于1
	int serverCount = SapManager::GetServerCount();

	for (int i = 0; i < serverCount; i++)
	{
		// Does this server support "Acq" (frame-grabber) or "AcqDevice" (camera)?

		bool bAcq = (m_serverCategory == ServerAcq || m_serverCategory == ServerAll)
			&& (GetResourceCount(i, ResourceAcq) > 0);

		// when m_serverCategory is ServerAcqDevice, show only servers that have only one resource that is an acqdevice and no acq
		bool bAcqDevice = (m_serverCategory == ServerAcqDevice || m_serverCategory == ServerAll)
			&& (GetResourceCount(i, ResourceAcqDevice) == 1) && (GetResourceCount(i, ResourceAcq) == 0);

		if (bAcq || bAcqDevice || m_bShowCameraLinkServers)
		{
			char name[128];
			if (GetServerName(i, name, sizeof(name)))
			{
				if (strstr(name, "CameraLink_") != NULL){
					if (m_bShowCameraLinkServers) {
						//m_vServerName.push_back(CString(name));
						m_vAcquireServerName.push_back(CString(name));
					}

					//m_cbServer.AddString(CString(name));
				}
			}
		}
	}

	//for (int m = 0; m < 4; m++) {
	//	char name[128] = "test";
	//	m_vAcquireServerName.push_back(CString(name));		
	//}

	return m_vAcquireServerName.size();
}


//图像采集系统初始化
BOOL CAcquireImage::CameraSystemInitial()
{
	InitialThisClass();

	CString cstrlog;
	int acquire_num = 0;
	if (!m_camera_system_initialled) {
		//acquire_num = ScanAcqDevice();
		acquire_num = InitServerOld();
		//acquire_num = 4;

		if (acquire_num == 0) {
			cstrlog.Format(_T("未发现采集设备，请检查线路和电源连接: %d"), GetLastError());
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
			return FALSE;
		}
		else {
			//acquire_num = InitialAcqDevices();
			acquire_num = InitialAcqDevices2();
			//acquire_num = InitialAllBoards();
			if (acquire_num == 0) {
				cstrlog.Format(_T("图像采集系统初始化失败: %d"), GetLastError());
				::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
				return FALSE;
			}
		}
	}
	m_camera_system_initialled = TRUE;
	cstrlog.Format(_T("图像采集系统初始化成功: 数量 %d"), acquire_num);
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
	return TRUE;
}

//初始化采集卡
int CAcquireImage::InitialAcqDevices()
{
	int initial_num = 0;
	for(int index = 0; index < (int)m_vAcquireServerName.size(); index++){
		CStringA cstrAcquireServerName = (CT2A)m_vAcquireServerName.at(index);
		CStringA cstrAcquireDeviceServerName = (CT2A)m_vAcquireDeviceServerName.at(index);

		CStringA filename;
		//if (!FREE_RUN) filename.Format(L"system\\T_LA_CM_08K08A_00_R_External_Trigger_Board%d.ccf", index + 1);
		//else filename.Format(L"system\\T_LA_CM_08K08A_00_R_FreeRun_%d.ccf", index + 1);	

		if (!FREE_RUN) 
			filename.Format("D:\\T_LA_CM_08K08A_00_R_External_Trigger_Board%d.ccf", index + 1);
		else 
			filename.Format("system\\T_LA_CM_08K08A_00_R_FreeRun_%d.ccf", index + 1);

		//CString cacquirename;
		//cacquirename.Format(L"Xtium-CL_MX4_1");
		//char name[128];
		//strcpy_s(name, "Xtium-CL_MX4_1");

		//SapLocation loc(cstrAcquireServerName, 0);
		//m_Acq[index].SetLocation(loc);
		//m_Acq.SetConfigFile(CStringA(m_configFile));


		SapLocation locAcquireServer(acqServerName4, 0);
		m_Acq[index] = new SapAcquisition(locAcquireServer, configFilename1);


		//SapLocation locAcquireServer(cstrAcquireServerName.GetBuffer(), 0);
		//m_Acq[index] = new SapAcquisition(locAcquireServer, filename.GetBuffer());
		m_Buffer[index] = new SapBufferWithTrash(2, m_Acq[index]);
		m_Xfer[index] = new SapAcqToBuf(m_Acq[index], m_Buffer[index], m_Callback[index], this);
		m_View[index] = new SapView(m_Buffer[index]);

		SapLocation locAcquireDeviceServer(cstrAcquireDeviceServerName.GetBuffer(), 0);
		m_AcqDevice[index] = new SapAcqDevice(locAcquireDeviceServer, FALSE);
		m_Feature[index] = new SapFeature(locAcquireDeviceServer);

		//  m_ImageWnd 需要和控件绑定
		m_pImageWnd[index]->AttachSapView(m_View[index]);

		if (m_Acq[index] && !*m_Acq[index]) m_Acq[index]->Create();
		if (m_AcqDevice[index] && !*m_AcqDevice[index]) m_AcqDevice[index]->Create();
		if (m_Feature[index] && !*m_Feature[index]) m_Feature[index]->Create();
		if (m_Buffer[index] && !*m_Buffer[index]) m_Buffer[index]->Create();
		if (m_View[index] && !*m_View[index]) m_View[index]->Create();
		if (m_Xfer[index] && !*m_Xfer[index]) m_Xfer[index]->Create();

		if (!FREE_RUN) 	m_AcqDevice[index]->SetFeatureValue("TriggerMode", "On");//触发模式打开		
		else {
			m_AcqDevice[index]->SetFeatureValue("TriggerMode", "Off");//触发模式关闭
			m_AcqDevice[index]->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定触发频率
		}

		m_pImageWnd[index]->AttachEventHandler(m_pWndHandle);
		m_pImageWnd[index]->CenterImage(true);
		m_pImageWnd[index]->Reset();
		
		initial_num += 1;
	}
	m_nCameraNum = initial_num;
	return initial_num;
}


int CAcquireImage::InitialAcqDevices2()
{

	int initial_num = 0;
	for (int index = 0; index < (int)m_vAcquireServerName.size(); index++) {
		char Filename[MAX_PATH];
		char ServerName[CORSERVER_MAX_STRLEN];
		char DeviceName[CORSERVER_MAX_STRLEN];
		switch (index)
		{
		case 0: {
			//(L"system\\T_LA_CM_08K08A_00_R_FreeRun_%d.ccf", index + 1)   External_Trigger_Board
			char file[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_1.ccf";
			char server[CORSERVER_MAX_STRLEN] = "Xtium-CL_MX4_1";
			char device[CORSERVER_MAX_STRLEN] = "CameraLink_1";
			memcpy(Filename, file, sizeof(file));
			memcpy(ServerName, server, sizeof(server));
			memcpy(DeviceName, device, sizeof(device));
			break;
		}
		case 1: {
			char file[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_2.ccf";
			char server[CORSERVER_MAX_STRLEN] = "Xtium-CL_MX4_2";
			char device[CORSERVER_MAX_STRLEN] = "CameraLink_2";
			memcpy(Filename, file, sizeof(file));
			memcpy(ServerName, server, sizeof(server));
			memcpy(DeviceName, device, sizeof(device));
			break;
		}
		case 2: {
			char file[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_3.ccf";
			char server[CORSERVER_MAX_STRLEN] = "Xtium-CL_MX4_3";
			char device[CORSERVER_MAX_STRLEN] = "CameraLink_3";
			memcpy(Filename, file, sizeof(file));
			memcpy(ServerName, server, sizeof(server));
			memcpy(DeviceName, device, sizeof(device));
			break;
		}
		case 3: {
			char file[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_4.ccf";
			char server[CORSERVER_MAX_STRLEN] = "Xtium-CL_MX4_4";
			char device[CORSERVER_MAX_STRLEN] = "CameraLink_4";
			memcpy(Filename, file, sizeof(file));
			memcpy(ServerName, server, sizeof(server));
			memcpy(DeviceName, device, sizeof(device));
			break;
		}
		default:
			break;
		}

		SapLocation locAcquireServer(ServerName, 0);
		m_Acq[index] = new SapAcquisition(locAcquireServer, Filename);
		//SapLocation locAcquireServer(cstrAcquireServerName.GetBuffer(), 0);
		//m_Acq[index] = new SapAcquisition(locAcquireServer, filename.GetBuffer());
		m_Buffer[index] = new SapBufferWithTrash(2, m_Acq[index]);
		m_Xfer[index] = new SapAcqToBuf(m_Acq[index], m_Buffer[index], m_Callback[index], this);
		m_View[index] = new SapView(m_Buffer[index]);

		SapLocation locAcquireDeviceServer(DeviceName, 0);
		//SapLocation locAcquireDeviceServer(cstrAcquireDeviceServerName.GetBuffer(), 0);
		m_AcqDevice[index] = new SapAcqDevice(locAcquireDeviceServer, FALSE);
		m_Feature[index] = new SapFeature(locAcquireDeviceServer);

		//  m_ImageWnd 需要和控件绑定
		m_pImageWnd[index]->AttachSapView(m_View[index]);

		if (m_Acq[index] && !*m_Acq[index]) m_Acq[index]->Create();
		if (m_AcqDevice[index] && !*m_AcqDevice[index]) m_AcqDevice[index]->Create();
		if (m_Feature[index] && !*m_Feature[index]) m_Feature[index]->Create();
		if (m_Buffer[index] && !*m_Buffer[index]) m_Buffer[index]->Create();
		if (m_View[index] && !*m_View[index]) m_View[index]->Create();
		if (m_Xfer[index] && !*m_Xfer[index]) m_Xfer[index]->Create();

		if (!FREE_RUN) 	m_AcqDevice[index]->SetFeatureValue("TriggerMode", "On");//触发模式打开		
		else {
			m_AcqDevice[index]->SetFeatureValue("TriggerMode", "Off");//触发模式关闭
			m_AcqDevice[index]->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定触发频率
		}

		m_pImageWnd[index]->AttachEventHandler(m_pWndHandle);
		m_pImageWnd[index]->CenterImage(true);
		m_pImageWnd[index]->Reset();

		initial_num += 1;
	}
	m_nCameraNum = initial_num;
	return initial_num;

}


BOOL CAcquireImage::InitialAllBoards()
{
	if (FREE_RUN) {
		char free_run_1[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_1.ccf";
		char free_run_2[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_2.ccf";
		char free_run_3[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_3.ccf";
		char free_run_4[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_FreeRun_4.ccf";
		memcpy(configFilename1, free_run_1, sizeof(free_run_1));
		memcpy(configFilename2, free_run_2, sizeof(free_run_2));
		memcpy(configFilename3, free_run_3, sizeof(free_run_3));
		memcpy(configFilename4, free_run_4, sizeof(free_run_4));
		CString cstr = L"已设为内部触发模式";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}
	else {
		char encode_trigger_1[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_External_Trigger_Board1.ccf";
		char encode_trigger_2[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_External_Trigger_Board2.ccf";
		char encode_trigger_3[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_External_Trigger_Board3.ccf";
		char encode_trigger_4[MAX_PATH] = "system\\T_LA_CM_08K08A_00_R_External_Trigger_Board4.ccf";
		memcpy(configFilename1, encode_trigger_1, sizeof(encode_trigger_1));
		memcpy(configFilename2, encode_trigger_2, sizeof(encode_trigger_2));
		memcpy(configFilename3, encode_trigger_3, sizeof(encode_trigger_3));
		memcpy(configFilename4, encode_trigger_4, sizeof(encode_trigger_4));
		CString cstr = L"已设为外部触发模式";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}

	if (1)
	{
		//板卡
		SapLocation loc1(acqServerName1, m_acqDeviceNumber1);
		m_Acq[0] = new SapAcquisition(loc1, configFilename1);
		m_Buffer[0] = new SapBufferWithTrash(2, m_Acq[0]);  // 双缓存
		m_Xfer[0] = new SapAcqToBuf(m_Acq[0], m_Buffer[0], AcqCallback1, this);
		m_View[0] = new SapView(m_Buffer[0]);

		SapLocation loc2(acqServerName2, m_acqDeviceNumber2);
		m_Acq[1] = new SapAcquisition(loc2, configFilename2);
		m_Buffer[1] = new SapBufferWithTrash(2, m_Acq[1]);  // 双缓存
		m_Xfer[1] = new SapAcqToBuf(m_Acq[1], m_Buffer[1], AcqCallback2, this);
		m_View[1] = new SapView(m_Buffer[1]);

		SapLocation loc3(acqServerName3, m_acqDeviceNumber3);
		m_Acq[2] = new SapAcquisition(loc3, configFilename3);
		m_Buffer[2] = new SapBufferWithTrash(2, m_Acq[2]);  // 双缓存
		m_Xfer[2] = new SapAcqToBuf(m_Acq[2], m_Buffer[2], AcqCallback3, this);
		m_View[2] = new SapView(m_Buffer[2]);

		SapLocation loc4(acqServerName4, m_acqDeviceNumber4);
		m_Acq[3] = new SapAcquisition(loc4, configFilename4);
		m_Buffer[3] = new SapBufferWithTrash(2, m_Acq[3]);  // 双缓存
		m_Xfer[3] = new SapAcqToBuf(m_Acq[3], m_Buffer[3], AcqCallback4, this);
		m_View[3] = new SapView(m_Buffer[3]);
	}
	if (1)
	{
		//相机
		SapLocation loc1(acqDeviceName1, m_acqDeviceNumber1);
		m_AcqDevice[0] = new SapAcqDevice(loc1, FALSE);

		SapLocation loc2(acqDeviceName2, m_acqDeviceNumber2);
		m_AcqDevice[1] = new SapAcqDevice(loc2, FALSE);

		SapLocation loc3(acqDeviceName3, m_acqDeviceNumber3);
		m_AcqDevice[2] = new SapAcqDevice(loc3, FALSE);

		SapLocation loc4(acqDeviceName4, m_acqDeviceNumber4);
		m_AcqDevice[3] = new SapAcqDevice(loc4, FALSE);

		m_Feature[0] = new SapFeature(acqDeviceName1);
		m_Feature[1] = new SapFeature(acqDeviceName2);
		m_Feature[2] = new SapFeature(acqDeviceName3);
		m_Feature[3] = new SapFeature(acqDeviceName4);
	}
	// Attach sapview to image viewer
	m_pImageWnd[0]->AttachSapView(m_View[0]);
	m_pImageWnd[1]->AttachSapView(m_View[1]);
	m_pImageWnd[2]->AttachSapView(m_View[2]);
	m_pImageWnd[3]->AttachSapView(m_View[3]);

	// Create all objects
	if (!CreateObjects()) { return FALSE; }

	//设置板卡的硬件滤波
	//SetHardwareFilter();


	//外部触发时无需设置以下参数
	if (FREE_RUN) {
		m_AcqDevice[0]->SetFeatureValue("TriggerMode", "Off");//触发模式关闭
		m_AcqDevice[0]->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定触发频率
		m_AcqDevice[1]->SetFeatureValue("TriggerMode", "Off");//触发模式打开
		m_AcqDevice[1]->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定曝光时间
		m_AcqDevice[2]->SetFeatureValue("TriggerMode", "Off");//触发模式打开
		m_AcqDevice[2]->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定曝光时间
		m_AcqDevice[3]->SetFeatureValue("TriggerMode", "Off");//触发模式打开
		m_AcqDevice[3]->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//设定曝光时间
	}
	else {
		m_AcqDevice[0]->SetFeatureValue("TriggerMode", "On");//触发模式打开
		m_AcqDevice[1]->SetFeatureValue("TriggerMode", "On");//触发模式打开
		m_AcqDevice[2]->SetFeatureValue("TriggerMode", "On");//触发模式打开
		m_AcqDevice[3]->SetFeatureValue("TriggerMode", "On");//触发模式打开
	}

	m_pImageWnd[0]->AttachEventHandler(m_pWndHandle);
	m_pImageWnd[0]->CenterImage(true);
	m_pImageWnd[0]->Reset();

	m_pImageWnd[1]->AttachEventHandler(m_pWndHandle);
	m_pImageWnd[1]->CenterImage(true);
	m_pImageWnd[1]->Reset();

	m_pImageWnd[2]->AttachEventHandler(m_pWndHandle);
	m_pImageWnd[2]->CenterImage(true);
	m_pImageWnd[2]->Reset();

	m_pImageWnd[3]->AttachEventHandler(m_pWndHandle);
	m_pImageWnd[3]->CenterImage(true);
	m_pImageWnd[3]->Reset();

	m_nCameraNum = 4;

	return m_nCameraNum;
}


BOOL CAcquireImage::CreateObjects()
{
	CWaitCursor wait;

	// Create acquisition object
	if (m_Acq[0] && !*m_Acq[0] && !m_Acq[0]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Acq[1] && !*m_Acq[1] && !m_Acq[1]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Acq[2] && !*m_Acq[2] && !m_Acq[2]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Acq[3] && !*m_Acq[3] && !m_Acq[3]->Create())
	{
		DestroyObjects();
		return FALSE;
	}

	//AcqDevice
	if (m_AcqDevice[0] && !*m_AcqDevice[0] && !m_AcqDevice[0]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_AcqDevice[1] && !*m_AcqDevice[1] && !m_AcqDevice[1]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_AcqDevice[2] && !*m_AcqDevice[2] && !m_AcqDevice[2]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_AcqDevice[3] && !*m_AcqDevice[3] && !m_AcqDevice[3]->Create())
	{
		DestroyObjects();
		return FALSE;
	}

	//Feature
	if (m_Feature[0] && !*m_Feature[0] && !m_Feature[0]->Create()) {
		DestroyObjects();
		return FALSE;
	}
	if (m_Feature[1] && !*m_Feature[1] && !m_Feature[1]->Create()) {
		DestroyObjects();
		return FALSE;
	}
	if (m_Feature[2] && !*m_Feature[2] && !m_Feature[2]->Create()) {
		DestroyObjects();
		return FALSE;
	}
	if (m_Feature[3] && !*m_Feature[3] && !m_Feature[3]->Create()) {
		DestroyObjects();
		return FALSE;
	}


	// Create buffer object
	if (m_Buffer[0] && !*m_Buffer[0])
	{
		if (!m_Buffer[0]->Create())
		{
			DestroyObjects();
			return FALSE;
		}
		// Clear all buffers
		m_Buffer[0]->Clear();
	}
	if (m_Buffer[1] && !*m_Buffer[1])
	{
		if (!m_Buffer[1]->Create())
		{
			DestroyObjects();
			return FALSE;
		}
		// Clear all buffers
		m_Buffer[1]->Clear();
	}
	if (m_Buffer[2] && !*m_Buffer[2])
	{
		if (!m_Buffer[2]->Create())
		{
			DestroyObjects();
			return FALSE;
		}
		// Clear all buffers
		m_Buffer[2]->Clear();
	}
	if (m_Buffer[3] && !*m_Buffer[3])
	{
		if (!m_Buffer[3]->Create())
		{
			DestroyObjects();
			return FALSE;
		}
		// Clear all buffers
		m_Buffer[3]->Clear();
	}


	// Create view object
	if (m_View[0] && !*m_View[0] && !m_View[0]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_View[1] && !*m_View[1] && !m_View[1]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_View[2] && !*m_View[2] && !m_View[2]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_View[3] && !*m_View[3] && !m_View[3]->Create())
	{
		DestroyObjects();
		return FALSE;
	}


	// Create transfer object
	if (m_Xfer[0] && !*m_Xfer[0] && !m_Xfer[0]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Xfer[1] && !*m_Xfer[1] && !m_Xfer[1]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Xfer[2] && !*m_Xfer[2] && !m_Xfer[2]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	if (m_Xfer[3] && !*m_Xfer[3] && !m_Xfer[3]->Create())
	{
		DestroyObjects();
		return FALSE;
	}
	return TRUE;

}


BOOL CAcquireImage::DestroyObjects()
{
	// Destroy transfer object
	if (m_Xfer[0] && *m_Xfer[0]) m_Xfer[0]->Destroy();
	if (m_Xfer[1] && *m_Xfer[1]) m_Xfer[1]->Destroy();
	if (m_Xfer[2] && *m_Xfer[2]) m_Xfer[2]->Destroy();
	if (m_Xfer[3] && *m_Xfer[3]) m_Xfer[3]->Destroy();

	// Destroy view object
	if (m_View[0] && *m_View[0]) m_View[0]->Destroy();
	if (m_View[1] && *m_View[1]) m_View[1]->Destroy();
	if (m_View[2] && *m_View[2]) m_View[2]->Destroy();
	if (m_View[3] && *m_View[3]) m_View[3]->Destroy();

	// Destroy buffer object
	if (m_Buffer[0] && *m_Buffer[0]) m_Buffer[0]->Destroy();
	if (m_Buffer[1] && *m_Buffer[1]) m_Buffer[1]->Destroy();
	if (m_Buffer[2] && *m_Buffer[2]) m_Buffer[2]->Destroy();
	if (m_Buffer[3] && *m_Buffer[3]) m_Buffer[3]->Destroy();

	if (m_Feature[0] && *m_Feature[0]) m_Feature[0]->Destroy();
	if (m_Feature[1] && *m_Feature[1]) m_Feature[1]->Destroy();
	if (m_Feature[2] && *m_Feature[2]) m_Feature[2]->Destroy();
	if (m_Feature[3] && *m_Feature[3]) m_Feature[3]->Destroy();

	if (m_AcqDevice[0] && *m_AcqDevice[0]) m_AcqDevice[0]->Destroy();
	if (m_AcqDevice[1] && *m_AcqDevice[1]) m_AcqDevice[1]->Destroy();
	if (m_AcqDevice[2] && *m_AcqDevice[2]) m_AcqDevice[2]->Destroy();
	if (m_AcqDevice[3] && *m_AcqDevice[3]) m_AcqDevice[3]->Destroy();

	// Destroy acquisition object
	if (m_Acq[0] && *m_Acq[0]) m_Acq[0]->Destroy();
	if (m_Acq[1] && *m_Acq[1]) m_Acq[1]->Destroy();
	if (m_Acq[2] && *m_Acq[2]) m_Acq[2]->Destroy();
	if (m_Acq[3] && *m_Acq[3]) m_Acq[3]->Destroy();

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
BOOL CAcquireImage::Grab()
{
	for (int index = 0; index < (int)m_vAcquireServerName.size(); index++) {
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
	for (int index = 0; index < (int)m_vAcquireServerName.size(); index++) {
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
	for (int index = 0; index < (int)m_vAcquireServerName.size(); index++) {
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
	for (int index = 0; index < (int)m_vAcquireServerName.size(); index++) {
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
	if (!m_vAcquireServerName.empty()) {
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
		if (pThis->SHOW_BUFFER) pThis->m_View[INDEX]->Show();
		if (!pThis->m_bSystemPause) {
			HImage ho_image;
			pThis->GenerateHImage(pThis->m_Buffer[INDEX], buffer_index, ho_image);
			if (pThis->m_pProcessing[INDEX] != NULL) {
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
			if (pThis->m_pProcessing[INDEX] != NULL) {
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
			if (pThis->m_pProcessing[INDEX] != NULL) {
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
			if (pThis->m_pProcessing[INDEX] != NULL) {
				pThis->m_pProcessing[INDEX]->m_listAcquiredImage.push_back(ho_image);
				pThis->m_arrayFrameCount[INDEX] += 1;
			}
			pThis->m_arrayFrameCount[INDEX] += 1;
		}
		if (buffer_index == 0)      pThis->m_arrayBufferIndex[INDEX] = 1;
		else if (buffer_index == 1) pThis->m_arrayBufferIndex[INDEX] = 0;
	}
}
