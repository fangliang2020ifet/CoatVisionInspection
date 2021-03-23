

#include "stdafx.h"
#include "DeVision.h"
#include "CImageProcessing.h"
#include "Log.h"
#include <winbase.h>
#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>




CImageProcessing::CImageProcessing(int ThreadNum, int Distribution, int FilterSize, float RadiusMin, float RadiusMax)
	: m_threadnum(ThreadNum), m_k_normal_distribution(Distribution), 
	m_median_filter_size(FilterSize), m_fMin_Radius(RadiusMin), m_fMax_Radius(RadiusMax)
{
	SYSTEM_PAUSE = FALSE;
	SAVE_REFERENCE_IMAGE = FALSE;
	m_referenceImage_OK = FALSE;
	m_strPath = "D:\\覴ü�����ݼ�¼\\2覴�ͼ���¼\\";
	m_nTotalListNumber = 0;
	m_unImageIndex = 0;

	m_nrankMethod = 0;
	m_frankValue1 = 10.0f;
	m_frankValue2 = 20.0f;
	m_frankValue3 = 50.0f;

	m_nCutBorderValue = 100;            //   100����Լ 5mm
	m_nCutBorderStatue = 0;

	InitializeCriticalSection(&m_csCalculateThread);
	InitializeCriticalSection(&m_csDefImgList1);
	InitializeCriticalSection(&m_csDefImgList2);
	InitializeCriticalSection(&m_csDefImgList3);
	InitializeCriticalSection(&m_csDefImgList4);
	InitializeCriticalSection(&m_csDefImgList5);

	InitialImageProcess();
}

CImageProcessing::~CImageProcessing()
{
	DeleteCriticalSection(&m_csCalculateThread);
	DeleteCriticalSection(&m_csDefImgList1);
	DeleteCriticalSection(&m_csDefImgList2);
	DeleteCriticalSection(&m_csDefImgList3);
	DeleteCriticalSection(&m_csDefImgList4);
	DeleteCriticalSection(&m_csDefImgList5);

	try	{
		m_listAcquiredImage.clear();
	}
	catch (...) {}

}

void CImageProcessing::ClearThisClass()
{
	m_unImageIndex = 1;
	m_nTotalListNumber = 1;

	m_listAcquiredImage.clear();
	m_listDftInfo.clear();
	m_listDftImg.clear();
}


void CImageProcessing::HalconInitAOP()
{
	HTuple hv_AOP, hv_method;
	//   nil     threshold     linear     mlp  
	hv_method = "mlp";

	//���ԣ��ֶ�AOP�Ż�
	HalconCpp::SetSystem("parallelize_operators", "true");
	HalconCpp::GetSystem("parallelize_operators", &hv_AOP);
	HalconCpp::OptimizeAop("median_image", "byte", "no_file", ((HTuple("file_mode").Append("model")).Append("parameters")),
		((HTuple("nil").Append(hv_method)).Append("false")));
	HalconCpp::OptimizeAop("sub_image", "byte", "no_file", ((HTuple("file_mode").Append("model")).Append("parameters")),
		((HTuple("nil").Append(hv_method)).Append("false")));
	HalconCpp::OptimizeAop("add_image", "byte", "no_file", ((HTuple("file_mode").Append("model")).Append("parameters")),
		((HTuple("nil").Append(hv_method)).Append("false")));
	HalconCpp::OptimizeAop("threshold", "byte", "no_file", ((HTuple("file_mode").Append("model")).Append("parameters")),
		((HTuple("nil").Append(hv_method)).Append("false")));
}

void CImageProcessing::HalconOpenGPU(HTuple &hv_DeviceHandle)
{
	//����3:GPU���٣�֧��GPU���ٵ�����Halcon19.11��82��
	//GPU�������ȴ�CPU�н����ݿ�����GPU�ϴ���������ɺ��ٽ����ݴ�GPU������CPU�ϡ���CPU��GPU�ٴ�GPU��CPU��Ҫ����ʱ��ġ�
	//GPU����һ�����������AOP�����ٶȿ���?��һ��!���ȡ�����Կ��ĺû�.
	HTuple  hv_DeviceIdentifiers, hv_i, hv_Nmae;

	//dev_update_off();


	HalconCpp::QueryAvailableComputeDevices(&hv_DeviceIdentifiers);
	hv_DeviceHandle = 0;
	{
		HTuple end_val34 = (hv_DeviceIdentifiers.TupleLength()) - 1;
		HTuple step_val34 = 1;
		for (hv_i = 0; hv_i.Continue(end_val34, step_val34); hv_i += step_val34)
		{
			HalconCpp::GetComputeDeviceInfo(HTuple(hv_DeviceIdentifiers[hv_i]), "name", &hv_Nmae);
			//     GeForce GTX 1050 Ti      Intel(R) HD Graphics 630 
			if (0 != (hv_Nmae == HTuple("GeForce GTX 1050 Ti")))
			{
				HalconCpp::OpenComputeDevice(HTuple(hv_DeviceIdentifiers[hv_i]), &hv_DeviceHandle);
				break;
			}
		}
	}

	if (0 != (hv_DeviceHandle != 0))
	{
		HalconCpp::SetComputeDeviceParam(hv_DeviceHandle, "asynchronous_execution", "false");
		HalconCpp::InitComputeDevice(hv_DeviceHandle, "median_image");
		//init_compute_device (DeviceHandle, 'sub_image')
		//init_compute_device (DeviceHandle, 'add_image')
		HalconCpp::ActivateComputeDevice(hv_DeviceHandle);
	}

	//HalconCpp::DeactivateComputeDevice(hv_DeviceHandle);
}

void CImageProcessing::InitialImageProcess()
{
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return;

	m_hFinishedProcess = CreateEventA(NULL, FALSE, FALSE, NULL);

	return;
}

BOOL CImageProcessing::BeginProcess()
{
	// Halcon �ٶ��Ż�
	HalconInitAOP();

	bool state;
	std::string str_path;
	if (!GetSavePath(str_path)) {
		cstrlog.Format(_T("����Ŀ¼������"));
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		return FALSE;
	}

	//���������߳�
	m_unImageIndex = 0;
	state = m_ManageThread = AfxBeginThread(ManageThread, this);
	if (!state) {
		cstrlog.Format(_T("�����̴߳���ʧ��"));
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		return FALSE;
	}

	for (int index = 0; index < m_threadnum; index++) {
		state = m_CalculateThread[index] = AfxBeginThread(m_threadCallBack[index], this);
		if (state) {
			cstrlog.Format(_T("ͼ�����߳�: %d, �����ɹ�"), index + 1);
			::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
		}
		else {
			cstrlog.Format(_T("ͼ�����߳�: %d, ����ʧ��"), index + 1);
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
			return FALSE;
		}
	}

	ResetEvent(m_hFinishedProcess);
	StopManage_Event.ResetEvent();
	//CalculateThread_1_StoppedEvent.ResetEvent();
	CalculateThread_2_StoppedEvent.ResetEvent();
	CalculateThread_3_StoppedEvent.ResetEvent();
	CalculateThread_4_StoppedEvent.ResetEvent();
	CalculateThread_5_StoppedEvent.ResetEvent();	

	return TRUE;
}

void CImageProcessing::StopManageThread()
{
	// �߳�ֹͣ
	StopManage_Event.SetEvent();

	StopCalculateThreads();
}

void CImageProcessing::StopCalculateThreads()
{
	for (int index = 0; index < m_threadnum; index++)
		m_bThreadAlive[index] = FALSE;

	if (hv_GPU_Handle.Length() > 0)
		HalconCpp::DeactivateComputeDevice(hv_GPU_Handle);

	m_listAcquiredImage.clear();
}

void CImageProcessing::RestartProcess()
{

}

BOOL CImageProcessing::IsThreadsAlive()
{
	for (int index = 0; index < m_threadnum; index++) {
		if (m_bThreadAlive[index] = TRUE)
			return TRUE;
	}
	return FALSE;
}

BOOL CImageProcessing::IsFileExist(const std::string &filename)
{
	std::ifstream fin(filename);
	if (!fin)
		return FALSE;
	else
		return TRUE;
}

