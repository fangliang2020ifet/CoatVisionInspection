

#include "stdafx.h"
#include "DeVision.h"
//#include "DeVisionDlg.h"
#include "CImageProcess.h"
#include "Log.h"

#include <winbase.h>
#include <windows.h>

//CEvent StopManage_Event;
//CEvent AllCalculateThreadStopped_Event;

CImageProcess::CImageProcess()
{
	InitializeCriticalSection(&m_csCalculateThread1);
	InitializeCriticalSection(&m_csCalculateThread2);
	InitializeCriticalSection(&m_csCalculateThread3);
	InitializeCriticalSection(&m_csCalculateThread4);
	//InitializeCriticalSection(&m_csNO_dft);
	InitializeCriticalSection(&m_csDefImgList);
}

CImageProcess::~CImageProcess()
{
	DeleteCriticalSection(&m_csCalculateThread1);
	DeleteCriticalSection(&m_csCalculateThread2);
	DeleteCriticalSection(&m_csCalculateThread3);
	DeleteCriticalSection(&m_csCalculateThread4);
	//DeleteCriticalSection(&m_csNO_dft);
	DeleteCriticalSection(&m_csDefImgList);

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

void CImageProcess::HalconInitAOP()
{
	HTuple hv_AOP, hv_method;
	//   nil     threshold     linear     mlp  
	hv_method = "mlp";

	//测试：手动AOP优化
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

void CImageProcess::HalconOpenGPU(HTuple &hv_DeviceHandle)
{
	//测试3:GPU加速，支持GPU加速的算子Halcon19.11有82个
	//GPU加速是先从CPU中将数据拷贝到GPU上处理，处理完成后再将数据从GPU拷贝到CPU上。从CPU到GPU再从GPU到CPU是要花费时间的。
	//GPU加速一定会比正常的AOP运算速度快吗?不一定!结果取决于显卡的好坏.
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

//获取主窗口类
BOOL CImageProcess::InitialImageProcess()
{
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	return TRUE;
}

BOOL CImageProcess::BeginProcess()
{
	std::string str_path;
	if (!GetSavePath(str_path)) return FALSE;	

	//创建线程
	if (!(m_ManageThread = AfxBeginThread(ManageThread, this))) {
		Win::log("创建图像参考图像处理线程失败");
		return FALSE;
	}

	if (m_threadnum >= 1) {
		if (!(m_CalculateThread1_1 = AfxBeginThread(ImageCalculate1_1, this))) {
			Win::log("创建图像处理线程1_1失败");
			return FALSE;
		}
		if (!(m_CalculateThread2_1 = AfxBeginThread(ImageCalculate2_1, this))) {
			Win::log("创建图像处理线程2_1失败");
			return FALSE;
		}
		if (!(m_CalculateThread3_1 = AfxBeginThread(ImageCalculate3_1, this))) {
			Win::log("创建图像处理线程3_1失败");
			return FALSE;
		}
		if (!(m_CalculateThread4_1 = AfxBeginThread(ImageCalculate4_1, this))) {
			Win::log("创建图像处理线程4_1失败");
			return FALSE;
		}
		CString cstr = L"创建第 1 个处理线程";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}

	if (m_threadnum >= 2) {
		if (!(m_CalculateThread1_2 = AfxBeginThread(ImageCalculate1_2, this))) {
			Win::log("创建图像处理线程1_2失败");
			return FALSE;
		}
		if (!(m_CalculateThread2_2 = AfxBeginThread(ImageCalculate2_2, this))) {
			Win::log("创建图像处理线程2_2失败");
			return FALSE;
		}
		if (!(m_CalculateThread3_2 = AfxBeginThread(ImageCalculate3_2, this))) {
			Win::log("创建图像处理线程3_2失败");
			return FALSE;
		}
		if (!(m_CalculateThread4_2 = AfxBeginThread(ImageCalculate4_2, this))) {
			Win::log("创建图像处理线程4_2失败");
			return FALSE;
		}
		CString cstr = L"创建第 2 个处理线程";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}

	if (m_threadnum >= 3) {
		if (!(m_CalculateThread1_3 = AfxBeginThread(ImageCalculate1_3, this))) {
			Win::log("创建图像处理线程1_3失败");
			return FALSE;
		}
		if (!(m_CalculateThread2_3 = AfxBeginThread(ImageCalculate2_3, this))) {
			Win::log("创建图像处理线程2_3失败");
			return FALSE;
		}
		if (!(m_CalculateThread3_3 = AfxBeginThread(ImageCalculate3_3, this))) {
			Win::log("创建图像处理线程3_3失败");
			return FALSE;
		}
		if (!(m_CalculateThread4_3 = AfxBeginThread(ImageCalculate4_3, this))) {
			Win::log("创建图像处理线程4_3失败");
			return FALSE;
		}
		CString cstr = L"创建第 3 个处理线程";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}

	if (m_threadnum >= 4) {
		if (!(m_CalculateThread1_4 = AfxBeginThread(ImageCalculate1_4, this))) {
			Win::log("创建图像处理线程1_4失败");
			return FALSE;
		}
		if (!(m_CalculateThread2_4 = AfxBeginThread(ImageCalculate2_4, this))) {
			Win::log("创建图像处理线程2_4失败");
			return FALSE;
		}
		if (!(m_CalculateThread3_4 = AfxBeginThread(ImageCalculate3_4, this))) {
			Win::log("创建图像处理线程3_4失败");
			return FALSE;
		}
		if (!(m_CalculateThread4_4 = AfxBeginThread(ImageCalculate4_4, this))) {
			Win::log("创建图像处理线程4_4失败");
			return FALSE;
		}
		CString cstr = L"创建第 4 个处理线程";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}

	if (m_threadnum >= 5) {
		if (!(m_CalculateThread1_5 = AfxBeginThread(ImageCalculate1_5, this))) {
			Win::log("创建图像处理线程1_5失败");
			return FALSE;
		}
		if (!(m_CalculateThread2_5 = AfxBeginThread(ImageCalculate2_5, this))) {
			Win::log("创建图像处理线程2_5失败");
			return FALSE;
		}
		if (!(m_CalculateThread3_5 = AfxBeginThread(ImageCalculate3_5, this))) {
			Win::log("创建图像处理线程3_5失败");
			return FALSE;
		}
		if (!(m_CalculateThread4_5 = AfxBeginThread(ImageCalculate4_5, this))) {
			Win::log("创建图像处理线程4_5失败");
			return FALSE;
		}
		CString cstr = L"创建第 5 个处理线程";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}	
	
	StopManage_Event.ResetEvent();
	AllCalculateThreadStopped_Event.ResetEvent();

	//CString cstr = L"创建处理线程";
	//::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	return TRUE;
}

BOOL CImageProcess::StopProcess()
{
	// 线程停止
	StopManage_Event.SetEvent();

	//while (is_manage_thread_alive)
	//{
	//	is_manage_thread_alive = FALSE;
	//}

	//StopCalculateThreads();


	return TRUE;
}

void CImageProcess::StopCalculateThreads()
{
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
}

void CImageProcess::RestartProcess()
{
	m_DFTList1.clear();
	m_DFTList2.clear();
	m_DFTList3.clear();
	m_DFTList4.clear();
	m_Sorted_DFTList.clear();

	m_current_position = 0.0f;

}

BOOL CImageProcess::IsThreadsAlive()
{
	if (   is_thread1_1_alive || is_thread2_1_alive || is_thread3_1_alive || is_thread4_1_alive
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
		CString cstr = L"参考图像目录不存在";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}

	//读取参考图像1
	if (1) {
		std::string ref_image_name1 = "reference_image1.bmp";
		HTuple hv_ref_image_name1 = (HTuple)((folder_path + ref_image_name1).c_str());
		if (!IsFileExist(folder_path + ref_image_name1)) {
			CString cstr = L"参考图像1不存在";
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return FALSE;
		}
		HImage img1;
		ReadImage(&img1, hv_ref_image_name1);
		//HalconCpp::MedianImage(img1, &m_hi_ref1, "circle", 1, "mirrored");
		//高斯滤波
		//HalconCpp::GaussFilter(img1, &m_hi_ref1, 5);
		m_hi_ref1 = img1;

		HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
		HTuple  hv_Deviation, hv_StandardDeviation;
		HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
		HalconCpp::GetImageSize(img1, &hv_Width, &hv_Height);
		HalconCpp::MedianImage(img1, &ho_ImageMedian, "square", m_median_filter_size, "mirrored");
		//创建 全 0 值图像
		HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
		HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
		HTuple end_val8 = hv_Width - 1;
		HTuple step_val8 = 1;
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
		{
			//参数2：左上角点的行，参数3：左上角点的列，参数4：右下角点的行，参数5：右下角点的列
			HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
			//均值/方差/标准差
			HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
			HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
			HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
			HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
				((m_k_normal_distribution * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
				"fill");
		}
		m_hi_average1 = ho_ImageAverage;
		m_hi_deviation1 = ho_ImageDeviation;

	}

	//读取参考图像2
	if (1) {
		std::string ref_image_name2 = "reference_image2.bmp";
		HTuple hv_ref_image_name2 = (HTuple)((folder_path + ref_image_name2).c_str());
		if (!IsFileExist(folder_path + ref_image_name2)) {
			CString cstr = L"参考图像2不存在";
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return FALSE;
		}
		HImage img2;
		ReadImage(&img2, hv_ref_image_name2);
		//HalconCpp::MedianImage(img2, &m_hi_ref2, "circle", 1, "mirrored");
			//高斯滤波
		//HalconCpp::GaussFilter(img2, &m_hi_ref2, 5);
		m_hi_ref2 = img2;

		HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
		HTuple  hv_Deviation, hv_StandardDeviation;
		HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
		HalconCpp::GetImageSize(img2, &hv_Width, &hv_Height);
		HalconCpp::MedianImage(img2, &ho_ImageMedian, "square", m_median_filter_size, "mirrored");
		//创建 全 0 值图像
		HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
		HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
		HTuple end_val8 = hv_Width - 1;
		HTuple step_val8 = 1;
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
		{
			//参数2：左上角点的行，参数3：左上角点的列，参数4：右下角点的行，参数5：右下角点的列
			HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
			//均值/方差/标准差
			HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
			HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
			HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
			HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
				((m_k_normal_distribution * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
				"fill");
		}
		m_hi_average2 = ho_ImageAverage;
		m_hi_deviation2 = ho_ImageDeviation;
	}

	//读取参考图像3
	if (1) {
		std::string ref_image_name3 = "reference_image3.bmp";
		HTuple hv_ref_image_name3 = (HTuple)((folder_path + ref_image_name3).c_str());
		if (!IsFileExist(folder_path + ref_image_name3)) {
			CString cstr = L"参考图像3不存在";
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return FALSE;
		}
		HImage img3;
		ReadImage(&img3, hv_ref_image_name3);
		//HalconCpp::MedianImage(img3, &m_hi_ref3, "circle", 1, "mirrored");
			//高斯滤波
		//HalconCpp::GaussFilter(img3, &m_hi_ref3, 5);
		m_hi_ref3 = img3;

		HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
		HTuple  hv_Deviation, hv_StandardDeviation;
		HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
		HalconCpp::GetImageSize(img3, &hv_Width, &hv_Height);
		HalconCpp::MedianImage(img3, &ho_ImageMedian, "square", m_median_filter_size, "mirrored");
		//创建 全 0 值图像
		HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
		HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
		HTuple end_val8 = hv_Width - 1;
		HTuple step_val8 = 1;
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
		{
			//参数2：左上角点的行，参数3：左上角点的列，参数4：右下角点的行，参数5：右下角点的列
			HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
			//均值/方差/标准差
			HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
			HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
			HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
			HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
				((m_k_normal_distribution * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
				"fill");
		}
		m_hi_average3 = ho_ImageAverage;
		m_hi_deviation3 = ho_ImageDeviation;
	}

	//读取参考图像4
	if (1) {
		std::string ref_image_name4 = "reference_image4.bmp";
		HTuple hv_ref_image_name4 = (HTuple)((folder_path + ref_image_name4).c_str());
		if (!IsFileExist(folder_path + ref_image_name4)) {
			CString cstr = L"参考图像4不存在";
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return FALSE;
		}
		HImage img4;
		ReadImage(&img4, hv_ref_image_name4);
		//HalconCpp::MedianImage(img4, &m_hi_ref4, "circle", 1, "mirrored");
			//高斯滤波
		//HalconCpp::GaussFilter(img4, &m_hi_ref4, 5);
		m_hi_ref4 = img4;

		HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
		HTuple  hv_Deviation, hv_StandardDeviation;
		HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
		HalconCpp::GetImageSize(img4, &hv_Width, &hv_Height);
		HalconCpp::MedianImage(img4, &ho_ImageMedian, "square", m_median_filter_size, "mirrored");
		//创建 全 0 值图像
		HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
		HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
		HTuple end_val8 = hv_Width - 1;
		HTuple step_val8 = 1;
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
		{
			//参数2：左上角点的行，参数3：左上角点的列，参数4：右下角点的行，参数5：右下角点的列
			HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
			//均值/方差/标准差
			HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
			HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
			HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
			HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
				((m_k_normal_distribution * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
				"fill");
		}
		m_hi_average4 = ho_ImageAverage;
		m_hi_deviation4 = ho_ImageDeviation;
	}

	return TRUE;
}

//把图像加载到内存队列
void CImageProcess::LoadImageToQueue()
{
	if (m_threadnum >= 1) {
		m_ImgList1_1.push_back(m_hi_test1);
		m_ImgList2_1.push_back(m_hi_test2);
		m_ImgList3_1.push_back(m_hi_test3);
		m_ImgList4_1.push_back(m_hi_test4);
	}
	if (m_threadnum >= 2) {
		m_ImgList1_2.push_back(m_hi_test1);
		m_ImgList2_2.push_back(m_hi_test2);
		m_ImgList3_2.push_back(m_hi_test3);
		m_ImgList4_2.push_back(m_hi_test4);
	}
	if (m_threadnum >= 3) {
		m_ImgList1_3.push_back(m_hi_test1);
		m_ImgList2_3.push_back(m_hi_test2);
		m_ImgList3_3.push_back(m_hi_test3);
		m_ImgList4_3.push_back(m_hi_test4);
	}
	if (m_threadnum >= 4) {
		m_ImgList1_4.push_back(m_hi_test1);
		m_ImgList2_4.push_back(m_hi_test2);
		m_ImgList3_4.push_back(m_hi_test3);
		m_ImgList4_4.push_back(m_hi_test4);
	}
	if (m_threadnum >= 5) {
		m_ImgList1_5.push_back(m_hi_test1);
		m_ImgList2_5.push_back(m_hi_test2);
		m_ImgList3_5.push_back(m_hi_test3);
		m_ImgList4_5.push_back(m_hi_test4);
	}
}

BOOL CImageProcess::LoadOneImageToQueue(std::string folder_path, int next_number)
{
	//读取待检图像
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
		Win::log("待检图像为空\n请检查图像名称是否从001开始");
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

	return TRUE;
}

HObject CImageProcess::CopyHobject(HObject ho_image)
{
	HObject copy;
	CopyImage(ho_image, &copy);

	return copy;
}

//获取处理线程的队列大小
int CImageProcess::CheckTotalListSize()
{
	size_t list_size = 0;
	if (m_threadnum == 1) {
		list_size = m_ImgList1_1.size() + m_ImgList2_1.size() + m_ImgList3_1.size() + m_ImgList4_1.size();
	}
	else if (m_threadnum == 2) {
		list_size = m_ImgList1_1.size() + m_ImgList2_1.size() + m_ImgList3_1.size() + m_ImgList4_1.size()
			      + m_ImgList1_2.size() + m_ImgList2_2.size() + m_ImgList3_2.size() + m_ImgList4_2.size();
	}
	else if (m_threadnum == 3) {
		list_size = m_ImgList1_1.size() + m_ImgList2_1.size() + m_ImgList3_1.size() + m_ImgList4_1.size()
			+ m_ImgList1_2.size() + m_ImgList2_2.size() + m_ImgList3_2.size() + m_ImgList4_2.size()
			+ m_ImgList1_3.size() + m_ImgList2_3.size() + m_ImgList3_3.size() + m_ImgList4_3.size();
	}
	else if (m_threadnum == 4) {
		list_size = m_ImgList1_1.size() + m_ImgList2_1.size() + m_ImgList3_1.size() + m_ImgList4_1.size()
			+ m_ImgList1_2.size() + m_ImgList2_2.size() + m_ImgList3_2.size() + m_ImgList4_2.size()
			+ m_ImgList1_3.size() + m_ImgList2_3.size() + m_ImgList3_3.size() + m_ImgList4_3.size()
			+ m_ImgList1_4.size() + m_ImgList2_4.size() + m_ImgList3_4.size() + m_ImgList4_4.size();
	}
	else if (m_threadnum == 5) {
		list_size = m_ImgList1_1.size() + m_ImgList2_1.size() + m_ImgList3_1.size() + m_ImgList4_1.size()
			+ m_ImgList1_2.size() + m_ImgList2_2.size() + m_ImgList3_2.size() + m_ImgList4_2.size()
			+ m_ImgList1_3.size() + m_ImgList2_3.size() + m_ImgList3_3.size() + m_ImgList4_3.size()
			+ m_ImgList1_4.size() + m_ImgList2_4.size() + m_ImgList3_4.size() + m_ImgList4_4.size()
			+ m_ImgList1_5.size() + m_ImgList2_5.size() + m_ImgList3_5.size() + m_ImgList4_5.size();
	}

	return (int)list_size;
}

//从本地加载参考图像
BOOL CImageProcess::LoadDefaultRefAndDevImage(std::string path)
{
	if (path.empty())
		return FALSE;

	std::string ref_image_name1 = "reference_image1.bmp";
	std::string ref_image_name2 = "reference_image2.bmp";
	std::string ref_image_name3 = "reference_image3.bmp";
	std::string ref_image_name4 = "reference_image4.bmp";
	std::string dev_image_name1 = "dev1.bmp";
	std::string dev_image_name2 = "dev2.bmp";
	std::string dev_image_name3 = "dev3.bmp";
	std::string dev_image_name4 = "dev4.bmp";

	if (!IsPathExist(path)) {
		CString cstr = L"读取图像失败：默认参考图像目录不存在";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}
	else {
		if (!IsFileExist(path + ref_image_name1) || !IsFileExist(path + dev_image_name1)) {
			CString cstr = L"读取图像失败：默认参考图像1不存在";
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return FALSE;
		}
		if (!IsFileExist(path + ref_image_name2) || !IsFileExist(path + dev_image_name2)) {
			CString cstr = L"读取图像失败：默认参考图像2不存在";
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return FALSE;
		}
		if (!IsFileExist(path + ref_image_name3) || !IsFileExist(path + dev_image_name3)) {
			CString cstr = L"读取图像失败：默认参考图像3不存在";
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return FALSE;
		}
		if (!IsFileExist(path + ref_image_name4) || !IsFileExist(path + dev_image_name4)) {
			CString cstr = L"读取图像失败：默认参考图像4不存在";
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return FALSE;
		}
	}

	HTuple hv_ref_image_name1 = (HTuple)((path + ref_image_name1).c_str());
	HTuple hv_ref_image_name2 = (HTuple)((path + ref_image_name2).c_str());
	HTuple hv_ref_image_name3 = (HTuple)((path + ref_image_name3).c_str());
	HTuple hv_ref_image_name4 = (HTuple)((path + ref_image_name4).c_str());
	HTuple hv_dev_image_name1 = (HTuple)((path + dev_image_name1).c_str());
	HTuple hv_dev_image_name2 = (HTuple)((path + dev_image_name2).c_str());
	HTuple hv_dev_image_name3 = (HTuple)((path + dev_image_name3).c_str());
	HTuple hv_dev_image_name4 = (HTuple)((path + dev_image_name4).c_str());
	ReadImage(&m_hi_average1, hv_ref_image_name1);
	ReadImage(&m_hi_average2, hv_ref_image_name2);
	ReadImage(&m_hi_average3, hv_ref_image_name3);
	ReadImage(&m_hi_average4, hv_ref_image_name4);
	ReadImage(&m_hi_deviation1, hv_dev_image_name1);
	ReadImage(&m_hi_deviation2, hv_dev_image_name2);
	ReadImage(&m_hi_deviation3, hv_dev_image_name3);
	ReadImage(&m_hi_deviation4, hv_dev_image_name4);
	return TRUE;
}

BOOL CImageProcess::GenerateReferenceImage1(HImage &hi_average, HImage &hi_deviation)
{	
	HImage result, img;
	if (m_threadnum == 1) {
		if (m_ImgList1_1.size() >= 4) {
			m_ImgList1_1.pop_front();
			for (int i = 0; i < 3; i++)
			{
				img = m_ImgList1_1.front();
				HalconCpp::AddImage(result, img, &result, 0.5, 0);
				m_ImgList1_1.pop_front();
			}
		}
		else return FALSE;
	}
	else if (m_threadnum == 2) {
		if (m_ImgList1_1.size() >= 3 && !m_ImgList1_2.empty()) {
			m_ImgList1_1.pop_front();
			img = m_ImgList1_1.front();
			m_ImgList1_1.pop_front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList1_2.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList1_1.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);
		}
		else return FALSE;
	}
	else if (m_threadnum > 2) {
		if (m_ImgList1_1.size() >= 2 && !m_ImgList1_2.empty() && !m_ImgList1_3.empty()) {
			m_ImgList1_1.pop_front();
			img = m_ImgList1_1.front();
			m_ImgList1_1.pop_front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList1_2.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList1_3.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);
		}
		else return FALSE;
	}

	m_hi_ref1 = result;
	HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
	HTuple  hv_Deviation, hv_StandardDeviation;
	HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
	HalconCpp::GetImageSize(result, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(result, &ho_ImageMedian, "square", m_median_filter_size, "mirrored");
	//创建 全 0 值图像
	HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
	HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
	HTuple end_val8 = hv_Width - 1;
	HTuple step_val8 = 1;
	for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
	{
		//参数2：左上角点的行，参数3：左上角点的列，参数4：右下角点的行，参数5：右下角点的列
		HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
		//均值/方差/标准差
		HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
		HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
		HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
		HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
			((m_k_normal_distribution * hv_StandardDeviation).TupleConcat(255)).TupleMin(),"fill");
	}
	hi_average = ho_ImageAverage;
	hi_deviation = ho_ImageDeviation;

	return TRUE;
}

BOOL CImageProcess::GenerateReferenceImage2(HImage &hi_average, HImage &hi_deviation)
{	
	HImage result, img;
	if (m_threadnum == 1) {
		if (m_ImgList2_1.size() >= 4) {
			m_ImgList2_1.pop_front();
			for (int i = 0; i < 3; i++)
			{
				img = m_ImgList2_1.front();
				HalconCpp::AddImage(result, img, &result, 0.5, 0);
				m_ImgList2_1.pop_front();
			}
		}
		else return FALSE;
	}
	else if (m_threadnum == 2) {
		if (m_ImgList2_1.size() >= 3 && !m_ImgList2_2.empty()) {
			m_ImgList2_1.pop_front();
			img = m_ImgList2_1.front();
			m_ImgList2_1.pop_front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList2_2.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList2_1.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);
		}
		else return FALSE;
	}
	else if (m_threadnum > 2) {
		if (m_ImgList2_1.size() >= 2 && !m_ImgList2_2.empty() && !m_ImgList2_3.empty()) {
			m_ImgList2_1.pop_front();
			img = m_ImgList2_1.front();
			m_ImgList2_1.pop_front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList2_2.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList2_3.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);
		}
		else return FALSE;
	}

	m_hi_ref2 = result;
	HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
	HTuple  hv_Deviation, hv_StandardDeviation;
	HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
	HalconCpp::GetImageSize(result, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(result, &ho_ImageMedian, "square", m_median_filter_size, "mirrored");
	//创建 全 0 值图像
	HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
	HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
	HTuple end_val8 = hv_Width - 1;
	HTuple step_val8 = 1;
	for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
	{
		//参数2：左上角点的行，参数3：左上角点的列，参数4：右下角点的行，参数5：右下角点的列
		HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
		//均值/方差/标准差
		HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
		HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
		HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
		HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
			((m_k_normal_distribution * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
			"fill");
	}
	hi_average = ho_ImageAverage;
	hi_deviation = ho_ImageDeviation;

	return TRUE;
}

BOOL CImageProcess::GenerateReferenceImage3(HImage &hi_average, HImage &hi_deviation)
{
	HImage result, img;
	if (m_threadnum == 1) {
		if (m_ImgList3_1.size() >= 4) {
			m_ImgList3_1.pop_front();
			for (int i = 0; i < 3; i++)
			{
				img = m_ImgList3_1.front();
				HalconCpp::AddImage(result, img, &result, 0.5, 0);
				m_ImgList3_1.pop_front();
			}
		}
		else return FALSE;
	}
	else if (m_threadnum == 2) {
		if (m_ImgList3_1.size() >= 3 && !m_ImgList3_2.empty()) {
			m_ImgList3_1.pop_front();
			img = m_ImgList3_1.front();
			m_ImgList3_1.pop_front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList3_2.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList3_1.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);
		}
		else return FALSE;
	}
	else if (m_threadnum > 2) {
		if (m_ImgList3_1.size() >= 2 && !m_ImgList3_2.empty() && !m_ImgList3_3.empty()) {
			m_ImgList3_1.pop_front();
			img = m_ImgList3_1.front();
			m_ImgList3_1.pop_front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList3_2.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList3_3.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);
		}
		else return FALSE;
	}

	m_hi_ref3 = result;
	HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
	HTuple  hv_Deviation, hv_StandardDeviation;
	HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
	HalconCpp::GetImageSize(result, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(result, &ho_ImageMedian, "square", m_median_filter_size, "mirrored");
	//创建 全 0 值图像
	HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
	HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
	HTuple end_val8 = hv_Width - 1;
	HTuple step_val8 = 1;
	for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
	{
		//参数2：左上角点的行，参数3：左上角点的列，参数4：右下角点的行，参数5：右下角点的列
		HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
		//均值/方差/标准差
		HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
		HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
		HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
		HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
			((m_k_normal_distribution * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
			"fill");
	}
	hi_average = ho_ImageAverage;
	hi_deviation = ho_ImageDeviation;

	return TRUE;
}

BOOL CImageProcess::GenerateReferenceImage4(HImage &hi_average, HImage &hi_deviation)
{
	HImage result, img;
	if (m_threadnum == 1) {
		if (m_ImgList4_1.size() >= 4) {
			m_ImgList4_1.pop_front();
			for (int i = 0; i < 3; i++)
			{
				img = m_ImgList4_1.front();
				HalconCpp::AddImage(result, img, &result, 0.5, 0);
				m_ImgList4_1.pop_front();
			}
		}
		else return FALSE;
	}
	else if (m_threadnum == 2) {
		if (m_ImgList4_1.size() >= 3 && !m_ImgList4_2.empty()) {
			m_ImgList4_1.pop_front();
			img = m_ImgList4_1.front();
			m_ImgList4_1.pop_front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList4_2.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList4_1.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);
		}
		else return FALSE;
	}
	else if (m_threadnum > 2) {
		if (m_ImgList4_1.size() >= 2 && !m_ImgList4_2.empty() && !m_ImgList4_3.empty()) {
			m_ImgList4_1.pop_front();
			img = m_ImgList4_1.front();
			m_ImgList4_1.pop_front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList4_2.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);

			img = m_ImgList4_3.front();
			HalconCpp::AddImage(result, img, &result, 0.5, 0);
		}
		else return FALSE;
	}

	m_hi_ref4 = result;
	HTuple  hv_Width, hv_Height, hv_column, hv_Mean;
	HTuple  hv_Deviation, hv_StandardDeviation;
	HObject ho_ImageMedian, ho_ImageAverage, ho_ImageDeviation, ho_Rectangle;
	HalconCpp::GetImageSize(result, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(result, &ho_ImageMedian, "square", m_median_filter_size, "mirrored");
	//创建 全 0 值图像
	HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
	HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
	HTuple end_val8 = hv_Width - 1;
	HTuple step_val8 = 1;
	for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)
	{
		//参数2：左上角点的行，参数3：左上角点的列，参数4：右下角点的行，参数5：右下角点的列
		HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
		//均值/方差/标准差
		HalconCpp::Intensity(ho_Rectangle, ho_ImageMedian, &hv_Mean, &hv_Deviation);
		HalconCpp::TupleSqrt(hv_Deviation, &hv_StandardDeviation);
		HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Rectangle, hv_Mean, "fill");
		HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Rectangle,
			((m_k_normal_distribution * hv_StandardDeviation).TupleConcat(255)).TupleMin(),
			"fill");
	}
	hi_average = ho_ImageAverage;
	hi_deviation = ho_ImageDeviation;

	return TRUE;
}

//判断参考图像：均值图和方差图，是否已经生成
BOOL CImageProcess::CheckReferenceImageAvilable()
{
	if (!m_hi_average1.IsInitialized())	return FALSE;
	if (!m_hi_average2.IsInitialized())	return FALSE;
	if (!m_hi_average3.IsInitialized())	return FALSE;
	if (!m_hi_average4.IsInitialized())	return FALSE;
	if (!m_hi_deviation1.IsInitialized())	return FALSE;
	if (!m_hi_deviation2.IsInitialized())	return FALSE;
	if (!m_hi_deviation3.IsInitialized())	return FALSE;
	if (!m_hi_deviation4.IsInitialized())	return FALSE;

	return TRUE;
}

//生成去掉黑边的图像
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

	//选取右侧边缘 256*256 大小区域用于计算像素平均值
	HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_Width1 - 256, 255, hv_Width1);
	HalconCpp::Intensity(ho_Rectangle, ho_Image, &hv_Mean, &hv_Deviation);
	//以平均像素值的 0.8 倍进行阈值化
	HalconCpp::Threshold(ho_Image, &ho_Region, hv_Mean*0.8, 255);
	HalconCpp::Connection(ho_Region, &ho_ConnectedRegions);
	//进行区域选择，选取有效区域
	HalconCpp::SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "height", "and",
		hv_height_ref2 - 1000, hv_height_ref2);
	//缩减图像的定义域(对原图选择新区域，但图像的大小不变)
	HalconCpp::ReduceDomain(ho_Image, ho_SelectedRegions, &ho_ImageReduced);
	//根据区域对灰度图像进行减除
	HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
	HalconCpp::GetImageSize(ho_ImagePart, &hv_width_ref1, &hv_height_ref1);

	// 加 32 用于避开边缘像素变化较大的区域
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

	//选取左侧边缘 256*256 大小区域用于计算像素平均值
	HalconCpp::GenRectangle1(&ho_Rectangle, 0, 0, 255, 255);
	HalconCpp::Intensity(ho_Rectangle, ho_Image, &hv_Mean, &hv_Deviation);
	//以平均像素值的 0.8 倍进行阈值化
	HalconCpp::Threshold(ho_Image, &ho_Region, hv_Mean*0.8, 255);
	HalconCpp::Connection(ho_Region, &ho_ConnectedRegions);
	//进行区域选择，选取有效区域
	HalconCpp::SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "height", "and",
		hv_height_ref2 - 1000, hv_height_ref2);
	//缩减图像的定义域(对原图选择新区域，但图像的大小不变)
	HalconCpp::ReduceDomain(ho_Image, ho_SelectedRegions, &ho_ImageReduced);
	//根据区域对灰度图像进行减除
	HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
	HalconCpp::GetImageSize(ho_ImagePart, &hv_width_ref1, &hv_height_ref1);

	// 加 32 用于避开边缘像素变化较大的区域
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

	CString cstr = L"图像已保存";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);


	return TRUE;
}

