// CCameraDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "DeVisionDlg.h"
#include "CCameraDlg.h"
#include "afxdialogex.h"
#include <string>

// CCameraDlg 对话框

IMPLEMENT_DYNAMIC(CCameraDlg, CDialogEx)

CCameraDlg::CCameraDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CAMERA, pParent)
{

}

CCameraDlg::~CCameraDlg()
{
	if (isOpened)
	{
		TerminateThread(hRxThread, 0);
		//关闭串口
		if (mUart.isConnected())
			mUart.ClosePort();
	}

}

void CCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_CAMERA1_IMAGE_VIEW, m_ImageWnd1);
	//DDX_Control(pDX, IDC_CAMERA2_IMAGE_VIEW, m_ImageWnd2);
	//DDX_Control(pDX, IDC_CAMERA3_IMAGE_VIEW, m_ImageWnd3);
	//DDX_Control(pDX, IDC_CAMERA4_IMAGE_VIEW, m_ImageWnd4);
	DDX_Control(pDX, IDC_RADIO1, m_camera1_selected);
	DDX_Control(pDX, IDC_RADIO2, m_camera2_selected);
	DDX_Control(pDX, IDC_RADIO3, m_camera3_selected);
	DDX_Control(pDX, IDC_RADIO4, m_camera4_selected);
	DDX_Control(pDX, IDC_CAMERA_NAME, m_camera_name);
	DDX_Control(pDX, IDC_CAMERA_TEMPERATURE, m_camera_temperature);
	DDX_Control(pDX, IDC_EDIT2, m_exposure_time);
	DDX_Control(pDX, IDC_CURRENT_FRAME, m_current_frame);
	DDX_Control(pDX, IDC_TOTAL_FRAME, m_total_frame);
	DDX_Control(pDX, IDC_EXPOSURETIME_DISPLAY, m_exposuretime_display);
	DDX_Control(pDX, IDC_BUFFER_SIZE, m_buffer_size);
}


BEGIN_MESSAGE_MAP(CCameraDlg, CDialogEx)

	ON_BN_CLICKED(IDC_BUTTON_OPENUART, &CCameraDlg::OnBnClickedButtonOpenuart)
	ON_BN_CLICKED(IDC_BUTTON_CLOSEUART, &CCameraDlg::OnBnClickedButtonCloseuart)
	ON_BN_CLICKED(IDC_BUTTON3, &CCameraDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CCameraDlg::OnBnClickedButton4)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_LIGHT, &CCameraDlg::OnNMCustomdrawSlider1)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ENDSESSION()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SET_EXPOSURE, &CCameraDlg::OnBnClickedButtonSetExposure)
END_MESSAGE_MAP()


// CCameraDlg 消息处理程序

BOOL CCameraDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//***********************相机******************************
	//获取父窗口指针
	CDeVisionDlg * pMainDlg = (CDeVisionDlg*)this->GetParent();
	pInspectDlg = &(pMainDlg->m_inspectDlg);

	m_camera1_selected.SetCheck(TRUE);
	//初始化图像显示窗
	//m_Image_Wnd_Initialized = InitializeCWndAndView();

	SetTimer(1, 1000, NULL);


	//***********************光源******************************
	InitialUartPort();
	CButton* pbtn = (CButton*)GetDlgItem(IDC_RADIO_A);
	pbtn->SetCheck(true);
	pbtn = (CButton*)GetDlgItem(IDC_RADIO_B);
	pbtn->SetCheck(false);

	CSliderCtrl* pslider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHT);
	pslider->SetRange(0, 255, 0);
	pslider->SetPos(150);	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CCameraDlg::OnDestroy()
{
	__super::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
	if (mTimer != 0)
	{
		KillTimer(1);
		mTimer = 0;
	}

	if (pInspectDlg)
	{
		pInspectDlg->camera1_show_buffer = FALSE;
		pInspectDlg->camera2_show_buffer = FALSE;
		pInspectDlg->camera3_show_buffer = FALSE;
		pInspectDlg->camera4_show_buffer = FALSE;

	}

	//m_ImageWnd1.DestroyWindow();
	//m_ImageWnd2.DestroyWindow();
	//m_ImageWnd3.DestroyWindow();
	//m_ImageWnd4.DestroyWindow();

	//if (pInspectDlg->m_View1 && *pInspectDlg->m_View1)
	//	pInspectDlg->m_View1->Destroy();
	//if (pInspectDlg->m_View2 && *pInspectDlg->m_View2)
	//	pInspectDlg->m_View2->Destroy();
	//if (pInspectDlg->m_View3 && *pInspectDlg->m_View3)
	//	pInspectDlg->m_View3->Destroy();
	//if (pInspectDlg->m_View4 && *pInspectDlg->m_View4)
	//	pInspectDlg->m_View4->Destroy();


}