BOOL CImageProcessing::IsPathExist(const std::string &pathname)
{
	DWORD ftyp = GetFileAttributesA(pathname.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

BOOL CImageProcessing::LoadRefImage(std::string folder_path)
{
	if (!IsPathExist(folder_path)) {
		CString cstr = L"�ο�ͼ��Ŀ¼������";
		SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}

	//��ȡ�ο�ͼ��1
	if (1) {
		std::string ref_image_name1 = "reference_imageX.png";
		HTuple hv_ref_image_name1 = (HTuple)((folder_path + ref_image_name1).c_str());
		if (!IsFileExist(folder_path + ref_image_name1)) {
			CString cstr = L"�ο�ͼ��1������";
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return FALSE;
		}
		HImage img1;
		ReadImage(&img1, hv_ref_image_name1);
		//HalconCpp::MedianImage(img1, &m_hi_ref1, "circle", 1, "mirrored");
		//��˹�˲�
		//HalconCpp::GaussFilter(img1, &m_hi_ref1, 5);
		m_hi_ref = img1;

		HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
		HTuple  hv_Deviation, hv_StandardDeviation;
		HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
		HalconCpp::GetImageSize(img1, &hv_Width, &hv_Height);
		HalconCpp::MedianImage(img1, &ho_ImageMedian, "square", m_median_filter_size, "mirrored");
		//���� ȫ 0 ֵͼ��
		HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
		HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
		HTuple end_val8 = hv_Width - 1;
		HTuple step_val8 = 1;
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)		{
			//����2�����Ͻǵ���У�����3�����Ͻǵ���У�����4�����½ǵ���У�����5�����½ǵ����
			HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
			//��ֵ/����/��׼��
			HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
			HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
			HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
			HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
				((m_k_normal_distribution * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
				"fill");
		}
		m_hi_average = ho_ImageAverage;
		m_hi_deviation = ho_ImageDeviation;
	}
	return TRUE;
}

BOOL CImageProcessing::GetRefImgWithoutBouder(const char* imgname)
{
	//��ȡ�ο�ͼ��1
	if (1) {
		//std::string ref_image_name1 = "reference_imageZ.bmp";
		HTuple hv_ref_image_name1 = (HTuple)((CA2W)imgname);
		HImage img1;
		HalconCpp::ReadImage(&img1, hv_ref_image_name1);

		// �жϹ�Դ�Ƿ��
		HObject ho_regionTotal;
		HTuple hv_meanTotal, hv_deviationTotal;
		HalconCpp::Threshold(img1, &ho_regionTotal, 0, 255);
		HalconCpp::Intensity(ho_regionTotal, img1, &hv_meanTotal, &hv_deviationTotal);
		m_hvGrayMeanValue = hv_meanTotal;
		m_hvGrayDeviationValue = hv_deviationTotal;
		if (hv_meanTotal < 32) {
			CString cstr = L"��Դδ������ͼ�����";
			::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return FALSE;
		}			

		HObject ho_ImagePart;
		m_nCutBorderStatue = ifCutBouder(img1, ho_ImagePart, m_hoBorderRegion);
		if (m_nCutBorderStatue > 0) {
			img1 = ho_ImagePart;
		}

		m_hi_ref = img1;
		HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
		HTuple  hv_Deviation, hv_StandardDeviation;
		HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;

		HalconCpp::GetImageSize(img1, &hv_Width, &hv_Height);
		HalconCpp::MedianImage(img1, &ho_ImageMedian, "square", m_median_filter_size, "mirrored");
		//���� ȫ 0 ֵͼ��
		HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
		HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
		HTuple end_val8 = hv_Width - 1;
		HTuple step_val8 = 1;
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8) {
			//����2�����Ͻǵ���У�����3�����Ͻǵ���У�����4�����½ǵ���У�����5�����½ǵ����
			HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
			//��ֵ/����/��׼��
			HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
			HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
			HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
			HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
				((m_k_normal_distribution * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
				"fill");
		}
		m_hi_average = ho_ImageAverage;
		m_hi_deviation = ho_ImageDeviation;
	}
	return TRUE;
}

int CImageProcessing::ifCutBouder(HObject src, HObject &dst, HObject &region)
{
	int state = 0;

	//�Զ�ȥ���߽磺��ȥƽ������ֵ, ����б߽��򷵻� true
	HObject ho_Region, ho_ImageMean, ho_ImageSub, ho_Region2, ho_RegionMoved, ho_RectanglePart;
	HTuple hv_Width, hv_Height, hv_Mean, hv_Deviation, hv_Row1, hv_Row2, hv_Column1, hv_Column2;
	HTuple hv_Value, hv_TopLeft, hv_Border;

	HalconCpp::GetImageSize(src, &hv_Width, &hv_Height);
	HalconCpp::Threshold(src, &ho_Region, 0, 255);
	HalconCpp::Intensity(ho_Region, src, &hv_Mean, &hv_Deviation);
	HalconCpp::GenImageConst(&ho_ImageMean, "byte", hv_Width, hv_Height);
	HalconCpp::OverpaintRegion(ho_ImageMean, ho_Region, hv_Mean, "fill");
	//  8 Ϊ���õ��˲�ֵ
	HalconCpp::SubImage(ho_ImageMean, src, &ho_ImageSub, 1, 8);
	HalconCpp::Threshold(ho_ImageSub, &ho_Region2, 1, 255);
	HObject ho_ConnectedRegion;
	HalconCpp::Connection(ho_Region2, &ho_ConnectedRegion);
	HObject ho_SelectedRegion;
	HalconCpp::SelectShape(ho_ConnectedRegion, &ho_SelectedRegion, "height", "and", hv_Height, 99999);
	HalconCpp::RegionFeatures(ho_SelectedRegion, "width", &hv_Value);
	if (0 != (hv_Value == hv_Width)){
		dst = src;
		region = ho_Region;
		return state;
	}

	//�ж������ȥ�߻����Ҳ�
	HalconCpp::RegionFeatures(ho_SelectedRegion, "column1", &hv_TopLeft);
	hv_Border = (HTuple)m_nCutBorderValue;      
	if (0 != (hv_TopLeft > 0)){
		HalconCpp::MoveRegion(ho_SelectedRegion, &ho_RegionMoved, 0, hv_Border);
		state = 1;
	}
	else{
		HalconCpp::MoveRegion(ho_SelectedRegion, &ho_RegionMoved, 0, -hv_Border);
		state = 2;
	}

	//��������ϳɾ���
	HalconCpp::SmallestRectangle1(ho_RegionMoved, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	HalconCpp::GenRectangle1(&ho_RectanglePart, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	//��ԭͼ���вü�
	HalconCpp::ReduceDomain(src, ho_RectanglePart, &region);
	HalconCpp::CropDomain(region, &dst);

	return state;
}

//��ͼ����ص��ڴ����
void CImageProcessing::LoadImageToQueue()
{
	for (int index = 0; index < m_threadnum; index++) {
		m_listAcquiredImage.push_back(m_hi_test);
	}
}

BOOL CImageProcessing::LoadOneImageToQueue(std::string folder_path, int next_number)
{
	//��ȡ����ͼ��
	folder_path = folder_path + "0";
	HObject ho_image;
	HTuple hv_image_name;
	std::string image_name = "";
	if (next_number < 10) {
		image_name = "0" + std::to_string(next_number);
		image_name = image_name + ".bmp";
	}
	else {
		image_name = std::to_string(next_number) + ".bmp";
	}

	hv_image_name = (HTuple)((folder_path + image_name).c_str());
	if (!IsFileExist(folder_path + image_name)) {
		Win::log("����ͼ��Ϊ��\n����ͼ�������Ƿ��001��ʼ");
		return FALSE;
	}
	ReadImage(&ho_image, hv_image_name);
	m_listAcquiredImage.push_back(ho_image);

	return TRUE;
}

BOOL CImageProcessing::LoadSingleImage(const char* imgname)
{
	HTuple hv_image_name1, hv_image_name2, hv_image_name3, hv_image_name4;
	hv_image_name1 = (HTuple)((CA2W)imgname);
	//ReadImage(&ho_test1, hv_image_name1);
	m_hi_test.ReadImage(hv_image_name1);

	return TRUE;
}

HObject CImageProcessing::CopyHobject(HObject ho_image)
{
	HObject copy;
	CopyImage(ho_image, &copy);

	return copy;
}

//��ȡ�����̵߳Ķ��д�С
int CImageProcessing::CheckTotalListSize()
{
	size_t list_size = 0;

	list_size = m_listAcquiredImage.size();

	return (int)list_size;
}

//�ӱ��ؼ��زο�ͼ��
BOOL CImageProcessing::LoadDefaultRefAndDevImage(std::string path)
{
	if (path.empty())
		return FALSE;

	std::string ref_image_name = "reference_image.bmp";
	std::string dev_image_name = "dev.bmp";

	if (!IsPathExist(path)) {
		CString cstr = L"��ȡͼ��ʧ�ܣ�Ĭ�ϲο�ͼ��Ŀ¼������";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}
	else {
		if (!IsFileExist(path + ref_image_name) || !IsFileExist(path + dev_image_name)) {
			CString cstr = L"��ȡͼ��ʧ�ܣ�Ĭ�ϲο�ͼ��1������";
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return FALSE;
		}
	}

	HTuple hv_ref_image_name = (HTuple)((path + ref_image_name).c_str());
	HTuple hv_dev_image_name = (HTuple)((path + dev_image_name).c_str());
	HalconCpp::ReadImage(&m_hi_average, hv_ref_image_name);
	HalconCpp::ReadImage(&m_hi_deviation, hv_dev_image_name);
	return TRUE;
}

//���ɲο�ͼ��
BOOL CImageProcessing::GenerateReferenceImage(HImage &hi_average, HImage &hi_deviation)
{
	HImage result, tempimg;
	if (m_listAcquiredImage.size() >= 4) {
		//for(int index=0;index<5;index++)
		m_listAcquiredImage.pop_front();

		for (int i = 0; i < 3; i++) {
			if (i == 0) {
				result = m_listAcquiredImage.front();
				m_listAcquiredImage.pop_front();
				continue;
			}
			else {
				tempimg = m_listAcquiredImage.front();
				m_listAcquiredImage.pop_front();
				HalconCpp::AddImage(result, tempimg, &result, 0.5, 0);
			}
		}
	}
	else
		return FALSE;

	// �жϹ�Դ�Ƿ��
	HObject ho_regionTotal;
	HTuple hv_meanTotal, hv_deviationTotal;
	HalconCpp::Threshold(result, &ho_regionTotal, 0, 255);
	HalconCpp::Intensity(ho_regionTotal, result, &hv_meanTotal, &hv_deviationTotal);
	m_hvGrayMeanValue = hv_meanTotal;
	m_hvGrayDeviationValue = hv_deviationTotal;
	if (hv_meanTotal < 32) {
		CString cstr = L"��Դδ������ͼ�����";
		::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}

	// ��Ⲣ�б�
	HObject ho_ImagePart;
	m_nCutBorderStatue = ifCutBouder(result, ho_ImagePart, m_hoBorderRegion);
	if (m_nCutBorderStatue > 0) {
		result = ho_ImagePart;
	}

	m_hi_ref = result;
	HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
	HTuple  hv_Deviation, hv_StandardDeviation;
	HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
	HalconCpp::GetImageSize(result, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(result, &ho_ImageMedian, "square", m_median_filter_size, "mirrored");
	//���� ȫ 0 ֵͼ��
	HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
	HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
	HTuple end_val8 = hv_Width - 1;
	HTuple step_val8 = 1;
	for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8){
		//����2�����Ͻǵ���У�����3�����Ͻǵ���У�����4�����½ǵ���У�����5�����½ǵ����
		HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
		//��ֵ/����/��׼��
		HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
		HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
		HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
		HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
			((m_k_normal_distribution * hv_StandardDeviation).TupleConcat(255)).TupleMin(), "fill");
	}
	hi_average = ho_ImageAverage;
	hi_deviation = ho_ImageDeviation;

	return TRUE;
}

//�жϲο�ͼ�񣺾�ֵͼ�ͷ���ͼ���Ƿ��Ѿ�����
BOOL CImageProcessing::CheckReferenceImageAvilable()
{
	if (!m_hi_average.IsInitialized())	return FALSE;
	if (!m_hi_deviation.IsInitialized())	return FALSE;

	return TRUE;
}

//����ȥ���ڱߵ�ͼ��
int CImageProcessing::ProduceReferenceImage1(HImage hi_ref1, HImage hi_ref2)
{
	HTuple hv_width_ref1, hv_height_ref1, hv_width_ref2, hv_height_ref2;
	HalconCpp::GetImageSize(hi_ref2, &hv_width_ref2, &hv_height_ref2);
	if (hv_width_ref2 == 0)
		return -1;

	// Local iconic variables
	HObject  ho_Image, ho_Rectangle, ho_Region, ho_ConnectedRegions, ho_SelectedRegions;
	HObject  ho_ImageReduced, ho_ImagePart;
	HTuple  hv_Width1, hv_Height1, hv_Mean, hv_Deviation;

	ho_Image = hi_ref1;
	GetImageSize(ho_Image, &hv_Width1, &hv_Height1);
	if (hv_Width1 == 0)
		return -1;

	//ѡȡ�Ҳ��Ե 256*256 ��С�������ڼ�������ƽ��ֵ
	HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_Width1 - 256, 255, hv_Width1);
	HalconCpp::Intensity(ho_Rectangle, ho_Image, &hv_Mean, &hv_Deviation);
	//��ƽ������ֵ�� 0.8 ��������ֵ��
	HalconCpp::Threshold(ho_Image, &ho_Region, hv_Mean*0.8, 255);
	HalconCpp::Connection(ho_Region, &ho_ConnectedRegions);
	//��������ѡ��ѡȡ��Ч����
	HalconCpp::SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "height", "and",
		hv_height_ref2 - 1000, hv_height_ref2);
	//����ͼ��Ķ�����(��ԭͼѡ�������򣬵�ͼ��Ĵ�С����)
	HalconCpp::ReduceDomain(ho_Image, ho_SelectedRegions, &ho_ImageReduced);
	//��������ԻҶ�ͼ����м���
	HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
	HalconCpp::GetImageSize(ho_ImagePart, &hv_width_ref1, &hv_height_ref1);

	// �� 32 ���ڱܿ���Ե���ر仯�ϴ������
	if (hv_width_ref1 != hv_width_ref2)
		return std::abs((int)hv_width_ref2 - (int)hv_width_ref1) + 32;
	else
		return 0;
}

int CImageProcessing::ProduceReferenceImage4(HImage hi_ref4, HImage hi_ref3)
{
	HTuple hv_width_ref1, hv_height_ref1, hv_width_ref2, hv_height_ref2;
	HalconCpp::GetImageSize(hi_ref3, &hv_width_ref2, &hv_height_ref2);
	if (hv_width_ref2 == 0)
		return -1;

	// Local iconic variables
	HObject  ho_Image, ho_Rectangle, ho_Region, ho_ConnectedRegions, ho_SelectedRegions;
	HObject  ho_ImageReduced, ho_ImagePart;
	HTuple  hv_Width1, hv_Height1, hv_Mean, hv_Deviation;

	ho_Image = hi_ref4;
	HalconCpp::GetImageSize(ho_Image, &hv_Width1, &hv_Height1);
	if (hv_Width1 == 0)
		return -1;

	//ѡȡ����Ե 256*256 ��С�������ڼ�������ƽ��ֵ
	HalconCpp::GenRectangle1(&ho_Rectangle, 0, 0, 255, 255);
	HalconCpp::Intensity(ho_Rectangle, ho_Image, &hv_Mean, &hv_Deviation);
	//��ƽ������ֵ�� 0.8 ��������ֵ��
	HalconCpp::Threshold(ho_Image, &ho_Region, hv_Mean*0.8, 255);
	HalconCpp::Connection(ho_Region, &ho_ConnectedRegions);
	//��������ѡ��ѡȡ��Ч����
	HalconCpp::SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "height", "and",
		hv_height_ref2 - 1000, hv_height_ref2);
	//����ͼ��Ķ�����(��ԭͼѡ�������򣬵�ͼ��Ĵ�С����)
	HalconCpp::ReduceDomain(ho_Image, ho_SelectedRegions, &ho_ImageReduced);
	//��������ԻҶ�ͼ����м���
	HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
	HalconCpp::GetImageSize(ho_ImagePart, &hv_width_ref1, &hv_height_ref1);

	// �� 32 ���ڱܿ���Ե���ر仯�ϴ������
	if (hv_width_ref1 != hv_width_ref2)
		return std::abs((int)hv_width_ref2 - (int)hv_width_ref1) + 32;
	else
		return 0;
}