BOOL CImageProcess::GetSavePath(std::string &path)
{
	path = m_strPath;

	//检查保存路径是否存在
	if (!IsPathExist(path)) {
		Win::log("图片保存路径不存在");
		CString cstr = L"图像处理失败：图片保存路径不存在";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}
	else
		return TRUE;
}

//瑕疵队列排序
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

	//根据瑕疵绝对位置排序后存入瑕疵队列
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

//分割后的瑕疵图像处理
DefectType CImageProcess::LocateDefectPosition(int camera_number, HObject ho_selectedregion, 
	                                          HTuple hv_Number, HTuple hv_colunm_origin, HObject ho_image)
{
	DefectType dtype;   //保存检测到的瑕疵信息

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
			//区域内有多个瑕疵则需要对位置进行平均
			hv_total_Row += hv_row;
			hv_total_Column += hv_column;
			hv_total_Radius += hv_radius;

			//计算像素平均值
			HTuple hv_mean, hv_deviation;
			HalconCpp::Intensity(ho_objectselected, ho_image, &hv_mean, &hv_deviation);
			hv_total_Mean += hv_mean;
		}
	}
	HTuple hv_average_Row = hv_total_Row / hv_Number;
	HTuple hv_average_Column = hv_total_Column / hv_Number;
	HTuple hv_average_Radius = hv_total_Radius / hv_Number;
	HTuple hv_average_Mean = hv_total_Mean / hv_Number;

	//根据圆心度分类...
	HTuple hv_circularity;
	HalconCpp::Circularity(ho_selectedregion, &hv_circularity);
	int circularity = hv_circularity.TupleInt();

	//计算区域面积
	HTuple hv_area, hv_row, hv_column;
	HalconCpp::AreaCenter(ho_selectedregion, &hv_area, &hv_row, &hv_column);
	dtype.area = hv_area.TupleInt();

	//缺陷在大图中的实际坐标
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

	//瑕疵信息写入
	dtype.center_x = (hv_average_Column.TupleInt() + IMAGE_WIDTH * (camera_number - 1))*HORIZON_PRECISION;     //单位：毫米
	//dtype.center_y = hv_average_Row.TupleInt();
	dtype.area = 0.255f;
	dtype.circle_radius = hv_average_Radius.TupleInt();
	dtype.pixel_value = hv_average_Mean.TupleInt();
	//计算绝对位置, 单位：米
	dtype.absolute_position = m_current_position + hv_average_Row.TupleInt() * VERTICAL_PRECISION / 1000.0f;
	//瑕疵分类
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

	//格式化文件名
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
	
	//保存瑕疵区域图片
	HObject ho_rectangle, ho_ImageReduced, ho_ImageCroped;
	HalconCpp::GenRectangle1(&ho_rectangle, hv_average_Row - 127, hv_average_Column - 127, hv_average_Row + 128, hv_average_Column + 128);

	HalconCpp::ReduceDomain(ho_image, ho_rectangle, &ho_ImageReduced);
	HalconCpp::CropDomain(ho_ImageReduced, &ho_ImageCroped);

	//瑕疵图像的名称
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
	DefectType dtype;   //保存检测到的瑕疵信息


	return dtype;
}