void CCameraDlg::OnEndSession(BOOL bEnding)
{
	__super::OnEndSession(bEnding);

	// TODO: 在此处添加消息处理程序代码
	if (bEnding)
	{
		// If ending the session, free the resources.
		OnDestroy();
	}

}


void CCameraDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rClient;
	GetClientRect(rClient);

	//// resize image viewer
	//if (m_ImageWnd1.GetSafeHwnd())
	//{
	//	CRect rWnd;
	//	m_ImageWnd1.GetWindowRect(rWnd);
	//	ScreenToClient(rWnd);
	//	//rWnd.right = rClient.right - 5;
	//	//rWnd.bottom = rClient.bottom - 5;
	//	m_ImageWnd1.MoveWindow(rWnd);
	//}
	//if (m_ImageWnd2.GetSafeHwnd())
	//{
	//	CRect rWnd;
	//	m_ImageWnd2.GetWindowRect(rWnd);
	//	ScreenToClient(rWnd);
	//	//rWnd.right = rClient.right - 5;
	//	//rWnd.bottom = rClient.bottom - 5;
	//	m_ImageWnd2.MoveWindow(rWnd);
	//}
	//if (m_ImageWnd3.GetSafeHwnd())
	//{
	//	CRect rWnd;
	//	m_ImageWnd3.GetWindowRect(rWnd);
	//	ScreenToClient(rWnd);
	//	//rWnd.right = rClient.right - 5;
	//	//rWnd.bottom = rClient.bottom - 5;
	//	m_ImageWnd3.MoveWindow(rWnd);
	//}
	//if (m_ImageWnd4.GetSafeHwnd())
	//{
	//	CRect rWnd;
	//	m_ImageWnd4.GetWindowRect(rWnd);
	//	ScreenToClient(rWnd);
	//	//rWnd.right = rClient.right - 5;
	//	//rWnd.bottom = rClient.bottom - 5;
	//	m_ImageWnd4.MoveWindow(rWnd);
	//}

}

void CCameraDlg::InitialUartPort()
{
	CComboBox* pComBox;
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_UART);
	pComBox->ResetContent();
	//获取串口列表
	mUart.GetComList(pComBox);
	//默认选择第一个
	pComBox->SetCurSel(0);

	//添加波特率
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_BAUDRATE);
	TCHAR BaudrateTable[][7] = { L"460800", L"230400", L"194000", L"115200", L"57600", L"56000", L"38400"
								, L"19200", L"14400", L"9600", L"4800", L"2400", L"1200" };
	for (int i = 0; i < (sizeof(BaudrateTable) / sizeof(BaudrateTable[0])); i++)
		pComBox->InsertString(0, BaudrateTable[i]);

	//选择19200为默认
	pComBox->SetCurSel(pComBox->FindString(-1, L"19200"));

	//数据位
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_BITS);
	pComBox->InsertString(0, L"8");
	pComBox->InsertString(0, L"7");
	pComBox->InsertString(0, L"6");
	pComBox->InsertString(0, L"5");
	pComBox->SetCurSel(3);//选择8位为默认

						  //停止位
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_STOP);
	pComBox->InsertString(0, L"2");
	pComBox->InsertString(0, L"1.5");
	pComBox->InsertString(0, L"1");
	pComBox->SetCurSel(0);//选择1位为默认

	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_ECC);
	pComBox->InsertString(0, L"even");
	pComBox->InsertString(0, L"odd ");
	pComBox->InsertString(0, L"none");
	//选择无校验为默认
	pComBox->SetCurSel(0);

	LoadRegConfig();

}

