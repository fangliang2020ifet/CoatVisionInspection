
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
	
	//创建线程
	if (!(m_ReferenceImage = AfxBeginThread(ReferenceImage, this))) {
		Win::log("创建图像参考图像处理线程失败");
		return FALSE;
	}

	if (!(m_CalculateThread1_1 = AfxBeginThread(ImageCalculate1_1, this))) {
		Win::log("创建图像处理线程1_1失败");
		return FALSE;
	}
	
	if (!(m_CalculateThread1_2 = AfxBeginThread(ImageCalculate1_2, this))) {
		Win::log("创建图像处理线程1_2失败");
		return FALSE;
	}
	if (!(m_CalculateThread1_3 = AfxBeginThread(ImageCalculate1_3, this))) {
		Win::log("创建图像处理线程1_3失败");
		return FALSE;
	}
	if (!(m_CalculateThread1_4 = AfxBeginThread(ImageCalculate1_4, this))) {
		Win::log("创建图像处理线程1_4失败");
		return FALSE;
	}
	if (!(m_CalculateThread1_5 = AfxBeginThread(ImageCalculate1_5, this))) {
		Win::log("创建图像处理线程1_5失败");
		return FALSE;
	}

	if (!(m_CalculateThread2_1 = AfxBeginThread(ImageCalculate2_1, this))) {
		Win::log("创建图像处理线程1_1失败");
		return FALSE;
	}
	if (!(m_CalculateThread2_2 = AfxBeginThread(ImageCalculate2_2, this))) {
		Win::log("创建图像处理线程1_2失败");
		return FALSE;
	}
	if (!(m_CalculateThread2_3 = AfxBeginThread(ImageCalculate2_3, this))) {
		Win::log("创建图像处理线程2_3失败");
		return FALSE;
	}
	if (!(m_CalculateThread2_4 = AfxBeginThread(ImageCalculate2_4, this))) {
		Win::log("创建图像处理线程2_4失败");
		return FALSE;
	}
	if (!(m_CalculateThread2_5 = AfxBeginThread(ImageCalculate2_5, this))) {
		Win::log("创建图像处理线程2_5失败");
		return FALSE;
	}

	if (!(m_CalculateThread3_1 = AfxBeginThread(ImageCalculate3_1, this))) {
		Win::log("创建图像处理线程3_1失败");
		return FALSE;
	}
	if (!(m_CalculateThread3_2 = AfxBeginThread(ImageCalculate3_2, this))) {
		Win::log("创建图像处理线程3_2失败");
		return FALSE;
	}
	if (!(m_CalculateThread3_3 = AfxBeginThread(ImageCalculate3_3, this))) {
		Win::log("创建图像处理线程3_3失败");
		return FALSE;
	}
	if (!(m_CalculateThread3_4 = AfxBeginThread(ImageCalculate3_4, this))) {
		Win::log("创建图像处理线程3_4失败");
		return FALSE;
	}
	if (!(m_CalculateThread3_5 = AfxBeginThread(ImageCalculate3_5, this))) {
		Win::log("创建图像处理线程3_5失败");
		return FALSE;
	}

	if (!(m_CalculateThread4_1 = AfxBeginThread(ImageCalculate4_1, this))) {
		Win::log("创建图像处理线程4_1失败");
		return FALSE;
	}
	if (!(m_CalculateThread4_2 = AfxBeginThread(ImageCalculate4_2, this))) {
		Win::log("创建图像处理线程4_2失败");
		return FALSE;
	}
	if (!(m_CalculateThread4_3 = AfxBeginThread(ImageCalculate4_3, this))) {
		Win::log("创建图像处理线程4_3失败");
		return FALSE;
	}
	if (!(m_CalculateThread4_4 = AfxBeginThread(ImageCalculate4_4, this))) {
		Win::log("创建图像处理线程4_4失败");
		return FALSE;
	}
	if (!(m_CalculateThread4_5 = AfxBeginThread(ImageCalculate4_5, this))) {
		Win::log("创建图像处理线程4_5失败");
		return FALSE;
	}
	

	Win::log("创建图像处理线程");


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
		Win::log("工作目录不存在");
		return FALSE;
	}
	//设置工作路径
	file_path = folder_path;

	//读取参考图像1
	std::string ref_image_name1 = "reference_image1.bmp";
	HTuple hv_ref_image_name1 = (HTuple)((folder_path + ref_image_name1).c_str());
	if (!IsFileExist(folder_path + ref_image_name1)) {
		Win::log("参考图像1不存在");
		return FALSE;
	}
	HImage img1;
	ReadImage(&img1, hv_ref_image_name1);
	//HalconCpp::MedianImage(img1, &m_hi_ref1, "circle", 1, "mirrored");
	//高斯滤波
	HalconCpp::GaussFilter(img1, &m_hi_ref1, 5);
	m_camera1_reference_image_acquired = TRUE;

	//读取参考图像2
	std::string ref_image_name2 = "reference_image2.bmp";
	HTuple hv_ref_image_name2 = (HTuple)((folder_path + ref_image_name2).c_str());
	if (!IsFileExist(folder_path + ref_image_name2)) {
		Win::log(L"参考图像2不存在");
		return FALSE;
	}
	HImage img2;
	ReadImage(&img2, hv_ref_image_name2);
	//HalconCpp::MedianImage(img2, &m_hi_ref2, "circle", 1, "mirrored");
		//高斯滤波
	HalconCpp::GaussFilter(img2, &m_hi_ref2, 5);
	m_camera2_reference_image_acquired = TRUE;


	//读取参考图像3
	std::string ref_image_name3 = "reference_image3.bmp";
	HTuple hv_ref_image_name3 = (HTuple)((folder_path + ref_image_name3).c_str());
	if (!IsFileExist(folder_path + ref_image_name3)) {
		Win::log(L"参考图像3不存在");
		return FALSE;
	}
	HImage img3;
	ReadImage(&img3, hv_ref_image_name3);
	//HalconCpp::MedianImage(img3, &m_hi_ref3, "circle", 1, "mirrored");
		//高斯滤波
	HalconCpp::GaussFilter(img3, &m_hi_ref3, 5);
	m_camera3_reference_image_acquired = TRUE;

	//读取参考图像4
	std::string ref_image_name4 = "reference_image4.bmp";
	HTuple hv_ref_image_name4 = (HTuple)((folder_path + ref_image_name4).c_str());
	if (!IsFileExist(folder_path + ref_image_name4)) {
		Win::log(L"参考图像4不存在");
		return FALSE;
	}
	HImage img4;
	ReadImage(&img4, hv_ref_image_name4);
	//HalconCpp::MedianImage(img4, &m_hi_ref4, "circle", 1, "mirrored");
		//高斯滤波
	HalconCpp::GaussFilter(img4, &m_hi_ref4, 5);
	m_camera4_reference_image_acquired = TRUE;


	return TRUE;
}

