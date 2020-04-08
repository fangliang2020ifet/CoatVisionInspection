
#include "stdafx.h"
#include "CImageProcess.h"
#include "Log.h"

CImageProcess::CImageProcess()
{
	InitializeCriticalSection(&m_csCalculateThread1);
	InitializeCriticalSection(&m_csCalculateThread2);
	InitializeCriticalSection(&m_csCalculateThread3);
	InitializeCriticalSection(&m_csCalculateThread4);
	//InitializeCriticalSection(&m_csNO_dft);
	m_referenceImage_OK = FALSE;
	m_camera1_reference_image_acquired = FALSE;
	m_camera2_reference_image_acquired = FALSE;
	m_camera3_reference_image_acquired = FALSE;
	m_camera4_reference_image_acquired = FALSE;

}

CImageProcess::~CImageProcess()
{
	DeleteCriticalSection(&m_csCalculateThread1);
	DeleteCriticalSection(&m_csCalculateThread2);
	DeleteCriticalSection(&m_csCalculateThread3);
	DeleteCriticalSection(&m_csCalculateThread4);
	//DeleteCriticalSection(&m_csNO_dft);

	try
	{
		m_ImgList1_1.clear();
		m_ImgList1_2.clear();
		m_ImgList1_3.clear();
		m_ImgList1_4.clear();
		m_ImgList1_5.clear();
		m_ImgList2_1.clear();
		m_ImgList2_2.clear();
		m_ImgList2_3.clear();
		m_ImgList2_4.clear();
		m_ImgList2_5.clear();
		m_ImgList3_1.clear();
		m_ImgList3_2.clear();
		m_ImgList3_3.clear();
		m_ImgList3_4.clear();
		m_ImgList3_5.clear();
		m_ImgList4_1.clear();
		m_ImgList4_2.clear();
		m_ImgList4_3.clear();
		m_ImgList4_4.clear();
		m_ImgList4_5.clear();
		m_DFTList1.clear();
		m_DFTList2.clear();
		m_DFTList3.clear();
		m_DFTList4.clear();
		m_Sorted_DFTList.clear();

	}
	catch (...){}
	   
}