//写入注册表数据
void CCameraDlg::WriteRegData(void)
{
	CComboBox *pCombox;
	CString strtmp;
	pCombox = (CComboBox *)GetDlgItem(IDC_COMBO_UART);
	//串口号
	pCombox->GetLBText(pCombox->GetCurSel(), strtmp);
	AfxGetApp()->WriteProfileString(L"Config", L"ComName", strtmp);

	strtmp.Empty();
	strtmp.Format(L"%d-%d-%d-%d",
		((CComboBox*)GetDlgItem(IDC_COMBO_BAUDRATE))->GetCurSel()
		, ((CComboBox*)GetDlgItem(IDC_COMBO_BITS))->GetCurSel()
		, ((CComboBox*)GetDlgItem(IDC_COMBO_STOP))->GetCurSel()
		, ((CComboBox*)GetDlgItem(IDC_COMBO_ECC))->GetCurSel());
	//配置数据
	AfxGetApp()->WriteProfileString(L"Config", L"BDSE", strtmp);
}

void CCameraDlg::GetRegData(CString &comName, CString &dcbConfig)
{
	comName = AfxGetApp()->GetProfileString(L"Config", L"ComName", L"NULL");
	dcbConfig = AfxGetApp()->GetProfileString(L"Config", L"BDSE", L"NULL");
	TRACE(comName + dcbConfig);
}

void CCameraDlg::LoadRegConfig()
{
	CString comName, dcbConfig;
	CComboBox *pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_UART);

	GetRegData(comName, dcbConfig);
	if (comName != "NULL")
	{
		int sel = pComBox->FindString(-1, comName);
		//没有该串口，或者不是串口号使用默认（初始化值）
		if (sel != CB_ERR && comName[0] == 'C')
			pComBox->SetCurSel(sel);

		int baudSel = 0, dataSel = 0, stopSel = 0, eccSel = 0;

		swscanf_s(dcbConfig, L"%d-%d-%d-%d", &baudSel, &dataSel, &stopSel, &eccSel);
		((CComboBox*)GetDlgItem(IDC_COMBO_BAUDRATE))->SetCurSel(baudSel);
		((CComboBox*)GetDlgItem(IDC_COMBO_BITS))->SetCurSel(dataSel);
		((CComboBox*)GetDlgItem(IDC_COMBO_STOP))->SetCurSel(stopSel);
		((CComboBox*)GetDlgItem(IDC_COMBO_ECC))->SetCurSel(eccSel);
	}
}

bool CCameraDlg::OpenUart()
{
	CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_UART);
	CString strtmp;

	//如果占用就取消占用
	if (mUart.isConnected())
		mUart.ClosePort();

	if (IsUartPortAvailable())
	{
		pCombox->GetLBText(pCombox->GetCurSel(), strtmp);
		mUart.ConfigUart(strtmp, GetUartConfigDCB());
	}
	else
	{
		AfxMessageBox(_T("串口无效,请刷新"));
		return false;
	}

	if (!mUart.OpenCom())
	{
		AfxMessageBox(_T("打开串口失败,请检查：\n1.串口序号是否正确\n2.串口是否被其他程序占用\n"));
		return false;
	}

	//创建接收线程
	hRxThread = AfxBeginThread(RxThreadFunc, mUart.GetThreadStartPara(), THREAD_PRIORITY_NORMAL);
	if (hRxThread == NULL)
	{
		TRACE("Rx Listenner Thread Created Failed");
		return false;
	}

	return true;
}

bool CCameraDlg::IsUartPortAvailable(void)
{
	CString comInfo;
	CComboBox* pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_UART);
	pCombox->GetLBText(pCombox->GetCurSel(), comInfo);

	if (comInfo.Left(3) == _T("COM"))
		return true;
	else
		return false;
}

//从控件中得到配置信息字符串
DCB CCameraDlg::GetUartConfigDCB(void)
{
	DCB configDCB;
	CString tmpStr, UartConfig;
	CComboBox *pCombox;

	//设定串口参数
	pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_BAUDRATE);
	pCombox->GetLBText(pCombox->GetCurSel(), tmpStr);
	configDCB.BaudRate = _ttoi(tmpStr);

	pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_ECC);
	configDCB.Parity = pCombox->GetCurSel();

	pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_BITS);
	pCombox->GetLBText(pCombox->GetCurSel(), tmpStr);
	configDCB.ByteSize = _wtoi(tmpStr);

	pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_STOP);
	configDCB.StopBits = pCombox->GetCurSel();
	   
	////设定串口参数
	//configDCB.BaudRate = _ttoi("19200");
	//configDCB.Parity = 0;
	//configDCB.ByteSize = atoi("8");
	//configDCB.StopBits = 0;

	return configDCB;
}