//瑕疵等级判定
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

//分类算法，分类器
int CImageProcess::ImageClassification(HObject ho_img)
{
	int kind = 0;
	HObject ho_region;
	HalconCpp::AutoThreshold(ho_img, &ho_region, 3);

	return kind;
}

//保存瑕疵图像
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



// 检测算法：2020.4.3 以前（分割成小图后检测）
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

	HalconCpp::Threshold(hi_img, &ho_Region_defth, 1, 255);
	HalconCpp::Connection(ho_Region_defth, &ho_ConnectedRegions_defth);
	SmallestRectangle1(ho_ConnectedRegions_defth, &hv_Row_origin_def, &hv_Column_origin_def,
		&hv_Row_end_def, &hv_Column_end_def);
	ReduceDomain(hi_img, ho_ConnectedRegions_defth, &ho_ImageReduced_def);
	CropDomain(ho_ImageReduced_def, &ho_ImagePart_def);
	MedianImage(ho_ImagePart_def, &ho_ImageMedian_def, "circle", 1, "mirrored");
	GetImageSize(ho_ImageMedian_def, &hv_Width_def, &hv_Height_def);

	//判断待检图像是否有黑边
	if (0 != (HTuple(hv_Width_ref != hv_Width_def).TupleOr(hv_Height_ref != hv_Height_def))) {
		ReduceDomain(hi_ref, ho_ConnectedRegions_defth, &ho_ImageReduced_ref);
		CropDomain(ho_ImageReduced_ref, &ho_ImagePart_ref);
		AbsDiffImage(ho_ImageMedian_def, ho_ImagePart_ref, &ho_ImageAbsDiff, 5);
		MedianImage(ho_ImageAbsDiff, &ho_ImageMedian_absdiff, "square", 2, "mirrored");
	}
	else {
		//参考图像减去待检测图像，结果取绝对值
		//可利用放大倍数设置检测的精度和等级
		AbsDiffImage(ho_ImageMedian_def, hi_ref, &ho_ImageAbsDiff, 5);
		MedianImage(ho_ImageAbsDiff, &ho_ImageMedian_absdiff, "square", 2, "mirrored");
	}

	//参考图像像素平均值
	GetDomain(hi_ref, &ho_Domain_ref);
	Intensity(ho_Domain_ref, hi_ref, &hv_Mean_ref, &hv_Deviation_ref);
	//差值图像的像素最大值
	//MinMaxGray(ho_Domain_ref, ho_ImageMedian_absdiff, 0, &hv_Min_ref, &hv_Max_ref, &hv_Range_ref);

	//判断是整除还是取模
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

	// vector 暂存瑕疵位置信息，排序后放入队列
	//std::vector<DefectType> vdef;
	HTuple end_val41 = hv_row_scale - 1;
	HTuple step_val41 = 1;
	for (hv_rownum = 0; hv_rownum.Continue(end_val41, step_val41); hv_rownum += step_val41)
	{
		HTuple end_val42 = hv_column_scale - 1;
		HTuple step_val42 = 1;
		for (hv_colnum = 0; hv_colnum.Continue(end_val42, step_val42); hv_colnum += step_val42)
		{
			//重定义图像的定义域，分切图像
			GenRectangle1(&ho_Rectangle, (hv_rownum * 256) + hv_Row_origin_def, (hv_colnum * 256) + hv_Column_origin_def,
				((hv_rownum + 1) * 256) + hv_Row_origin_def, ((hv_colnum + 1) * 256) + hv_Column_origin_def);

			ReduceDomain(ho_ImageMedian_absdiff, ho_Rectangle, &ho_ImageReduced_defsmall);
			//帧相减后的图像在缩减区域的像素平均值
			//Intensity(ho_Rectangle, ho_ImageReduced_defsmall, &hv_Mean_small_abs_diff, &hv_Deviation_small_abs_diff);
			//hv_between_mean = (hv_Mean_ref - hv_Mean_small_abs_diff).TupleAbs();
			//求区域内像素的最大最小值
			//reduce_domain (ImageAbsDiff, Rectangle, ImageReduced3)
			//MinMaxGray(ho_Rectangle, ho_ImageReduced_defsmall, 25, &hv_Min_defsmall, &hv_Max_defsmall, &hv_Range_defsmall);
			//分切后的图像动态阈值化
			//Threshold (ImageReduced_defsmall, Region, 0.8*max([min([255,Mean * 1.2]), min([255,Mean_small_abs_diff * 1.2])]), 255)

			Threshold(ho_ImageReduced_defsmall, &ho_Region, ((hv_Mean_ref*0.5).TupleConcat(255)).TupleMin(), 255);

			//连接区域
			HalconCpp::Connection(ho_Region, &ho_ConnectedRegions);
			//选择ROI
			SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "contlength", "and", 5, 999999);

			//对单张小图进行处理, 计算ROI数量
			CountObj(ho_SelectedRegions, &hv_Number);
			if (hv_Number != 0) {

				//保存瑕疵图片
				DefectType temp_def = LocateDefectPosition(cameraNO, ho_SelectedRegions,
					hv_Number, hv_Column_origin_def, ho_ImagePart_def);
				//保存瑕疵信息
				vDFT.push_back(temp_def);
			}
		}
	}

	return 0;
}