BOOL CImageProcess::BeginProcess()
{
	std::string str_path;
	if (!GetSavePath(str_path)) return FALSE;
	
	//�����߳�
	if (!(m_ReferenceImage = AfxBeginThread(ReferenceImage, this))) {
		Win::log("����ͼ��ο�ͼ�����߳�ʧ��");
		return FALSE;
	}

	if (!(m_CalculateThread1_1 = AfxBeginThread(ImageCalculate1_1, this))) {
		Win::log("����ͼ�����߳�1_1ʧ��");
		return FALSE;
	}
	
	if (!(m_CalculateThread1_2 = AfxBeginThread(ImageCalculate1_2, this))) {
		Win::log("����ͼ�����߳�1_2ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread1_3 = AfxBeginThread(ImageCalculate1_3, this))) {
		Win::log("����ͼ�����߳�1_3ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread1_4 = AfxBeginThread(ImageCalculate1_4, this))) {
		Win::log("����ͼ�����߳�1_4ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread1_5 = AfxBeginThread(ImageCalculate1_5, this))) {
		Win::log("����ͼ�����߳�1_5ʧ��");
		return FALSE;
	}

	if (!(m_CalculateThread2_1 = AfxBeginThread(ImageCalculate2_1, this))) {
		Win::log("����ͼ�����߳�1_1ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread2_2 = AfxBeginThread(ImageCalculate2_2, this))) {
		Win::log("����ͼ�����߳�1_2ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread2_3 = AfxBeginThread(ImageCalculate2_3, this))) {
		Win::log("����ͼ�����߳�2_3ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread2_4 = AfxBeginThread(ImageCalculate2_4, this))) {
		Win::log("����ͼ�����߳�2_4ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread2_5 = AfxBeginThread(ImageCalculate2_5, this))) {
		Win::log("����ͼ�����߳�2_5ʧ��");
		return FALSE;
	}

	if (!(m_CalculateThread3_1 = AfxBeginThread(ImageCalculate3_1, this))) {
		Win::log("����ͼ�����߳�3_1ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread3_2 = AfxBeginThread(ImageCalculate3_2, this))) {
		Win::log("����ͼ�����߳�3_2ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread3_3 = AfxBeginThread(ImageCalculate3_3, this))) {
		Win::log("����ͼ�����߳�3_3ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread3_4 = AfxBeginThread(ImageCalculate3_4, this))) {
		Win::log("����ͼ�����߳�3_4ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread3_5 = AfxBeginThread(ImageCalculate3_5, this))) {
		Win::log("����ͼ�����߳�3_5ʧ��");
		return FALSE;
	}

	if (!(m_CalculateThread4_1 = AfxBeginThread(ImageCalculate4_1, this))) {
		Win::log("����ͼ�����߳�4_1ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread4_2 = AfxBeginThread(ImageCalculate4_2, this))) {
		Win::log("����ͼ�����߳�4_2ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread4_3 = AfxBeginThread(ImageCalculate4_3, this))) {
		Win::log("����ͼ�����߳�4_3ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread4_4 = AfxBeginThread(ImageCalculate4_4, this))) {
		Win::log("����ͼ�����߳�4_4ʧ��");
		return FALSE;
	}
	if (!(m_CalculateThread4_5 = AfxBeginThread(ImageCalculate4_5, this))) {
		Win::log("����ͼ�����߳�4_5ʧ��");
		return FALSE;
	}
	

	Win::log("����ͼ�����߳�");


	return TRUE;
}

BOOL CImageProcess::StopProcess()
{
	while (is_reference_thread_alive)
	{
		is_reference_thread_alive = FALSE;
	}

	while (is_thread1_1_alive || is_thread2_1_alive || is_thread3_1_alive || is_thread4_1_alive
		|| is_thread1_2_alive || is_thread2_2_alive || is_thread3_2_alive || is_thread4_2_alive
		|| is_thread1_3_alive || is_thread2_3_alive || is_thread3_3_alive || is_thread4_3_alive
		|| is_thread1_4_alive || is_thread2_4_alive || is_thread3_4_alive || is_thread4_4_alive
		|| is_thread1_5_alive || is_thread2_5_alive || is_thread3_5_alive || is_thread4_5_alive)

	{
		is_thread1_1_alive = FALSE;
		is_thread1_2_alive = FALSE;
		is_thread1_3_alive = FALSE;
		is_thread1_4_alive = FALSE;
		is_thread1_5_alive = FALSE;

		is_thread2_1_alive = FALSE;
		is_thread2_2_alive = FALSE;
		is_thread2_3_alive = FALSE;
		is_thread2_4_alive = FALSE;
		is_thread2_5_alive = FALSE;

		is_thread3_1_alive = FALSE;
		is_thread3_2_alive = FALSE;
		is_thread3_3_alive = FALSE;
		is_thread3_4_alive = FALSE;
		is_thread3_5_alive = FALSE;

		is_thread4_1_alive = FALSE;
		is_thread4_2_alive = FALSE;
		is_thread4_3_alive = FALSE;
		is_thread4_4_alive = FALSE;
		is_thread4_5_alive = FALSE;
	}

	return TRUE;
}

void CImageProcess::RestartProcess()
{
	m_DFTList1.clear();
	m_DFTList2.clear();
	m_DFTList3.clear();
	m_DFTList4.clear();
	m_Sorted_DFTList.clear();

	m_NO_IMG = 0;
	m_NO_produced1 = 0;
	m_NO_produced2 = 0;
	m_NO_produced3 = 0;
	m_NO_produced4 = 0;
	m_current_position = 0.0f;
	m_NO_dft = 0;

	return;
}

BOOL CImageProcess::IsThreadsAlive()
{
	if (is_reference_thread_alive 
		|| is_thread1_1_alive || is_thread2_1_alive || is_thread3_1_alive || is_thread4_1_alive
		|| is_thread1_2_alive || is_thread2_2_alive || is_thread3_2_alive || is_thread4_2_alive
		|| is_thread1_3_alive || is_thread2_3_alive || is_thread3_3_alive || is_thread4_3_alive
		|| is_thread1_4_alive || is_thread2_4_alive || is_thread3_4_alive || is_thread4_4_alive
		|| is_thread1_5_alive || is_thread2_5_alive || is_thread3_5_alive || is_thread4_5_alive)
		return TRUE;
	else
		return FALSE;
}

BOOL CImageProcess::IsFileExist(const std::string &filename)
{
	std::ifstream fin(filename);
	if (!fin)
		return FALSE;
	else
		return TRUE;
}

BOOL CImageProcess::IsPathExist(const std::string &pathname)
{
	DWORD ftyp = GetFileAttributesA(pathname.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}

BOOL CImageProcess::LoadRefImage(std::string folder_path)
{
	if (!IsPathExist(folder_path)) {
		Win::log("����Ŀ¼������");
		return FALSE;
	}
	//���ù���·��
	file_path = folder_path;

	//��ȡ�ο�ͼ��1
	if (1) {
		std::string ref_image_name1 = "reference_image1.bmp";
		HTuple hv_ref_image_name1 = (HTuple)((folder_path + ref_image_name1).c_str());
		if (!IsFileExist(folder_path + ref_image_name1)) {
			Win::log("�ο�ͼ��1������");
			return FALSE;
		}
		HImage img1;
		ReadImage(&img1, hv_ref_image_name1);
		//HalconCpp::MedianImage(img1, &m_hi_ref1, "circle", 1, "mirrored");
		//��˹�˲�
		//HalconCpp::GaussFilter(img1, &m_hi_ref1, 5);
		m_hi_ref1 = img1;

		HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
		HTuple  hv_Deviation, hv_StandardDeviation;
		HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
		HalconCpp::GetImageSize(img1, &hv_Width, &hv_Height);
		HalconCpp::MedianImage(img1, &ho_ImageMedian, "square", 1, "mirrored");
		//���� ȫ 0 ֵͼ��
		HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
		HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
		HTuple end_val8 = hv_Width - 1;
		HTuple step_val8 = 1;
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
		{
			//����2�����Ͻǵ���У�����3�����Ͻǵ���У�����4�����½ǵ���У�����5�����½ǵ����
			HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
			//��ֵ/����/��׼��
			HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
			HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
			HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
			HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
				((m_camera1_standart_deviation * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
				"fill");
		}
		m_hi_average1 = ho_ImageAverage;
		m_hi_deviation1 = ho_ImageDeviation;

	}
	m_camera1_reference_image_acquired = TRUE;

	//��ȡ�ο�ͼ��2
	if (1) {
		std::string ref_image_name2 = "reference_image2.bmp";
		HTuple hv_ref_image_name2 = (HTuple)((folder_path + ref_image_name2).c_str());
		if (!IsFileExist(folder_path + ref_image_name2)) {
			Win::log(L"�ο�ͼ��2������");
			return FALSE;
		}
		HImage img2;
		ReadImage(&img2, hv_ref_image_name2);
		//HalconCpp::MedianImage(img2, &m_hi_ref2, "circle", 1, "mirrored");
			//��˹�˲�
		//HalconCpp::GaussFilter(img2, &m_hi_ref2, 5);
		m_hi_ref2 = img2;

		HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
		HTuple  hv_Deviation, hv_StandardDeviation;
		HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
		HalconCpp::GetImageSize(img2, &hv_Width, &hv_Height);
		HalconCpp::MedianImage(img2, &ho_ImageMedian, "square", 1, "mirrored");
		//���� ȫ 0 ֵͼ��
		HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
		HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
		HTuple end_val8 = hv_Width - 1;
		HTuple step_val8 = 1;
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
		{
			//����2�����Ͻǵ���У�����3�����Ͻǵ���У�����4�����½ǵ���У�����5�����½ǵ����
			HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
			//��ֵ/����/��׼��
			HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
			HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
			HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
			HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
				((m_camera2_standart_deviation * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
				"fill");
		}
		m_hi_average2 = ho_ImageAverage;
		m_hi_deviation2 = ho_ImageDeviation;
	}
	m_camera2_reference_image_acquired = TRUE;


	//��ȡ�ο�ͼ��3
	if (1) {
		std::string ref_image_name3 = "reference_image3.bmp";
		HTuple hv_ref_image_name3 = (HTuple)((folder_path + ref_image_name3).c_str());
		if (!IsFileExist(folder_path + ref_image_name3)) {
			Win::log(L"�ο�ͼ��3������");
			return FALSE;
		}
		HImage img3;
		ReadImage(&img3, hv_ref_image_name3);
		//HalconCpp::MedianImage(img3, &m_hi_ref3, "circle", 1, "mirrored");
			//��˹�˲�
		//HalconCpp::GaussFilter(img3, &m_hi_ref3, 5);
		m_hi_ref3 = img3;

		HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
		HTuple  hv_Deviation, hv_StandardDeviation;
		HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
		HalconCpp::GetImageSize(img3, &hv_Width, &hv_Height);
		HalconCpp::MedianImage(img3, &ho_ImageMedian, "square", 1, "mirrored");
		//���� ȫ 0 ֵͼ��
		HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
		HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
		HTuple end_val8 = hv_Width - 1;
		HTuple step_val8 = 1;
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
		{
			//����2�����Ͻǵ���У�����3�����Ͻǵ���У�����4�����½ǵ���У�����5�����½ǵ����
			HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
			//��ֵ/����/��׼��
			HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
			HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
			HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
			HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
				((m_camera3_standart_deviation * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
				"fill");
		}
		m_hi_average3 = ho_ImageAverage;
		m_hi_deviation3 = ho_ImageDeviation;
	}
	m_camera3_reference_image_acquired = TRUE;

	//��ȡ�ο�ͼ��4
	if (1) {
		std::string ref_image_name4 = "reference_image4.bmp";
		HTuple hv_ref_image_name4 = (HTuple)((folder_path + ref_image_name4).c_str());
		if (!IsFileExist(folder_path + ref_image_name4)) {
			Win::log(L"�ο�ͼ��4������");
			return FALSE;
		}
		HImage img4;
		ReadImage(&img4, hv_ref_image_name4);
		//HalconCpp::MedianImage(img4, &m_hi_ref4, "circle", 1, "mirrored");
			//��˹�˲�
		//HalconCpp::GaussFilter(img4, &m_hi_ref4, 5);
		m_hi_ref4 = img4;

		HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
		HTuple  hv_Deviation, hv_StandardDeviation;
		HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
		HalconCpp::GetImageSize(img4, &hv_Width, &hv_Height);
		HalconCpp::MedianImage(img4, &ho_ImageMedian, "square", 1, "mirrored");
		//���� ȫ 0 ֵͼ��
		HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
		HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
		HTuple end_val8 = hv_Width - 1;
		HTuple step_val8 = 1;
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
		{
			//����2�����Ͻǵ���У�����3�����Ͻǵ���У�����4�����½ǵ���У�����5�����½ǵ����
			HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
			//��ֵ/����/��׼��
			HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
			HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
			HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
			HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
				((m_camera4_standart_deviation * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
				"fill");
		}
		m_hi_average4 = ho_ImageAverage;
		m_hi_deviation4 = ho_ImageDeviation;
	}
	m_camera4_reference_image_acquired = TRUE;


	return TRUE;
}

//��ͼ����ص��ڴ����
BOOL CImageProcess::LoadImageToQueue(std::string folder_path, int numbers)
{
	if (!IsPathExist(folder_path)) {
		AfxMessageBox(L"����Ŀ¼������");
		return FALSE;
	}
	//���ù���·��
	file_path = folder_path;

	//��ȡ�ο�ͼ��
	std::string ref_image_name = "reference_image.bmp";
	HTuple hv_ref_image_name = (HTuple)((folder_path + ref_image_name).c_str());
	if (!IsFileExist(folder_path + ref_image_name)) {
		AfxMessageBox(L"�ο�ͼ�񲻴���");
		return FALSE;
	}
	ReadImage(&m_hi_ref1, hv_ref_image_name);

	//��ȡ����ͼ��
	folder_path = folder_path + "0";
	HObject ho_image;	
	for (int i = 1; i <= numbers; i++)
	{	
		HTuple hv_image_name;
		std::string image_name = "";
		if (i < 10) {
			image_name = "0" + std::to_string(i);
			image_name = image_name + ".bmp";
		}
		else {
			image_name = std::to_string(i) + ".bmp";
		}

		hv_image_name = (HTuple)((folder_path + image_name).c_str());
		if (!IsFileExist(folder_path + image_name)) {
			AfxMessageBox(L"����ͼ��Ϊ��\n����ͼ�������Ƿ��001��ʼ");
			return FALSE;
		}
		ReadImage(&ho_image, hv_image_name);

		m_ImgList1_1.push_back(ho_image);
	}

	return TRUE;
}

BOOL CImageProcess::LoadOneImageToQueue(std::string folder_path, int next_number)
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
	m_ImgList1_1.push_back(ho_image);

	return TRUE;
}

BOOL CImageProcess::LoadSingleImage(std::string image_name)
{
	std::string temp1 = "_1.bmp";
	std::string temp2 = "_2.bmp";
	std::string temp3 = "_3.bmp";
	std::string temp4 = "_4.bmp";

	HTuple hv_image_name1, hv_image_name2, hv_image_name3, hv_image_name4;
	hv_image_name1 = (HTuple)((image_name+temp1).c_str());
	//ReadImage(&ho_test1, hv_image_name1);
	m_hi_test1.ReadImage(hv_image_name1);

	hv_image_name2 = (HTuple)((image_name + temp2).c_str());
	//ReadImage(&ho_test2, hv_image_name2);
	m_hi_test2.ReadImage(hv_image_name2);

	hv_image_name3 = (HTuple)((image_name + temp3).c_str());
	//ReadImage(&ho_test3, hv_image_name3);
	m_hi_test3.ReadImage(hv_image_name3);

	hv_image_name4 = (HTuple)((image_name + temp4).c_str());
	//ReadImage(&ho_test4, hv_image_name4);
	m_hi_test4.ReadImage(hv_image_name4);

	Win::log("��ȡtestͼ�����");

	return TRUE;
}

HObject CImageProcess::CopyHobject(HObject ho_image)
{
	HObject copy;
	CopyImage(ho_image, &copy);

	return copy;
}

int CImageProcess::CheckTotalListSize()
{
	auto size = m_ImgList1_1.size() + m_ImgList1_2.size() + m_ImgList1_3.size() + m_ImgList1_4.size() + m_ImgList1_5.size()
		+ m_ImgList2_1.size() + m_ImgList2_2.size() + m_ImgList2_3.size() + m_ImgList2_4.size() + m_ImgList2_5.size()
		+ m_ImgList3_1.size() + m_ImgList3_2.size() + m_ImgList3_3.size() + m_ImgList3_4.size() + m_ImgList3_5.size()
		+ m_ImgList4_1.size() + m_ImgList4_2.size() + m_ImgList4_3.size() + m_ImgList4_4.size() + m_ImgList4_5.size();

	return (int)size;
}

BOOL CImageProcess::GenerateReferenceImage1(HImage &hi_average, HImage &hi_deviation)
{
	HImage result;
	HImage img1, img2, img3, img4, img5;
	if (!m_ImgList1_1.empty()) {
		img1 = m_ImgList1_1.front();
		m_ImgList1_1.pop_front();
	}
	else return FALSE;

	if (m_ImgList1_2.size() > 1) {
		m_ImgList1_2.pop_front();
		img2 = m_ImgList1_2.front();
		m_ImgList1_2.pop_front();
	}
	else return FALSE;

	if (!m_ImgList1_3.empty()) {
		img3 = m_ImgList1_3.front();
		m_ImgList1_3.pop_front();
	}
	else return FALSE;

	if (!m_ImgList1_4.empty()) {
		img4 = m_ImgList1_4.front();
		m_ImgList1_4.pop_front();
	}
	else return FALSE;

	if (!m_ImgList1_5.empty()) {
		img5 = m_ImgList1_5.front();
		m_ImgList1_5.pop_front();
	}
	else return FALSE;

	HalconCpp::AddImage(img1, img2, &result, 0.5, 0);
	HalconCpp::AddImage(result, img3, &result, 0.5, 0);
	HalconCpp::AddImage(result, img4, &result, 0.5, 0);
	HalconCpp::AddImage(result, img5, &result, 0.5, 0);
	//ho_Image_ref1 = result;
	////��ֵ�˲�
	//HalconCpp::MedianImage(result, &hi_ref, "circle", 1, "mirrored");
	//��˹�˲�
	//HalconCpp::GaussFilter(result, &hi_ref, 5);
	m_hi_ref1 = result;

	HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
	HTuple  hv_Deviation, hv_StandardDeviation;
	HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
	HalconCpp::GetImageSize(result, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(result, &ho_ImageMedian, "square", 1, "mirrored");
	//���� ȫ 0 ֵͼ��
	HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
	HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
	HTuple end_val8 = hv_Width - 1;
	HTuple step_val8 = 1;
	for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
	{
		//����2�����Ͻǵ���У�����3�����Ͻǵ���У�����4�����½ǵ���У�����5�����½ǵ����
		HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
		//��ֵ/����/��׼��
		HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
		HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
		HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
		HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
			((m_camera1_standart_deviation * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
			"fill");
	}
	hi_average = ho_ImageAverage;
	hi_deviation = ho_ImageDeviation;

	m_camera1_reference_image_acquired = TRUE;

	return TRUE;
}

BOOL CImageProcess::GenerateReferenceImage2(HImage &hi_average, HImage &hi_deviation)
{
	HImage result;
	HImage img1, img2, img3, img4, img5;
	if (!m_ImgList2_1.empty()) {
		img1 = m_ImgList2_1.front();
		m_ImgList2_1.pop_front();
	}
	else return FALSE;

	if (m_ImgList2_2.size() > 1) {
		m_ImgList2_2.pop_front();
		img2 = m_ImgList2_2.front();
		m_ImgList2_2.pop_front();
	}
	else return FALSE;

	if (!m_ImgList2_3.empty()) {
		img3 = m_ImgList2_3.front();
		m_ImgList2_3.pop_front();
	}
	else return FALSE;

	if (!m_ImgList2_4.empty()) {
		img4 = m_ImgList2_4.front();
		m_ImgList2_4.pop_front();
	}
	else return FALSE;

	if (!m_ImgList2_5.empty()) {
		img5 = m_ImgList2_5.front();
		m_ImgList2_5.pop_front();
	}
	else return FALSE;

	HalconCpp::AddImage(img1, img2, &result, 0.5, 0);
	HalconCpp::AddImage(result, img3, &result, 0.5, 0);
	HalconCpp::AddImage(result, img4, &result, 0.5, 0);
	HalconCpp::AddImage(result, img5, &result, 0.5, 0);
	//ho_Image_ref2 = result;
	//HalconCpp::MedianImage(result, &hi_ref, "circle", 1, "mirrored");
		//��˹�˲�
	//HalconCpp::GaussFilter(result, &hi_ref, 5);
	m_hi_ref2 = result;

	HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
	HTuple  hv_Deviation, hv_StandardDeviation;
	HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
	HalconCpp::GetImageSize(result, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(result, &ho_ImageMedian, "square", 1, "mirrored");
	//���� ȫ 0 ֵͼ��
	HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
	HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
	HTuple end_val8 = hv_Width - 1;
	HTuple step_val8 = 1;
	for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
	{
		//����2�����Ͻǵ���У�����3�����Ͻǵ���У�����4�����½ǵ���У�����5�����½ǵ����
		HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
		//��ֵ/����/��׼��
		HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
		HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
		HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
		HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
			((m_camera2_standart_deviation * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
			"fill");
	}
	hi_average = ho_ImageAverage;
	hi_deviation = ho_ImageDeviation;

	m_camera2_reference_image_acquired = TRUE;

	return TRUE;
}

BOOL CImageProcess::GenerateReferenceImage3(HImage &hi_average, HImage &hi_deviation)
{
	HImage result;
	HImage img1, img2, img3, img4, img5;
	if (!m_ImgList3_1.empty()) {
		img1 = m_ImgList3_1.front();
		m_ImgList3_1.pop_front();
	}
	else return FALSE;

	if (m_ImgList3_2.size() > 1) {
		m_ImgList3_2.pop_front();
		img2 = m_ImgList3_2.front();
		m_ImgList3_2.pop_front();
	}
	else return FALSE;

	if (!m_ImgList3_3.empty()) {
		img3 = m_ImgList3_3.front();
		m_ImgList3_3.pop_front();
	}
	else return FALSE;

	if (!m_ImgList3_4.empty()) {
		img4 = m_ImgList3_4.front();
		m_ImgList3_4.pop_front();
	}
	else return FALSE;

	if (!m_ImgList3_5.empty()) {
		img5 = m_ImgList3_5.front();
		m_ImgList3_5.pop_front();
	}
	else return FALSE;

	HalconCpp::AddImage(img1, img2, &result, 0.5, 0);
	HalconCpp::AddImage(result, img3, &result, 0.5, 0);
	HalconCpp::AddImage(result, img4, &result, 0.5, 0);
	HalconCpp::AddImage(result, img5, &result, 0.5, 0);
	//ho_Image_ref3 = result;
	//HalconCpp::MedianImage(result, &hi_ref, "circle", 1, "mirrored");
		//��˹�˲�
	//HalconCpp::GaussFilter(result, &hi_ref, 5);
	m_hi_ref3 = result;

	HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
	HTuple  hv_Deviation, hv_StandardDeviation;
	HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
	HalconCpp::GetImageSize(result, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(result, &ho_ImageMedian, "square", 1, "mirrored");
	//���� ȫ 0 ֵͼ��
	HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
	HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
	HTuple end_val8 = hv_Width - 1;
	HTuple step_val8 = 1;
	for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
	{
		//����2�����Ͻǵ���У�����3�����Ͻǵ���У�����4�����½ǵ���У�����5�����½ǵ����
		HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
		//��ֵ/����/��׼��
		HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
		HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
		HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
		HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
			((m_camera3_standart_deviation * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
			"fill");
	}
	hi_average = ho_ImageAverage;
	hi_deviation = ho_ImageDeviation;
	m_camera3_reference_image_acquired = TRUE;

	return TRUE;
}

BOOL CImageProcess::GenerateReferenceImage4(HImage &hi_average, HImage &hi_deviation)
{
	HImage result;
	HImage img1, img2, img3, img4, img5;
	if (!m_ImgList4_1.empty()) {
		img1 = m_ImgList4_1.front();
		m_ImgList4_1.pop_front();
	}
	else return FALSE;

	if (m_ImgList4_2.size() > 1) {
		m_ImgList4_2.pop_front();
		img2 = m_ImgList4_2.front();
		m_ImgList4_2.pop_front();
	}
	else return FALSE;

	if (!m_ImgList4_3.empty()) {
		img3 = m_ImgList4_3.front();
		m_ImgList4_3.pop_front();
	}
	else return FALSE;

	if (!m_ImgList4_4.empty()) {
		img4 = m_ImgList4_4.front();
		m_ImgList4_4.pop_front();
	}
	else return FALSE;

	if (!m_ImgList4_5.empty()) {
		img5 = m_ImgList4_5.front();
		m_ImgList4_5.pop_front();
	}
	else return FALSE;

	HalconCpp::AddImage(img1, img2, &result, 0.5, 0);
	HalconCpp::AddImage(result, img3, &result, 0.5, 0);
	HalconCpp::AddImage(result, img4, &result, 0.5, 0);
	HalconCpp::AddImage(result, img5, &result, 0.5, 0);
	//ho_Image_ref4 = result;
	//HalconCpp::MedianImage(result, &hi_ref, "circle", 1, "mirrored");
		//��˹�˲�
	//HalconCpp::GaussFilter(result, &hi_ref, 5);
	m_hi_ref4 = result;

	HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
	HTuple  hv_Deviation, hv_StandardDeviation;
	HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
	HalconCpp::GetImageSize(result, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(result, &ho_ImageMedian, "square", 1, "mirrored");
	//���� ȫ 0 ֵͼ��
	HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
	HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
	HTuple end_val8 = hv_Width - 1;
	HTuple step_val8 = 1;
	for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
	{
		//����2�����Ͻǵ���У�����3�����Ͻǵ���У�����4�����½ǵ���У�����5�����½ǵ����
		HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
		//��ֵ/����/��׼��
		HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
		HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
		HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
		HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
			((m_camera4_standart_deviation * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
			"fill");
	}
	hi_average = ho_ImageAverage;
	hi_deviation = ho_ImageDeviation;

	m_camera4_reference_image_acquired = TRUE;

	return TRUE;
}

BOOL CImageProcess::CheckReferenceImageState()
{
	if (m_camera1_reference_image_acquired && m_camera2_reference_image_acquired &&
		m_camera3_reference_image_acquired && m_camera4_reference_image_acquired)
		return TRUE;
	else return FALSE;
}

int CImageProcess::ProduceReferenceImage1(HImage hi_ref1, HImage hi_ref2)
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

int CImageProcess::ProduceReferenceImage4(HImage hi_ref4, HImage hi_ref3)
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

BOOL CImageProcess::SaveReferenceImage()
{
	SaveDefectImage(m_hi_ref1, "D:/SaveImage/ref1.bmp");
	SaveDefectImage(m_hi_ref2, "D:/SaveImage/ref2.bmp");
	SaveDefectImage(m_hi_ref3, "D:/SaveImage/ref3.bmp");
	SaveDefectImage(m_hi_ref4, "D:/SaveImage/ref4.bmp");

	return TRUE;
}

BOOL CImageProcess::GetSavePath(std::string &path)
{
	path = m_strPath;

	//��鱣��·���Ƿ����
	if (!IsPathExist(path)) {
		Win::log("ͼƬ����·��������");
		return FALSE;
	}
	else
		return TRUE;
}

//覴ö�������
void CImageProcess::ReSortDefectQueue()
{
	std::vector<DefectType> vSort;
	DefectType d1, d2, d3, d4;
	if (!m_DFTList1.empty()) {
		EnterCriticalSection(&m_csCalculateThread1);
		for (int i = 0; i < m_DFTList1.size(); i++)
		{
			d1 = m_DFTList1.front();
			m_DFTList1.pop_front();
			vSort.push_back(d1);
		}
		LeaveCriticalSection(&m_csCalculateThread1);

	}

	if (!m_DFTList2.empty()) {
		EnterCriticalSection(&m_csCalculateThread2);
		for (int i = 0; i < m_DFTList2.size(); i++)
		{
			d2 = m_DFTList2.front();
			m_DFTList2.pop_front();
			vSort.push_back(d2);
		}
		LeaveCriticalSection(&m_csCalculateThread2);

	}

	if (!m_DFTList3.empty()) {
		EnterCriticalSection(&m_csCalculateThread3);
		for (int i = 0; i < m_DFTList3.size(); i++)
		{
			d3 = m_DFTList3.front();
			m_DFTList3.pop_front();
			vSort.push_back(d3);
		}
		LeaveCriticalSection(&m_csCalculateThread3);

	}

	if (!m_DFTList4.empty()) {
		EnterCriticalSection(&m_csCalculateThread4);
		for (int i = 0; i < m_DFTList4.size(); i++)
		{
			d4 = m_DFTList4.front();
			m_DFTList4.pop_front();
			vSort.push_back(d4);
		}
		LeaveCriticalSection(&m_csCalculateThread4);

	}

	//����覴þ���λ����������覴ö���
	if (!vSort.empty()) {
		std::sort(vSort.begin(), vSort.end());
		std::vector<DefectType>::iterator it;
		for (it = vSort.begin(); it != vSort.end(); ++it)
		{
			m_Sorted_DFTList.push_back(*it);
		}
	}

	return;
}

//��������ͼ������
int CImageProcess::FindMaxProducedNO()
{
	int max1, max2;
	if (m_NO_produced1 < m_NO_produced2)
		max1 = m_NO_produced2;
	else
		max1 = m_NO_produced1;

	if (m_NO_produced3 < m_NO_produced4)
		max2 = m_NO_produced4;
	else
		max2 = m_NO_produced3;

	if (max1 < max2) {
		return max2;
	}
	else {
		max1 = m_NO_IMG;
		return max1;
	}
}

//�ָ���覴�ͼ����
DefectType CImageProcess::LocateDefectPosition(int camera_number, HObject ho_selectedregion, 
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
	else if (0 != (hv_average_Row > IMAGE_WIDTH -127))
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
	dtype.absolute_position = m_current_position + hv_average_Row.TupleInt() * VERTICAL_PRECISION / 1000.0f;
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

DefectType CImageProcess::LocateDefectPosition(int camera_number, HObject ho_selectedregion)
{
	DefectType dtype;   //�����⵽��覴���Ϣ


	return dtype;
}

//覴õȼ��ж�
int CImageProcess::RankDivide(DefectType dtype)
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

//�����㷨��������
int CImageProcess::ImageClassification(HObject ho_img)
{
	int kind = 0;
	HObject ho_region;
	HalconCpp::AutoThreshold(ho_img, &ho_region, 3);

	return kind;
}

//����覴�ͼ��
void CImageProcess::SaveDefectImage(HObject &ho_img, HTuple name)
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
int CImageProcess::DetectAlgorithem(int cameraNO, HImage hi_ref, HImage hi_img, std::vector<DefectType> &vDFT)
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

	Threshold(hi_img, &ho_Region_defth, 1, 255);
	Connection(ho_Region_defth, &ho_ConnectedRegions_defth);
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
			Connection(ho_Region, &ho_ConnectedRegions);
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
int CImageProcess::DetectAlgorithemSimple(int cameraNO, HImage hi_ref, HImage hi_img, std::vector<DefectType> &vDFT)
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
	GaussFilter(ho_Image_def, &ho_ImageGauss1, 5);

	//����ͼ - �ο�ͼ
	AbsDiffImage(ho_ImageGauss1, ho_Image_ref, &ho_ImageAbsDiff1, 1);

	//�Զ���ֵ����������ǵ�ͨ��ͼ�񣬻��ж���ֵ�ָSigma���ڶԻҶ�ֱ��ͼ���и�˹ƽ����������ƽ���ĳ̶ȣ��ָ�ϸ�³̶ȣ���
	//��sigma�ܴ�ʱ���Ҷ�ֱ��ͼ�����ᱻƽ��Ϊֻʣ��һ�����壬���ָ��Ǹ���ƽ����ֱ��ͼ�Ĳ��������еģ�SigmaС���ָ��Խϸ��
	//AutoThreshold(ho_ImageAbsDiff1, &ho_Regions, 3);

	//��һ��������Ĥ�������ص�ĻҶȷ�Χ
	GrayRangeRect(ho_ImageAbsDiff1, &ho_ImageResult, 10, 10);
	MinMaxGray(ho_ImageResult, ho_ImageResult, 0, &hv_Min, &hv_Max, &hv_Range);
	Threshold(ho_ImageResult, &ho_Regions, (HTuple(5.55).TupleConcat(hv_Max*0.8)).TupleMax(), 255);
	Connection(ho_Regions, &ho_ConnectedRegions);
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
				dtype.absolute_position = m_current_position + hv_Row.TupleInt() * VERTICAL_PRECISION / 1000.0f;   //��λ����
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
int CImageProcess::StandDeviationAlgorithm(int cameraNO, HImage hi_average, HImage hi_deviation,
											HImage hi_img, std::vector<DefectType> &vDFT)
{
	HObject  ho_Image, ho_ImageAverage, ho_ImageDeviation, ho_ImageMedianDFT;
	HObject  ho_ImageSub1, ho_ImageSub2, ho_ImageAddSub, ho_ImageResult;
	HObject  ho_Region, ho_ConnectedRegions, ho_SelectedRegions, ho_ObjectSelected, ho_Rectangle;
	HObject  ho_ImageReduced, ho_ImagePart;
	HTuple   hv_Width, hv_Height, hv_Number;
	HTuple   hv_i, hv_Area, hv_Row, hv_Column, hv_RowCircle, hv_ColumnCircle, hv_Radius, hv_Contlength;

	ho_Image = hi_img;
	ho_ImageAverage = hi_average;
	ho_ImageDeviation = hi_deviation;
	HalconCpp::GetImageSize(ho_Image, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(ho_Image, &ho_ImageMedianDFT, "square", 1, "mirrored");

	//�ο����ף�һ�ֻ��ڶ�Ŀ�����Ӿ��Ĺ�ѧ��Ĥ覴ü��ϵͳ
	//������������ֵС���㣬����ͼ�лᱻ��0��ͬ���������ֵ����255��Ҳ�ᱻ�ض�ʹ�����ֵΪ255
	//����λ�����������ӣ��൱�����
	HalconCpp::SubImage(ho_ImageMedianDFT, ho_ImageAverage, &ho_ImageSub1, 1, 0);
	HalconCpp::SubImage(ho_ImageAverage, ho_ImageMedianDFT, &ho_ImageSub2, 1, 0);
	HalconCpp::AddImage(ho_ImageSub1, ho_ImageSub2, &ho_ImageAddSub, 0.5, 0);
	HalconCpp::SubImage(ho_ImageAddSub, ho_ImageDeviation, &ho_ImageResult, 10, 0);
	HalconCpp::Threshold(ho_ImageResult, &ho_Region, 1, 255);
	HalconCpp::Connection(ho_Region, &ho_ConnectedRegions);
	HalconCpp::SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 9, 15999999);
	HalconCpp::CountObj(ho_SelectedRegions, &hv_Number);
	if (0 != hv_Number)
	{
		//������ͼ����ѡ������򳬹� 100 ��������Ϊ��ͼ����ʧ��
		if (hv_Number > 100) {
			Win::log("��� %d ����ʧ�ܣ�����ͼ���Ƿ�����", cameraNO);
			return -1;
		}

		std::vector<SelectRegion> vSelect;
		HTuple end_val19 = hv_Number;
		HTuple step_val19 = 1;
		for (hv_i = 1; hv_i.Continue(end_val19, step_val19); hv_i += step_val19)
		{
			HalconCpp::SelectObj(ho_SelectedRegions, &ho_ObjectSelected, hv_i);
			//�����Ƿ�Ҫ�滻ΪAreaCenterGray()����
			HalconCpp::AreaCenter(ho_ObjectSelected, &hv_Area, &hv_Row, &hv_Column);
			//��С���Բ
			HalconCpp::SmallestCircle(ho_ObjectSelected, &hv_RowCircle, &hv_ColumnCircle, &hv_Radius);
			//�����ܳ�
			HalconCpp::Contlength(ho_ObjectSelected, &hv_Contlength);			
			//��������ƽ��ֵ
			HTuple hv_mean, hv_deviation;
			HalconCpp::Intensity(ho_ObjectSelected, ho_Image, &hv_mean, &hv_deviation);

			SelectRegion region;
			region.index            = hv_i.TupleInt();
			region.hv_Row_Center    = hv_Row;
			region.hv_Column_Center = hv_Column;
			region.area             = (float)hv_Area.D();
			region.radius           = (float)hv_Radius.D();
			region.contlength       = (float)hv_Contlength.D();
			region.pixelvalue       = (float)hv_mean;
			vSelect.push_back(region);
		}
				
		//ɾ�����ھ���С��256������
		std::vector<SelectRegion>::iterator it;
		for (it = vSelect.begin(); it != vSelect.end(); it++)
		{
			if (it != vSelect.end() - 1) {
				HTuple x, y;
				HalconCpp::TupleAbs(it->hv_Row_Center - (it + 1)->hv_Row_Center, &y);
				HalconCpp::TupleAbs(it->hv_Column_Center - (it + 1)->hv_Column_Center, &x);
				if (x < 128 || y < 128) {
					//��ͬ����������������Ϊ 0
					it->area = 0.0f;
				}
			}
			else break;
		}
		
		//���������������覴�ͼ����Ϣ
		std::vector<SelectRegion>::reverse_iterator rit;
		for (rit = vSelect.rbegin(); rit != vSelect.rend(); rit++)
		{
			SelectRegion region;
			region = *rit;
			if (region.area == 0.0f)
				continue;
			else{
				//����ѡ��������������256*256=65536��������Χ����覴�ͼ��ĳߴ�ʱ��������ʵ��ͼ��
				if (region.area >= 65536.0f)
				{
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
					if (0 != (region.hv_Row_Center < 127))
					{
						region.hv_Row_Center = 127;
					}
					else if (0 != (region.hv_Row_Center > (hv_Height - 127)))
					{
						region.hv_Row_Center = hv_Height - 127;
					}
					if (0 != (region.hv_Column_Center < 127))
					{
						region.hv_Column_Center = 127;
					}
					else if (0 != (region.hv_Column_Center > (hv_Width - 127)))
					{
						region.hv_Column_Center = hv_Width - 127;
					}
					GenRectangle1(&ho_Rectangle, region.hv_Row_Center - 127, region.hv_Column_Center - 127,
						region.hv_Row_Center + 128, region.hv_Column_Center + 128);
					HalconCpp::ReduceDomain(ho_Image, ho_Rectangle, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
				}

				//覴���Ϣ����
				DefectType dtype;
				dtype.type              = ImageClassification(ho_ImagePart);
				dtype.center_x          = ((float)region.hv_Column_Center.D() + IMAGE_WIDTH * (cameraNO - 1))*HORIZON_PRECISION;
				dtype.absolute_position = m_current_position + (float)region.hv_Row_Center.D() * VERTICAL_PRECISION / 1000.0f;
				dtype.area              = region.area * HORIZON_PRECISION * VERTICAL_PRECISION;
				dtype.circle_radius     = region.radius * HORIZON_PRECISION;
				dtype.contlength        = region.contlength * HORIZON_PRECISION;
				dtype.pixel_value       = (int)region.pixelvalue;
				int rank = RankDivide(dtype);
				dtype.rank              = rank;
				//����覴���Ϣ
				vDFT.push_back(dtype);

				//ͼ�񱣴棬��ʽ���ļ���
				HTuple hv_path = (HTuple)m_strPath.c_str();
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

//�ο�ͼ�����߳�
UINT CImageProcess::ReferenceImage(LPVOID pParam)
{
	CImageProcess *pThis = (CImageProcess *)pParam;
	pThis->is_reference_thread_alive = TRUE;
	pThis->m_referenceImage_OK = FALSE;

	pThis->m_camera1_invalid_area = 0;
	pThis->m_camera1_invalid_area = 0;
	while (pThis->is_reference_thread_alive)
	{
		if (!pThis->TEST_MODEL) {
			if (!pThis->m_camera1_reference_image_acquired) {
				pThis->GenerateReferenceImage1(pThis->m_hi_average1, pThis->m_hi_deviation1);
				Win::log("��ȡ1#�ο�ͼ��");
			}
			if (!pThis->m_camera2_reference_image_acquired) {
				pThis->GenerateReferenceImage2(pThis->m_hi_average2, pThis->m_hi_deviation2);
				Win::log("��ȡ2#�ο�ͼ��");
			}
			if (!pThis->m_camera3_reference_image_acquired) {
				pThis->GenerateReferenceImage3(pThis->m_hi_average3, pThis->m_hi_deviation3);
				Win::log("��ȡ3#�ο�ͼ��");
			}
			if (!pThis->m_camera4_reference_image_acquired) {
				pThis->GenerateReferenceImage4(pThis->m_hi_average4, pThis->m_hi_deviation4);
				Win::log("��ȡ4#�ο�ͼ��");
			}
		}
		else {
			pThis->LoadRefImage("C:/DeVisionProject/sample0408/");
		}

		if (pThis->CheckReferenceImageState()) {

			if (pThis->SAVE_REFERENCE_IMAGE) {
				pThis->SaveDefectImage(pThis->m_hi_ref1, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image1.bmp");
				pThis->SaveDefectImage(pThis->m_hi_ref2, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image2.bmp");
				pThis->SaveDefectImage(pThis->m_hi_ref3, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image3.bmp");
				pThis->SaveDefectImage(pThis->m_hi_ref4, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image4.bmp");
			}
			
			if (pThis->REDUCE_BLACK_EDGE) {
				pThis->m_camera1_invalid_area = pThis->ProduceReferenceImage1(pThis->m_hi_ref1, pThis->m_hi_ref2);
				if (pThis->m_camera1_invalid_area != 0 && pThis->m_camera1_invalid_area != -1) {
					HObject ho_ImagePart, ho_Region, ho_ImageReduced;
					HTuple hv_width_ref1, hv_height_ref1;

					HalconCpp::GetImageSize(pThis->m_hi_ref1, &hv_width_ref1, &hv_height_ref1);
					HalconCpp::GenRectangle1(&ho_Region, 0, pThis->m_camera1_invalid_area, hv_height_ref1, hv_width_ref1);
					HalconCpp::ReduceDomain(pThis->m_hi_ref1, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					pThis->m_hi_ref1 = ho_ImagePart;
				}

				pThis->m_camera4_invalid_area = pThis->ProduceReferenceImage4(pThis->m_hi_ref4, pThis->m_hi_ref3);
				if (pThis->m_camera1_invalid_area != 0 && pThis->m_camera1_invalid_area != -1) {
					HObject ho_ImagePart, ho_Region, ho_ImageReduced;
					HTuple hv_width_ref4, hv_height_ref4;

					HalconCpp::GetImageSize(pThis->m_hi_ref4, &hv_width_ref4, &hv_height_ref4);
					HalconCpp::GenRectangle1(&ho_Region, 0, 0, hv_height_ref4, hv_width_ref4 - (HTuple)pThis->m_camera4_invalid_area);
					HalconCpp::ReduceDomain(pThis->m_hi_ref4, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					pThis->m_hi_ref4 = ho_ImagePart;
				}
			}

			pThis->m_referenceImage_OK = TRUE;
			pThis->is_reference_thread_alive = FALSE;
		}
		else
			Sleep(50);
	}
	pThis->is_reference_thread_alive = FALSE;

	return 0;
}

//  1# ��������߳�
UINT CImageProcess::ImageCalculate1_1(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread1_1_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while(!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref1;
	int valid_area = pImgProc->m_camera1_invalid_area;
	HImage hi_average = pImgProc->m_hi_average1;
	HImage hi_deviation = pImgProc->m_hi_deviation1;

	while (pImgProc->is_thread1_1_alive)
	{
		if(!pImgProc->m_ImgList1_1.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList1_1.front();
			pImgProc->m_ImgList1_1.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, valid_area, hv_height_def, hv_width_def);
					//�ض���ͼ��Ķ����򣬷���ͼ�� 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_1, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread1);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList1.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}			
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced1 += 1;
			//����λ����Ϣ: ��֡�� * ͼ��߶� * ���򾫶�
			pImgProc->m_current_position = (pImgProc->m_NO_produced1) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		}else{
			Sleep(5);
		}
	}

	//���ͼ�����
	pImgProc->m_ImgList1_1.clear();

	pImgProc->is_thread1_1_alive = FALSE;
	Win::log("1#��������߳� 1 ����");
	return 0;
}

UINT CImageProcess::ImageCalculate1_2(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread1_2_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref1;
	int valid_area = pImgProc->m_camera1_invalid_area;
	HImage hi_average = pImgProc->m_hi_average1;
	HImage hi_deviation = pImgProc->m_hi_deviation1;

	while (pImgProc->is_thread1_2_alive)
	{
		if (!pImgProc->m_ImgList1_2.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList1_2.front();
			pImgProc->m_ImgList1_2.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, valid_area, hv_height_def, hv_width_def);
					//�ض���ͼ��Ķ����򣬷���ͼ�� 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_1, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread1);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList1.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced1 += 1;
			//����λ����Ϣ: ��֡�� * ͼ��߶� * ���򾫶�
			pImgProc->m_current_position = (pImgProc->m_NO_produced1) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		} else {
			Sleep(5);
		}
	}
	//���ͼ�����
	pImgProc->m_ImgList1_2.clear();

	pImgProc->is_thread1_2_alive = FALSE;
	Win::log("1#��������߳� 2 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate1_3(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread1_3_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref1;
	int valid_area = pImgProc->m_camera1_invalid_area;
	HImage hi_average = pImgProc->m_hi_average1;
	HImage hi_deviation = pImgProc->m_hi_deviation1;

	while (pImgProc->is_thread1_3_alive)
	{
		if (!pImgProc->m_ImgList1_3.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList1_3.front();
			pImgProc->m_ImgList1_3.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, valid_area, hv_height_def, hv_width_def);
					//�ض���ͼ��Ķ����򣬷���ͼ�� 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_1, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread1);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList1.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced1 += 1;
			//����λ����Ϣ: ��֡�� * ͼ��߶� * ���򾫶�
			pImgProc->m_current_position = (pImgProc->m_NO_produced1) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);

		} else {
			Sleep(5);
		}
	}

	//���ͼ�����
	pImgProc->m_ImgList1_3.clear();

	pImgProc->is_thread1_3_alive = FALSE;
	Win::log("1#��������߳� 3 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate1_4(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread1_4_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref1;
	int valid_area = pImgProc->m_camera1_invalid_area;
	HImage hi_average = pImgProc->m_hi_average1;
	HImage hi_deviation = pImgProc->m_hi_deviation1;

	while (pImgProc->is_thread1_4_alive)
	{
		if (!pImgProc->m_ImgList1_4.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList1_4.front();
			pImgProc->m_ImgList1_4.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, valid_area, hv_height_def, hv_width_def);
					//�ض���ͼ��Ķ����򣬷���ͼ�� 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_1, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread1);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList1.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced1 += 1;
			//����λ����Ϣ: ��֡�� * ͼ��߶� * ���򾫶�
			pImgProc->m_current_position = (pImgProc->m_NO_produced1) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		} else {
			Sleep(5);
		}
	}

	//���ͼ�����
	pImgProc->m_ImgList1_4.clear();

	pImgProc->is_thread1_4_alive = FALSE;
	Win::log("1#��������߳� 4 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate1_5(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread1_5_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref1;
	int valid_area = pImgProc->m_camera1_invalid_area;
	HImage hi_average = pImgProc->m_hi_average1;
	HImage hi_deviation = pImgProc->m_hi_deviation1;

	std::vector<DefectType> vdef;
	while (pImgProc->is_thread1_5_alive)
	{
		if (!pImgProc->m_ImgList1_5.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList1_5.front();
			pImgProc->m_ImgList1_5.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, valid_area, hv_height_def, hv_width_def);
					//�ض���ͼ��Ķ����򣬷���ͼ�� 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_1, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread1);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList1.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced1 += 1;
			//����λ����Ϣ: ��֡�� * ͼ��߶� * ���򾫶�
			pImgProc->m_current_position = (pImgProc->m_NO_produced1) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		} else {
			Sleep(5);
		}
	}

	//���ͼ�����
	pImgProc->m_ImgList1_5.clear();

	pImgProc->is_thread1_5_alive = FALSE;
	Win::log("1#��������߳� 5 ����");

	return 0;
}

//  2# ��������߳�
UINT CImageProcess::ImageCalculate2_1(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread2_1_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref2;
	HImage hi_average = pImgProc->m_hi_average2;
	HImage hi_deviation = pImgProc->m_hi_deviation2;

	while (pImgProc->is_thread2_1_alive)
	{
		if (!pImgProc->m_ImgList2_1.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList2_1.front();
			pImgProc->m_ImgList2_1.pop_front();
			
			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_2, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_2, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread2);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList2.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced2 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		} else {
			Sleep(5);
		}
	}
	pImgProc->m_ImgList2_1.clear();

	pImgProc->is_thread2_1_alive = FALSE;
	Win::log("2#��������߳� 1 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate2_2(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread2_2_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref2;
	HImage hi_average = pImgProc->m_hi_average2;
	HImage hi_deviation = pImgProc->m_hi_deviation2;

	while (pImgProc->is_thread2_2_alive)
	{
		if (!pImgProc->m_ImgList2_2.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList2_2.front();
			pImgProc->m_ImgList2_2.pop_front();

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_2, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_2, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread2);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList2.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced2 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList2_2.clear();

	pImgProc->is_thread2_2_alive = FALSE;
	Win::log("2#��������߳� 2 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate2_3(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread2_3_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref2;
	HImage hi_average = pImgProc->m_hi_average2;
	HImage hi_deviation = pImgProc->m_hi_deviation2;

	while (pImgProc->is_thread2_3_alive)
	{
		if (!pImgProc->m_ImgList2_3.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList2_3.front();
			pImgProc->m_ImgList2_3.pop_front();

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_2, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_2, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread2);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList2.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced2 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList2_3.clear();

	pImgProc->is_thread2_3_alive = FALSE;
	Win::log("2#��������߳� 3 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate2_4(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread2_4_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref2;
	HImage hi_average = pImgProc->m_hi_average2;
	HImage hi_deviation = pImgProc->m_hi_deviation2;

	while (pImgProc->is_thread2_4_alive)
	{
		if (!pImgProc->m_ImgList2_4.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList2_4.front();
			pImgProc->m_ImgList2_4.pop_front();

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_2, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_2, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread2);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList2.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced2 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList2_4.clear();

	pImgProc->is_thread2_4_alive = FALSE;
	Win::log("2#��������߳� 4 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate2_5(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread2_5_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref2;
	HImage hi_average = pImgProc->m_hi_average2;
	HImage hi_deviation = pImgProc->m_hi_deviation2;

	while (pImgProc->is_thread2_5_alive)
	{
		if (!pImgProc->m_ImgList2_5.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList2_5.front();
			pImgProc->m_ImgList2_5.pop_front();

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_2, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_2, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread2);

			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList2.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced2 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList2_5.clear();

	pImgProc->is_thread2_5_alive = FALSE;
	Win::log("2#��������߳� 5 ����");

	return 0;
}

//  3# ��������߳�
UINT CImageProcess::ImageCalculate3_1(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread3_1_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref3;
	HImage hi_average = pImgProc->m_hi_average3;
	HImage hi_deviation = pImgProc->m_hi_deviation3;

	while (pImgProc->is_thread3_1_alive)
	{
		if (!pImgProc->m_ImgList3_1.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList3_1.front();
			pImgProc->m_ImgList3_1.pop_front();

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_3, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_3, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread3);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList3.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced3 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList3_1.clear();

	pImgProc->is_thread3_1_alive = FALSE;
	Win::log("3#��������߳� 1 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate3_2(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread3_2_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref3;
	HImage hi_average = pImgProc->m_hi_average3;
	HImage hi_deviation = pImgProc->m_hi_deviation3;

	while (pImgProc->is_thread3_2_alive)
	{
		if (!pImgProc->m_ImgList3_2.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList3_2.front();
			pImgProc->m_ImgList3_2.pop_front();

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_3, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_3, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread3);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList3.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced3 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList3_2.clear();

	pImgProc->is_thread3_2_alive = FALSE;
	Win::log("3#��������߳� 2 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate3_3(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread3_3_alive = TRUE;
	pImgProc->m_NO_produced3 = 0;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref3;
	HImage hi_average = pImgProc->m_hi_average3;
	HImage hi_deviation = pImgProc->m_hi_deviation3;

	while (pImgProc->is_thread3_3_alive)
	{
		if (!pImgProc->m_ImgList3_3.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList3_3.front();
			pImgProc->m_ImgList3_3.pop_front();

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_3, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_3, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread3);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList3.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced3 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList3_3.clear();

	pImgProc->is_thread3_3_alive = FALSE;
	Win::log("3#��������߳� 3 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate3_4(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread3_4_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref3;
	HImage hi_average = pImgProc->m_hi_average3;
	HImage hi_deviation = pImgProc->m_hi_deviation3;

	while (pImgProc->is_thread3_4_alive)
	{
		if (!pImgProc->m_ImgList3_4.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList3_4.front();
			pImgProc->m_ImgList3_4.pop_front();

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_3, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_3, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread3);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList3.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced3 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList3_4.clear();

	pImgProc->is_thread3_4_alive = FALSE;
	Win::log("3#��������߳� 4 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate3_5(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread3_5_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref3;
	HImage hi_average = pImgProc->m_hi_average3;
	HImage hi_deviation = pImgProc->m_hi_deviation3;

	while (pImgProc->is_thread3_5_alive)
	{
		if (!pImgProc->m_ImgList3_5.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList3_5.front();
			pImgProc->m_ImgList3_5.pop_front();

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_3, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_3, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread3);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList3.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced3 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList3_5.clear();

	pImgProc->is_thread3_5_alive = FALSE;
	Win::log("3#��������߳� 5 ����");

	return 0;
}

//  4# ��������߳�
UINT CImageProcess::ImageCalculate4_1(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread4_1_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref4;
	int valid_area = pImgProc->m_camera4_invalid_area;
	HImage hi_average = pImgProc->m_hi_average4;
	HImage hi_deviation = pImgProc->m_hi_deviation4;

	while (pImgProc->is_thread4_1_alive)
	{
		if (!pImgProc->m_ImgList4_1.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList4_1.front();
			pImgProc->m_ImgList4_1.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, 0, hv_height_def, hv_width_def - valid_area);
					//�ض���ͼ��Ķ����򣬷���ͼ�� 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_4, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread4);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList4.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced4 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList4_1.clear();

	pImgProc->is_thread4_1_alive = FALSE;
	Win::log("4#��������߳� 1 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate4_2(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread4_2_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref4;
	int valid_area = pImgProc->m_camera4_invalid_area;
	HImage hi_average = pImgProc->m_hi_average4;
	HImage hi_deviation = pImgProc->m_hi_deviation4;

	while (pImgProc->is_thread4_2_alive)
	{
		if (!pImgProc->m_ImgList4_2.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList4_2.front();
			pImgProc->m_ImgList4_2.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, 0, hv_height_def, hv_width_def - valid_area);
					//�ض���ͼ��Ķ����򣬷���ͼ�� 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_4, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread4);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList4.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced4 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList4_2.clear();

	pImgProc->is_thread4_2_alive = FALSE;
	Win::log("4#��������߳� 2 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate4_3(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread4_3_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref4;
	int valid_area = pImgProc->m_camera4_invalid_area;
	HImage hi_average = pImgProc->m_hi_average4;
	HImage hi_deviation = pImgProc->m_hi_deviation4;

	while (pImgProc->is_thread4_3_alive)
	{
		if (!pImgProc->m_ImgList4_3.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList4_3.front();
			pImgProc->m_ImgList4_3.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, 0, hv_height_def, hv_width_def - valid_area);
					//�ض���ͼ��Ķ����򣬷���ͼ�� 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_4, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread4);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList4.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}

			//�Ѵ����ͼ������
			pImgProc->m_NO_produced4 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList4_3.clear();

	pImgProc->is_thread4_3_alive = FALSE;
	Win::log("4#��������߳� 3 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate4_4(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread4_4_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref4;
	int valid_area = pImgProc->m_camera4_invalid_area;
	HImage hi_average = pImgProc->m_hi_average4;
	HImage hi_deviation = pImgProc->m_hi_deviation4;

	while (pImgProc->is_thread4_4_alive)
	{
		if (!pImgProc->m_ImgList4_4.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList4_4.front();
			pImgProc->m_ImgList4_4.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, 0, hv_height_def, hv_width_def - valid_area);
					//�ض���ͼ��Ķ����򣬷���ͼ�� 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}

			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_4, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread4);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList4.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced4 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList4_4.clear();

	pImgProc->is_thread4_4_alive = FALSE;
	Win::log("4#��������߳� 4 ����");

	return 0;
}

UINT CImageProcess::ImageCalculate4_5(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread4_5_alive = TRUE;

	//����ο�ͼ��
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref4;
	int valid_area = pImgProc->m_camera4_invalid_area;
	HImage hi_average = pImgProc->m_hi_average4;
	HImage hi_deviation = pImgProc->m_hi_deviation4;

	while (pImgProc->is_thread4_5_alive)
	{
		if (!pImgProc->m_ImgList4_5.empty())
		{
			std::vector<DefectType> vdef;

			//�Ӷ�����ȡ������ͼ��
			HImage hi_def = pImgProc->m_ImgList4_5.front();
			pImgProc->m_ImgList4_5.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, 0, hv_height_def, hv_width_def - valid_area);
					//�ض���ͼ��Ķ����򣬷���ͼ�� 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//覴ü���㷨
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_4, hi_average, hi_deviation, hi_def, vdef);

			EnterCriticalSection(&pImgProc->m_csCalculateThread4);
			//����覴þ���λ����������覴ö���
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList4.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//�Ѵ����ͼ������
			pImgProc->m_NO_produced4 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList4_5.clear();

	pImgProc->is_thread4_5_alive = FALSE;
	Win::log("4#��������߳� 5 ����");

	return 0;
}
