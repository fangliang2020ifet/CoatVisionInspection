#pragma once
#include "afxwin.h"

#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <direct.h>
#include <fstream>

#include "HalconCpp.h"
#include "HDevThread.h"

using namespace HalconCpp;

namespace HDevExportCpp
{
	// Parallel execution wrapper for write_image(...) 
	static void* _hcppthread_write_image(void *hcthread);
}

// Generated stubs for parallel procedure calls. Wrapped in name
// space to avoid name conflicts with actual procedure names
namespace HDevExportCpp
{
	// Parallel execution wrapper for write_image(...) 
	static void* _hcppthread_write_image(void *hcthread)
	{
		// +++ define thread context for this procedure
		HDevThread*  hcppthread = (HDevThread*)hcthread;
		try
		{
			// Input parameters
			const HObject       &cbho_Image = hcppthread->GetInputIconicParamObject(0);
			const HTuple        &cbhv_Format = hcppthread->GetInputCtrlParamTuple(1);
			const HTuple        &cbhv_FillColor = hcppthread->GetInputCtrlParamTuple(2);
			const HTuple        &cbhv_FileName = hcppthread->GetInputCtrlParamTuple(3);

			// Call write_image
			WriteImage(cbho_Image, cbhv_Format, cbhv_FillColor, cbhv_FileName);

			// Reduce reference counter of thread object
			hcppthread->Exit();
			delete hcppthread;

		}
		catch (HException& exc)
		{
			// No exceptions may be raised from stub in parallel case,
			// so we need to store this information prior to cleanup
			bool is_direct_call = hcppthread->IsDirectCall();
			// Attempt to clean up in error case, too
			hcppthread->Exit();
			delete hcppthread;
			// Propagate exception if called directly
			if (is_direct_call)
				throw exc;
		}
		return NULL;
	}

}

struct DefectType
{
	int rank = 0;                     //瑕疵等级
	int type = 0;                     //瑕疵分类/类型
	float center_x = 0;               //水平位置(毫米)
	float absolute_position = 0.0f;   //纵向位置(米)
	float area = 0.0f;                //面积
	float contlength = 5.0f;          //周长
	float circle_radius = 1.0f;       //外接圆直径
	int pixel_value = 128;            //像素平均灰度值
	bool operator < (const DefectType& def) const {
		return absolute_position < def.absolute_position;
	}
};

struct SelectRegion
{
	//单位均为像素
	int index;
	HTuple hv_Row_Center;
	HTuple hv_Column_Center;
	float area;
	float radius;
	float contlength;
	float pixelvalue;
};

typedef std::list<HImage> ImgList;

typedef std::list<DefectType> DFTList;




class CImageProcess
{
public:
	CImageProcess();
	~CImageProcess();

public:
	HWND hMainWnd;                           //主窗口句柄

	CEvent StopManage_Event;
	CEvent AllCalculateThreadStopped_Event;


	BOOL TEST_MODEL = FALSE;               //使用本地图像运行程序
	BOOL REDUCE_BLACK_EDGE = FALSE;
	BOOL SAVE_REFERENCE_IMAGE = TRUE;

	BOOL m_referenceImage_OK = FALSE;
	BOOL m_bLoad_Default_Ref_Dev = TRUE;
	//算法参数
	int m_k_normal_distribution = 15;       //概率密度(3 = 92%, 5 = 98%)，标准差的倍数
	int m_k_min_select_area = 5;		    //面积删选
	int m_median_filter_size = 1;           //滤波器大小,直接关系检出率,并且size越大计算速度越慢

	int m_threadnum = 1;                    //单相机处理的线程数量

	BOOL InitialImageProcess();
	BOOL BeginProcess();
	BOOL StopProcess();
	void RestartProcess();
	BOOL IsThreadsAlive();
	int CheckTotalListSize();
	BOOL LoadRefImage(std::string folder_path);
	void LoadImageToQueue();
	BOOL LoadOneImageToQueue(std::string folder_path, int next_number);
	BOOL LoadSingleImage(std::string image_name);
	HObject CopyHobject(HObject ho_image);
	void ReSortDefectQueue();

