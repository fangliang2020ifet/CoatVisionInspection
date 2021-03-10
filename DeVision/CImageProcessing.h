#pragma once

#include "afxwin.h"
#include <mutex>
#include <assert.h>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <direct.h>
#include <fstream>
#include "ImportHalconCpp.h"


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

typedef struct DeffectInfo 
{
	unsigned short int rank = 0;          //瑕疵等级
	unsigned short int type = 0;          //瑕疵分类/类型
	float              x = 0;             //水平位置(单位：像素)
	float              y = 0;             //纵向位置
	float              area = 0.0f;       //面积
	unsigned short int contlength = 0;    //周长
	unsigned short int radius = 0;        //外接圆直径
	unsigned int       image_index = 0;   //瑕疵所在图像序号
	unsigned short int pixel_value = 0;   //像素平均灰度值

}DeffectInfo;

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

//struct DefectImage
//{
//	HObject ho_img;
//	HTuple  hv_name;
//};

//typedef std::list<HImage> ImgList;
//typedef std::list<DefectType> DFTList;


class CImageProcessing
{
public:
	CImageProcessing(int ThreadNum, int Distribution, int FilterSize, float RadiusMin, float RadiusMax);
	~CImageProcessing();
	void ClearThisClass();

public:
	HWND hMainWnd;                               //主窗口句柄
	HANDLE m_hFinishedProcess;                     //处理完成
	CEvent StopManage_Event;
	//CEvent CalculateThread_1_StoppedEvent;
	CEvent CalculateThread_2_StoppedEvent;
	CEvent CalculateThread_3_StoppedEvent;
	CEvent CalculateThread_4_StoppedEvent;
	CEvent CalculateThread_5_StoppedEvent;


	BOOL SYSTEM_PAUSE;
	BOOL TEST_MODEL;                            //测试模式
	BOOL SAVE_REFERENCE_IMAGE;                  //是否保存生成的参考图像
	BOOL m_referenceImage_OK;                   //参考图像是否已生成
	int m_threadnum;                            //线程数量
	int m_k_normal_distribution;                //概率密度(3 = 92%, 5 = 98%)，标准差的倍数
	int m_median_filter_size;                   //滤波器大小,直接关系检出率,并且size越大计算速度越慢
	float m_fMin_Radius;	                    //半径筛选
	float m_fMax_Radius;
	std::string m_strPath;                      //保存路径
	int m_nTotalListNumber;                  //待处理的图像数量

	std::list<HObject>                m_listAcquiredImage;
	std::list<DeffectInfo>            m_listDftInfo;
	std::list<HalconCpp::HObject>     m_listDftImg;
	UINT64 m_unImageIndex;

	void InitialImageProcess();
	BOOL BeginProcess();
	void StopManageThread();
	void StopCalculateThreads();
	void RestartProcess();
	BOOL IsThreadsAlive();
	int CheckTotalListSize();
	BOOL LoadRefImage(std::string folder_path);
	void LoadImageToQueue();
	BOOL LoadOneImageToQueue(std::string folder_path, int next_number);
	BOOL LoadSingleImage(std::string image_name);
	HObject CopyHobject(HObject ho_image);
	void SaveDefectImage(HObject &ho_img, HTuple name);
	BOOL CheckReferenceImageAvilable();

protected:
	enum { CAMERA_1 = 1, CAMERA_2, CAMERA_3, CAMERA_4 };
	//异物， 凹凸， 擦伤， 晶点， 漏涂
	enum { DFT_MATTER = 0, DFT_BUMP, DFT_GRAZE, DFT_CRYSTAL, DFT_COATING };
	BOOL IsFileExist(const std::string &filename);
	BOOL IsPathExist(const std::string &pathname);

private:
	CString cstrlog;

	//测试变量
	HImage m_hi_test;
	HImage m_hi_ref;       //参考图像
	HImage m_hi_average;   //平均图像
	HImage m_hi_deviation;    //标准差图像

	HTuple hv_GPU_Handle;
	int m_camera1_invalid_area;
	int m_camera4_invalid_area;

	HANDLE                m_hStopEvent;
	CWinThread *m_ManageThread;
	CWinThread *m_ImageSaveThread;
	CWinThread *m_CalculateThread[5];
	AFX_THREADPROC m_threadCallBack[5] = { ImageCalculate1 ,ImageCalculate2,
			ImageCalculate3, ImageCalculate4, ImageCalculate5 };
	BOOL m_bThreadAlive[5] = { 0 };
	CRITICAL_SECTION m_csCalculateThread;
	CRITICAL_SECTION m_csDefImgList1;
	CRITICAL_SECTION m_csDefImgList2;
	CRITICAL_SECTION m_csDefImgList3;
	CRITICAL_SECTION m_csDefImgList4;
	CRITICAL_SECTION m_csDefImgList5;
	std::mutex mtx;

	void HalconInitAOP();
	BOOL LoadDefaultRefAndDevImage(std::string path);
	
	int ProduceReferenceImage1(HImage hi_ref1, HImage hi_ref2);
	int ProduceReferenceImage4(HImage hi_ref4, HImage hi_ref3);
	BOOL GetSavePath(std::string &path);
	BOOL GenerateReferenceImage(HImage &hi_average, HImage &hi_deviation);
	BOOL SaveReferenceImage();
	DefectType LocateDefectPosition(int camera_number, HObject ho_selectedregion,
		HTuple hv_Number, HTuple hv_colunm_origin, HObject ho_image);
	DefectType LocateDefectPosition(int camera_number, HObject ho_selectedregion);
	unsigned short int  ImageClassification(float radius);
	int  RankDivide(DefectType dtype);
	unsigned short int RankDivide(DeffectInfo dft_info);
	
	void HalconOpenGPU(HTuple &hv_DeviceHandle);
	int DetectAlgorithem(int cameraNO, HImage hi_ref, HImage hi_img, std::vector<DefectType> &vDFT);
	int DetectAlgorithemSimple(int cameraNO, HImage hi_ref, HImage hi_img, std::vector<DefectType> &vDFT);
	int StandDeviationAlgorithm(HImage hi_img, std::vector<DeffectInfo> &vecDftInfo, std::vector<HalconCpp::HObject> &vecDftImg);
	void GetOutDeviationArea(HImage hiImg, HObject &hoSelectedArea);
	void SplitAndMeasureDeffect(HObject selectArea);
	void AddNoise(HObject hoImg);

	static UINT ManageThread(LPVOID pParam);
	static UINT ImageCalculate1(LPVOID pParam);
	static UINT ImageCalculate2(LPVOID pParam);
	static UINT ImageCalculate3(LPVOID pParam);
	static UINT ImageCalculate4(LPVOID pParam);
	static UINT ImageCalculate5(LPVOID pParam);
};