void CCameraDlg::OnChangeEditTxData(CStringA strtmp)
{
	//CString strtmp;

	//DataTx.ReString(strtmp);

	// 若是CMD模式回车发送(数据包含回车)，并将发送的内容添以特殊格式加到接收框的新行中
	if (1)
	{
		// 删除回车
		//strtmp = DataTx.GetCStrData();
		strtmp.Remove('\r');
		strtmp.Remove('\n');

		// 自动在AT指令后加上换行符
		//strtmp.Append("\r\n");
		AsyncSend(strtmp);

	}

}


void CCameraDlg::OnBnClickedButtonOpenuart()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!OpenUart())
	{
		AfxMessageBox(L"打开串口失败");
		return;
	}

	isOpened = true;

	GetDlgItem(IDC_BUTTON_OPENUART)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_CLOSEUART)->EnableWindow(true);

	GetDlgItem(IDC_SLIDER_LIGHT)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(true);
}


void CCameraDlg::OnBnClickedButtonCloseuart()
{
	// TODO: 在此添加控件通知处理程序代码
	TerminateThread(hRxThread, 0);
	//关闭串口
	if (mUart.isConnected())
		mUart.ClosePort();

	isOpened = false;

	GetDlgItem(IDC_BUTTON_OPENUART)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_CLOSEUART)->EnableWindow(false);

}

//常亮
void CCameraDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	if (isOpened)
	{
		std::string str = "SH#";
		CStringA cstrtmp(str.c_str());
		OnChangeEditTxData(cstrtmp);
	}
}

//长灭
void CCameraDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	if (isOpened)
	{
		std::string str = "SL#";
		CStringA cstrtmp(str.c_str());
		OnChangeEditTxData(cstrtmp);
	}

}


void CCameraDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (isOpened)
	{
		CButton* pbtn = (CButton*)GetDlgItem(IDC_RADIO_A);
		int selectA = pbtn->GetCheck();
		CSliderCtrl* pslider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHT);
		int position = pslider->GetPos();
		if (position < 100)
		{
			std::string positionstr = std::to_string(position);
			std::string str1 = "SA00";
			if (selectA == 1)
				str1 = "SA00" + positionstr;
			else
				str1 = "SB00" + positionstr;
			std::string strtmp = str1 + "#";
			CStringA cstrtmp(strtmp.c_str());
			//CString cstrtmp;

			OnChangeEditTxData(cstrtmp);
		}
		else
		{
			std::string positionstr = std::to_string(position);
			std::string str1;
			if (selectA == 1)
				str1 = "SA0" + positionstr;
			else
				str1 = "SB0" + positionstr;
			std::string strtmp = str1 + "#";
			CStringA cstrtmp(strtmp.c_str());
			OnChangeEditTxData(cstrtmp);
		}

		Sleep(50);

		CString respon;
		AsyncRead(respon);
		int t = 0;
		/*
		if (selectA)
		{
			CString temp = "A";
			if (respon != temp)
			{
				AfxMessageBox("亮度调节失败");
				//pslider->
				//return;
			}
		}
		else
		{
			CString temp = "B";
			if (respon != temp)
			{
				AfxMessageBox("亮度调节失败");
				//return;
			}
		}
		*/
	}

	*pResult = 0;
}

