
#include "stdafx.h"
#include "DeVision.h"
#include "CImageProcessing.h"
#include "Log.h"
#include <winbase.h>
#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>


CImageProcessing::CImageProcessing()
{
	m_pnSystemState = nullptr;
	SAVE_REFERENCE_IMAGE = FALSE;
	m_strPath = "D:\\瑕疵检测数据记录\\2瑕疵图像记录\\";
	m_nTotalListNumber = 0;
	m_unImageIndex = 0;

	m_nrankMethod = 0;
	m_frankValue1 = 10.0f;
	m_frankValue2 = 20.0f;
	m_frankValue3 = 50.0f;

	m_nCutBorderValue = 100;            //   100像素约 5mm
	m_nCutBorderStatue = 0;

	InitializeCriticalSection(&m_csCalculateThread);

	InitialImageProcess();
}

CImageProcessing::~CImageProcessing()
{
	DeleteCriticalSection(&m_csCalculateThread);

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

int CImageProcessing::getSystemState()
{
	if (m_pnSystemState != nullptr)
		return *m_pnSystemState;
	else
		return 0;
}

void CImageProcessing::InitialImageProcess()
{
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return;

	m_hFinishedProcess = CreateEventA(NULL, FALSE, FALSE, NULL);

	return;
}

// CWinThread 线程
BOOL CImageProcessing::BeginProcess()
{
	// Halcon 速度优化
	//HalconInitAOP();
	//Sleep(300);

	bool state;
	std::string str_path;
	if (!GetSavePath(str_path)) {
		cstrlog.Format(_T("保存目录不存在"));
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		return FALSE;
	}

	//创建管理线程
	m_unImageIndex = 0;
	state = m_ManageThread = AfxBeginThread(ManageThread, this);
	if (!state) {
		cstrlog.Format(_T("管理线程创建失败"));
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		return FALSE;
	}

#ifdef TESTMODEL
	state = acquireImageThread = AfxBeginThread(run, this);
	if (!state) {
		cstrlog.Format(_T("图像加载线程创建失败"));
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		return FALSE;
	}
#endif

	for (int index = 0; index < m_threadnum; index++) {
		state = m_CalculateThread[index] = AfxBeginThread(m_threadCallBack[index], this);
		if (state) {
			cstrlog.Format(_T("图像处理线程: %d, 创建成功"), index + 1);
			::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrlog, NULL);
		}
		else {
			cstrlog.Format(_T("图像处理线程: %d, 创建失败"), index + 1);
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
			return FALSE;
		}
	}

	ResetEvent(m_hFinishedProcess);
	//StopManage_Event.ResetEvent();
	//CalculateThread_1_StoppedEvent.ResetEvent();
	CalculateThread_2_StoppedEvent.ResetEvent();
	CalculateThread_3_StoppedEvent.ResetEvent();
	CalculateThread_4_StoppedEvent.ResetEvent();
	CalculateThread_5_StoppedEvent.ResetEvent();

	return TRUE;
}

// std 线程
BOOL CImageProcessing::BeginProcess(int std)
{
	// Halcon 速度优化
	//HalconInitAOP();
	//Sleep(300);

	std::string str_path;
	if (!GetSavePath(str_path)) {
		cstrlog.Format(_T("保存目录不存在"));
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		return FALSE;
	}

	//创建管理线程
	m_hi_average.Clear();
	m_hi_deviation.Clear();
	m_unImageIndex = 0;
	if (thdManageThread != NULL)
		thdManageThread = NULL;
	if(!thdManageThread)
		thdManageThread = new std::thread(&CImageProcessing::stdManageThread, this);

#ifdef TESTMODEL
	if (thdAcquireImageThread != NULL)
		thdAcquireImageThread = NULL;
	if (!thdAcquireImageThread)
		thdAcquireImageThread = new std::thread(&CImageProcessing::stdRunLoad, this);
#endif

	for (int index = 0; index < m_threadnum; index++) {
		if (thdCalculateThread[index] != NULL)
			thdCalculateThread[index] = NULL;
		if (!thdCalculateThread[index])
			thdCalculateThread[index] = new std::thread(&CImageProcessing::stdImageCalculate, this, index);
	}

	ResetEvent(m_hFinishedProcess);

	return TRUE;
}