// 检测算法：2020.4.5（直接对大图进行检测）
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

	//读取参考图像
	ho_Image_ref = hi_ref;
	//GetImageSize(ho_Image_ref, &hv_Width_ref, &hv_Height_ref);
	//GaussFilter(ho_Image_ref, &ho_ImageGauss, 5);
	//读取待检测图像
	ho_Image_def = hi_img;
	GetImageSize(ho_Image_def, &hv_Width, &hv_Height);
	//GaussFilter(ho_Image_def, &ho_ImageGauss1, 5);
	HalconCpp::MedianImage(ho_Image_def, &ho_ImageGauss1, "square", m_median_filter_size, "mirrored");

	//待检图 - 参考图
	AbsDiffImage(ho_ImageGauss1, ho_Image_ref, &ho_ImageAbsDiff1, 1);

	//自动阈值输入必须是是单通道图像，会有多阈值分割，Sigma用于对灰度直方图进行高斯平滑，决定了平滑的程度（分割细致程度），
	//当sigma很大时，灰度直方图基本会被平滑为只剩下一个波峰，而分割是根据平滑后直方图的波谷来进行的，Sigma小，分割的越细致
	//AutoThreshold(ho_ImageAbsDiff1, &ho_Regions, 3);

	//用一个矩形掩膜计算像素点的灰度范围
	GrayRangeRect(ho_ImageAbsDiff1, &ho_ImageResult, 10, 10);
	MinMaxGray(ho_ImageResult, ho_ImageResult, 0, &hv_Min, &hv_Max, &hv_Range);
	Threshold(ho_ImageResult, &ho_Regions, (HTuple(5.55).TupleConcat(hv_Max*0.8)).TupleMax(), 255);
	//Threshold(ho_ImageResult, &ho_Regions, 200, 255);

	HalconCpp::Connection(ho_Regions, &ho_ConnectedRegions);
	//区域选择参数：1.轮廓周长(pixel)  2.最大内接圆半径(pixel)
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
				//当所选的区域的外接圆半径大于2048，即区域范围大于图像的三分之二时，默认跳过
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

				//计算像素平均值
				HTuple hv_mean, hv_deviation;
				HalconCpp::Intensity(ho_ObjectSelected, ho_ImagePart, &hv_mean, &hv_deviation);

				//保存检测到的瑕疵信息
				DefectType dtype; 
				dtype.center_x = (hv_Column.TupleInt() + IMAGE_WIDTH * (cameraNO - 1))*HORIZON_PRECISION;     //单位：毫米
				dtype.area = 0.255f;
				dtype.circle_radius = hv_Radius.TupleInt();
				dtype.pixel_value = hv_mean.TupleInt();
				dtype.absolute_position = m_current_position + hv_Row.TupleInt() * VERTICAL_PRECISION / 1000.0f;   //单位：米
				//瑕疵分类
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

				//格式化文件名
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

				//保存瑕疵信息
				vDFT.push_back(dtype);

				//瑕疵图像的名称
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