void CImageProcessing::SaveReferenceImage(const char* filename)
{
	HTuple hv_name;
	hv_name = (HTuple)filename;
	if (!m_hi_ref.IsInitialized())
		return;

	HalconCpp::WriteImage(m_hi_ref, "png", 0, hv_name);
	return;
}

BOOL CImageProcessing::GetSavePath(std::string &path)
{
	path = m_strPath;

	//��鱣��·���Ƿ����
	if (!IsPathExist(path)) {
		Win::log("ͼƬ����·��������");
		CString cstr = L"ͼ����ʧ�ܣ�ͼƬ����·��������";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}
	else
		return TRUE;
}

//�ָ���覴�ͼ����
DefectType CImageProcessing::LocateDefectPosition(int camera_number, HObject ho_selectedregion,
	HTuple hv_Number, HTuple hv_colunm_origin, HObject ho_image)
{
	DefectType dtype;   //�����⵽��覴���Ϣ

	HTuple hv_total_Row = 0;
	HTuple hv_total_Column = 0;
	HTuple hv_total_Radius = 0;
	HTuple hv_total_Mean = 0;
	{
		HTuple end_val40 = hv_Number;
		HTuple step_val40 = 1;
		HObject ho_objectselected;
		for (HTuple hv_i = 1; hv_i.Continue(end_val40, step_val40); hv_i += step_val40)
		{
			SelectObj(ho_selectedregion, &ho_objectselected, hv_i);

			HTuple hv_row, hv_column, hv_radius;
			HalconCpp::SmallestCircle(ho_objectselected, &hv_row, &hv_column, &hv_radius);
			//�������ж��覴�����Ҫ��λ�ý���ƽ��
			hv_total_Row += hv_row;
			hv_total_Column += hv_column;
			hv_total_Radius += hv_radius;

			//��������ƽ��ֵ
			HTuple hv_mean, hv_deviation;
			HalconCpp::Intensity(ho_objectselected, ho_image, &hv_mean, &hv_deviation);
			hv_total_Mean += hv_mean;
		}
	}
	HTuple hv_average_Row = hv_total_Row / hv_Number;
	HTuple hv_average_Column = hv_total_Column / hv_Number;
	HTuple hv_average_Radius = hv_total_Radius / hv_Number;
	HTuple hv_average_Mean = hv_total_Mean / hv_Number;

	//����Բ�Ķȷ���...
	HTuple hv_circularity;
	HalconCpp::Circularity(ho_selectedregion, &hv_circularity);
	int circularity = hv_circularity.TupleInt();

	//�����������
	HTuple hv_area, hv_row, hv_column;
	HalconCpp::AreaCenter(ho_selectedregion, &hv_area, &hv_row, &hv_column);
	dtype.area = hv_area.TupleInt();

	//ȱ���ڴ�ͼ�е�ʵ������
	if (0 != (hv_average_Row < 127))
	{
		hv_average_Row = 127;
	}
	else if (0 != (hv_average_Row > IMAGE_WIDTH - 127))
	{
		hv_average_Row = IMAGE_WIDTH - 127;
	}
	if (0 != (hv_average_Column < 127))
	{
		hv_average_Column = 127;
	}
	else if (0 != (hv_average_Column > IMAGE_HEIGHT - 127))
	{
		hv_average_Column = IMAGE_HEIGHT - 127;
	}

	//覴���Ϣд��
	dtype.center_x = (hv_average_Column.TupleInt() + IMAGE_WIDTH * (camera_number - 1))*HORIZON_PRECISION;     //��λ������
	//dtype.center_y = hv_average_Row.TupleInt();
	dtype.area = 0.255f;
	dtype.circle_radius = hv_average_Radius.TupleInt();
	dtype.pixel_value = hv_average_Mean.TupleInt();
	//�������λ��, ��λ����
	dtype.absolute_position = hv_average_Row.TupleInt() * VERTICAL_PRECISION / 1000.0f;
	//覴÷���
	if (0 < dtype.pixel_value && dtype.pixel_value <= 92) {
		if (dtype.area < 100) {
			dtype.type = 0;
		}
		else
			dtype.type = 2;
	}
	else if (92 < dtype.pixel_value && dtype.pixel_value <= 168) {
		if (dtype.area < 100) {
			dtype.type = 0;
		}
		else
			dtype.type = 1;
	}
	else
		dtype.type = 2;

	//��ʽ���ļ���
	char cpos[16];
	sprintf_s(cpos, "%.3f", dtype.absolute_position);
	//HTuple hv_position = (HTuple)(std::to_string(dtype.absolute_position).c_str());
	HTuple hv_position = (HTuple)cpos;
	char cX[16];
	sprintf_s(cX, "%.3f", dtype.center_x);
	HTuple hv_X = (HTuple)cX;
	char cradius[16];
	sprintf_s(cradius, "%.3f", dtype.circle_radius);
	HTuple hv_radius = (HTuple)cradius;
	char carea[16];
	sprintf_s(carea, "%.3f", dtype.area);
	HTuple hv_harea = (HTuple)carea;
	HTuple hv_kind = (HTuple)dtype.type;

	//����覴�����ͼƬ
	HObject ho_rectangle, ho_ImageReduced, ho_ImageCroped;
	HalconCpp::GenRectangle1(&ho_rectangle, hv_average_Row - 127, hv_average_Column - 127, hv_average_Row + 128, hv_average_Column + 128);

	HalconCpp::ReduceDomain(ho_image, ho_rectangle, &ho_ImageReduced);
	HalconCpp::CropDomain(ho_ImageReduced, &ho_ImageCroped);

	//覴�ͼ�������
	HTuple hv_path = (HTuple)m_strPath.c_str();
	HTuple hv_img_name = "P" + hv_position
		+ "_X" + hv_X
		+ "_R" + hv_radius
		+ "_A" + hv_harea
		+ "_K" + hv_kind;

	SaveDefectImage(ho_ImageCroped, hv_path + hv_img_name);

	//std::string file_name = hv_img_name.S();

	//m_vFileName.push_back(file_name);


	return dtype;
}


DefectType CImageProcessing::LocateDefectPosition(int camera_number, HObject ho_selectedregion)
{
	DefectType dtype;   //�����⵽��覴���Ϣ


	return dtype;
}

//覴õȼ��ж�
int CImageProcessing::RankDivide(DefectType dtype)
{
	int rank = 0;
	float area = dtype.area / (HORIZON_PRECISION * VERTICAL_PRECISION);
	if (area < 5.0f)
		rank = 0;
	else if (5.0f <= area && area < 20.0f)
		rank = 1;
	else if (20.0f <= area && area < 100.0f)
		rank = 2;
	else if (100.0f <= area && area < 500.0f)
		rank = 3;
	else
		rank = 4;

	return rank;
}

unsigned short int CImageProcessing::RankDivide(float area, float radius, float contlength)
{
	unsigned short int rank = 0;

	// ���������覴õĵȼ�����, �����areaΪ����ֵ
	//float value = area * (HORIZON_PRECISION * VERTICAL_PRECISION);   
	float value = 0.0f;
	switch (m_nrankMethod)
	{
	case 0:
		value = area;
		break;
	case 1:
		value = radius;
		break;
	case 2:
		value = contlength;
		break;
	default:
		value = area;
		break;
	}

	if (value < 10.0f)
		rank = 0;
	else if (10.0f <= value && value < 20.0f)
		rank = 1;
	else if (20.0f <= value && value < 50.0f)
		rank = 2;
	else if (50.0f <= value)
		rank = 3;

	return rank;
}

//�����㷨��������    SVM
unsigned short int CImageProcessing::ImageClassification(float radius)
{
	unsigned short int kind = 0;
	//HObject ho_region;
	//HalconCpp::AutoThreshold(ho_img, &ho_region, 3);

	//HTuple hv_RowCircle, hv_ColumnCircle, hv_Radius;
	//HalconCpp::SmallestCircle(ho_img, &hv_RowCircle, &hv_ColumnCircle, &hv_Radius);
	//float radius = (float)hv_Radius.D();

	if (0 <= radius && radius < 16)
		kind = 0;
	else if (16 <= radius && radius < 32)
		kind = 1;
	else if (32 <= radius && radius < 48)
		kind = 2;
	else if (48 <= radius && radius < 64)
		kind = 3;
	else if (64 <= radius && radius < 80)
		kind = 4;
	else if (80 <= radius && radius < 96)
		kind = 5;
	else if (96 <= radius && radius < 112)
		kind = 6;
	else if (112 <= radius && radius <= 128)
		kind = 7;

	return kind;
}


void CImageProcessing::InitialClassify(const char * svm_file_name, HTuple &hv_SVMHandle)
{
	//m_hvClass.Clear();
	//m_hvClass[0] = "matter";         //  ����
	//m_hvClass[1] = "bubble";         //  ����
	//m_hvClass[2] = "convex";         //  ��͹
	//m_hvClass[3] = "coatting";       //  Ϳ��

	//   ��ȡ������
	HalconCpp::ReadClassSvm(svm_file_name, &hv_SVMHandle);

}


