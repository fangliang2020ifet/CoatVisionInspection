#pragma once
#include "afxwin.h"
#include <string>
#include <vector>
#include <list>
//#include <map>
#include <algorithm>
#include <direct.h>
#include <fstream>
//#include "LinkQueue.h"

#include "HalconCpp.h"
#include "HDevThread.h"



#define IMAGE_WIDTH 8192
#define IMAGE_HEIGHT 8192
// 纵向的精度, 单位： mm
#define VERTICAL_PRECISION 0.035f
// 横向的精度，单位： mm
#define HORIZON_PRECISION 0.05f


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
	float center_x = 0;               //水平位置(mm)
	float absolute_position = 0.0f;   //纵向位置(m)
	float area = 0.1f;                //面积
	float contlength = 5.0f;          //周长
	float circle_radius = 1.0f;       //外接圆直径
	int pixel_value = 128;            //像素平均灰度值

	bool operator < (const DefectType& def) const {
		return absolute_position < def.absolute_position;
	}

};


typedef std::list<HObject> ImgList;
typedef std::list<DefectType> DFTList;


class CImageProcess
{
public:
	CImageProcess();
	~CImageProcess();

public:
	BOOL BeginProcess();
	BOOL StopProcess();
	void RestartProcess();
	BOOL IsThreadsAlive();

	BOOL LoadRefImage(std::string folder_path);
	BOOL LoadImageToQueue(std::string folder_path, int numbers);
	BOOL LoadOneImageToQueue(std::string folder_path, int next_number);
	BOOL LoadSingleImage(std::string image_name);
	HObject CopyHobject(HObject ho_image);
	BOOL GetSavePath(std::string &path);
	//BOOL GenerateRefImg(int cameraNo, HObject &ho_ref);
	BOOL GenerateRefImg(int cameraNo, HObject &ho_ref, int list);
	DefectType LocateDefectPosition(int camera_number, std::string save_path, HObject ho_selectedregion,
									HTuple hv_Number, HTuple hv_colunm_origin, HObject ho_image);
	void SaveDefectImage(HObject &ho_img, HTuple name);
	void ReSortDefectQueue();

	std::string m_strPath;                  //保存路径
	int m_NO_IMG;                          //已处理的图像总数
	int m_NO_produced1;
	int m_NO_produced2;
	int m_NO_produced3;
	int m_NO_produced4;
	int FindMaxProducedNO();                       //找到已处理最多图像的线程

	float m_current_position = 0.0f;
	//瑕疵序号
	int NO_dft = 0;

	std::vector<std::string> m_vFileName;           //记录瑕疵图像名称 的 vector

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
	std::string file_path;    //文件路径,参考图像
	HObject ho_test1;
	HObject ho_test2;
	HObject ho_test3;
	HObject ho_test4;

protected:
	//异物， 凹凸， 擦伤， 晶点， 漏涂
	enum { DFT_MATTER = 0, DFT_BUMP, DFT_GRAZE, DFT_CRYSTAL, DFT_COATING};

	BOOL IsFileExist(const std::string &filename);
	BOOL IsPathExist(const std::string &pathname);
private:

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
	CRITICAL_SECTION m_csCalculateThread5;

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

	HObject ho_Image_ref1;       //参考图像
	HObject ho_Image_ref2;
	HObject ho_Image_ref3;
	HObject ho_Image_ref4;


};