void CImageProcessing::StopManageThread()
{
	// 线程停止
	//StopManage_Event.SetEvent();

#ifdef TESTMODEL
	bstop_add = false;
	run_add = false;
#endif

	StopCalculateThreads();
}

void CImageProcessing::StopCalculateThreads()
{
	for (int index = 0; index < m_threadnum; index++) {
		m_bThreadAlive[index] = FALSE;
		run_calculate[index] = false;
	}

	//if (hv_GPU_Handle.Length() > 0)
	//	HalconCpp::DeactivateComputeDevice(hv_GPU_Handle);

	//m_listAcquiredImage.clear();
}

void CImageProcessing::RestartProcess()
{

}

BOOL CImageProcessing::IsThreadsAlive()
{
	for (int index = 0; index < m_threadnum; index++) {
		if (m_bThreadAlive[index] == TRUE)
			return TRUE;
	}
	return FALSE;
}

BOOL CImageProcessing::IsThreadsAlive(int std)
{
	for (int index = 0; index < m_threadnum; index++) {
		if (run_calculate[index] == true)
			return true;
	}

	return false;
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
		CString cstr = L"参考图像目录不存在";
		SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}

	//读取参考图像1
	if (1) {
		std::string ref_image_name1 = "reference_imageX.png";
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
		m_hi_ref = img1;

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
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8)		{
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
		m_hi_average = ho_ImageAverage;
		m_hi_deviation = ho_ImageDeviation;
	}
	return TRUE;
}

