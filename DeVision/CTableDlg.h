#pragma once

#include <string>
#include <vector>
#include "CInspectDlg.h"



// CTableDlg 对话框

class CTableDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTableDlg)

public:
	CTableDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTableDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TABLE };
#endif

public:
	HWND hMainWnd;                           //主窗口句柄	
	int* m_pnSystemState;                      //系统状态
	std::string m_DFT_img_path;
	std::string m_save_path;
	std::wstring m_wstr_batch;                 //批号
	std::wstring m_wstr_name;                  //型号
	std::wstring m_wstr_schedule;              //班次
	std::wstring m_wstr_addition;              //备注
	std::wstring m_wstr_width;              
	std::wstring m_wstr_length;                //长度
	std::wstring m_wstr_speed;                 //平均速度
	int m_product_rank;                        //产品评级
	int m_DFT_rank[5];                         //每种瑕疵类型的个数统计
	int m_serious_num;                         //D级缺陷数目
	float *m_pfCurPos;


	void RefrushDistributeWnd();
	void GetDetectResult(int rank0, int rank1, int rank2, int rank3, int rank4);
	void DrawTable(CDC *mDC, CRect rect, float x, float y);
	void CreateFlag(CDC *mDC, int x, int y, int kind);
	void DrawAllFlag(CDC *mDC, int wnd_width, int wnd_height);
	void DrawSelectDFT(CDC *mDC, int x, int y);
	void AddToDetailList(int NO, int kind, float position, float radius, int rank);
	void SaveDistributeImage();
	HBITMAP GetSrcBit(HDC hDC, LPRECT rEct);
	bool SaveBMPToFile(HBITMAP hBitmap, LPSTR lpFileName);
	bool SaveBitmapToFile(HBITMAP hBitmap, const char* name);
	void SaveToExcelUseDefault(CString &name);
	void SaveScatterPlotUseDefault();
	void FormatTableHead(CWorksheet &sheet, CRange &range, BOOL bhead);
	void BeginSaveTable();

	CFont m_font;
	std::vector<DeffectInfo> *m_pvDFT;         //  用于显示分布图
	//std::vector<DeffectInfo> m_vecDFT;         //  用于写入报表
	float scale_x;
	float scale_y;

	CEvent TableSaved_Event;

	CRITICAL_SECTION m_csvec;                  //定义一个临界区

private:
	CPen m_pen[7];
	COLORREF red_color = RGB(255, 35, 15);             //红色
	COLORREF green_color = RGB(25, 255, 35);          //绿色
	COLORREF blue_color = RGB(35, 55, 225);            //蓝色
	COLORREF yellow_color = RGB(255, 255, 0);          //黄色

	int m_selected_x = 0;
	int m_selected_y = 0;
	CDC m_memTablePaintDC;                             //  内存DC
	CRect m_rectTableControl;
	CWinThread *m_pDrawTableThread;
	bool m_bTableThreadAlive;
	static UINT userDrawTable(LPVOID pParam);


	std::wstring m_wstr_savetime;                        //保存时间
	CWinThread *m_SaveTable;
	static UINT SaveTableThreadDefault(LPVOID pParam);
	HBITMAP m_hbitmap;
	void GenerateReportName(std::wstring &wstrname);
	void InitialHistoryList();
	void InitialDetailList();
	CString GenerateRankText(int rank);
	std::vector<std::string> m_vstring;
	void GetExcelsInfo(std::string filename, std::string &time, std::string &number,
		std::string &ID, std::string &longth, std::string &operators);

	CString m_current_excel_name;                       //当前使用的 excel 的文件名
	void OpenExcelFile(std::wstring excelname);
	void ShowBitmap(CWnd *pWnd, CString BmpName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrlHis;
	CListCtrl m_ListCtrlDetail;
	CComboBox m_comSearchOption;
	CEdit m_editSearchWord;
	CButton m_open_inprogram;

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedBtnRefrush();
	afx_msg void OnPaint();
	afx_msg void OnPaintClipboard(CWnd* pClipAppWnd, HGLOBAL hPaintStruct);
	afx_msg void OnBnClickedButtonSearch();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedButtonShowall();
	afx_msg void OnBnClickedButtonDelselect();
	afx_msg void OnBnClickedButtonDelall();
	afx_msg void OnBnClickedButtonPrint();
	afx_msg void OnBnClickedButtonSaveas();
	afx_msg void OnBnClickedButtonOpenexcel();
	afx_msg void OnBnClickedButtonOpeninprogram();
	afx_msg void OnBnClickedButtonOpenexcelpath();
	afx_msg void OnDestroy();
	afx_msg void OnNMDblclkListDetail(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};
