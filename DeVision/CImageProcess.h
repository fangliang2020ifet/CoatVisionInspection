#pragma once

#include "afxwin.h"
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

struct DefectImage
{
	HObject ho_img;
	HTuple  hv_name;
};

typedef std::list<HImage> ImgList;
typedef std::list<DefectType> DFTList;


class CImageProcess
{
public:
	CImageProcess();
	~CImageProcess();

public:
	HWND hMainWnd;                           //�����ھ��

	CEvent StopManage_Event;
	CEvent AllCalculateThreadStopped_Event;

	BOOL SYSTEM_PAUSE = FALSE;
	BOOL TEST_MODEL = FALSE;               //ʹ�ñ���ͼ�����г���
	BOOL LOAD_LOCAL_IMAGE = FALSE;
	BOOL REDUCE_BLACK_EDGE = FALSE;        //�޳��ڱ�
	BOOL SAVE_REFERENCE_IMAGE = FALSE;     //�Ƿ񱣴����ɵĲο�ͼ��

	BOOL m_referenceImage_OK = FALSE;
	//�㷨����
	int m_k_normal_distribution = 5;       //�����ܶ�(3 = 92%, 5 = 98%)����׼��ı���
	int m_median_filter_size = 1;          //�˲�����С,ֱ�ӹ�ϵ�����,����sizeԽ������ٶ�Խ��
	float m_fMin_Radius = 0.05f;	           //�뾶ɾѡ
	float m_fMax_Radius = 50.0f;
	int m_threadnum = 1;                    //�����������߳�����

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

	std::string m_strPath;                  //����·��

	float m_current_position = 0.0f;
	int m_total_list_size = 0;              //  list ���ܴ�С,�������ͼ������

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
	std::list<DefectImage> DefImgList;

	//���Ա���
	HImage m_hi_test1;
	HImage m_hi_test2;
	HImage m_hi_test3;
	HImage m_hi_test4;
	HImage m_hi_ref1;       //�ο�ͼ��
	HImage m_hi_ref2;
	HImage m_hi_ref3;
	HImage m_hi_ref4;
	HImage m_hi_average1;   //ƽ��ͼ��
	HImage m_hi_average2;
	HImage m_hi_average3;
	HImage m_hi_average4;
	HImage m_hi_deviation1;    //��׼��ͼ��
	HImage m_hi_deviation2;
	HImage m_hi_deviation3;
	HImage m_hi_deviation4;

protected:
	enum{ CAMERA_1 = 1, CAMERA_2, CAMERA_3, CAMERA_4};
	//��� ��͹�� ���ˣ� ���㣬 ©Ϳ
	enum { DFT_MATTER = 0, DFT_BUMP, DFT_GRAZE, DFT_CRYSTAL, DFT_COATING};

	BOOL IsFileExist(const std::string &filename);
	BOOL IsPathExist(const std::string &pathname);

private:
	HTuple hv_GPU_Handle;
	int m_camera1_invalid_area;
	int m_camera4_invalid_area;

	void StopCalculateThreads();

	BOOL LoadDefaultRefAndDevImage(std::string path);
	BOOL CheckReferenceImageAvilable();

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
	void HalconOpenGPU(HTuple &hv_DeviceHandle);
	int DetectAlgorithem(int cameraNO, HImage hi_ref, HImage hi_img, std::vector<DefectType> &vDFT);
	int DetectAlgorithemSimple(int cameraNO, HImage hi_ref, HImage hi_img, std::vector<DefectType> &vDFT);
	int StandDeviationAlgorithm(int cameraNO, HImage hi_average, HImage hi_deviation,
		HImage hi_img, UINT64 produced_num, std::vector<DefectType> &vDFT);

	void HalconInitAOP();

	HANDLE                m_hStopEvent;

	CWinThread *m_ManageThread;
	CWinThread *m_ImageSaveThread;
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
	CRITICAL_SECTION m_csDefImgList;
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