//把图像加载到内存队列
BOOL CImageProcess::LoadImageToQueue(std::string folder_path, int numbers)
{
	if (!IsPathExist(folder_path)) {
		AfxMessageBox(L"工作目录不存在");
		return FALSE;
	}
	//设置工作路径
	file_path = folder_path;

	//读取参考图像
	std::string ref_image_name = "reference_image.bmp";
	HTuple hv_ref_image_name = (HTuple)((folder_path + ref_image_name).c_str());
	if (!IsFileExist(folder_path + ref_image_name)) {
		AfxMessageBox(L"参考图像不存在");
		return FALSE;
	}
	ReadImage(&m_hi_ref1, hv_ref_image_name);

	//读取待检图像
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
			AfxMessageBox(L"待检图像为空\n请检查图像名称是否从001开始");
			return FALSE;
		}
		ReadImage(&ho_image, hv_image_name);

		m_ImgList1_1.push_back(ho_image);
	}

	return TRUE;
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

	Win::log("读取test图像完成");

	return TRUE;
}

HObject CImageProcess::CopyHobject(HObject ho_image)
{
	HObject copy;
	CopyImage(ho_image, &copy);

	return copy;
}

BOOL CImageProcess::GenerateRefImg(int cameraNo, std::string save_path, HObject &ho_ref)
{
	//获取 3 张图像，相加平均后得到参考图像
	HImage hi_img;
	HImage hi_added_img;
	ImgList *plist = NULL;
	switch (cameraNo)
	{
	case 1:
		plist = &m_ImgList1_1;
		break;
	case 2:
		plist = &m_ImgList2_1;
		break;
	case 3:
		plist = &m_ImgList3_1;
		break;
	case 4:
		plist = &m_ImgList4_1;
		break;
	default:
		break;
	}

	if (plist->size() < 3) {
		Win::log("获取参考图像失败，请重试");
		return FALSE;
	}
	else {
		hi_added_img = plist->front();
		plist->pop_front();
		for (int i = 0; i < 2; i++)
		{
			hi_img = plist->front();
			plist->pop_front();
			AddImage(hi_added_img, hi_img, &hi_added_img, 0.5, 0);
		}
	}

	//中值滤波
	MedianImage(hi_added_img, &ho_ref, "circle", 1, "mirrored");

	HTuple name = (HTuple)save_path.c_str() + "reference_image_" + (HTuple)cameraNo + ".tiff";
	if(m_save_reference_image)
		WriteImage(ho_ref, "tiff", 0, name);

	// +++ Threading variables 
	HDevThread*         hcppthread_handle;
	HDevThreadContext   hcppthread_context; // <-signals begin of procedure

	// Create a thread instance
	hcppthread_handle = new HDevThread(hcppthread_context, (void*)HDevExportCpp::_hcppthread_write_image, 4, 0);
	// Set thread procedure call arguments 
	hcppthread_handle->SetInputIconicParamObject(0, ho_ref);
	hcppthread_handle->SetInputCtrlParamTuple(1, "tiff");
	hcppthread_handle->SetInputCtrlParamTuple(2, 0);
	hcppthread_handle->SetInputCtrlParamTuple(3, name);

	// Start proc line in thread
	HTuple hv_ThreadID;
	hcppthread_handle->ParStart(&hv_ThreadID);

	return TRUE;
}