//检测算法：一种基于多目机器视觉的光学薄膜瑕疵检测系统
int CImageProcess::StandDeviationAlgorithm(int cameraNO, HImage hi_average, HImage hi_deviation,
								HImage hi_img, UINT64 produced_num, std::vector<DefectType> &vDFT)
{
	HObject  ho_Image, ho_ImageAverage, ho_ImageDeviation, ho_ImageMedianDFT;
	HObject  ho_ImageSub1, ho_ImageSub2, ho_ImageAddSub, ho_ImageResult;
	HObject  ho_Region, ho_RegionDilation, ho_RegionErosion;
	HObject  ho_ConnectedRegions, ho_SelectedRegions, ho_ObjectSelected, ho_Rectangle;
	HObject  ho_ImageReduced, ho_ImagePart;
	HTuple   hv_Width, hv_Height, hv_Number;
	HTuple   hv_i, hv_Area, hv_Row, hv_Column, hv_RowCircle, hv_ColumnCircle, hv_Radius, hv_Contlength;

	ho_Image = hi_img;
	ho_ImageAverage = hi_average;
	ho_ImageDeviation = hi_deviation;
	HalconCpp::GetImageSize(ho_Image, &hv_Width, &hv_Height);
	HalconCpp::MedianImage(ho_Image, &ho_ImageMedianDFT, "square", m_median_filter_size, "mirrored");
	//HalconCpp::MedianRect(ho_Image, &ho_ImageMedianDFT, 1, 11);

	//参考文献：一种基于多目机器视觉的光学薄膜瑕疵检测系统
	//如果相减后像素值小于零，其结果图中会被置0；同理，如果像素值大于255，也会被截断使其最大值为255
	//交换位置相减后再相加，相当于异或
	HalconCpp::SubImage(ho_ImageMedianDFT, ho_ImageAverage, &ho_ImageSub1, 1, 0);
	HalconCpp::SubImage(ho_ImageAverage, ho_ImageMedianDFT, &ho_ImageSub2, 1, 0);
	HalconCpp::AddImage(ho_ImageSub1, ho_ImageSub2, &ho_ImageAddSub, 0.5, 0);
	HalconCpp::SubImage(ho_ImageAddSub, ho_ImageDeviation, &ho_ImageResult, 1, 0);
	HalconCpp::Threshold(ho_ImageResult, &ho_Region, 1, 255);
	//膨胀,腐蚀,用于减少region的数量
	HalconCpp::DilationCircle(ho_Region, &ho_RegionDilation, 64);
	HalconCpp::ErosionCircle(ho_RegionDilation, &ho_RegionErosion, 64);
	HalconCpp::Connection(ho_RegionErosion, &ho_ConnectedRegions);
	int min_select = (int)std::pow(2 * m_fMin_Radius / HORIZON_PRECISION, 2);
	int max_select = (int)std::pow(2 * m_fMax_Radius / HORIZON_PRECISION, 2);
	HalconCpp::SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and",
		min_select,
		max_select);
	HalconCpp::CountObj(ho_SelectedRegions, &hv_Number);
	if (0 != hv_Number)
	{
		//当单张图像中选择的区域超过 64 个，则认为此图计算失败
		if (hv_Number > 64) {
			CString cstr;
			cstr.Format(_T("相机%d#超出检测阈值,位置%.3f米,请检查膜面是否正常"),
				cameraNO, m_current_position);
			::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			return -1;
		}

		std::vector<SelectRegion> vSelect;
		HTuple end_val19 = hv_Number;
		HTuple step_val19 = 1;
		for (hv_i = 1; hv_i.Continue(end_val19, step_val19); hv_i += step_val19)
		{
			HalconCpp::SelectObj(ho_SelectedRegions, &ho_ObjectSelected, hv_i);
			//考虑是否要替换为AreaCenterGray()算子
			HalconCpp::AreaCenter(ho_ObjectSelected, &hv_Area, &hv_Row, &hv_Column);
			//最小外接圆
			HalconCpp::SmallestCircle(ho_ObjectSelected, &hv_RowCircle, &hv_ColumnCircle, &hv_Radius);
			//默认如果选择的区域大于图像尺寸的一半，则视为无效区域
			if (hv_Radius > 2048)
				continue;
			//区域周长
			HalconCpp::Contlength(ho_ObjectSelected, &hv_Contlength);			
			//计算像素平均值
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
				
		////删除相邻距离小于256的区域
		//std::vector<SelectRegion>::iterator it;
		//for (it = vSelect.begin(); it != vSelect.end(); it++)
		//{
		//	if (it != vSelect.end() - 1) {
		//		HTuple x, y;
		//		HalconCpp::TupleAbs(it->hv_Row_Center - (it + 1)->hv_Row_Center, &y);
		//		HalconCpp::TupleAbs(it->hv_Column_Center - (it + 1)->hv_Column_Center, &x);
		//		if (x < 128 || y < 128) {
		//			//在同已区域内则面积标记为 0
		//			it->area = 0.0f;
		//		}
		//	}
		//	else break;
		//}
		
		//反向迭代器，保存瑕疵图像及信息
		std::vector<SelectRegion>::reverse_iterator rit;
		for (rit = vSelect.rbegin(); rit != vSelect.rend(); rit++)
		{
			SelectRegion region;
			region = *rit;
			if (region.area == 0.0f)
				continue;
			else{
				//当所选的区域的面积大于256*256=65536，即区域范围大于瑕疵图像的尺寸时，保存其实际图像
				if (region.area >= 65536.0f)
				{
					//直接保存整个区域
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

				//瑕疵信息处理
				DefectType dtype;
				dtype.type              = ImageClassification(ho_ImagePart);
				dtype.center_x          = ((float)region.hv_Column_Center.D() + IMAGE_WIDTH * (cameraNO - 1))*HORIZON_PRECISION;
				//dtype.absolute_position = m_current_position + (float)region.hv_Row_Center.D() * VERTICAL_PRECISION / 1000.0f;
				dtype.absolute_position = (produced_num * IMAGE_HEIGHT + (float)region.hv_Row_Center.D()) * VERTICAL_PRECISION / 1000.0f;
				dtype.area              = region.area * HORIZON_PRECISION * VERTICAL_PRECISION;
				dtype.circle_radius     = region.radius * HORIZON_PRECISION;
				dtype.contlength        = region.contlength * HORIZON_PRECISION;
				dtype.pixel_value       = (int)region.pixelvalue;
				int rank = RankDivide(dtype);
				dtype.rank              = rank;
				//保存瑕疵信息
				vDFT.push_back(dtype);

				//图像保存，格式化文件名
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
				//DefectImage img;
				//img.ho_img = ho_ImagePart;
				//img.hv_name = hv_img_name;
				//EnterCriticalSection(&m_csDefImgList);
				//DefImgList.push_back(img);
				//LeaveCriticalSection(&m_csDefImgList);

			}
		}		
	}

	return 0;
}

//管理线程
UINT CImageProcess::ManageThread(LPVOID pParam)
{
	CImageProcess *pThis = (CImageProcess *)pParam;
	DWORD dwStop = 0;

	// Halcon 速度优化
	pThis->HalconInitAOP();
	//pThis->HalconOpenGPU(pThis->hv_GPU_Handle);

	//判断是否内存中是否已有参考图像
	pThis->m_referenceImage_OK = pThis->CheckReferenceImageAvilable();
	BOOL got_ref1 = FALSE, got_ref2 = FALSE, got_ref3 = FALSE, got_ref4 = FALSE;

	for(;;)
	{
		if (!pThis->m_referenceImage_OK) {
			//生成参考图像的三种方式：
			//1、从本地加载均值图像和标准差图像；
			//2、从本地加载测试图然后程序生成；
			//3、从相机获取图像然后生成；
			if (!pThis->TEST_MODEL) {
				if (!got_ref1) {
					got_ref1 = pThis->GenerateReferenceImage1(pThis->m_hi_average1, pThis->m_hi_deviation1);
					if (got_ref1) {
						CString cstr = L"获取1#相机参考图像";
						::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
					}
				}
				if (!got_ref2) {
					got_ref2 = pThis->GenerateReferenceImage2(pThis->m_hi_average2, pThis->m_hi_deviation2);
					if (got_ref2) {
						CString cstr = L"获取2#相机参考图像";
						::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
					}
				}
				if (!got_ref3) {
					got_ref3 = pThis->GenerateReferenceImage3(pThis->m_hi_average3, pThis->m_hi_deviation3);
					if (got_ref3) {
						CString cstr = L"获取3#相机参考图像";
						::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
					}
				}
				if (!got_ref4) {
					got_ref4 = pThis->GenerateReferenceImage4(pThis->m_hi_average4, pThis->m_hi_deviation4);
					if (got_ref4) {
						CString cstr = L"获取4#相机参考图像";
						::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
					}
				}
			}
			else {
				if (pThis->LoadRefImage("E:/DeVisionProject/OneCamera_0417/")) {
					got_ref1 = TRUE;
					got_ref2 = TRUE;
					got_ref3 = TRUE;
					got_ref4 = TRUE;
					CString cstr = L"已加载测试参考图像";
					::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
				}

				pThis->LoadSingleImage("E:/DeVisionProject/OneCamera_0417/test1");
				CString cstr = L"已加载测试图";
				::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
			}

			//四台相机都获取到后保存图像并结束获取
			if (got_ref1 && got_ref2 && got_ref3 && got_ref4) {
				pThis->m_referenceImage_OK = TRUE;

				if (pThis->SAVE_REFERENCE_IMAGE) {
					pThis->SaveDefectImage(pThis->m_hi_average1, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image1.bmp");
					pThis->SaveDefectImage(pThis->m_hi_average2, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image2.bmp");
					pThis->SaveDefectImage(pThis->m_hi_average3, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image3.bmp");
					pThis->SaveDefectImage(pThis->m_hi_average4, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image4.bmp");
					pThis->SaveDefectImage(pThis->m_hi_deviation1, (HTuple)pThis->m_strPath.c_str() + "ref\\dev1.bmp");
					pThis->SaveDefectImage(pThis->m_hi_deviation2, (HTuple)pThis->m_strPath.c_str() + "ref\\dev2.bmp");
					pThis->SaveDefectImage(pThis->m_hi_deviation3, (HTuple)pThis->m_strPath.c_str() + "ref\\dev3.bmp");
					pThis->SaveDefectImage(pThis->m_hi_deviation4, (HTuple)pThis->m_strPath.c_str() + "ref\\dev4.bmp");
				}
				::SendMessage(pThis->hMainWnd, WM_UPDATE_CONTROLS, 0, 0);
			}
		}

		//更新队列的总大小
		pThis->m_total_list_size = pThis->CheckTotalListSize();

		dwStop = WaitForSingleObject(pThis->StopManage_Event, 400);
		switch (dwStop)
		{
		case WAIT_TIMEOUT: {
			if (pThis->TEST_MODEL && !pThis->SYSTEM_PAUSE) {
				pThis->LoadImageToQueue();
				//CString cstr = L"加载测试图像到处理队列";
				//::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
			}
			break;
		}
		case WAIT_FAILED:
			return -1;
		case WAIT_OBJECT_0: {
			//while (pThis->CheckTotalListSize() > 0)
			//{
			//	//Sleep(200);
			//}
			pThis->m_total_list_size = 0;
			pThis->StopCalculateThreads();
			pThis->AllCalculateThreadStopped_Event.SetEvent();

			if(pThis->hv_GPU_Handle.Length() > 0)
				HalconCpp::DeactivateComputeDevice(pThis->hv_GPU_Handle);			

			CString cstr = L"结束处理线程";
			::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
			return 0;
		}
		}   

	}
	return 0;
}

//  1# 相机处理线程
UINT CImageProcess::ImageCalculate1_1(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread1_1_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while(!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref1;
	int valid_area = pImgProc->m_camera1_invalid_area;
	HImage hi_average = pImgProc->m_hi_average1;
	HImage hi_deviation = pImgProc->m_hi_deviation1;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread1_1_alive)
	{

		if(!pImgProc->m_ImgList1_1.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList1_1.front();
			pImgProc->m_ImgList1_1.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, valid_area, hv_height_def, hv_width_def);
					//重定义图像的定义域，分切图像 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_1, hi_average, hi_deviation, hi_def, produced_num*5, vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread1);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList1.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}			
			//更新位置信息: 总帧数 * 图像高度 * 纵向精度
			pImgProc->m_current_position = (produced_num * 5) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		}

		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}


	}

	//清空图像队列
	pImgProc->m_ImgList1_1.clear();
	pImgProc->is_thread1_1_alive = FALSE;
	Win::log("1#相机处理线程 1 结束");
	return 0;
}

UINT CImageProcess::ImageCalculate1_2(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread1_2_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref1;
	int valid_area = pImgProc->m_camera1_invalid_area;
	HImage hi_average = pImgProc->m_hi_average1;
	HImage hi_deviation = pImgProc->m_hi_deviation1;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread1_2_alive)
	{
		if (!pImgProc->m_ImgList1_2.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList1_2.front();
			pImgProc->m_ImgList1_2.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, valid_area, hv_height_def, hv_width_def);
					//重定义图像的定义域，分切图像 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_1, hi_average, hi_deviation, hi_def, produced_num*5+1,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread1);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList1.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//更新位置信息: 总帧数 * 图像高度 * 纵向精度
			pImgProc->m_current_position = (produced_num * 5 + 1) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		}

		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	//清空图像队列
	pImgProc->m_ImgList1_2.clear();

	pImgProc->is_thread1_2_alive = FALSE;
	Win::log("1#相机处理线程 2 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate1_3(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread1_3_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref1;
	int valid_area = pImgProc->m_camera1_invalid_area;
	HImage hi_average = pImgProc->m_hi_average1;
	HImage hi_deviation = pImgProc->m_hi_deviation1;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread1_3_alive)
	{
		if (!pImgProc->m_ImgList1_3.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList1_3.front();
			pImgProc->m_ImgList1_3.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, valid_area, hv_height_def, hv_width_def);
					//重定义图像的定义域，分切图像 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_1, hi_average, hi_deviation, hi_def, produced_num*5+2,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread1);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList1.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//更新位置信息: 总帧数 * 图像高度 * 纵向精度
			pImgProc->m_current_position = (produced_num * 5 + 2) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		} 
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}

	}

	//清空图像队列
	pImgProc->m_ImgList1_3.clear();

	pImgProc->is_thread1_3_alive = FALSE;
	Win::log("1#相机处理线程 3 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate1_4(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread1_4_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref1;
	int valid_area = pImgProc->m_camera1_invalid_area;
	HImage hi_average = pImgProc->m_hi_average1;
	HImage hi_deviation = pImgProc->m_hi_deviation1;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread1_4_alive)
	{
		if (!pImgProc->m_ImgList1_4.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList1_4.front();
			pImgProc->m_ImgList1_4.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, valid_area, hv_height_def, hv_width_def);
					//重定义图像的定义域，分切图像 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_1, hi_average, hi_deviation, hi_def, produced_num*5+3,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread1);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList1.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//更新位置信息: 总帧数 * 图像高度 * 纵向精度
			pImgProc->m_current_position = (produced_num * 5 + 3) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}

	//清空图像队列
	pImgProc->m_ImgList1_4.clear();

	pImgProc->is_thread1_4_alive = FALSE;
	Win::log("1#相机处理线程 4 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate1_5(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread1_5_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref1;
	int valid_area = pImgProc->m_camera1_invalid_area;
	HImage hi_average = pImgProc->m_hi_average1;
	HImage hi_deviation = pImgProc->m_hi_deviation1;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread1_5_alive)
	{
		if (!pImgProc->m_ImgList1_5.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList1_5.front();
			pImgProc->m_ImgList1_5.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, valid_area, hv_height_def, hv_width_def);
					//重定义图像的定义域，分切图像 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_1, hi_average, hi_deviation, hi_def, produced_num*5+4, vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread1);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList1.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			//更新位置信息: 总帧数 * 图像高度 * 纵向精度
			pImgProc->m_current_position = (produced_num * 5 + 4) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}

	//清空图像队列
	pImgProc->m_ImgList1_5.clear();

	pImgProc->is_thread1_5_alive = FALSE;
	Win::log("1#相机处理线程 5 结束");

	return 0;
}

