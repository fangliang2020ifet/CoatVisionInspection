//ͼ���ȡ��

#include "stdafx.h"
#include "DeVision.h"
#include "CAcquireImage.h"


CAcquireImage::CAcquireImage()
{
	m_bSystemPause = FALSE;
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

//�Զ������ɼ��豸
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

	for (int i = 0; i < GetServerCount(); i++){
		if (GetResourceCount(i, ResourceAcq) > 0 && GetResourceCount(i, ResourceAcqDevice) == 0) {
			char name[128];
			if (GetServerName(i, name, sizeof(name))) {
				if (strstr(name, "Xtium-CL_MX4_") != NULL) {
					m_vAcquireServerName.push_back(CString(name));
				}
			}
		}
		if (GetResourceCount(i, ResourceAcq) == 0 && GetResourceCount(i, ResourceAcqDevice) == 1) {
			char name[128];
			if (GetServerName(i, name, sizeof(name))) {
				if (strstr(name, "CameraLink_") != NULL) {
					m_vAcquireDeviceServerName.push_back(CString(name));
				}
			}
		}
	}

	return (int)m_vAcquireServerName.size();
}

//ͼ��ɼ�ϵͳ��ʼ��
BOOL CAcquireImage::CameraSystemInitial()
{
	InitialThisClass();

	CString cstrlog;
	int acquire_num = 0;
	if (!m_camera_system_initialled) {
		acquire_num = ScanAcqDevice();
		if (acquire_num == 0) {
			cstrlog.Format(_T("δ���ֲɼ��豸��������·�͵�Դ����: %d"), GetLastError());
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
			return FALSE;
		}
		else {
			acquire_num = InitialAcqDevices();
			if (acquire_num == 0) {
				cstrlog.Format(_T("ͼ��ɼ�ϵͳ��ʼ��ʧ��: %d"), GetLastError());
				::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
				return FALSE;
			}
		}
	}
	m_camera_system_initialled = TRUE;
	cstrlog.Format(_T("ͼ��ɼ�ϵͳ��ʼ���ɹ�: ���� %d"), acquire_num);
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
	return TRUE;
}

//��ʼ���ɼ���
int CAcquireImage::InitialAcqDevices()
{
	int initial_num = 0;
	for(int index = 0; index < (int)m_vAcquireServerName.size(); index++){
		CString cstrAcquireServerName = m_vAcquireServerName.at(index);
		CString cstrAcquireDeviceServerName = m_vAcquireDeviceServerName.at(index);

		CString filename;
		if (!FREE_RUN) filename.Format(L"system\\T_LA_CM_08K08A_00_R_External_Trigger_Board%d.ccf", index + 1);
		else filename.Format(L"system\\T_LA_CM_08K08A_00_R_FreeRun_%d.ccf", index + 1);		

		SapLocation locAcquireServer((CW2A)cstrAcquireServerName.GetBuffer(), 0);
		m_Acq[index] = new SapAcquisition(locAcquireServer, (CW2A)filename.GetBuffer());
		m_Buffer[index] = new SapBufferWithTrash(2, m_Acq[index]);
		m_Xfer[index] = new SapAcqToBuf(m_Acq[index], m_Buffer[index], m_Callback[index], this);
		m_View[index] = new SapView(m_Buffer[index]);

		SapLocation locAcquireDeviceServer((CW2A)cstrAcquireDeviceServerName.GetBuffer(), 0);
		m_AcqDevice[index] = new SapAcqDevice(locAcquireDeviceServer, FALSE);
		m_Feature[index] = new SapFeature(locAcquireDeviceServer);

		//  m_ImageWnd ��Ҫ�Ϳؼ���
		m_pImageWnd[index]->AttachSapView(m_View[index]);

		if (m_Acq[index] && !*m_Acq[index]) m_Acq[index]->Create();
		if (m_AcqDevice[index] && !*m_AcqDevice[index]) m_AcqDevice[index]->Create();
		if (m_Feature[index] && !*m_Feature[index]) m_Feature[index]->Create();
		if (m_Buffer[index] && !*m_Buffer[index]) m_Buffer[index]->Create();
		if (m_View[index] && !*m_View[index]) m_View[index]->Create();
		if (m_Xfer[index] && !*m_Xfer[index]) m_Xfer[index]->Create();

		if (!FREE_RUN) 	m_AcqDevice[index]->SetFeatureValue("TriggerMode", "On");//����ģʽ��		
		else {
			m_AcqDevice[index]->SetFeatureValue("TriggerMode", "Off");//����ģʽ�ر�
			m_AcqDevice[index]->SetFeatureValue("AcquisitionLineRate", SCANE_RATE);//�趨����Ƶ��
		}

		m_pImageWnd[index]->AttachEventHandler(m_pWndHandle);
		m_pImageWnd[index]->CenterImage(true);
		m_pImageWnd[index]->Reset();
		
		initial_num += 1;
	}

	return initial_num;
}