BOOL CImageProcess::GenerateReferenceImage1(HImage &hi_ref)
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
	////中值滤波
	//HalconCpp::MedianImage(result, &hi_ref, "circle", 1, "mirrored");
	//高斯滤波
	HalconCpp::GaussFilter(result, &hi_ref, 5);
	m_camera1_reference_image_acquired = TRUE;

	return TRUE;
}

BOOL CImageProcess::GenerateReferenceImage2(HImage &hi_ref)
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
		//高斯滤波
	HalconCpp::GaussFilter(result, &hi_ref, 5);
	m_camera2_reference_image_acquired = TRUE;

	return TRUE;
}

BOOL CImageProcess::GenerateReferenceImage3(HImage &hi_ref)
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
		//高斯滤波
	HalconCpp::GaussFilter(result, &hi_ref, 5);
	m_camera3_reference_image_acquired = TRUE;

	return TRUE;
}

BOOL CImageProcess::GenerateReferenceImage4(HImage &hi_ref)
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
		//高斯滤波
	HalconCpp::GaussFilter(result, &hi_ref, 5);
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
	HObject  ho_Image, ho_Rectangle, ho_Region, ho_ConnectedRegions;
	HObject  ho_ImageReduced;
	HTuple  hv_Width1, hv_Height1, hv_Mean, hv_Deviation;
	HTuple  hv_Row1, hv_Column1, hv_Row2, hv_Column2;

	ho_Image = hi_ref1;
	GetImageSize(ho_Image, &hv_Width1, &hv_Height1);
	if (hv_Width1 == 0)
		return -1;

	GenRectangle1(&ho_Rectangle, 0, hv_Width1 - 256, 256, hv_Width1);
	Intensity(ho_Rectangle, ho_Image, &hv_Mean, &hv_Deviation);
	Threshold(ho_Image, &ho_Region, hv_Mean*0.8, 255);
	Connection(ho_Region, &ho_ConnectedRegions);
	SmallestRectangle1(ho_ConnectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	ReduceDomain(ho_Image, ho_ConnectedRegions, &ho_ImageReduced);
	GetImageSize(ho_ImageReduced, &hv_width_ref1, &hv_height_ref1);

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
	HObject  ho_Image, ho_Rectangle, ho_Region, ho_ConnectedRegions;
	HObject  ho_ImageReduced;
	HTuple  hv_Width1, hv_Height1, hv_Mean, hv_Deviation;
	HTuple  hv_Row1, hv_Column1, hv_Row2, hv_Column2;

	ho_Image = hi_ref4;
	GetImageSize(ho_Image, &hv_Width1, &hv_Height1);
	if (hv_Width1 == 0)
		return -1;

	GenRectangle1(&ho_Rectangle, 0, hv_Width1 - 256, 256, hv_Width1);
	Intensity(ho_Rectangle, ho_Image, &hv_Mean, &hv_Deviation);
	Threshold(ho_Image, &ho_Region, hv_Mean*0.8, 255);
	Connection(ho_Region, &ho_ConnectedRegions);
	SmallestRectangle1(ho_ConnectedRegions, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
	ReduceDomain(ho_Image, ho_ConnectedRegions, &ho_ImageReduced);
	GetImageSize(ho_ImageReduced, &hv_width_ref1, &hv_height_ref1);

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

BOOL CImageProcess::GenerateAndSaveRefImage()
{
	if (!GenerateReferenceImage1(m_hi_ref1))
		return FALSE;
	else
		HalconCpp::WriteImage(m_hi_ref1, "bmp", 0, "D:/SaveImage/ref1.bmp");

	if (!GenerateReferenceImage2(m_hi_ref2))
		return FALSE;
	else
		HalconCpp::WriteImage(m_hi_ref2, "bmp", 0, "D:/SaveImage/ref2.bmp");

	if (!GenerateReferenceImage3(m_hi_ref3))
		return FALSE;
	else
		HalconCpp::WriteImage(m_hi_ref3, "bmp", 0, "D:/SaveImage/ref3.bmp");

	if (!GenerateReferenceImage4(m_hi_ref4))
		return FALSE;
	else
		HalconCpp::WriteImage(m_hi_ref4, "bmp", 0, "D:/SaveImage/ref4.bmp");

	return TRUE;
}

BOOL CImageProcess::GetSavePath(std::string &path)
{
	path = m_strPath;

	//检查保存路径是否存在
	if (!IsPathExist(path)) {
		Win::log("图片保存路径不存在");
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

//处理最多的图像数量
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
			Connection(ho_Region, &ho_ConnectedRegions);
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
	GaussFilter(ho_Image_def, &ho_ImageGauss1, 5);

	//待检图 - 参考图
	AbsDiffImage(ho_ImageGauss1, ho_Image_ref, &ho_ImageAbsDiff1, 1);

	//自动阈值输入必须是是单通道图像，会有多阈值分割，Sigma用于对灰度直方图进行高斯平滑，决定了平滑的程度（分割细致程度），
	//当sigma很大时，灰度直方图基本会被平滑为只剩下一个波峰，而分割是根据平滑后直方图的波谷来进行的，Sigma小，分割的越细致
	//AutoThreshold(ho_ImageAbsDiff1, &ho_Regions, 3);
	GrayRangeRect(ho_ImageAbsDiff1, &ho_ImageResult, 10, 10);
	MinMaxGray(ho_ImageResult, ho_ImageResult, 0, &hv_Min, &hv_Max, &hv_Range);
	Threshold(ho_ImageResult, &ho_Regions, (HTuple(5.55).TupleConcat(hv_Max*0.8)).TupleMax(), 255);
	Connection(ho_Regions, &ho_ConnectedRegions);
	//区域选择参数：1.轮廓周长(pixel)  2.最大内接圆半径(pixel)
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, (HTuple("contlength").Append("inner_radius")),
		"and", (HTuple(11).Append(1)), (HTuple(9999999).Append(9999999)));
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
				if (0 != (hv_Radius > 4000))
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

//参考图像处理线程
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
				pThis->GenerateReferenceImage1(pThis->m_hi_ref1);
				pThis->SaveDefectImage(pThis->m_hi_ref1, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image1.bmp");
				Win::log("获取1#参考图像");
			}
			if (!pThis->m_camera2_reference_image_acquired) {
				pThis->GenerateReferenceImage2(pThis->m_hi_ref2);
				pThis->SaveDefectImage(pThis->m_hi_ref2, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image2.bmp");
				Win::log("获取2#参考图像");
			}
			if (!pThis->m_camera3_reference_image_acquired) {
				pThis->GenerateReferenceImage3(pThis->m_hi_ref3);
				pThis->SaveDefectImage(pThis->m_hi_ref3, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image3.bmp");
				Win::log("获取3#参考图像");
			}
			if (!pThis->m_camera4_reference_image_acquired) {
				pThis->GenerateReferenceImage4(pThis->m_hi_ref4);
				pThis->SaveDefectImage(pThis->m_hi_ref4, (HTuple)pThis->m_strPath.c_str() + "ref\\reference_image4.bmp");
				Win::log("获取4#参考图像");
			}
		}
		else {
			pThis->LoadRefImage("C:/DeVisionProject/sample0403/");
			pThis->LoadSingleImage("C:/DeVisionProject/sample0403/test1");
		}

		if (pThis->CheckReferenceImageState()) {
			pThis->m_camera1_invalid_area = pThis->ProduceReferenceImage1(pThis->m_hi_ref1, pThis->m_hi_ref2);
			if (pThis->m_camera1_invalid_area != 0 && pThis->m_camera1_invalid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_ref1, hv_height_ref1;

				HalconCpp::GetImageSize(pThis->m_hi_ref1, &hv_width_ref1, &hv_height_ref1);
				HalconCpp::GenRectangle1(&ho_Region, hv_width_ref1 - pThis->m_camera1_invalid_area, 0,
					hv_width_ref1, hv_height_ref1);
				HalconCpp::ReduceDomain(pThis->m_hi_ref1, ho_Region, &ho_ImageReduced);
				pThis->m_hi_ref1 = ho_ImageReduced;
			}

			pThis->m_camera4_invalid_area = pThis->ProduceReferenceImage4(pThis->m_hi_ref4, pThis->m_hi_ref3);
			if (pThis->m_camera1_invalid_area != 0 && pThis->m_camera1_invalid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_ref4, hv_height_ref4;

				HalconCpp::GetImageSize(pThis->m_hi_ref4, &hv_width_ref4, &hv_height_ref4);
				HalconCpp::GenRectangle1(&ho_Region, 0, 0,
					hv_width_ref4 - pThis->m_camera4_invalid_area, hv_height_ref4);
				HalconCpp::ReduceDomain(pThis->m_hi_ref4, ho_Region, &ho_ImageReduced);
				pThis->m_hi_ref4 = ho_ImageReduced;
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

	while (pImgProc->is_thread1_1_alive)
	{
		if(!pImgProc->m_ImgList1_1.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList1_1.front();
			pImgProc->m_ImgList1_1.pop_front();
			if (valid_area != 0 && valid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_def, hv_height_def;

				HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
				HalconCpp::GenRectangle1(&ho_Region, hv_width_def - valid_area, 0,
					hv_width_def, hv_height_def);
				HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
				hi_def = ho_ImageReduced;
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced1 += 1;
			//更新位置信息: 总帧数 * 图像高度 * 纵向精度
			pImgProc->m_current_position = (pImgProc->m_NO_produced1) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		}else{
			Sleep(5);
		}
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

	while (pImgProc->is_thread1_2_alive)
	{
		if (!pImgProc->m_ImgList1_2.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList1_2.front();
			pImgProc->m_ImgList1_2.pop_front();
			if (valid_area != 0 && valid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_def, hv_height_def;

				HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
				HalconCpp::GenRectangle1(&ho_Region, hv_width_def - valid_area, 0,
					hv_width_def, hv_height_def);
				HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
				hi_def = ho_ImageReduced;
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced1 += 1;
			//更新位置信息: 总帧数 * 图像高度 * 纵向精度
			pImgProc->m_current_position = (pImgProc->m_NO_produced1) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		} else {
			Sleep(5);
		}
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

	while (pImgProc->is_thread1_3_alive)
	{
		if (!pImgProc->m_ImgList1_3.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList1_3.front();
			pImgProc->m_ImgList1_3.pop_front();
			if (valid_area != 0 && valid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_def, hv_height_def;

				HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
				HalconCpp::GenRectangle1(&ho_Region, hv_width_def - valid_area, 0,
					hv_width_def, hv_height_def);
				HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
				hi_def = ho_ImageReduced;
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced1 += 1;
			//更新位置信息: 总帧数 * 图像高度 * 纵向精度
			pImgProc->m_current_position = (pImgProc->m_NO_produced1) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);

		} else {
			Sleep(5);
		}
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

	while (pImgProc->is_thread1_4_alive)
	{
		if (!pImgProc->m_ImgList1_4.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList1_4.front();
			pImgProc->m_ImgList1_4.pop_front();
			if (valid_area != 0 && valid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_def, hv_height_def;

				HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
				HalconCpp::GenRectangle1(&ho_Region, hv_width_def - valid_area, 0,
					hv_width_def, hv_height_def);
				HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
				hi_def = ho_ImageReduced;
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced1 += 1;
			//更新位置信息: 总帧数 * 图像高度 * 纵向精度
			pImgProc->m_current_position = (pImgProc->m_NO_produced1) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		} else {
			Sleep(5);
		}
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

	std::vector<DefectType> vdef;
	while (pImgProc->is_thread1_5_alive)
	{
		if (!pImgProc->m_ImgList1_5.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList1_5.front();
			pImgProc->m_ImgList1_5.pop_front();
			if (valid_area != 0 && valid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_def, hv_height_def;

				HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
				HalconCpp::GenRectangle1(&ho_Region, hv_width_def - valid_area, 0,
					hv_width_def, hv_height_def);
				HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
				hi_def = ho_ImageReduced;
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_1, hi_ref, hi_def, vdef);
			pImgProc->DetectAlgorithemSimple(CAMERA_1, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced1 += 1;
			//更新位置信息: 总帧数 * 图像高度 * 纵向精度
			pImgProc->m_current_position = (pImgProc->m_NO_produced1) * IMAGE_HEIGHT * VERTICAL_PRECISION / 1000.0f;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread1);
		} else {
			Sleep(5);
		}
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
			pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced2 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		} else {
			Sleep(5);
		}
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
			pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced2 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}else{
			Sleep(5);
		}
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
			pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced2 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}else{
			Sleep(5);
		}
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
			pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced2 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}else{
			Sleep(5);
		}
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
			pImgProc->DetectAlgorithemSimple(CAMERA_2, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced2 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread2);
		}else{
			Sleep(5);
		}
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
			pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced3 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}else{
			Sleep(5);
		}
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
			pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced3 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}else{
			Sleep(5);
		}
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
	pImgProc->m_NO_produced3 = 0;

	//处理参考图像
	HImage hi_ref;
	while (!pImgProc->m_referenceImage_OK)
	{
		Sleep(50);
	}
	hi_ref = pImgProc->m_hi_ref3;

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
			pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced3 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}else{
			Sleep(5);
		}
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
			pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced3 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}else{
			Sleep(5);
		}
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
			pImgProc->DetectAlgorithemSimple(CAMERA_3, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced3 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread3);
		}else{
			Sleep(5);
		}
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

	while (pImgProc->is_thread4_1_alive)
	{
		if (!pImgProc->m_ImgList4_1.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList4_1.front();
			pImgProc->m_ImgList4_1.pop_front();
			if (valid_area != 0 && valid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_def, hv_height_def;

				HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
				HalconCpp::GenRectangle1(&ho_Region, 0, 0,
					hv_width_def - valid_area, hv_height_def);
				HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
				hi_def = ho_ImageReduced;
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced4 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}else{
			Sleep(5);
		}
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

	while (pImgProc->is_thread4_2_alive)
	{
		if (!pImgProc->m_ImgList4_2.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList4_2.front();
			pImgProc->m_ImgList4_2.pop_front();
			if (valid_area != 0 && valid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_def, hv_height_def;

				HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
				HalconCpp::GenRectangle1(&ho_Region, 0, 0,
					hv_width_def - valid_area, hv_height_def);
				HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
				hi_def = ho_ImageReduced;
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced4 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}else{
			Sleep(5);
		}
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

	while (pImgProc->is_thread4_3_alive)
	{
		if (!pImgProc->m_ImgList4_3.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList4_3.front();
			pImgProc->m_ImgList4_3.pop_front();
			if (valid_area != 0 && valid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_def, hv_height_def;

				HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
				HalconCpp::GenRectangle1(&ho_Region, 0, 0,
					hv_width_def - valid_area, hv_height_def);
				HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
				hi_def = ho_ImageReduced;
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);

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

			//已处理的图像总数
			pImgProc->m_NO_produced4 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}else{
			Sleep(5);
		}
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

	while (pImgProc->is_thread4_4_alive)
	{
		if (!pImgProc->m_ImgList4_4.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList4_4.front();
			pImgProc->m_ImgList4_4.pop_front();
			if (valid_area != 0 && valid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_def, hv_height_def;

				HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
				HalconCpp::GenRectangle1(&ho_Region, 0, 0,
					hv_width_def - valid_area, hv_height_def);
				HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
				hi_def = ho_ImageReduced;
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced4 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}else{
			Sleep(5);
		}
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

	while (pImgProc->is_thread4_5_alive)
	{
		if (!pImgProc->m_ImgList4_5.empty())
		{
			std::vector<DefectType> vdef;

			//从队列中取出待检图像
			HImage hi_def = pImgProc->m_ImgList4_5.front();
			pImgProc->m_ImgList4_5.pop_front();
			if (valid_area != 0 && valid_area != -1) {
				HObject ho_Image, ho_Region, ho_ImageReduced;
				HTuple hv_width_def, hv_height_def;

				HalconCpp::GetImageSize(hi_def, &hv_width_def, &hv_height_def);
				HalconCpp::GenRectangle1(&ho_Region, 0, 0,
					hv_width_def - valid_area, hv_height_def);
				HalconCpp::ReduceDomain(hi_def, ho_Region, &ho_ImageReduced);
				hi_def = ho_ImageReduced;
			}

			//瑕疵检测算法
			//pImgProc->DetectAlgorithem(CAMERA_4, hi_ref, hi_def, vdef);
			pImgProc->DetectAlgorithemSimple(CAMERA_4, hi_ref, hi_def, vdef);

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
			//已处理的图像总数
			pImgProc->m_NO_produced4 += 1;
			LeaveCriticalSection(&pImgProc->m_csCalculateThread4);
		}else{
			Sleep(5);
		}
	}
	pImgProc->m_ImgList4_5.clear();

	pImgProc->is_thread4_5_alive = FALSE;
	Win::log("4#相机处理线程 5 结束");

	return 0;
}