//******************************************相机板卡****************************************
BOOL CCameraDlg::InitializeCWndAndView()
{
	// Attach sapview to image viewer
	//if (pInspectDlg->m_View1)
	//	m_ImageWnd1.AttachSapView(pInspectDlg->m_View1);
	//if (pInspectDlg->m_View2)
	//	m_ImageWnd2.AttachSapView(pInspectDlg->m_View2);
	//if (pInspectDlg->m_View3) 	m_ImageWnd3.AttachSapView(pInspectDlg->m_View3);
	//if (pInspectDlg->m_View4) 	m_ImageWnd4.AttachSapView(pInspectDlg->m_View4);


	//// Create view object
	//if (pInspectDlg->m_View1)
	//{
	//	pInspectDlg->m_View1->Create();
	//}
	//if (pInspectDlg->m_View2)
	//{
	//	pInspectDlg->m_View2->Create();
	//}
	//if (pInspectDlg->m_View3)
	//{
	//	pInspectDlg->m_View3->Create();
	//}
	//if (pInspectDlg->m_View4)
	//{
	//	pInspectDlg->m_View4->Create();
	//}

	////m_ImageWnd1.AttachEventHandler(this);
	//m_ImageWnd1.CenterImage(true);
	//m_ImageWnd1.Reset();

	//m_ImageWnd2.CenterImage(true);
	//m_ImageWnd2.Reset();

	//m_ImageWnd3.AttachEventHandler(this);
	//m_ImageWnd3.CenterImage(true);
	//m_ImageWnd3.Reset();

	//m_ImageWnd4.AttachEventHandler(this);
	//m_ImageWnd4.CenterImage(true);
	//m_ImageWnd4.Reset();

	//pInspectDlg->camera1_show_buffer = TRUE;
	//pInspectDlg->camera2_show_buffer = TRUE;
	//pInspectDlg->camera3_show_buffer = TRUE;
	//pInspectDlg->camera4_show_buffer = TRUE;

	return TRUE;
}

//更新相机参数
void CCameraDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		if(m_Image_Wnd_Initialized) 		UpdateCameraParameter();
	}

	__super::OnTimer(nIDEvent);
}

//设置相机参数
void CCameraDlg::GetCameraParameter(SapAcqDevice* m_AcqDevice)
{
	CString camera_name, camera_temp, camera_exposure;
	char modelName[64], cameraTemperature[6], exposureTime[6];
	if (m_AcqDevice && *m_AcqDevice)
	{
		BOOL status;
		m_AcqDevice->IsFeatureAvailable("DeviceModelName", &status);
		if (status)
		{
			m_AcqDevice->GetFeatureValue("DeviceFirmwareVersion", modelName, sizeof(modelName));
			camera_name = modelName;
			m_camera_name.SetWindowText(camera_name);

			m_AcqDevice->GetFeatureValue("DeviceTemperature", cameraTemperature, sizeof(cameraTemperature));
			camera_temp = cameraTemperature;
			m_camera_temperature.SetWindowTextW(camera_temp);

			m_AcqDevice->GetFeatureValue("ExposureTime", exposureTime, sizeof(exposureTime));
			camera_exposure = exposureTime;
			m_exposuretime_display.SetWindowTextW(camera_exposure);					

		}

		
	}

}

//更新帧率
void CCameraDlg::GetFrameRate(SapTransfer *m_Xfer)
{
	if (1)
	{
		SapXferFrameRateInfo* pStats = m_Xfer->GetFrameRateStatistics();

		if (pStats->IsLiveFrameRateAvailable() && !pStats->IsLiveFrameRateStalled())
		{
			CString sLiveFrameRate;
			sLiveFrameRate.Format(_T("%.1f"), pStats->GetLiveFrameRate());
			m_current_frame.SetWindowTextW(sLiveFrameRate);
		}
		else
		{
			m_current_frame.SetWindowTextW(_T("N/A"));
		}

	}
} 

//获取图像尺寸
void CCameraDlg::GetBufferSize(SapBuffer * m_buffer)
{
	int width = m_buffer->GetWidth();
	int height = m_buffer->GetHeight();

	CString csize, cwidth, cheight;
	cwidth.Format(L"%d", width);
	cheight.Format(L"%d", height);
	csize = L"W=" + cwidth;
	cheight = L", H=" + cheight;
	csize = csize + cheight;

	m_buffer_size.SetWindowTextW(csize);

}