int CImageProcessing::ClassifyRegionsWithSVM(HTuple hv_SVMHandle, HObject img)
{
	HObject ho_ImageMedian, ho_Region, ho_ImageAverage, ho_ImageDeviation;
	HObject ho_ImageSub1, ho_ImageSub2, ho_ImageResult, ho_ImageOut;
	HObject ho_RegionDft, ho_RegionDilation, ho_RegionErosion, ho_ConnectedRegions;
	HObject ho_SelectedRegions, ho_RegionUnion;
	HTuple hv_Width, hv_Height, hv_Sqrt;

	HalconCpp::GetImageSize(img, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(img, &ho_ImageMedian, "square", 1, "mirrored");
	HalconCpp::Threshold(img, &ho_Region, 0, 255);
	HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
	HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
	//�����׼��
	HalconCpp::TupleSqrt(m_hvGrayDeviationValue, &hv_Sqrt);
	HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Region, m_hvGrayMeanValue, "fill");
	HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Region, ((5 * hv_Sqrt).TupleConcat(255)).TupleMin(), "fill");
	//�õ�覴�����
	HalconCpp::SubImage(ho_ImageMedian, ho_ImageAverage, &ho_ImageSub1, 1, 0);
	HalconCpp::SubImage(ho_ImageAverage, ho_ImageMedian, &ho_ImageSub2, 1, 0);
	HalconCpp::AddImage(ho_ImageSub1, ho_ImageSub2, &ho_ImageResult, 0.5, 0);
	HalconCpp::SubImage(ho_ImageResult, ho_ImageDeviation, &ho_ImageOut, 1, 0);
	//ͨ���ı���ֵ�޸���ѡ����
	HalconCpp::Threshold(ho_ImageOut, &ho_RegionDft, 5, 255);
	if (!ho_RegionDft.IsInitialized())
		return 4;
	else {
		HTuple areaValue;
		HalconCpp::RegionFeatures(ho_RegionDft, "area", &areaValue);
		if (areaValue < 3)
			return 4;
	}

	HalconCpp::DilationCircle(ho_RegionDft, &ho_RegionDilation, 2.5);
	HalconCpp::ErosionCircle(ho_RegionDilation, &ho_RegionErosion, 2.5);
	HalconCpp::Connection(ho_RegionErosion, &ho_ConnectedRegions);
	HalconCpp::SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 3, 65536);
	HalconCpp::Union1(ho_SelectedRegions, &ho_RegionUnion);

	//��������ֵ: 15 ��
	HTuple hv_Features;
	CalculateFeatures(img, ho_RegionUnion, hv_Features);

	//����
	HTuple hv_Class;
	HalconCpp::ClassifyClassSvm(hv_SVMHandle, hv_Features, 1, &hv_Class);

	return hv_Class.TupleInt();
}


void CImageProcessing::CalculateFeatures(HObject img, HObject region, HTuple &features)
{
	// Local iconic variables

	// Local control variables
	HTuple  hv_Area, hv_Row, hv_Column, hv_Grayval;
	HTuple  hv_WidthValue, hv_HeightValue, hv_InnerRadius, hv_ContLength;
	HTuple  hv_HolesNum, hv_Roundness, hv_Bulkiness, hv_Mean;
	HTuple  hv_Deviation, hv_PSI1, hv_PSI2, hv_PSI3, hv_PSI4;

	HalconCpp::AreaCenter(region, &hv_Area, &hv_Row, &hv_Column);
	HalconCpp::GetGrayval(img, hv_Row, hv_Column, &hv_Grayval);
	HalconCpp::RegionFeatures(region, "width", &hv_WidthValue);
	HalconCpp::RegionFeatures(region, "height", &hv_HeightValue);
	HalconCpp::RegionFeatures(region, "inner_radius", &hv_InnerRadius);
	HalconCpp::RegionFeatures(region, "contlength", &hv_ContLength);
	HalconCpp::RegionFeatures(region, "holes_num", &hv_HolesNum);
	HalconCpp::RegionFeatures(region, "roundness", &hv_Roundness);
	HalconCpp::RegionFeatures(region, "bulkiness", &hv_Bulkiness);
	HalconCpp::Intensity(region, img, &hv_Mean, &hv_Deviation);
	HalconCpp::MomentsRegionCentralInvar(region, &hv_PSI1, &hv_PSI2, &hv_PSI3, &hv_PSI4);

	features = ((((((((((((((hv_Area
		.TupleConcat(hv_Grayval))
		.TupleConcat(hv_WidthValue))
		.TupleConcat(hv_HeightValue))
		.TupleConcat(hv_Mean))
		.TupleConcat(hv_Deviation))
		.TupleConcat(hv_InnerRadius))
		.TupleConcat(hv_ContLength))
		.TupleConcat(hv_HolesNum))
		.TupleConcat(hv_Roundness))
		.TupleConcat(hv_Bulkiness))
		.TupleConcat(hv_PSI1))
		.TupleConcat(hv_PSI2))
		.TupleConcat(hv_PSI3))
		.TupleConcat(hv_PSI4))
		.TupleReal();

	//features.Clear();
	//features[0] = hv_Area;
	//features[1] = hv_Grayval;
	//features[2] = hv_WidthValue;
	//features[3] = hv_HeightValue;
	//features[4] = hv_Mean;
	//features[5] = hv_Deviation;
	//features[6] = hv_InnerRadius;
	//features[7] = hv_ContLength;
	//features[8] = hv_HolesNum;
	//features[9] = hv_Roundness;
	//features[10] = hv_Bulkiness;
	//features[11] = hv_PSI1;
	//features[12] = hv_PSI2;
	//features[13] = hv_PSI3;
	//features[14] = hv_PSI4;

	return;
}

//����覴�ͼ��
void CImageProcessing::SaveDefectImage(HObject &ho_img, HTuple name)
{
	// +++ Threading variables 
	HDevThread*         hcppthread_handle;
	HDevThreadContext   hcppthread_context; // <-signals begin of procedure

	// Create a thread instance
	hcppthread_handle = new HDevThread(hcppthread_context, (void*)HDevExportCpp::_hcppthread_write_image, 4, 0);
	// Set thread procedure call arguments 
	hcppthread_handle->SetInputIconicParamObject(0, ho_img);
	hcppthread_handle->SetInputCtrlParamTuple(1, "bmp");
	hcppthread_handle->SetInputCtrlParamTuple(2, 0);
	hcppthread_handle->SetInputCtrlParamTuple(3, name);

	// Start proc line in thread
	HTuple hv_ThreadID;
	hcppthread_handle->ParStart(&hv_ThreadID);

	return;
}

// ����㷨��2020.4.3 ��ǰ���ָ��Сͼ���⣩
int CImageProcessing::DetectAlgorithem(int cameraNO, HImage hi_ref, HImage hi_img, std::vector<DefectType> &vDFT)
{
	// Local iconic variables
	//HObject  ho_ImageMedian_ref, ho_Image_def;
	HObject  ho_Region_defth, ho_ConnectedRegions_defth, ho_ImageReduced_def;
	HObject  ho_ImagePart_def, ho_ImageMedian_def, ho_ImageReduced_ref;
	HObject  ho_ImagePart_ref, ho_ImageAbsDiff, ho_ImageMedian_absdiff;
	HObject  ho_Domain_ref, ho_Rectangle, ho_ImageReduced_defsmall;
	HObject  ho_Region, ho_ConnectedRegions, ho_SelectedRegions;
	HObject  ho_ObjectSelected, ho_Rectangle1, ho_ImageReduced1;
	HObject  ho_ImagePart;

	// Local control variables
	HTuple  hv_Width_ref, hv_Height_ref, hv_Width_def, hv_Height_def;
	HTuple  hv_Row_origin_def, hv_Column_origin_def, hv_Row_end_def, hv_Column_end_def;
	HTuple  hv_Mean_ref, hv_Deviation_ref, hv_Min_ref;
	HTuple  hv_Max_ref, hv_Range_ref, hv_row_scale, hv_column_scale;
	HTuple  hv_rownum, hv_colnum, hv_Mean_small_abs_diff, hv_Deviation_small_abs_diff;
	HTuple  hv_between_mean, hv_Min_defsmall, hv_Max_defsmall, hv_Range_defsmall, hv_Number;

	GetImageSize(hi_ref, &hv_Width_ref, &hv_Height_ref);

	HalconCpp::Threshold(hi_img, &ho_Region_defth, 1, 255);
	HalconCpp::Connection(ho_Region_defth, &ho_ConnectedRegions_defth);
	SmallestRectangle1(ho_ConnectedRegions_defth, &hv_Row_origin_def, &hv_Column_origin_def,
		&hv_Row_end_def, &hv_Column_end_def);
	ReduceDomain(hi_img, ho_ConnectedRegions_defth, &ho_ImageReduced_def);
	CropDomain(ho_ImageReduced_def, &ho_ImagePart_def);
	MedianImage(ho_ImagePart_def, &ho_ImageMedian_def, "circle", 1, "mirrored");
	GetImageSize(ho_ImageMedian_def, &hv_Width_def, &hv_Height_def);

	//�жϴ���ͼ���Ƿ��кڱ�
	if (0 != (HTuple(hv_Width_ref != hv_Width_def).TupleOr(hv_Height_ref != hv_Height_def))) {
		ReduceDomain(hi_ref, ho_ConnectedRegions_defth, &ho_ImageReduced_ref);
		CropDomain(ho_ImageReduced_ref, &ho_ImagePart_ref);
		AbsDiffImage(ho_ImageMedian_def, ho_ImagePart_ref, &ho_ImageAbsDiff, 5);
		MedianImage(ho_ImageAbsDiff, &ho_ImageMedian_absdiff, "square", 2, "mirrored");
	}
	else {
		//�ο�ͼ���ȥ�����ͼ�񣬽��ȡ����ֵ
		//�����÷Ŵ������ü��ľ��Ⱥ͵ȼ�
		AbsDiffImage(ho_ImageMedian_def, hi_ref, &ho_ImageAbsDiff, 5);
		MedianImage(ho_ImageAbsDiff, &ho_ImageMedian_absdiff, "square", 2, "mirrored");
	}

	//�ο�ͼ������ƽ��ֵ
	GetDomain(hi_ref, &ho_Domain_ref);
	Intensity(ho_Domain_ref, hi_ref, &hv_Mean_ref, &hv_Deviation_ref);
	//��ֵͼ����������ֵ
	//MinMaxGray(ho_Domain_ref, ho_ImageMedian_absdiff, 0, &hv_Min_ref, &hv_Max_ref, &hv_Range_ref);

	//�ж�����������ȡģ
	hv_row_scale = hv_Height_def / 256;
	if (0 != ((hv_Height_def % 256) != 0))
	{
		hv_row_scale += 1;
	}
	hv_column_scale = hv_Width_def / 256;
	if (0 != ((hv_Width_def % 256) != 0))
	{
		hv_column_scale += 1;
	}

	// vector �ݴ�覴�λ����Ϣ�������������
	//std::vector<DefectType> vdef;
	HTuple end_val41 = hv_row_scale - 1;
	HTuple step_val41 = 1;
	for (hv_rownum = 0; hv_rownum.Continue(end_val41, step_val41); hv_rownum += step_val41)
	{
		HTuple end_val42 = hv_column_scale - 1;
		HTuple step_val42 = 1;
		for (hv_colnum = 0; hv_colnum.Continue(end_val42, step_val42); hv_colnum += step_val42)
		{
			//�ض���ͼ��Ķ����򣬷���ͼ��
			GenRectangle1(&ho_Rectangle, (hv_rownum * 256) + hv_Row_origin_def, (hv_colnum * 256) + hv_Column_origin_def,
				((hv_rownum + 1) * 256) + hv_Row_origin_def, ((hv_colnum + 1) * 256) + hv_Column_origin_def);

			ReduceDomain(ho_ImageMedian_absdiff, ho_Rectangle, &ho_ImageReduced_defsmall);
			//֡������ͼ�����������������ƽ��ֵ
			//Intensity(ho_Rectangle, ho_ImageReduced_defsmall, &hv_Mean_small_abs_diff, &hv_Deviation_small_abs_diff);
			//hv_between_mean = (hv_Mean_ref - hv_Mean_small_abs_diff).TupleAbs();
			//�����������ص������Сֵ
			//reduce_domain (ImageAbsDiff, Rectangle, ImageReduced3)
			//MinMaxGray(ho_Rectangle, ho_ImageReduced_defsmall, 25, &hv_Min_defsmall, &hv_Max_defsmall, &hv_Range_defsmall);
			//���к��ͼ��̬��ֵ��
			//Threshold (ImageReduced_defsmall, Region, 0.8*max([min([255,Mean * 1.2]), min([255,Mean_small_abs_diff * 1.2])]), 255)

			Threshold(ho_ImageReduced_defsmall, &ho_Region, ((hv_Mean_ref*0.5).TupleConcat(255)).TupleMin(), 255);

			//��������
			HalconCpp::Connection(ho_Region, &ho_ConnectedRegions);
			//ѡ��ROI
			SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "contlength", "and", 5, 999999);

			//�Ե���Сͼ���д���, ����ROI����
			CountObj(ho_SelectedRegions, &hv_Number);
			if (hv_Number != 0) {

				//����覴�ͼƬ
				DefectType temp_def = LocateDefectPosition(cameraNO, ho_SelectedRegions,
					hv_Number, hv_Column_origin_def, ho_ImagePart_def);
				//����覴���Ϣ
				vDFT.push_back(temp_def);
			}
		}
	}

	return 0;
}