// �忨��Ӳ���˲�
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
			//����ϸ�Ķ������ĵ���ʵ������
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

	//�˲�����С  3*3
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

	//�忨 һ
	if (bStatus && m_Acq[0]->IsImageFilterAvailable()) {
		if (!m_Acq[0]->IsImageFilterEnabled()) {
			m_Acq[0]->EnableImageFilter(1);
		}
		if (!m_Acq[0]->SetImageFilter(0, &buffer))
			bStatus = false;
	}

	//�忨 ��
	if (bStatus && m_Acq[1]->IsImageFilterAvailable()) {
		if (!m_Acq[1]->IsImageFilterEnabled()) {
			m_Acq[1]->EnableImageFilter(1);
		}
		if (!m_Acq[1]->SetImageFilter(0, &buffer))
			bStatus = false;
	}

	//�忨 ��
	if (bStatus && m_Acq[2]->IsImageFilterAvailable()) {
		if (!m_Acq[2]->IsImageFilterEnabled()) {
			m_Acq[2]->EnableImageFilter(1);
		}
		if (!m_Acq[2]->SetImageFilter(0, &buffer))
			bStatus = false;
	}

	//�忨 ��
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


//��ʼ�����ɼ�
BOOL CAcquireImage::Grab()
{
	for (int index = 0; index < (int)m_vAcquireServerName.size(); index++) {
		if (!m_Xfer[index]->Grab()) {
			CString cstrlog;
			cstrlog.Format(L"�豸 %d �ɼ�ʧ�ܣ�Grab Error %d", index + 1, GetLastError());
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
			return FALSE;
		}
	}
	return TRUE;
}

//�ɼ�һ֡
BOOL CAcquireImage::Snap()
{
	for (int index = 0; index < (int)m_vAcquireServerName.size(); index++) {
		if (!m_Xfer[index]->Snap()) {
			CString cstrlog;
			cstrlog.Format(L"�豸 %d �ɼ�ʧ�ܣ�Snap Error %d", index + 1, GetLastError());
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
			return FALSE;
		}
	}
	return TRUE;
}

//ֹͣ�ɼ�
BOOL CAcquireImage::Freeze()
{
	for (int index = 0; index < (int)m_vAcquireServerName.size(); index++) {
		if (m_Xfer[index]->Freeze()) {
			if (CAbortDlg(AfxGetApp()->m_pMainWnd, m_Xfer[index]).DoModal() != IDOK)
				m_Xfer[index]->Abort();
			return FALSE;
		}
	}
	return TRUE;
}

//�ɼ���λ
void CAcquireImage::ResetAcquire()
{
	for (int i = 0; i < 4; i++) {
		m_arrayFrameCount[i] = 0;
		m_arrayTrashCount[i] = 0;
		m_arrayBufferIndex[i] = 0;
	}

	return;
}

//���Ͳɼ�����������
void CAcquireImage::DropAcquireSpeed(int k)
{
	for (int index = 0; index < (int)m_vAcquireServerName.size(); index++) {
		if (m_Acq[index]->SetParameter(CORACQ_PRM_SHAFT_ENCODER_DROP, k, TRUE)) {
			CString cstrlog;
			cstrlog.Format(L"�豸 %d �ѽ��Ͳɼ�Ƶ��: %d", index + 1, k);
			::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
		}
	}
}

