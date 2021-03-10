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
	int rank = 0;                     //覴õȼ�
	int type = 0;                     //覴÷���/����
	float center_x = 0;               //ˮƽλ��(����)
	float absolute_position = 0.0f;   //����λ��(��)
	float area = 0.0f;                //���
	float contlength = 5.0f;          //�ܳ�
	float circle_radius = 1.0f;       //���Բֱ��
	int pixel_value = 128;            //����ƽ���Ҷ�ֵ
	bool operator < (const DefectType& def) const {
		return absolute_position < def.absolute_position;
	}
};

typedef struct DeffectInfo 
{
	unsigned short int rank = 0;          //覴õȼ�
	unsigned short int type = 0;          //覴÷���/����
	float              x = 0;             //ˮƽλ��(��λ������)
	float              y = 0;             //����λ��
	float              area = 0.0f;       //���
	unsigned short int contlength = 0;    //�ܳ�
	unsigned short int radius = 0;        //���Բֱ��
	unsigned int       image_index = 0;   //覴�����ͼ�����
	unsigned short int pixel_value = 0;   //����ƽ���Ҷ�ֵ

}DeffectInfo;

struct SelectRegion
{
	//��λ��Ϊ����
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
	HWND hMainWnd;                               //�����ھ��
	HANDLE m_hFinishedProcess;                     //�������
	CEvent StopManage_Event;
	//CEvent CalculateThread_1_StoppedEvent;
	CEvent CalculateThread_2_StoppedEvent;
	CEvent CalculateThread_3_StoppedEvent;
	CEvent CalculateThread_4_StoppedEvent;
	CEvent CalculateThread_5_StoppedEvent;


	BOOL SYSTEM_PAUSE;
	BOOL TEST_MODEL;                            //����ģʽ
	BOOL SAVE_REFERENCE_IMAGE;                  //�Ƿ񱣴����ɵĲο�ͼ��
	BOOL m_referenceImage_OK;                   //�ο�ͼ���Ƿ�������
	int m_threadnum;                            //�߳�����
	int m_k_normal_distribution;                //�����ܶ�(3 = 92%, 5 = 98%)����׼��ı���
	int m_median_filter_size;                   //�˲�����С,ֱ�ӹ�ϵ�����,����sizeԽ������ٶ�Խ��
	float m_fMin_Radius;	                    //�뾶ɸѡ
	float m_fMax_Radius;
	std::string m_strPath;                      //����·��
	int m_nTotalListNumber;                  //�������ͼ������

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
	//��� ��͹�� ���ˣ� ���㣬 ©Ϳ
	enum { DFT_MATTER = 0, DFT_BUMP, DFT_GRAZE, DFT_CRYSTAL, DFT_COATING };
	BOOL IsFileExist(const std::string &filename);
	BOOL IsPathExist(const std::string &pathname);

private:
	CString cstrlog;

	//���Ա���
	HImage m_hi_test;
	HImage m_hi_ref;       //�ο�ͼ��
	HImage m_hi_average;   //ƽ��ͼ��
	HImage m_hi_deviation;    //��׼��ͼ��

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