// ����㷨��2020.4.5��ֱ�ӶԴ�ͼ���м�⣩
int CImageProcessing::DetectAlgorithemSimple(int cameraNO, HImage hi_ref, HImage hi_img, std::vector<DefectType> &vDFT)
{
	// Local iconic variables
	HObject  ho_Image_ref, ho_Image_def;
	HObject  ho_ImageGauss1, ho_ImageAbsDiff1, ho_Regions, ho_ConnectedRegions, ho_ImageResult;;
	HObject  ho_SelectedRegions, ho_ObjectSelected, ho_Rectangle;
	HObject  ho_ImageReduced, ho_ImagePart;

	// Local control variables
	//HTuple  hv_Width_ref, hv_Height_ref;
	HTuple  hv_Width, hv_Height, hv_Min, hv_Max, hv_Range, hv_Number;
	HTuple  hv_i, hv_Row, hv_Column, hv_Radius;
	//HTuple  hv_ThreadID;

	//��ȡ�ο�ͼ��
	ho_Image_ref = hi_ref;
	//GetImageSize(ho_Image_ref, &hv_Width_ref, &hv_Height_ref);
	//GaussFilter(ho_Image_ref, &ho_ImageGauss, 5);
	//��ȡ�����ͼ��
	ho_Image_def = hi_img;
	GetImageSize(ho_Image_def, &hv_Width, &hv_Height);
	//GaussFilter(ho_Image_def, &ho_ImageGauss1, 5);
	HalconCpp::MedianImage(ho_Image_def, &ho_ImageGauss1, "square", m_median_filter_size, "mirrored");

	//����ͼ - �ο�ͼ
	AbsDiffImage(ho_ImageGauss1, ho_Image_ref, &ho_ImageAbsDiff1, 1);

	//�Զ���ֵ����������ǵ�ͨ��ͼ�񣬻��ж���ֵ�ָSigma���ڶԻҶ�ֱ��ͼ���и�˹ƽ����������ƽ���ĳ̶ȣ��ָ�ϸ�³̶ȣ���
	//��sigma�ܴ�ʱ���Ҷ�ֱ��ͼ�����ᱻƽ��Ϊֻʣ��һ�����壬���ָ��Ǹ���ƽ����ֱ��ͼ�Ĳ��������еģ�SigmaС���ָ��Խϸ��
	//AutoThreshold(ho_ImageAbsDiff1, &ho_Regions, 3);

	//��һ��������Ĥ�������ص�ĻҶȷ�Χ
	GrayRangeRect(ho_ImageAbsDiff1, &ho_ImageResult, 10, 10);
	MinMaxGray(ho_ImageResult, ho_ImageResult, 0, &hv_Min, &hv_Max, &hv_Range);
	Threshold(ho_ImageResult, &ho_Regions, (HTuple(5.55).TupleConcat(hv_Max*0.8)).TupleMax(), 255);
	//Threshold(ho_ImageResult, &ho_Regions, 200, 255);

	HalconCpp::Connection(ho_Regions, &ho_ConnectedRegions);
	//����ѡ�������1.�����ܳ�(pixel)  2.����ڽ�Բ�뾶(pixel)
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, (HTuple("contlength").Append("inner_radius")),
		"and", (HTuple(5).Append(1)), (HTuple(32768).Append(2048)));
	CountObj(ho_SelectedRegions, &hv_Number);
	if (0 != hv_Number)
	{
		{
			HTuple end_val19 = hv_Number;
			HTuple step_val19 = 1;
			for (hv_i = 1; hv_i.Continue(end_val19, step_val19); hv_i += step_val19)
			{
				SelectObj(ho_SelectedRegions, &ho_ObjectSelected, hv_i);
				SmallestCircle(ho_ObjectSelected, &hv_Row, &hv_Column, &hv_Radius);
				//����ѡ����������Բ�뾶����2048��������Χ����ͼ�������֮��ʱ��Ĭ������
				// min([hv_width, hv_height]) / 3
				if (0 != (hv_Radius > ((hv_Width.TupleConcat(hv_Height)).TupleMin()) / 3))
				{
					continue;
				}
				if (0 != (hv_Row < 127))
				{
					hv_Row = 127;
				}
				else if (0 != (hv_Row > (hv_Width - 127)))
				{
					hv_Row = hv_Width - 127;
				}
				if (0 != (hv_Column < 127))
				{
					hv_Column = 127;
				}
				else if (0 != (hv_Column > (hv_Height - 127)))
				{
					hv_Column = hv_Height - 127;
				}
				GenRectangle1(&ho_Rectangle, hv_Row - 127, hv_Column - 127, hv_Row + 128, hv_Column + 128);
				ReduceDomain(ho_Image_def, ho_Rectangle, &ho_ImageReduced);
				CropDomain(ho_ImageReduced, &ho_ImagePart);

				//��������ƽ��ֵ
				HTuple hv_mean, hv_deviation;
				HalconCpp::Intensity(ho_ObjectSelected, ho_ImagePart, &hv_mean, &hv_deviation);

				//�����⵽��覴���Ϣ
				DefectType dtype;
				dtype.center_x = (hv_Column.TupleInt() + IMAGE_WIDTH * (cameraNO - 1))*HORIZON_PRECISION;     //��λ������
				dtype.area = 0.255f;
				dtype.circle_radius = hv_Radius.TupleInt();
				dtype.pixel_value = hv_mean.TupleInt();
				dtype.absolute_position = hv_Row.TupleInt() * VERTICAL_PRECISION / 1000.0f;   //��λ����
				//覴÷���
				if (0 < dtype.pixel_value && dtype.pixel_value <= 92) {
					if (dtype.area < 100) {
						dtype.type = 0;
					}
					else
						dtype.type = 2;
				}
				else if (92 < dtype.pixel_value && dtype.pixel_value <= 168) {
					if (dtype.area < 100) {
						dtype.type = 0;
					}
					else
						dtype.type = 1;
				}
				else
					dtype.type = 2;

				//��ʽ���ļ���
				char cpos[16];
				sprintf_s(cpos, "%.3f", dtype.absolute_position);
				//HTuple hv_position = (HTuple)(std::to_string(dtype.absolute_position).c_str());
				HTuple hv_position = (HTuple)cpos;
				char cX[16];
				sprintf_s(cX, "%.3f", dtype.center_x);
				HTuple hv_X = (HTuple)cX;
				char cradius[16];
				sprintf_s(cradius, "%.3f", dtype.circle_radius);
				HTuple hv_radius = (HTuple)cradius;
				char carea[16];
				sprintf_s(carea, "%.3f", dtype.area);
				HTuple hv_harea = (HTuple)carea;
				HTuple hv_kind = (HTuple)dtype.type;

				//����覴���Ϣ
				vDFT.push_back(dtype);

				//覴�ͼ�������
				HTuple hv_path = (HTuple)m_strPath.c_str();
				HTuple hv_img_name = hv_path + "P" + hv_position
					+ "_X" + hv_X
					+ "_R" + hv_radius
					+ "_A" + hv_harea
					+ "_K" + hv_kind;
				SaveDefectImage(ho_ImagePart, hv_img_name);

			}
		}
	}
	return 0;
}