BOOL CImageProcessing::GetRefImgWithoutBouder(const char* imgname)
{
	//读取参考图像1
	if (1) {
		//std::string ref_image_name1 = "reference_imageZ.bmp";
		HTuple hv_ref_image_name1 = (HTuple)((CA2W)imgname);
		HImage img1;
		HalconCpp::ReadImage(&img1, hv_ref_image_name1);

		// 判断光源是否打开
		HObject ho_regionTotal;
		HTuple hv_meanTotal, hv_deviationTotal;
		HalconCpp::Threshold(img1, &ho_regionTotal, 0, 255);
		HalconCpp::Intensity(ho_regionTotal, img1, &hv_meanTotal, &hv_deviationTotal);
		m_hvGrayMeanValue = hv_meanTotal;
		m_hvGrayDeviationValue = hv_deviationTotal;
		if (hv_meanTotal < 32) {
			CString cstr = L"光源未启动，图像过暗";
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
		//创建 全 0 值图像
		HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
		HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
		HTuple end_val8 = hv_Width - 1;
		HTuple step_val8 = 1;
		for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8) {
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
		m_hi_average = ho_ImageAverage;
		m_hi_deviation = ho_ImageDeviation;
	}
	return TRUE;
}

int CImageProcessing::ifCutBouder(HObject src, HObject &dst, HObject &region)
{
	int state = 0;

	//自动去除边界：减去平均像素值, 如过有边界则返回 true
	HObject ho_Region, ho_ImageMean, ho_ImageSub, ho_Region2, ho_RegionMoved, ho_RectanglePart;
	HTuple hv_Width, hv_Height, hv_Mean, hv_Deviation, hv_Row1, hv_Row2, hv_Column1, hv_Column2;
	HTuple hv_Value, hv_TopLeft, hv_Border;

	HalconCpp::GetImageSize(src, &hv_Width, &hv_Height);
	HalconCpp::Threshold(src, &ho_Region, 0, 255);
	HalconCpp::Intensity(ho_Region, src, &hv_Mean, &hv_Deviation);
	HalconCpp::GenImageConst(&ho_ImageMean, "byte", hv_Width, hv_Height);
	HalconCpp::OverpaintRegion(ho_ImageMean, ho_Region, hv_Mean, "fill");
	//  8 为设置的滤波值
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

	//判断是左侧去边还是右侧
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

	//将区域拟合成矩形
	HalconCpp::SmallestRectangle1(ho_RegionMoved, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	HalconCpp::GenRectangle1(&ho_RectanglePart, hv_Row1, hv_Column1, hv_Row2, hv_Column2);
	//对原图进行裁剪
	HalconCpp::ReduceDomain(src, ho_RectanglePart, &region);
	HalconCpp::CropDomain(region, &dst);

	return state;
}

//把图像加载到内存队列
void CImageProcessing::LoadImageToQueue()
{
	for (int index = 0; index < m_threadnum; index++) {
		m_listAcquiredImage.push_back(m_hi_test);
	}
}

BOOL CImageProcessing::LoadOneImageToQueue(std::string folder_path, int next_number)
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

//获取处理线程的队列大小
int CImageProcessing::CheckTotalListSize()
{
	size_t list_size = 0;

	list_size = m_listAcquiredImage.size();

	return (int)list_size;
}

//从本地加载参考图像
BOOL CImageProcessing::LoadDefaultRefAndDevImage(std::string path)
{
	if (path.empty())
		return FALSE;

	std::string ref_image_name = "reference_image.bmp";
	std::string dev_image_name = "dev.bmp";

	if (!IsPathExist(path)) {
		CString cstr = L"读取图像失败：默认参考图像目录不存在";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}
	else {
		if (!IsFileExist(path + ref_image_name) || !IsFileExist(path + dev_image_name)) {
			CString cstr = L"读取图像失败：默认参考图像1不存在";
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

//生成参考图像
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

	// 判断光源是否打开
	HObject ho_regionTotal;
	HTuple hv_meanTotal, hv_deviationTotal;
	HalconCpp::Threshold(result, &ho_regionTotal, 0, 255);
	HalconCpp::Intensity(ho_regionTotal, result, &hv_meanTotal, &hv_deviationTotal);
	m_hvGrayMeanValue = hv_meanTotal;
	m_hvGrayDeviationValue = hv_deviationTotal;
	if (hv_meanTotal < 32) {
		CString cstr = L"光源未启动，图像过暗";
		::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return FALSE;
	}

	// 检测并切边
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
	//创建 全 0 值图像
	HalconCpp::GenImageConst(&ho_ImageAverage, "byte", hv_Width, hv_Height);
	HalconCpp::GenImageConst(&ho_ImageDeviation, "byte", hv_Width, hv_Height);
	HTuple end_val8 = hv_Width - 1;
	HTuple step_val8 = 1;
	for (hv_column = 0; hv_column.Continue(end_val8, step_val8); hv_column += step_val8){
		//参数2：左上角点的行，参数3：左上角点的列，参数4：右下角点的行，参数5：右下角点的列
		HalconCpp::GenRectangle1(&ho_Rectangle, 0, hv_column, 8192, hv_column + 1);
		//均值/方差/标准差
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

//判断参考图像：均值图和方差图，是否已经生成
BOOL CImageProcessing::CheckReferenceImageAvilable()
{
	if (!m_hi_average.IsInitialized())	return FALSE;
	if (!m_hi_deviation.IsInitialized())	return FALSE;

	return TRUE;
}

//生成去掉黑边的图像
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

//瑕疵等级判定
unsigned short int CImageProcessing::RankDivide(float area, unsigned radius, unsigned contlength)
{
	unsigned short int rank = 0;

	// 按面积进行瑕疵的等级划分, 输入的area为像素值
	//float value = area * (HORIZON_PRECISION * VERTICAL_PRECISION);   
	float value = 0.0f;
	switch (m_nrankMethod)
	{
	case 0:
		value = area;
		break;
	case 1:
		value = (float)radius;
		break;
	case 2:
		value = (float)contlength;
		break;
	}

	if (value < m_frankValue1)
		rank = 0;
	else if (m_frankValue1 <= value && value < m_frankValue2)
		rank = 1;
	else if (m_frankValue2 <= value && value < m_frankValue3)
		rank = 2;
	else if (m_frankValue3 <= value)
		rank = 3;

	return rank;
}

//分类算法，分类器    SVM
void CImageProcessing::InitialClassify(const char * svm_file_name, HTuple &hv_SVMHandle)
{
	//m_hvClass.Clear();
	//m_hvClass[0] = "matter";         //  异物
	//m_hvClass[1] = "bubble";         //  气泡
	//m_hvClass[2] = "convex";         //  凹凸
	//m_hvClass[3] = "coatting";       //  涂布

	//   读取分类器
	HalconCpp::ReadClassSvm(svm_file_name, &hv_SVMHandle);

}


int CImageProcessing::ClassifyRegionsWithSVM(HTuple hv_SVMHandle, HObject img)
{
	if (!img.IsInitialized())
		return 4;

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
	//计算标准差
	HalconCpp::TupleSqrt(m_hvGrayDeviationValue, &hv_Sqrt);
	HalconCpp::OverpaintRegion(ho_ImageAverage, ho_Region, m_hvGrayMeanValue, "fill");
	HalconCpp::OverpaintRegion(ho_ImageDeviation, ho_Region, ((5 * hv_Sqrt).TupleConcat(255)).TupleMin(), "fill");
	//得到瑕疵区域
	HalconCpp::SubImage(ho_ImageMedian, ho_ImageAverage, &ho_ImageSub1, 1, 0);
	HalconCpp::SubImage(ho_ImageAverage, ho_ImageMedian, &ho_ImageSub2, 1, 0);
	HalconCpp::AddImage(ho_ImageSub1, ho_ImageSub2, &ho_ImageResult, 0.5, 0);
	HalconCpp::SubImage(ho_ImageResult, ho_ImageDeviation, &ho_ImageOut, 1, 0);
	//通过改变阈值修改所选区域
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
	HTuple hv_num;
	HalconCpp::CountObj(ho_SelectedRegions, &hv_num);
	if (hv_num.TupleInt() == 0)
		return 4;
	HalconCpp::Union1(ho_SelectedRegions, &ho_RegionUnion);

	//计算特征值: 15 个
	HTuple hv_Features;
	CalculateFeatures(img, ho_RegionUnion, hv_Features);

	//分类
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

//保存瑕疵图像
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

//检测算法：一种基于多目机器视觉的光学薄膜瑕疵检测系统
int CImageProcessing::StandDeviationAlgorithm(
	HImage hi_img
	, HTuple hv_SVM
	, std::vector<DeffectInfo> &vecDftInfo
	, std::vector<HalconCpp::HObject> &vecDftImg)
{
	HObject  ho_Image, ho_ImageAverage, ho_ImageDeviation, ho_ImageMedianDFT;
	HObject  ho_ImageSub1, ho_ImageSub2, ho_ImageAddSub, ho_ImageResult;
	HObject  ho_Region, ho_RegionDilation, ho_RegionErosion;
	HObject  ho_ConnectedRegions, ho_SelectedRegions, ho_ObjectSelected, ho_Rectangle;
	HObject  ho_ImageReduced, ho_ImagePart;
	HTuple   hv_Width, hv_Height, hv_Number;
	HTuple   hv_i, hv_Area, hv_Row, hv_Column, hv_RowCircle, hv_ColumnCircle, hv_Radius, hv_Contlength;

	ho_Image = hi_img;
	//HalconCpp::CopyImage(hi_img, &ho_Image);

	//是否需要切边
	if (m_nCutBorderStatue > 0) {
		// m_nCutBorder = 1为左切边， m_nCutBorder = 2为右切边
		HObject ho_ImageReduced, ho_ImagePart;
		HalconCpp::ReduceDomain(ho_Image, m_hoBorderRegion, &ho_ImageReduced);
		HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
		ho_Image = ho_ImagePart;
	}

	ho_ImageAverage = m_hi_average;
	ho_ImageDeviation = m_hi_deviation;
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
	//通过阈值控制检出精度
	HalconCpp::Threshold(ho_ImageResult, &ho_Region, 1, 255);
	if (!ho_Region.IsInitialized())
		return 0;

	//膨胀,腐蚀,用于减少region的数量
	HalconCpp::DilationCircle(ho_Region, &ho_RegionDilation, 63.5);
	HalconCpp::ErosionCircle(ho_RegionDilation, &ho_RegionErosion, 61.5);
	HalconCpp::Connection(ho_RegionErosion, &ho_ConnectedRegions);
	// m_fMin_Radius 与 m_fMax_Radius均为直径
	int min_select = (int)std::pow(m_fMin_Radius / HORIZON_PRECISION, 2);
	int max_select = (int)std::pow(m_fMax_Radius / HORIZON_PRECISION, 2);
	HalconCpp::SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and",	min_select + 1,	max_select + 1);
	HalconCpp::CountObj(ho_SelectedRegions, &hv_Number);
	if (hv_Number == 0)
		return 0;
	else if (hv_Number >= 20) {
		//当单张图像中选择的区域超过 20 个，则报警
		CString cstr;
		cstr.Format(_T("超出检测阈值,位置%.3f米,请检查膜面是否正常"), m_unImageIndex * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f);
		::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
	}

	//对区域进行排序
	HObject ho_SortedRegion;
	HalconCpp::SortRegion(ho_SelectedRegions, &ho_SortedRegion, "lower_left", "true", "column");
	for (hv_i = 1; hv_i.Continue(hv_Number, 1); hv_i += 1) {
		HalconCpp::SelectObj(ho_SortedRegion, &ho_ObjectSelected, hv_i);
		HalconCpp::AreaCenter(ho_ObjectSelected, &hv_Area, &hv_Row, &hv_Column);
		HTuple hv_X = hv_Column;
		HTuple hv_Y = hv_Row;
		//最小外接圆
		HalconCpp::SmallestCircle(ho_ObjectSelected, &hv_RowCircle, &hv_ColumnCircle, &hv_Radius);
		//默认如果选择的区域大于图像尺寸的一半，则视为无效区域
		if (hv_Radius >= 2048)
			continue;
		//区域周长
		HalconCpp::Contlength(ho_ObjectSelected, &hv_Contlength);
		//计算像素平均值
		HTuple hv_mean, hv_deviation;
		HalconCpp::Intensity(ho_ObjectSelected, ho_Image, &hv_mean, &hv_deviation);
		//当所选的区域的面积大于256*256=65536，即区域范围大于瑕疵图像的尺寸时，保存整个区域
		if (hv_Area > 65535) {
			HTuple hv_x1, hv_y1, hv_x2, hv_y2;
			hv_x1 = hv_Column - hv_Radius;
			hv_y1 = hv_Row - hv_Radius;
			hv_x2 = hv_Column + hv_Radius;
			hv_y2 = hv_Row + hv_Radius;
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
			if (0 != (hv_Row < 127)) {
				hv_Row = 127;
			}
			else if (0 != (hv_Row > (hv_Height - 127))) {
				hv_Row = hv_Height - 127;
			}
			if (0 != (hv_Column < 127)) {
			}
			else if (0 != (hv_Column > (hv_Width - 127))) {
				hv_Column = hv_Width - 127;
			}
			HalconCpp::GenRectangle1(&ho_Rectangle, hv_Row - 127, hv_Column - 127, hv_Row + 128, hv_Column + 128);
			HalconCpp::ReduceDomain(ho_Image, ho_Rectangle, &ho_ImageReduced);
			HalconCpp::CropDomain(ho_ImageReduced, &ho_ImagePart);
		}
		// 保存瑕疵图像
		vecDftImg.push_back(ho_ImagePart);
		// 保存瑕疵信息
		DeffectInfo dftInfo;
		dftInfo.type = (unsigned short)ClassifyRegionsWithSVM(hv_SVM, ho_ImagePart);  //  分类
		dftInfo.x = (float)hv_X[0].D();
		dftInfo.y = (float)hv_Y[0].D();
		dftInfo.area = (float)hv_Area[0].D();
		dftInfo.radius = (unsigned short)hv_Radius[0].D() * 2;    // 直径
		dftInfo.contlength = (unsigned short)hv_Contlength[0].D();
		dftInfo.rank = RankDivide(dftInfo.area, dftInfo.radius, dftInfo.contlength);    //  等级
		dftInfo.pixel_value = (unsigned short)hv_mean.D();
		vecDftInfo.push_back(dftInfo);
	}

	return 0;
}

// add noise
void CImageProcessing::AddNoise(HObject &hoImg)
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
		// 填充随机像素值
		hv_value = hv_minvalue + (HTuple::TupleRand(1)*(hv_maxvalue - hv_minvalue));
		OverpaintRegion(hoImg, ho_ObjectSelected, hv_value, "fill");
	}
}


void CImageProcessing::AddNoise(HObject imgIn, HObject &imgOut)
{
	HTuple  hv_Width, hv_Height, hv_minvalue, hv_maxvalue;
	HTuple  hv_Number, hv_index, hv_value;
	HObject ho_Regions, ho_ObjectSelected;

	HObject hoImg;
	HalconCpp::CopyImage(imgIn, &hoImg);
	GetImageSize(hoImg, &hv_Width, &hv_Height);

	unsigned seed = (unsigned)std::time(0);
	std::srand(seed);
	int noiseNum = std::rand() % 2;
	if (noiseNum == 0) {
		imgOut = imgIn;
		return;
	}
	GenRandomRegions(&ho_Regions, "ellipse", 1, 32, 1, 32, -0.7854, 0.7854, noiseNum, hv_Width, hv_Height);
	CountObj(ho_Regions, &hv_Number);

	hv_minvalue = 0;
	hv_maxvalue = 255;
	for (hv_index = 1; hv_index.Continue(hv_Number, 1); hv_index += 1) {
		SelectObj(ho_Regions, &ho_ObjectSelected, hv_index);
		// 填充随机像素值
		hv_value = hv_minvalue + (HTuple::TupleRand(1)*(hv_maxvalue - hv_minvalue));
		OverpaintRegion(hoImg, ho_ObjectSelected, hv_value, "fill");
	}
	imgOut = hoImg;
}

//管理线程
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
		std::string imgname = "D:\\瑕疵检测数据记录\\test_image\\reference_image.png";
		bresult = pThis->GetRefImgWithoutBouder(imgname.c_str());
#else
		bresult = pThis->GenerateReferenceImage(pThis->m_hi_average, pThis->m_hi_deviation);
#endif					
		if (bresult) {
			if (pThis->SAVE_REFERENCE_IMAGE) {
				//std::string filename = "D:\\DetectRecords\\ReferenceImage.png";
				//pThis->SaveReferenceImage(filename.c_str());
				CString cstr = L"参考图像已保存";
				::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
			}
			CString cstr = L"参考图像已生成";
			::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
			::SendMessage(pThis->hMainWnd, WM_UPDATE_CONTROLS, NULL, NULL);
			break;
		}
		else
			continue;
	}

	return 0;
}

// 测试模式读取图像
UINT CImageProcessing::run(LPVOID pParam)
{
	CImageProcessing *pThis = (CImageProcessing*)pParam;
	pThis->bstop_add = true;
	//CSingleLock singlelock(&pThis->mutex);

	std::string imgname = "D:\\瑕疵检测数据记录\\test_image\\test_image.png";
	HTuple hv_image_name = (HTuple)((CA2W)imgname.c_str());
	HObject test_img;
	HalconCpp::ReadImage(&test_img, hv_image_name);
	while (pThis->bstop_add) {
		if (*pThis->m_pnSystemState == 4) {
			Sleep(500);
			continue;
		}

		if (pThis->m_listAcquiredImage.size() < 1) {
			HObject noise_img;
			pThis->AddNoise(test_img, noise_img);
			//singlelock.Lock();
			//if (singlelock.IsLocked()) {
			//	pThis->m_listAcquiredImage.push_back(noise_img);
			//	pThis->m_nTotalListNumber += 1;
			//}
			//singlelock.Unlock();

			pThis->m_listAcquiredImage.push_back(noise_img);
			pThis->m_nTotalListNumber += 1;
		}
		Sleep(733);
	}

	return 0;
}

//图像处理线程
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
			singlelock.Lock();     //  没被调用就上锁自己用，已被调用就等着
			if (singlelock.IsLocked()) {
				hi_acquire = pImgProc->m_listAcquiredImage.front();
				pImgProc->m_listAcquiredImage.pop_front();
				pImgProc->m_unImageIndex += 1;
			}
			singlelock.Unlock();   //  解锁

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

			//瑕疵检测算法
			pImgProc->StandDeviationAlgorithm(hi_acquire, hv_SVMHandle, vec_dft_info, vec_dft_img);
			//瑕疵信息和图像存入队列
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
			CString cstr = L"计算线程已结束";
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
			singlelock.Lock();     //  没被调用就上锁自己用，已被调用就等着
			if (singlelock.IsLocked()) {
				hi_acquire = pImgProc->m_listAcquiredImage.front();
				pImgProc->m_listAcquiredImage.pop_front();
				pImgProc->m_unImageIndex += 1;
			}
			singlelock.Unlock();   //  解锁


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

			//瑕疵检测算法
			pImgProc->StandDeviationAlgorithm(hi_acquire, hv_SVMHandle, vec_dft_info, vec_dft_img);
			//瑕疵信息和图像存入队列
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
			singlelock.Lock();     //  没被调用就上锁自己用，已被调用就等着
			if (singlelock.IsLocked()) {
				hi_acquire = pImgProc->m_listAcquiredImage.front();
				pImgProc->m_listAcquiredImage.pop_front();
				pImgProc->m_unImageIndex += 1;
			}
			singlelock.Unlock();   //  解锁


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

			//瑕疵检测算法
			pImgProc->StandDeviationAlgorithm(hi_acquire, hv_SVMHandle, vec_dft_info, vec_dft_img);
			//瑕疵信息和图像存入队列
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
			singlelock.Lock();     //  没被调用就上锁自己用，已被调用就等着
			if (singlelock.IsLocked()) {
				hi_acquire = pImgProc->m_listAcquiredImage.front();
				pImgProc->m_listAcquiredImage.pop_front();
				pImgProc->m_unImageIndex += 1;
			}
			singlelock.Unlock();   //  解锁


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

			//瑕疵检测算法
			pImgProc->StandDeviationAlgorithm(hi_acquire, hv_SVMHandle, vec_dft_info, vec_dft_img);
			//瑕疵信息和图像存入队列
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
			singlelock.Lock();     //  没被调用就上锁自己用，已被调用就等着
			if (singlelock.IsLocked()) {
				hi_acquire = pImgProc->m_listAcquiredImage.front();
				pImgProc->m_listAcquiredImage.pop_front();
				pImgProc->m_unImageIndex += 1;
			}
			singlelock.Unlock();   //  解锁



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

			//瑕疵检测算法
			pImgProc->StandDeviationAlgorithm(hi_acquire, hv_SVMHandle, vec_dft_info, vec_dft_img);
			//瑕疵信息和图像存入队列
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

// STD
void CImageProcessing::stdManageThread()
{
	std::string model_filename = ".\\system\\Model.svm";
	m_modelFileName = (CA2W)model_filename.c_str();
	m_nTotalListNumber = 0;
	bool bresult = false;
	while (!bresult) {
		if (ImgListMutex.try_lock()) {
			bresult = GenerateReferenceImage(m_hi_average, m_hi_deviation);
			ImgListMutex.unlock();
		}
		else
			Sleep(200);
	}
	m_unImageIndex += 4;

	if (SAVE_REFERENCE_IMAGE) {
		//std::string filename = "D:\\DetectRecords\\ReferenceImage.png";
		//SaveReferenceImage(filename.c_str());
		CString cstr = L"参考图像已保存";
		::SendMessage(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}

	CString cstr;
	cstr.Format(_T("检测算法初始化完成"));
	::SendMessage(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	::SendMessage(hMainWnd, WM_UPDATE_CONTROLS, NULL, NULL);
}

// index 为线程序号： 0,1,2,3,4
void CImageProcessing::stdImageCalculate(int index)
{
	run_calculate[index] = true;
	for (;;) {
		if (CheckReferenceImageAvilable())
			break;
		else {
			Sleep(200);
			continue;
		}
	}

	HImage hi_average = m_hi_average;
	HImage hi_deviation = m_hi_deviation;
	HImage hi_acquire;
	HTuple hv_SVMHandle;
	InitialClassify((CW2A)m_modelFileName.GetBuffer(), hv_SVMHandle);
	if (hv_SVMHandle.Length() == 0) {
		CString cstr;
		cstr.Format(_T("分类器初始化失败:thread = %d, size = %d"), index + 1);
		::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return;
	}

	while (run_calculate[index]) {
		// lock()阻塞式   try_lock 非阻塞式
		ImgListMutex.lock();
		// 如果图像队列为空则重新循环
		if (m_listAcquiredImage.empty()) {
			ImgListMutex.unlock();
			Sleep(50);
			continue;
		}
		hi_acquire.Clear();
		hi_acquire = m_listAcquiredImage.front();
		m_listAcquiredImage.pop_front();
		m_unImageIndex += 1;
		ImgListMutex.unlock();  // 解锁

		std::vector<DeffectInfo> vec_dft_info;
		std::vector<HalconCpp::HObject> vec_dft_img;
		//瑕疵检测算法
		if (!hi_acquire.IsInitialized()) {
			CString cstr;
			cstr.Format(_T("图像未被初始化:thread = %d"), index + 1);
			::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
			continue;
		}
		StandDeviationAlgorithm(hi_acquire, hv_SVMHandle, vec_dft_info, vec_dft_img);
		if (vec_dft_img.empty())
			continue;

		//瑕疵信息和图像存入队列
		DeffectInfo tempInfo;
		HObject ho_tempImg;
		WriteDFTInfo.lock();
		for (int i = 0; i < (int)vec_dft_info.size(); i++) {
			tempInfo = vec_dft_info.at(i);
			tempInfo.image_index = (unsigned)m_unImageIndex;
			m_listDftInfo.push_back(tempInfo);
			ho_tempImg = vec_dft_img.at(i);
			m_listDftImg.push_back(ho_tempImg);
		}
		WriteDFTInfo.unlock();
	}
		   
	run_calculate[index] = false;
	switch (index + 1)
	{
	case 1: {
		//通过第一个线程判断所有计算线程均已结束
		if (m_threadnum > 1) {
			DWORD dwStop = WaitForSingleObject(CalculateThread_2_StoppedEvent, INFINITE);
			if (dwStop == WAIT_OBJECT_0)
				SetEvent(m_hFinishedProcess);
		}
		else
			SetEvent(m_hFinishedProcess);
		CString cstr = L"图像处理线程已结束";
		::SendMessage(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
		break;
	}
	case 2: {
		if (m_threadnum > 2) {
			DWORD dwStop = WaitForSingleObject(CalculateThread_3_StoppedEvent, INFINITE);
			if (dwStop == WAIT_OBJECT_0)
				CalculateThread_2_StoppedEvent.SetEvent();
		}
		else
			CalculateThread_2_StoppedEvent.SetEvent();
		break;
	}
	case 3: {
		if (m_threadnum > 3) {
			DWORD dwStop = WaitForSingleObject(CalculateThread_4_StoppedEvent, INFINITE);
			if (dwStop == WAIT_OBJECT_0)
				CalculateThread_3_StoppedEvent.SetEvent();
		}
		else
			CalculateThread_3_StoppedEvent.SetEvent();
		break;
	}
	case 4: {
		if (m_threadnum > 4) {
			DWORD dwStop = WaitForSingleObject(CalculateThread_5_StoppedEvent, INFINITE);
			if(dwStop == WAIT_OBJECT_0)
				CalculateThread_4_StoppedEvent.SetEvent();
		}
		else
			CalculateThread_4_StoppedEvent.SetEvent();
		break;
	}
	case 5:
		CalculateThread_5_StoppedEvent.SetEvent();
		break;
	}

	return;
}


void CImageProcessing::stdRunLoad()
{
	run_add = true;
	std::string imgname = "D:\\瑕疵检测数据记录\\test_image\\test_image.png";
	HTuple hv_image_name = (HTuple)((CA2W)imgname.c_str());
	HObject test_img;
	HObject noise_img;
	HalconCpp::ReadImage(&test_img, hv_image_name);

	while (run_add) {
		// 系统暂停
		if (getSystemState() == 4) {
			Sleep(500);
			continue;
		}

		noise_img.Clear();
		AddNoise(test_img, noise_img);
		ImgListMutex.lock();
		m_listAcquiredImage.push_back(noise_img);
		m_nTotalListNumber += 1;
		ImgListMutex.unlock();
		Sleep(433);
	}
	run_add = false;
}