//  2# 相机处理线程
UINT CImageProcess::ImageCalculate2_1(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread2_1_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref2;
	HImage hi_average = pImgProc->m_hi_average2;
	HImage hi_deviation = pImgProc->m_hi_deviation2;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread2_1_alive)
	{
		if (!pImgProc->m_ImgList2_1.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList2_1.front();
			pImgProc->m_ImgList2_1.pop_front();
			
			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_2, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_2, hi_average, hi_deviation, hi_def, produced_num*5,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread2);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList2.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList2_1.clear();
	pImgProc->is_thread2_1_alive = FALSE;
	Win::log("2#相机处理线程 1 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate2_2(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread2_2_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref2;
	HImage hi_average = pImgProc->m_hi_average2;
	HImage hi_deviation = pImgProc->m_hi_deviation2;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread2_2_alive)
	{
		if (!pImgProc->m_ImgList2_2.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList2_2.front();
			pImgProc->m_ImgList2_2.pop_front();

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_2, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_2, hi_average, hi_deviation, hi_def, produced_num*5+1,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread2);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList2.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList2_2.clear();

	pImgProc->is_thread2_2_alive = FALSE;
	Win::log("2#相机处理线程 2 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate2_3(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread2_3_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref2;
	HImage hi_average = pImgProc->m_hi_average2;
	HImage hi_deviation = pImgProc->m_hi_deviation2;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread2_3_alive)
	{
		if (!pImgProc->m_ImgList2_3.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList2_3.front();
			pImgProc->m_ImgList2_3.pop_front();

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_2, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_2, hi_average, hi_deviation, hi_def, produced_num*5+2,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread2);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList2.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList2_3.clear();

	pImgProc->is_thread2_3_alive = FALSE;
	Win::log("2#相机处理线程 3 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate2_4(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread2_4_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref2;
	HImage hi_average = pImgProc->m_hi_average2;
	HImage hi_deviation = pImgProc->m_hi_deviation2;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread2_4_alive)
	{
		if (!pImgProc->m_ImgList2_4.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList2_4.front();
			pImgProc->m_ImgList2_4.pop_front();

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_2, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_2, hi_average, hi_deviation, hi_def, produced_num*5+3, vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread2);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList2.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList2_4.clear();

	pImgProc->is_thread2_4_alive = FALSE;
	Win::log("2#相机处理线程 4 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate2_5(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread2_5_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref2;
	HImage hi_average = pImgProc->m_hi_average2;
	HImage hi_deviation = pImgProc->m_hi_deviation2;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread2_5_alive)
	{
		if (!pImgProc->m_ImgList2_5.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList2_5.front();
			pImgProc->m_ImgList2_5.pop_front();

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_2, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_2, hi_average, hi_deviation, hi_def, produced_num*5+4,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread2);

			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList2.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList2_5.clear();

	pImgProc->is_thread2_5_alive = FALSE;
	Win::log("2#相机处理线程 5 结束");

	return 0;
}

//  3# 相机处理线程
UINT CImageProcess::ImageCalculate3_1(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread3_1_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref3;
	HImage hi_average = pImgProc->m_hi_average3;
	HImage hi_deviation = pImgProc->m_hi_deviation3;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread3_1_alive)
	{
		if (!pImgProc->m_ImgList3_1.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList3_1.front();
			pImgProc->m_ImgList3_1.pop_front();

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_3, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_3, hi_average, hi_deviation, hi_def, produced_num*5,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread3);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList3.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList3_1.clear();

	pImgProc->is_thread3_1_alive = FALSE;
	Win::log("3#相机处理线程 1 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate3_2(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread3_2_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref3;
	HImage hi_average = pImgProc->m_hi_average3;
	HImage hi_deviation = pImgProc->m_hi_deviation3;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread3_2_alive)
	{
		if (!pImgProc->m_ImgList3_2.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList3_2.front();
			pImgProc->m_ImgList3_2.pop_front();

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_3, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_3, hi_average, hi_deviation, hi_def, produced_num*5+1,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread3);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList3.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList3_2.clear();

	pImgProc->is_thread3_2_alive = FALSE;
	Win::log("3#相机处理线程 2 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate3_3(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread3_3_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref3;
	HImage hi_average = pImgProc->m_hi_average3;
	HImage hi_deviation = pImgProc->m_hi_deviation3;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread3_3_alive)
	{
		if (!pImgProc->m_ImgList3_3.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList3_3.front();
			pImgProc->m_ImgList3_3.pop_front();

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_3, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_3, hi_average, hi_deviation, hi_def, produced_num*5+2,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread3);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList3.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList3_3.clear();

	pImgProc->is_thread3_3_alive = FALSE;
	Win::log("3#相机处理线程 3 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate3_4(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread3_4_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref3;
	HImage hi_average = pImgProc->m_hi_average3;
	HImage hi_deviation = pImgProc->m_hi_deviation3;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread3_4_alive)
	{
		if (!pImgProc->m_ImgList3_4.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList3_4.front();
			pImgProc->m_ImgList3_4.pop_front();

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_3, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_3, hi_average, hi_deviation, hi_def, produced_num*5+3,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread3);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList3.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList3_4.clear();

	pImgProc->is_thread3_4_alive = FALSE;
	Win::log("3#相机处理线程 4 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate3_5(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread3_5_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref3;
	HImage hi_average = pImgProc->m_hi_average3;
	HImage hi_deviation = pImgProc->m_hi_deviation3;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread3_5_alive)
	{
		if (!pImgProc->m_ImgList3_5.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList3_5.front();
			pImgProc->m_ImgList3_5.pop_front();

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_3, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_3, hi_average, hi_deviation, hi_def, produced_num*5+4,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread3);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList3.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList3_5.clear();

	pImgProc->is_thread3_5_alive = FALSE;
	Win::log("3#相机处理线程 5 结束");

	return 0;
}

//  4# 相机处理线程
UINT CImageProcess::ImageCalculate4_1(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread4_1_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref4;
	int valid_area = pImgProc->m_camera4_invalid_area;
	HImage hi_average = pImgProc->m_hi_average4;
	HImage hi_deviation = pImgProc->m_hi_deviation4;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread4_1_alive)
	{
		if (!pImgProc->m_ImgList4_1.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList4_1.front();
			pImgProc->m_ImgList4_1.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, 0, hv_height_def, hv_width_def - valid_area);
					//重定义图像的定义域，分切图像 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_4, hi_average, hi_deviation, hi_def, produced_num*5, vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread4);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList4.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList4_1.clear();

	pImgProc->is_thread4_1_alive = FALSE;
	Win::log("4#相机处理线程 1 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate4_2(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread4_2_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref4;
	int valid_area = pImgProc->m_camera4_invalid_area;
	HImage hi_average = pImgProc->m_hi_average4;
	HImage hi_deviation = pImgProc->m_hi_deviation4;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread4_2_alive)
	{
		if (!pImgProc->m_ImgList4_2.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList4_2.front();
			pImgProc->m_ImgList4_2.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, 0, hv_height_def, hv_width_def - valid_area);
					//重定义图像的定义域，分切图像 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_4, hi_average, hi_deviation, hi_def, produced_num*5+1,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread4);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList4.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList4_2.clear();

	pImgProc->is_thread4_2_alive = FALSE;
	Win::log("4#相机处理线程 2 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate4_3(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread4_3_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref4;
	int valid_area = pImgProc->m_camera4_invalid_area;
	HImage hi_average = pImgProc->m_hi_average4;
	HImage hi_deviation = pImgProc->m_hi_deviation4;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread4_3_alive)
	{
		if (!pImgProc->m_ImgList4_3.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList4_3.front();
			pImgProc->m_ImgList4_3.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, 0, hv_height_def, hv_width_def - valid_area);
					//重定义图像的定义域，分切图像 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_4, hi_average, hi_deviation, hi_def, produced_num*5+2,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread4);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList4.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}

			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList4_3.clear();

	pImgProc->is_thread4_3_alive = FALSE;
	Win::log("4#相机处理线程 3 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate4_4(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread4_4_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref4;
	int valid_area = pImgProc->m_camera4_invalid_area;
	HImage hi_average = pImgProc->m_hi_average4;
	HImage hi_deviation = pImgProc->m_hi_deviation4;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread4_4_alive)
	{
		if (!pImgProc->m_ImgList4_4.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList4_4.front();
			pImgProc->m_ImgList4_4.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, 0, hv_height_def, hv_width_def - valid_area);
					//重定义图像的定义域，分切图像 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_4, hi_average, hi_deviation, hi_def, produced_num*5+3,vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread4);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList4.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList4_4.clear();

	pImgProc->is_thread4_4_alive = FALSE;
	Win::log("4#相机处理线程 4 结束");

	return 0;
}

UINT CImageProcess::ImageCalculate4_5(LPVOID pParam)
{
	CImageProcess *pImgProc = (CImageProcess *)pParam;
	pImgProc->is_thread4_5_alive = TRUE;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref4;
	int valid_area = pImgProc->m_camera4_invalid_area;
	HImage hi_average = pImgProc->m_hi_average4;
	HImage hi_deviation = pImgProc->m_hi_deviation4;

	UINT64 produced_num = 0;
	while (pImgProc->is_thread4_5_alive)
	{
		if (!pImgProc->m_ImgList4_5.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList4_5.front();
			pImgProc->m_ImgList4_5.pop_front();
			HObject ho_ImagePart;
			if (pImgProc->REDUCE_BLACK_EDGE) {
				if (valid_area != 0 && valid_area != -1) {
					HObject ho_Image, ho_Region, ho_ImageReduced;
					HTuple hv_width_def, hv_height_def;

					HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
					HalconCpp::GenRectangle1(&ho_Region, 0, 0, hv_height_def, hv_width_def - valid_area);
					//重定义图像的定义域，分切图像 
					HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
					HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
					hi_def = ho_ImagePart;
				}
			}


			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			//pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->StandDeviationAlgorithm(CAMERA_4, hi_average, hi_deviation, hi_def, produced_num*5+4, vdef);

			//已处理的图像总数
			produced_num += 1;

			EnterCriticalSection(&pImgProc->m_csCalculateThread4);
			//根据瑕疵绝对位置排序后存入瑕疵队列
			if (!vdef.empty()) {
				std::sort(vdef.begin(), vdef.end());
				std::vector<DefectType>::iterator it;
				for (it = vdef.begin(); it != vdef.end(); ++it)
				{
					pImgProc->m_DFTList4.push_back(*it);
					//pImgProc->m_NO_dft += 1;
				}
			}
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}
		
		//if (WAIT_OBJECT_0 == WaitForSingleObject(StopCalculate_Event, INFINITE)) {
		//	break;
		//}
	}
	pImgProc->m_ImgList4_5.clear();

	pImgProc->is_thread4_5_alive = FALSE;
	Win::log("4#相机处理线程 5 结束");

	return 0;
}