	std::string m_strPath;                  //保存路径
	//int m_NO_produced1 = 0;
	//int m_NO_produced2 = 0;
	//int m_NO_produced3 = 0;
	//int m_NO_produced4 = 0;

	float m_current_position = 0.0f;
	int m_total_list_size = 0;              //  list 的总大小,待处理的图像数量
	ImgList m_ImgList1_1;
	ImgList m_ImgList1_2;
	ImgList m_ImgList1_3;
	ImgList m_ImgList1_4;
	ImgList m_ImgList1_5;
	ImgList m_ImgList2_1;
	ImgList m_ImgList2_2;
	ImgList m_ImgList2_3;
	ImgList m_ImgList2_4;
	ImgList m_ImgList2_5;
	ImgList m_ImgList3_1;
	ImgList m_ImgList3_2;
	ImgList m_ImgList3_3;
	ImgList m_ImgList3_4;
	ImgList m_ImgList3_5;
	ImgList m_ImgList4_1;
	ImgList m_ImgList4_2;
	ImgList m_ImgList4_3;
	ImgList m_ImgList4_4;
	ImgList m_ImgList4_5;
	DFTList m_DFTList1;
	DFTList m_DFTList2;
	DFTList m_DFTList3;
	DFTList m_DFTList4;
	DFTList m_Sorted_DFTList;

	//测试变量
	HImage m_hi_test1;
	HImage m_hi_test2;
	HImage m_hi_test3;
	HImage m_hi_test4;
	HImage m_hi_ref1;       //参考图像
	HImage m_hi_ref2;
	HImage m_hi_ref3;
	HImage m_hi_ref4;
	HImage m_hi_average1;   //平均图像
	HImage m_hi_average2;
	HImage m_hi_average3;
	HImage m_hi_average4;
	HImage m_hi_deviation1;    //标准差图像
	HImage m_hi_deviation2;
	HImage m_hi_deviation3;
	HImage m_hi_deviation4;

private:
	std::string m_default_ref_dev_path = "";
	BOOL m_camera1_reference_image_acquired;
	BOOL m_camera2_reference_image_acquired;
	BOOL m_camera3_reference_image_acquired;
	BOOL m_camera4_reference_image_acquired;
	int m_camera1_invalid_area;
	int m_camera4_invalid_area;

	BOOL LoadDefaultRefAndDevImage(std::string path);
	BOOL CheckReferenceImageState();
	int ProduceReferenceImage1(HImage hi_ref1, HImage hi_ref2);
	int ProduceReferenceImage4(HImage hi_ref4, HImage hi_ref3);
	BOOL GetSavePath(std::string &path);
	BOOL GenerateReferenceImage1(HImage &hi_average, HImage &hi_deviation);
	BOOL GenerateReferenceImage2(HImage &hi_average, HImage &hi_deviation);
	BOOL GenerateReferenceImage3(HImage &hi_average, HImage &hi_deviation);
	BOOL GenerateReferenceImage4(HImage &hi_average, HImage &hi_deviation);
	BOOL SaveReferenceImage();
	DefectType LocateDefectPosition(int camera_number, HObject ho_selectedregion,
		HTuple hv_Number, HTuple hv_colunm_origin, HObject ho_image);
	DefectType LocateDefectPosition(int camera_number, HObject ho_selectedregion);
	int  ImageClassification(HObject ho_img);
	int  RankDivide(DefectType dtype);
	void SaveDefectImage(HObject &ho_img, HTuple name);

	int DetectAlgorithem(int cameraNO, HImage hi_ref, HImage hi_img, std::vector<DefectType> &vDFT);
	int DetectAlgorithemSimple(int cameraNO, HImage hi_ref, HImage hi_img, std::vector<DefectType> &vDFT);
	int StandDeviationAlgorithm(int cameraNO, HImage hi_average, HImage hi_deviation,
		HImage hi_img, UINT64 produced_num, std::vector<DefectType> &vDFT);

protected:

	enum{ CAMERA_1 = 1, CAMERA_2, CAMERA_3, CAMERA_4};
	//异物， 凹凸， 擦伤， 晶点， 漏涂
	enum { DFT_MATTER = 0, DFT_BUMP, DFT_GRAZE, DFT_CRYSTAL, DFT_COATING};

	BOOL IsFileExist(const std::string &filename);
	BOOL IsPathExist(const std::string &pathname);
private:
	void StopCalculateThreads();

	HANDLE                m_hStopEvent;

	CWinThread *m_ManageThread;
	CWinThread *m_CalculateThread1_1;
	CWinThread *m_CalculateThread1_2;
	CWinThread *m_CalculateThread1_3;
	CWinThread *m_CalculateThread1_4;
	CWinThread *m_CalculateThread1_5;
	CWinThread *m_CalculateThread2_1;
	CWinThread *m_CalculateThread2_2;
	CWinThread *m_CalculateThread2_3;
	CWinThread *m_CalculateThread2_4;
	CWinThread *m_CalculateThread2_5;
	CWinThread *m_CalculateThread3_1;
	CWinThread *m_CalculateThread3_2;
	CWinThread *m_CalculateThread3_3;
	CWinThread *m_CalculateThread3_4;
	CWinThread *m_CalculateThread3_5;
	CWinThread *m_CalculateThread4_1;
	CWinThread *m_CalculateThread4_2;
	CWinThread *m_CalculateThread4_3;
	CWinThread *m_CalculateThread4_4;
	CWinThread *m_CalculateThread4_5;
	CRITICAL_SECTION m_csCalculateThread1;
	CRITICAL_SECTION m_csCalculateThread2;
	CRITICAL_SECTION m_csCalculateThread3;
	CRITICAL_SECTION m_csCalculateThread4;
	CRITICAL_SECTION m_csNO_dft;
	BOOL is_manage_thread_alive;
	BOOL is_thread1_1_alive;
	BOOL is_thread1_2_alive;
	BOOL is_thread1_3_alive;
	BOOL is_thread1_4_alive;
	BOOL is_thread1_5_alive;
	BOOL is_thread2_1_alive;
	BOOL is_thread2_2_alive;
	BOOL is_thread2_3_alive;
	BOOL is_thread2_4_alive;
	BOOL is_thread2_5_alive;
	BOOL is_thread3_1_alive;
	BOOL is_thread3_2_alive;
	BOOL is_thread3_3_alive;
	BOOL is_thread3_4_alive;
	BOOL is_thread3_5_alive;
	BOOL is_thread4_1_alive;
	BOOL is_thread4_2_alive;
	BOOL is_thread4_3_alive;
	BOOL is_thread4_4_alive;
	BOOL is_thread4_5_alive;
	static UINT ManageThread(LPVOID pParam);
	static UINT ManageThread2(LPVOID pParam);

	static UINT ImageCalculate1_1(LPVOID pParam);
	static UINT ImageCalculate1_2(LPVOID pParam);
	static UINT ImageCalculate1_3(LPVOID pParam);
	static UINT ImageCalculate1_4(LPVOID pParam);
	static UINT ImageCalculate1_5(LPVOID pParam);
	static UINT ImageCalculate2_1(LPVOID pParam);
	static UINT ImageCalculate2_2(LPVOID pParam);
	static UINT ImageCalculate2_3(LPVOID pParam);
	static UINT ImageCalculate2_4(LPVOID pParam);
	static UINT ImageCalculate2_5(LPVOID pParam);
	static UINT ImageCalculate3_1(LPVOID pParam);
	static UINT ImageCalculate3_2(LPVOID pParam);
	static UINT ImageCalculate3_3(LPVOID pParam);
	static UINT ImageCalculate3_4(LPVOID pParam);
	static UINT ImageCalculate3_5(LPVOID pParam);
	static UINT ImageCalculate4_1(LPVOID pParam);
	static UINT ImageCalculate4_2(LPVOID pParam);
	static UINT ImageCalculate4_3(LPVOID pParam);
	static UINT ImageCalculate4_4(LPVOID pParam);
	static UINT ImageCalculate4_5(LPVOID pParam);


};