//����������ٶ�
float CAcquireImage::CalculateEncoderSpeed()
{
	//����ʱ���    
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

//������֡��
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
	cfilename.Format(_T("%d"), t);   //��ʽ������ʱ��
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

//�ɼ����ص�����
void CAcquireImage::AcqCallback1(SapXferCallbackInfo *pInfo)
{
	CAcquireImage *pThis = (CAcquireImage*)pInfo->GetContext();
	//������λ�ȡ���ɼ��豸����Ż�����
	int INDEX = 0;
	int buffer_index = pThis->m_arrayBufferIndex[INDEX];
	if (pInfo->IsTrash())
		pThis->m_arrayTrashCount[INDEX] += 1;
	else {
		if (pThis->SHOW_BUFFER) pThis->m_View[INDEX]->Show();
		if (!pThis->m_bSystemPause) {
			HImage ho_image;
			pThis->GenerateHImage(pThis->m_Buffer[INDEX], buffer_index, ho_image);
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
		}

		if (buffer_index == 0)      pThis->m_arrayBufferIndex[INDEX] = 1;
		else if (buffer_index == 1) pThis->m_arrayBufferIndex[INDEX] = 0;
	}
}


void CAcquireImage::AcqCallback2(SapXferCallbackInfo *pInfo)
{
	CAcquireImage *pThis = (CAcquireImage*)pInfo->GetContext();
	//������λ�ȡ���ɼ��豸����Ż�����
	int INDEX = 1;
	int buffer_index = pThis->m_arrayBufferIndex[INDEX];
	if (pInfo->IsTrash())
		pThis->m_arrayTrashCount[INDEX] += 1;
	else {
		if (pThis->SHOW_BUFFER) pThis->m_View[INDEX]->Show();
		if (!pThis->m_bSystemPause) {
			HImage ho_image;
			pThis->GenerateHImage(pThis->m_Buffer[INDEX], buffer_index, ho_image);
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
		}

		if (buffer_index == 0)      pThis->m_arrayBufferIndex[INDEX] = 1;
		else if (buffer_index == 1) pThis->m_arrayBufferIndex[INDEX] = 0;
	}
}


void CAcquireImage::AcqCallback3(SapXferCallbackInfo *pInfo)
{
	CAcquireImage *pThis = (CAcquireImage*)pInfo->GetContext();
	//������λ�ȡ���ɼ��豸����Ż�����
	int INDEX = 2;
	int buffer_index = pThis->m_arrayBufferIndex[INDEX];
	if (pInfo->IsTrash())
		pThis->m_arrayTrashCount[INDEX] += 1;
	else {
		if (pThis->SHOW_BUFFER) pThis->m_View[INDEX]->Show();
		if (!pThis->m_bSystemPause) {
			HImage ho_image;
			pThis->GenerateHImage(pThis->m_Buffer[INDEX], buffer_index, ho_image);
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
		}

		if (buffer_index == 0)      pThis->m_arrayBufferIndex[INDEX] = 1;
		else if (buffer_index == 1) pThis->m_arrayBufferIndex[INDEX] = 0;
	}
}


void CAcquireImage::AcqCallback4(SapXferCallbackInfo *pInfo)
{
	CAcquireImage *pThis = (CAcquireImage*)pInfo->GetContext();
	//������λ�ȡ���ɼ��豸����Ż�����
	int INDEX = 3;
	int buffer_index = pThis->m_arrayBufferIndex[INDEX];
	if (pInfo->IsTrash())
		pThis->m_arrayTrashCount[INDEX] += 1;
	else {
		if (pThis->SHOW_BUFFER) pThis->m_View[INDEX]->Show();
		if (!pThis->m_bSystemPause) {
			HImage ho_image;
			pThis->GenerateHImage(pThis->m_Buffer[INDEX], buffer_index, ho_image);
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
		}

		if (buffer_index == 0)      pThis->m_arrayBufferIndex[INDEX] = 1;
		else if (buffer_index == 1) pThis->m_arrayBufferIndex[INDEX] = 0;
	}
}