//����㷨��һ�ֻ��ڶ�Ŀ�����Ӿ��Ĺ�ѧ��Ĥ覴ü��ϵͳ
int CImageProcessing::StandDeviationAlgorithm(HImage hi_img, HTuple hv_SVM, std::vector<DeffectInfo> &vecDftInfo,
	std::vector<HalconCpp::HObject> &vecDftImg)
{
	HObject  ho_Image, ho_ImageAverage, ho_ImageDeviation, ho_ImageMedianDFT;
	HObject  ho_ImageSub1, ho_ImageSub2, ho_ImageAddSub, ho_ImageResult;
	HObject  ho_Region, ho_RegionDilation, ho_RegionErosion;
	HObject  ho_ConnectedRegions, ho_SelectedRegions, ho_ObjectSelected, ho_Rectangle;
	HObject  ho_ImageReduced, ho_ImagePart;
	HTuple   hv_Width, hv_Height, hv_Number;
	HTuple   hv_i, hv_Area, hv_Row, hv_Column, hv_RowCircle, hv_ColumnCircle, hv_Radius, hv_Contlength;

	//ho_Image = hi_img;
	HalconCpp::CopyImage(hi_img, &ho_Image);
	//�Ƿ���Ҫ�б�
	if (m_nCutBorderStatue > 0) {
		// m_nCutBorder = 1Ϊ���бߣ� m_nCutBorder = 2Ϊ���б�
		HObject ho_ImageReduced, ho_ImagePart;
		HalconCpp::ReduceDomain(ho_Image, m_hoBorderRegion, &ho_ImageReduced);
		HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
		ho_Image = ho_ImagePart;
	}


#ifdef TESTMODEL
	AddNoise(ho_Image);
#endif

	ho_ImageAverage = m_hi_average;
	ho_ImageDeviation = m_hi_deviation;
	HalconCpp::GetImageSize(ho_Image, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(ho_Image, &ho_ImageMedianDFT, "square", m_median_filter_size, "mirrored");
	//HalconCpp::MedianRect(ho_Image, &ho_ImageMedianDFT, 1, 11);

	//�ο����ף�һ�ֻ��ڶ�Ŀ�����Ӿ��Ĺ�ѧ��Ĥ覴ü��ϵͳ
	//������������ֵС���㣬����ͼ�лᱻ��0��ͬ���������ֵ����255��Ҳ�ᱻ�ض�ʹ�����ֵΪ255
	//����λ�����������ӣ��൱�����
	HalconCpp::SubImage(ho_ImageMedianDFT, ho_ImageAverage, &ho_ImageSub1, 1, 0);
	HalconCpp::SubImage(ho_ImageAverage, ho_ImageMedianDFT, &ho_ImageSub2, 1, 0);
	HalconCpp::AddImage(ho_ImageSub1, ho_ImageSub2, &ho_ImageAddSub, 0.5, 0);
	HalconCpp::SubImage(ho_ImageAddSub, ho_ImageDeviation, &ho_ImageResult, 1, 0);
	//ͨ����ֵ���Ƽ������
	HalconCpp::Threshold(ho_ImageResult, &ho_Region, 1, 255);
	if (!ho_Region.IsInitialized())
		return 0;

	//����,��ʴ,���ڼ���region������
	HalconCpp::DilationCircle(ho_Region, &ho_RegionDilation, 64);
	HalconCpp::ErosionCircle(ho_RegionDilation, &ho_RegionErosion, 64);
	HalconCpp::Connection(ho_RegionErosion, &ho_ConnectedRegions);
	int min_select = (int)std::pow(2 * m_fMin_Radius / HORIZON_PRECISION, 2);
	int max_select = (int)std::pow(2 * m_fMax_Radius / HORIZON_PRECISION, 2);
	HalconCpp::SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and",	min_select,	max_select);
	HalconCpp::CountObj(ho_SelectedRegions, &hv_Number);
	//TRACE("DFT in current image = %d", hv_Number[0].I());
	//CString cstr;
	//cstr.Format(_T("DFT in current image = %d"), hv_Number[0].I());
	//::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);

	if (0 != hv_Number){
		//������ͼ����ѡ������򳬹� 64 ��������Ϊ��ͼ����ʧ��
		if (hv_Number > 128) {
			CString cstr;
			cstr.Format(_T("���������ֵ,λ��%.3f��,����Ĥ���Ƿ�����"),	m_unImageIndex * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f);
			::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return -1;
		}

		std::vector<SelectRegion> vSelect;
		HTuple end_val19 = hv_Number;
		HTuple step_val19 = 1;
		for (hv_i = 1; hv_i.Continue(end_val19, step_val19); hv_i += step_val19){
			HalconCpp::SelectObj(ho_SelectedRegions, &ho_ObjectSelected, hv_i);
			//�����Ƿ�Ҫ�滻ΪAreaCenterGray()����
			HalconCpp::AreaCenter(ho_ObjectSelected, &hv_Area, &hv_Row, &hv_Column);
			//��С���Բ
			HalconCpp::SmallestCircle(ho_ObjectSelected, &hv_RowCircle, &hv_ColumnCircle, &hv_Radius);
			//Ĭ�����ѡ����������ͼ��ߴ��һ�룬����Ϊ��Ч����
			if (hv_Radius > 2048)
				continue;
			//�����ܳ�
			HalconCpp::Contlength(ho_ObjectSelected, &hv_Contlength);
			//��������ƽ��ֵ
			HTuple hv_mean, hv_deviation;
			HalconCpp::Intensity(ho_ObjectSelected, ho_Image, &hv_mean, &hv_deviation);

			SelectRegion region;
			region.index = hv_i.TupleInt();
			region.hv_Row_Center = hv_Row;
			region.hv_Column_Center = hv_Column;
			region.area = (float)hv_Area.D();
			region.radius = (float)hv_Radius.D();
			region.contlength = (float)hv_Contlength.D();
			region.pixelvalue = (float)hv_mean;
			vSelect.push_back(region);
		}

		//���������������覴�ͼ����Ϣ
		std::vector<SelectRegion>::reverse_iterator rit;
		for (rit = vSelect.rbegin(); rit != vSelect.rend(); rit++){
			SelectRegion region;
			region = *rit;
			if (region.area == 0.0f)
				continue;
			else {
				//����ѡ��������������256*256=65536��������Χ����覴�ͼ��ĳߴ�ʱ��������ʵ��ͼ��
				if (region.area >= 65536.0f){
					//ֱ�ӱ�����������
					HTuple hv_x1, hv_y1, hv_x2, hv_y2;
					hv_x1 = region.hv_Column_Center - (HTuple)region.radius;
					hv_y1 = region.hv_Row_Center - (HTuple)region.radius;
					hv_x2 = region.hv_Column_Center + (HTuple)region.radius;
					hv_y2 = region.hv_Row_Center + (HTuple)region.radius;
					if (hv_x1 < 0)
						hv_x1 = 0;
					if (hv_y1 < 0)
						hv_y1 = 0;
					if (hv_x2 > hv_Width)
						hv_x2 = hv_Width;
					if (hv_y2 > hv_Height)
						hv_y2 = hv_Height;
					HalconCpp::GenRectangle1(&ho_Rectangle, hv_y1, hv_x1, hv_y2, hv_x2);
					HalconCpp::ReduceDomain(ho_Image, ho_Rectangle, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
				}
				else {
					if (0 != (region.hv_Row_Center < 127)){
						region.hv_Row_Center = 127;
					}
					else if (0 != (region.hv_Row_Center > (hv_Height - 127))){
						region.hv_Row_Center = hv_Height - 127;
					}
					if (0 != (region.hv_Column_Center < 127)){
						region.hv_Column_Center = 127;
					}
					else if (0 != (region.hv_Column_Center > (hv_Width - 127))){
						region.hv_Column_Center = hv_Width - 127;
					}
					GenRectangle1(&ho_Rectangle, region.hv_Row_Center - 127, region.hv_Column_Center - 127,
						region.hv_Row_Center + 128, region.hv_Column_Center + 128);
					HalconCpp::ReduceDomain(ho_Image, ho_Rectangle, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
				}

				//覴���Ϣ����
				DeffectInfo dftInfo;
				dftInfo.type = (unsigned short)ClassifyRegionsWithSVM(hv_SVM, ho_ImagePart);
				//dftInfo.type = ImageClassification(region.radius);
				dftInfo.x = (float)region.hv_Column_Center.D();
				dftInfo.y = (float)region.hv_Row_Center.D();
				dftInfo.area = region.area;
				dftInfo.radius = (unsigned short)region.radius;
				dftInfo.contlength = (unsigned short)region.contlength;
				dftInfo.rank = RankDivide(region.area, region.radius, region.contlength);    //  �ȼ�
				vecDftInfo.push_back(dftInfo);
				vecDftImg.push_back(ho_ImagePart);

				//DefectType dtype;
				//dtype.type = ImageClassification(ho_ImagePart);
				//dtype.center_x = (float)region.hv_Column_Center.D();
				////dtype.absolute_position = m_current_position + (float)region.hv_Row_Center.D() * VERTICAL_PRECISION / 1000.0f;
				//dtype.absolute_position = (produced_num * IMAGE_HEIGHT + (float)region.hv_Row_Center.D()) * VERTICAL_PRECISION / 1000.0f;
				//dtype.area = region.area * HORIZON_PRECISION * VERTICAL_PRECISION;
				//dtype.circle_radius = region.radius * HORIZON_PRECISION;
				//dtype.contlength = region.contlength * HORIZON_PRECISION;
				//dtype.pixel_value = (int)region.pixelvalue;
				//int rank = RankDivide(dtype);
				//dtype.rank = rank;
				////����覴���Ϣ
				//vDFT.push_back(dtype);

				////ͼ�񱣴棬��ʽ���ļ���
				//HTuple hv_path = (HTuple)m_strPath.c_str();
				//char cpos[16];
				//sprintf_s(cpos, "%.3f", dtype.absolute_position);
				////HTuple hv_position = (HTuple)(std::to_string(dtype.absolute_position).c_str());
				//HTuple hv_position = (HTuple)cpos;
				//char cX[16];
				//sprintf_s(cX, "%.3f", dtype.center_x);
				//HTuple hv_X = (HTuple)cX;
				//char cradius[16];
				//sprintf_s(cradius, "%.3f", dtype.circle_radius);
				//HTuple hv_radius = (HTuple)cradius;
				//char carea[16];
				//sprintf_s(carea, "%.3f", dtype.area);
				//HTuple hv_harea = (HTuple)carea;
				//HTuple hv_kind = (HTuple)dtype.type;
				//HTuple hv_img_name = hv_path + "P" + hv_position
				//	+ "_X" + hv_X
				//	+ "_R" + hv_radius
				//	+ "_A" + hv_harea
				//	+ "_K" + hv_kind;
				//SaveDefectImage(ho_ImagePart, hv_img_name);

			}
		}
	}

	return 0;
}

// ����㷨��2021.2.26
// step 1
void CImageProcessing::GetOutDeviationArea(HImage hiImg, HObject &hoSelectedArea)
{
	//assert(m_hi_average);
	//assert(m_hi_deviation);

	HTuple hvWidth, hvHeight;
	HalconCpp::GetImageSize(hiImg, &hvWidth, &hvHeight);
	HObject hoImgMedianDFT;
	HalconCpp::MedianImage(hiImg, &hoImgMedianDFT, "square", m_median_filter_size, "mirrored");
	//HalconCpp::MedianRect(ho_Image, &ho_ImageMedianDFT, 1, 11);

	//�ο����ף�һ�ֻ��ڶ�Ŀ�����Ӿ��Ĺ�ѧ��Ĥ覴ü��ϵͳ
	//������������ֵС���㣬����ͼ�лᱻ��0��ͬ���������ֵ����255��Ҳ�ᱻ�ض�ʹ�����ֵΪ255
	//����λ�����������ӣ��൱�����
	HObject hoImgSub1, hoImgSub2, hoImgAddSub;
	HalconCpp::SubImage(hoImgMedianDFT, m_hi_average, &hoImgSub1, 1, 0);
	HalconCpp::SubImage(m_hi_average, hoImgMedianDFT, &hoImgSub2, 1, 0);
	HalconCpp::AddImage(hoImgSub1, hoImgSub2, &hoImgAddSub, 0.5, 0);
	HObject hoImgResult;
	HalconCpp::SubImage(hoImgAddSub, m_hi_deviation, &hoImgResult, 1, 0);
	HObject hoRegion;
	HalconCpp::Threshold(hoImgResult, &hoRegion, 1, 255);

	//����,��ʴ,���ڼ���region������
	HObject hoRegionDilation, hoRegionErosion, hoConnectedRegion;
	HalconCpp::DilationCircle(hoRegion, &hoRegionDilation, 64);
	HalconCpp::ErosionCircle(hoRegionDilation, &hoRegionErosion, 64);
	HalconCpp::Connection(hoRegionErosion, &hoConnectedRegion);

	int min_select = (int)std::pow(2 * m_fMin_Radius / HORIZON_PRECISION, 2);
	int max_select = (int)std::pow(2 * m_fMax_Radius / HORIZON_PRECISION, 2);
	HalconCpp::SelectShape(hoConnectedRegion, &hoSelectedArea, "area", "and",	min_select,	max_select);

}

// step 2
void CImageProcessing::SplitAndMeasureDeffect(HObject selectArea)
{
	//assert(selectArea);
	HTuple hvCounts;
	HalconCpp::CountObj(selectArea, &hvCounts);
	if (hvCounts == 0)
		return;

	//������ͼ����ѡ������򳬹� 64 ��������Ϊ��ͼ����ʧ��
	if (hvCounts > 64) {
		CString cstr;
		cstr.Format(_T("���������ֵ,λ��%.3f��,����Ĥ���Ƿ�����"), m_unImageIndex * IMAGE_HEIGHT * VERTICAL_PRECISION);
		::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return;
	}

	std::vector<SelectRegion> vSelect;
	HObject hvCurrentSelected;
	HTuple hvArea, hvRow, hvColumn;
	HTuple hvCircleRow, hvCircleColumn, hvRadius, hvContlength;
	for (HTuple hv_i = 1; hv_i.Continue(hvCounts, 1); hv_i += 1) {
		HalconCpp::SelectObj(selectArea, &hvCurrentSelected, hv_i);
		//�����Ƿ�Ҫ�滻ΪAreaCenterGray()����
		HalconCpp::AreaCenter(hvCurrentSelected, &hvArea, &hvRow, &hvColumn);
		//��С���Բ
		HalconCpp::SmallestCircle(hvCurrentSelected, &hvCircleRow, &hvCircleColumn, &hvRadius);
		//Ĭ�����ѡ����������ͼ��ߴ��һ�룬����Ϊ��Ч����
		if (hvRadius > 2048)
			continue;
		//�����ܳ�
		HalconCpp::Contlength(hvCurrentSelected, &hvContlength);
		//��������ƽ��ֵ
		//HTuple hv_mean, hv_deviation;
		//HalconCpp::Intensity(hvCurrentSelected, ho_Image, &hv_mean, &hv_deviation);

		SelectRegion region;
		region.index = hv_i.TupleInt();
		region.hv_Row_Center = hvRow;
		region.hv_Column_Center = hvColumn;
		region.area = (float)hvArea.D();
		region.radius = (float)hvRadius.D();
		region.contlength = (float)hvContlength.D();
		region.pixelvalue = 0.0f;
		vSelect.push_back(region);
	}
}

// add noise
void CImageProcessing::AddNoise(HObject hoImg)
{
	HTuple  hv_Width, hv_Height, hv_minvalue, hv_maxvalue;
	HTuple  hv_Number, hv_index, hv_value;
	HObject ho_Regions, ho_ObjectSelected;

	GetImageSize(hoImg, &hv_Width, &hv_Height);

	unsigned seed = (unsigned)std::time(0);
	std::srand(seed);
	int noiseNum = std::rand() % 2;
	if (noiseNum == 0)
		return;
	GenRandomRegions(&ho_Regions, "ellipse", 1, 32, 1, 32, -0.7854, 0.7854, noiseNum, hv_Width, hv_Height);
	CountObj(ho_Regions, &hv_Number);

	hv_minvalue = 0;
	hv_maxvalue = 255;

	HTuple end_val19 = hv_Number;
	HTuple step_val19 = 1;
	for (hv_index = 1; hv_index.Continue(end_val19, step_val19); hv_index += step_val19){
		SelectObj(ho_Regions, &ho_ObjectSelected, hv_index);
		// ����������ֵ
		hv_value = hv_minvalue + (HTuple::TupleRand(1)*(hv_maxvalue - hv_minvalue));
		OverpaintRegion(hoImg, ho_ObjectSelected, hv_value, "fill");
	}
}

//�����߳�
UINT CImageProcessing::ManageThread(LPVOID pParam)
{
	CImageProcessing *pThis = (CImageProcessing *)pParam;
	//TCHAR path[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH, path);
	//CString curpath = path;
	std::string model_filename = ".\\system\\Model.svm";
	//pThis->m_modelFileName = curpath + (CA2W)model_filename.c_str();
	pThis->m_modelFileName = (CA2W)model_filename.c_str();

	pThis->m_nTotalListNumber = 0;
	bool bresult = false;
	while (1) {
		Sleep(200);
#ifdef TESTMODEL
		std::string imgname = "D:\\覴ü�����ݼ�¼\\test_image\\reference_image.png";
		bresult = pThis->GetRefImgWithoutBouder(imgname.c_str());
#else
		bresult = pThis->GenerateReferenceImage(pThis->m_hi_average, pThis->m_hi_deviation);
#endif					
		if (bresult) {
			if (pThis->SAVE_REFERENCE_IMAGE) {
				//std::string filename = "D:\\DetectRecords\\ReferenceImage.png";
				//pThis->SaveReferenceImage(filename.c_str());
				CString cstr = L"�ο�ͼ���ѱ���";
				::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
			}
			CString cstr = L"�ο�ͼ��������";
			::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
			::SendMessage(pThis->hMainWnd, WM_UPDATE_CONTROLS, NULL, NULL);
			break;
		}
		else
			continue;
	}

#ifdef TESTMODEL
	std::string imgname = "D:\\覴ü�����ݼ�¼\\test_image\\test_image.png";
	pThis->LoadSingleImage(imgname.c_str());
	DWORD dwStop = 0;
	for (;;) {
		dwStop = WaitForSingleObject(pThis->StopManage_Event, 1637);
		switch (dwStop)
		{
		case WAIT_TIMEOUT: {
			if (!pThis->SYSTEM_PAUSE) {
				pThis->LoadImageToQueue();
				pThis->m_nTotalListNumber += pThis->m_threadnum;
			}
			break;
		}
		case WAIT_FAILED:
			return -1;
		case WAIT_OBJECT_0:
			return 0;
		}
	}
#endif

	return 0;
}

//ͼ�����߳�
UINT CImageProcessing::ImageCalculate1(LPVOID pParam)
{
	CImageProcessing *pImgProc = (CImageProcessing *)pParam;
	CSingleLock singlelock(&pImgProc->mutex);

	int index = 0;
	pImgProc->m_bThreadAlive[index] = TRUE;
	for (;;) {
		if (pImgProc->CheckReferenceImageAvilable())
			break;
		else {
			Sleep(200);
			continue;
		}
	}
	HImage hi_average = pImgProc->m_hi_average;
	HImage hi_deviation = pImgProc->m_hi_deviation;
	HImage hi_acquire;
	HTuple hv_SVMHandle;
	pImgProc->InitialClassify((CW2A)pImgProc->m_modelFileName.GetBuffer(), hv_SVMHandle);
	while (pImgProc->m_bThreadAlive[index]){
		if (pImgProc->m_listAcquiredImage.empty()) {
			Sleep(10);
			continue;
		}
		else {
			std::vector<DeffectInfo> vec_dft_info;
			std::vector<HalconCpp::HObject> vec_dft_img;
			singlelock.Lock(1000);     //  û�����þ������Լ��ã��ѱ����þ͵���
			if (singlelock.IsLocked()) {
				hi_acquire = pImgProc->m_listAcquiredImage.front();
				pImgProc->m_listAcquiredImage.pop_front();
				pImgProc->m_unImageIndex += 1;
			}
			singlelock.Unlock();   //  ����

			//EnterCriticalSection(&pImgProc->m_csDefImgList1);
			//if (pImgProc->mtx.try_lock()) {
			//	hi_acquire = pImgProc->m_listAcquiredImage.front();
			//	pImgProc->m_listAcquiredImage.pop_front();
			//	pImgProc->m_unImageIndex += 1;
			//	pImgProc->mtx.unlock();
			//}
			//else
			//	continue;
			//LeaveCriticalSection(&pImgProc->m_csDefImgList1);

			//覴ü���㷨
			pImgProc->StandDeviationAlgorithm(hi_acquire, hv_SVMHandle, vec_dft_info, vec_dft_img);
			//覴���Ϣ��ͼ��������
			DeffectInfo tempInfo;
			HObject ho_tempImg;
			EnterCriticalSection(&pImgProc->m_csCalculateThread);
			for (int i = 0; i < (int)vec_dft_info.size(); i++) {
				tempInfo = vec_dft_info.at(i);
				tempInfo.image_index = (unsigned)pImgProc->m_unImageIndex;
				pImgProc->m_listDftInfo.push_back(tempInfo);
				ho_tempImg = vec_dft_img.at(i);
				pImgProc->m_listDftImg.push_back(ho_tempImg);
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread);
		}
	}
	pImgProc->m_bThreadAlive[index] = FALSE;
	if (pImgProc->m_threadnum > 1) {
		DWORD dwStop = 0;
		dwStop = WaitForSingleObject(pImgProc->CalculateThread_2_StoppedEvent, INFINITE);
		switch (dwStop)
		{
		case WAIT_TIMEOUT:
			break;
		case WAIT_FAILED:
			return -1;
		case WAIT_OBJECT_0: {
			pImgProc->m_listAcquiredImage.clear();
			SetEvent(pImgProc->m_hFinishedProcess);
			CString cstr = L"�����߳��ѽ���";
			::SendMessage(pImgProc->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
			::SendMessage(pImgProc->hMainWnd, WM_UPDATE_CONTROLS, NULL, NULL);
			return 0;
		}
		}
	}
	else {
		pImgProc->m_listAcquiredImage.clear();
		//pImgProc->CalculateThread_1_StoppedEvent.SetEvent();
		SetEvent(pImgProc->m_hFinishedProcess);
	}		

	return 0;
}

UINT CImageProcessing::ImageCalculate2(LPVOID pParam)
{
	CImageProcessing *pImgProc = (CImageProcessing *)pParam;
	CSingleLock singlelock(&pImgProc->mutex);

	int index = 1;
	pImgProc->m_bThreadAlive[index] = TRUE;
	for (;;) {
		if (pImgProc->CheckReferenceImageAvilable())
			break;
		else {
			Sleep(200);
			continue;
		}
	}
	HImage hi_average = pImgProc->m_hi_average;
	HImage hi_deviation = pImgProc->m_hi_deviation;
	HImage hi_acquire;
	HTuple hv_SVMHandle;
	pImgProc->InitialClassify((CW2A)pImgProc->m_modelFileName.GetBuffer(), hv_SVMHandle);
	while (pImgProc->m_bThreadAlive[index]) {
		if (pImgProc->m_listAcquiredImage.empty()) {
			Sleep(10);
			continue;
		}
		else {
			std::vector<DeffectInfo> vec_dft_info;
			std::vector<HalconCpp::HObject> vec_dft_img;
			singlelock.Lock(1000);     //  û�����þ������Լ��ã��ѱ����þ͵���
			if (singlelock.IsLocked()) {
				hi_acquire = pImgProc->m_listAcquiredImage.front();
				pImgProc->m_listAcquiredImage.pop_front();
				pImgProc->m_unImageIndex += 1;
			}
			singlelock.Unlock();   //  ����


			//EnterCriticalSection(&pImgProc->m_csDefImgList2);
			//if (pImgProc->mtx.try_lock()) {
			//	hi_acquire = pImgProc->m_listAcquiredImage.front();
			//	pImgProc->m_listAcquiredImage.pop_front();
			//	pImgProc->m_unImageIndex += 1;
			//	pImgProc->mtx.unlock();
			//}
			//else
			//	continue;
			//LeaveCriticalSection(&pImgProc->m_csDefImgList2);

			//覴ü���㷨
			pImgProc->StandDeviationAlgorithm(hi_acquire, hv_SVMHandle, vec_dft_info, vec_dft_img);
			//覴���Ϣ��ͼ��������
			DeffectInfo tempInfo;
			HObject ho_tempImg;
			EnterCriticalSection(&pImgProc->m_csCalculateThread);
			for (int i = 0; i < (int)vec_dft_info.size(); i++) {
				tempInfo = vec_dft_info.at(i);
				tempInfo.image_index = (unsigned)pImgProc->m_unImageIndex;
				pImgProc->m_listDftInfo.push_back(tempInfo);
				ho_tempImg = vec_dft_img.at(i);
				pImgProc->m_listDftImg.push_back(ho_tempImg);
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread);
		}
	}
	pImgProc->m_bThreadAlive[index] = FALSE;
	if (pImgProc->m_threadnum > 2) {
		DWORD dwStop = 0;
		dwStop = WaitForSingleObject(pImgProc->CalculateThread_3_StoppedEvent, INFINITE);
		switch (dwStop)
		{
		case WAIT_TIMEOUT:
			break;
		case WAIT_FAILED:
			return -1;
		case WAIT_OBJECT_0:
			pImgProc->CalculateThread_2_StoppedEvent.SetEvent();
			return 0;
		}
	}
	else
		pImgProc->CalculateThread_2_StoppedEvent.SetEvent();

	return 0;
}

UINT CImageProcessing::ImageCalculate3(LPVOID pParam)
{
	CImageProcessing *pImgProc = (CImageProcessing *)pParam;
	CSingleLock singlelock(&pImgProc->mutex);

	int index = 2;
	pImgProc->m_bThreadAlive[index] = TRUE;
	for (;;) {
		if (pImgProc->CheckReferenceImageAvilable())
			break;
		else {
			Sleep(200);
			continue;
		}
	}
	HImage hi_average = pImgProc->m_hi_average;
	HImage hi_deviation = pImgProc->m_hi_deviation;
	HImage hi_acquire;
	HTuple hv_SVMHandle;
	pImgProc->InitialClassify((CW2A)pImgProc->m_modelFileName.GetBuffer(), hv_SVMHandle);
	while (pImgProc->m_bThreadAlive[index]) {
		if (pImgProc->m_listAcquiredImage.empty()) {
			Sleep(10);
			continue;
		}
		else {
			std::vector<DeffectInfo> vec_dft_info;
			std::vector<HalconCpp::HObject> vec_dft_img;
			singlelock.Lock(1000);     //  û�����þ������Լ��ã��ѱ����þ͵���
			if (singlelock.IsLocked()) {
				hi_acquire = pImgProc->m_listAcquiredImage.front();
				pImgProc->m_listAcquiredImage.pop_front();
				pImgProc->m_unImageIndex += 1;
			}
			singlelock.Unlock();   //  ����


			//EnterCriticalSection(&pImgProc->m_csDefImgList3);
			//if (pImgProc->mtx.try_lock()) {
			//	hi_acquire = pImgProc->m_listAcquiredImage.front();
			//	pImgProc->m_listAcquiredImage.pop_front();
			//	pImgProc->m_unImageIndex += 1;
			//	pImgProc->mtx.unlock();
			//}
			//else
			//	continue;
			//LeaveCriticalSection(&pImgProc->m_csDefImgList3);

			//覴ü���㷨
			pImgProc->StandDeviationAlgorithm(hi_acquire, hv_SVMHandle, vec_dft_info, vec_dft_img);
			//覴���Ϣ��ͼ��������
			DeffectInfo tempInfo;
			HObject ho_tempImg;
			EnterCriticalSection(&pImgProc->m_csCalculateThread);
			for (int i = 0; i < (int)vec_dft_info.size(); i++) {
				tempInfo = vec_dft_info.at(i);
				tempInfo.image_index = (unsigned)pImgProc->m_unImageIndex;
				pImgProc->m_listDftInfo.push_back(tempInfo);
				ho_tempImg = vec_dft_img.at(i);
				pImgProc->m_listDftImg.push_back(ho_tempImg);
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread);
		}
	}
	pImgProc->m_bThreadAlive[index] = FALSE;
	if (pImgProc->m_threadnum > 3) {
		DWORD dwStop = 0;
		dwStop = WaitForSingleObject(pImgProc->CalculateThread_4_StoppedEvent, INFINITE);
		switch (dwStop)
		{
		case WAIT_FAILED:
			return -1;
		case WAIT_OBJECT_0:
			pImgProc->CalculateThread_3_StoppedEvent.SetEvent();
			return 0;
		}
	}
	else
		pImgProc->CalculateThread_3_StoppedEvent.SetEvent();

	return 0;
}

UINT CImageProcessing::ImageCalculate4(LPVOID pParam)
{
	CImageProcessing *pImgProc = (CImageProcessing *)pParam;
	CSingleLock singlelock(&pImgProc->mutex);

	int index = 3;
	pImgProc->m_bThreadAlive[index] = TRUE;
	for (;;) {
		if (pImgProc->CheckReferenceImageAvilable())
			break;
		else {
			Sleep(200);
			continue;
		}
	}
	HImage hi_average = pImgProc->m_hi_average;
	HImage hi_deviation = pImgProc->m_hi_deviation;
	HImage hi_acquire;
	HTuple hv_SVMHandle;
	pImgProc->InitialClassify((CW2A)pImgProc->m_modelFileName.GetBuffer(), hv_SVMHandle);
	while (pImgProc->m_bThreadAlive[index]) {
		if (pImgProc->m_listAcquiredImage.empty()) {
			Sleep(10);
			continue;
		}
		else {
			std::vector<DeffectInfo> vec_dft_info;
			std::vector<HalconCpp::HObject> vec_dft_img;
			singlelock.Lock(1000);     //  û�����þ������Լ��ã��ѱ����þ͵���
			if (singlelock.IsLocked()) {
				hi_acquire = pImgProc->m_listAcquiredImage.front();
				pImgProc->m_listAcquiredImage.pop_front();
				pImgProc->m_unImageIndex += 1;
			}
			singlelock.Unlock();   //  ����


			//EnterCriticalSection(&pImgProc->m_csDefImgList4);
			//if (pImgProc->mtx.try_lock()) {
			//	hi_acquire = pImgProc->m_listAcquiredImage.front();
			//	pImgProc->m_listAcquiredImage.pop_front();
			//	pImgProc->m_unImageIndex += 1;
			//	pImgProc->mtx.unlock();
			//}
			//else
			//	continue;
			//LeaveCriticalSection(&pImgProc->m_csDefImgList4);

			//覴ü���㷨
			pImgProc->StandDeviationAlgorithm(hi_acquire, hv_SVMHandle, vec_dft_info, vec_dft_img);
			//覴���Ϣ��ͼ��������
			DeffectInfo tempInfo;
			HObject ho_tempImg;
			EnterCriticalSection(&pImgProc->m_csCalculateThread);
			for (int i = 0; i < (int)vec_dft_info.size(); i++) {
				tempInfo = vec_dft_info.at(i);
				tempInfo.image_index = (unsigned)pImgProc->m_unImageIndex;
				pImgProc->m_listDftInfo.push_back(tempInfo);
				ho_tempImg = vec_dft_img.at(i);
				pImgProc->m_listDftImg.push_back(ho_tempImg);
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread);
		}
	}
	pImgProc->m_bThreadAlive[index] = FALSE;
	if (pImgProc->m_threadnum > 4) {
		DWORD dwStop = 0;
		dwStop = WaitForSingleObject(pImgProc->CalculateThread_5_StoppedEvent, INFINITE);
		switch (dwStop)
		{
		case WAIT_FAILED:
			return -1;
		case WAIT_OBJECT_0:
			pImgProc->CalculateThread_4_StoppedEvent.SetEvent();
			return 0;
		}
	}
	else
		pImgProc->CalculateThread_4_StoppedEvent.SetEvent();

	return 0;
}

UINT CImageProcessing::ImageCalculate5(LPVOID pParam)
{
	CImageProcessing *pImgProc = (CImageProcessing *)pParam;
	CSingleLock singlelock(&pImgProc->mutex);

	int index = 4;
	pImgProc->m_bThreadAlive[index] = TRUE;
	for (;;) {
		if (pImgProc->CheckReferenceImageAvilable())
			break;
		else {
			Sleep(200);
			continue;
		}
	}
	HImage hi_average = pImgProc->m_hi_average;
	HImage hi_deviation = pImgProc->m_hi_deviation;
	HImage hi_acquire;
	HTuple hv_SVMHandle;
	pImgProc->InitialClassify((CW2A)pImgProc->m_modelFileName.GetBuffer(), hv_SVMHandle);
	while (pImgProc->m_bThreadAlive[index]) {
		if (pImgProc->m_listAcquiredImage.empty()) {
			Sleep(10);
			continue;
		}
		else {
			std::vector<DeffectInfo> vec_dft_info;
			std::vector<HalconCpp::HObject> vec_dft_img;
			singlelock.Lock(1000);     //  û�����þ������Լ��ã��ѱ����þ͵���
			if (singlelock.IsLocked()) {
				hi_acquire = pImgProc->m_listAcquiredImage.front();
				pImgProc->m_listAcquiredImage.pop_front();
				pImgProc->m_unImageIndex += 1;
			}
			singlelock.Unlock();   //  ����



			//EnterCriticalSection(&pImgProc->m_csDefImgList5);
			//if (pImgProc->mtx.try_lock()) {
			//	hi_acquire = pImgProc->m_listAcquiredImage.front();
			//	pImgProc->m_listAcquiredImage.pop_front();
			//	pImgProc->m_unImageIndex += 1;
			//	pImgProc->mtx.unlock();
			//}
			//else
			//	continue;
			//LeaveCriticalSection(&pImgProc->m_csDefImgList5);

			//覴ü���㷨
			pImgProc->StandDeviationAlgorithm(hi_acquire, hv_SVMHandle, vec_dft_info, vec_dft_img);
			//覴���Ϣ��ͼ��������
			DeffectInfo tempInfo;
			HObject ho_tempImg;
			EnterCriticalSection(&pImgProc->m_csCalculateThread);
			for (int i = 0; i < (int)vec_dft_info.size(); i++) {
				tempInfo = vec_dft_info.at(i);
				tempInfo.image_index = (unsigned)pImgProc->m_unImageIndex;
				pImgProc->m_listDftInfo.push_back(tempInfo);
				ho_tempImg = vec_dft_img.at(i);
				pImgProc->m_listDftImg.push_back(ho_tempImg);
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread);
		}
	}
	pImgProc->m_bThreadAlive[index] = FALSE;
	pImgProc->CalculateThread_5_StoppedEvent.SetEvent();

	return 0;
}