void CCameraDlg::UpdateCameraParameter()
{
	SapAcqDevice * m_pAcqDevice = NULL;
	SapBuffer * m_pBuffer = NULL;
	SapTransfer *m_pXfer = NULL;
	if (m_camera1_selected.GetCheck())
	{
		m_pAcqDevice = pInspectDlg->m_AcqDevice1;
		m_pBuffer = pInspectDlg->m_Buffers1;
		m_pXfer = pInspectDlg->m_Xfer1;
		select_camera_number = 1;
	}
	else if (m_camera2_selected.GetCheck())
	{
		m_pAcqDevice = pInspectDlg->m_AcqDevice2;
		m_pBuffer = pInspectDlg->m_Buffers2;
		m_pXfer = pInspectDlg->m_Xfer2;
		select_camera_number = 2;
	}
	else if (m_camera3_selected.GetCheck())
	{
		m_pAcqDevice = pInspectDlg->m_AcqDevice3;
		m_pBuffer = pInspectDlg->m_Buffers3;
		m_pXfer = pInspectDlg->m_Xfer3;
		select_camera_number = 3;
	}
	else if (m_camera4_selected.GetCheck())
	{
		m_pAcqDevice = pInspectDlg->m_AcqDevice4;
		m_pBuffer = pInspectDlg->m_Buffers4;
		m_pXfer = pInspectDlg->m_Xfer4;
		select_camera_number = 4;
	}

	//获取相机参数
	GetCameraParameter(m_pAcqDevice);
	//获取帧率
	GetFrameRate(m_pXfer);
	//获取图像尺寸
	GetBufferSize(pInspectDlg->m_Buffers1);


}

//设置曝光时间
void CCameraDlg::OnBnClickedButtonSetExposure()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str_edit_exposure_time;
	m_exposure_time.GetWindowTextW(str_edit_exposure_time);
	exposure_time = _ttof(str_edit_exposure_time);

	if (m_camera1_selected.GetCheck())
	{
		if (pInspectDlg->m_AcqDevice1)
		{
			char exposureTime[6];
			CString str_exposure_time;
			pInspectDlg->m_AcqDevice1->GetFeatureValue("ExposureTime", exposureTime, sizeof(exposureTime));
			str_exposure_time = exposureTime;
			double f_time_read = _ttof(str_exposure_time);

			if (abs(exposure_time - f_time_read) > 0.1)
			{
				if (3 < exposure_time && exposure_time < 80)
				{
					pInspectDlg->m_AcqDevice1->SetFeatureValue("ExposureTime", exposure_time);//设定曝光时间
				}
			}
		}

	}
	else	if (m_camera2_selected.GetCheck())
	{
		if (pInspectDlg->m_AcqDevice2)
		{
			char exposureTime[6];
			CString str_exposure_time;
			pInspectDlg->m_AcqDevice2->GetFeatureValue("ExposureTime", exposureTime, sizeof(exposureTime));
			str_exposure_time = exposureTime;
			double f_time_read = _ttof(str_exposure_time);

			if (abs(exposure_time - f_time_read) > 0.1)
			{
				if (3 < exposure_time && exposure_time < 80)
				{
					pInspectDlg->m_AcqDevice2->SetFeatureValue("ExposureTime", exposure_time);//设定曝光时间
				}
			}
		}

	}
	else	if (m_camera3_selected.GetCheck())
	{
		if (pInspectDlg->m_AcqDevice3)
		{
			char exposureTime[6];
			CString str_exposure_time;
			pInspectDlg->m_AcqDevice3->GetFeatureValue("ExposureTime", exposureTime, sizeof(exposureTime));
			str_exposure_time = exposureTime;
			double f_time_read = _ttof(str_exposure_time);

			if (abs(exposure_time - f_time_read) > 0.1)
			{
				if (3 < exposure_time && exposure_time < 80)
				{
					pInspectDlg->m_AcqDevice3->SetFeatureValue("ExposureTime", exposure_time);//设定曝光时间
				}
			}
		}
	}
	else	if (m_camera4_selected.GetCheck())
	{
		if (pInspectDlg->m_AcqDevice4)
		{
			char exposureTime[6];
			CString str_exposure_time;
			pInspectDlg->m_AcqDevice4->GetFeatureValue("ExposureTime", exposureTime, sizeof(exposureTime));
			str_exposure_time = exposureTime;
			double f_time_read = _ttof(str_exposure_time);

			if (abs(exposure_time - f_time_read) > 0.1)
			{
				if (3 < exposure_time && exposure_time < 80)
				{
					pInspectDlg->m_AcqDevice4->SetFeatureValue("ExposureTime", exposure_time);//设定曝光时间
				}
			}
		}
	}
}
