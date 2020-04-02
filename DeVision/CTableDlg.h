﻿#pragma once

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
	std::wstring m_save_path = L"D:\\report\\";
	std::wstring m_wstr_num;               //批号
	std::wstring m_wstr_id;                //型号
	std::wstring m_wstr_width;
	std::wstring m_wstr_user;
	std::wstring m_wstr_speed = L"20";              //速度

	void DrawTable(CDC *mDC, CRect rect, float x, float y);
	void CreateFlag(CDC *mDC, int x, int y, int kind);
	void DrawAllFlag(CDC *mDC, int wnd_width, int wnd_height);
	void AddToDetailList(int NO, int kind, float position, float radius, int rank);
	HBITMAP GetSrcBit(HDC hDC, LPRECT rEct);
	bool SaveBMPToFile(HBITMAP hBitmap, LPSTR lpFileName);
	bool SaveBitmapToFile(HBITMAP hBitmap, LPSTR lpFileName);
	void SaveToExcel(std::vector<DefectType> vDFT);
	void FormatTableHead(CWorksheet &sheet, CRange &range, BOOL bhead);
	void BeginSaveTable();

	CFont m_font;
	std::vector<DefectType> *m_pvDFT;
	std::vector<DefectType> m_vecDFT;
	float m_current_position;
	float scale_x;
	float scale_y;
	BOOL m_save_successfully = TRUE;

	CRITICAL_SECTION m_csvec;                  //定义一个临界区

private:
	CPen m_pen[6];
	COLORREF red_color = RGB(255, 35, 15);             //红色
	COLORREF green_color = RGB(25, 255, 35);          //绿色
	COLORREF blue_color = RGB(35, 55, 225);            //蓝色
	COLORREF yellow_color = RGB(255, 255, 0);          //黄色

	std::wstring m_wstr_savetime;                        //保存时间
	CWinThread *m_SaveTable;
	static UINT SaveTableThread(LPVOID pParam);
	HBITMAP m_hbitmap;
	void GenerateReportName(std::wstring &wstrname);
	void InitialHistoryList();
	void InitialDetailList();

	std::vector<std::string> m_vstring;
	void GetExcelsInfo(std::string filename, std::string &time, std::string &number,
		std::string &ID, std::string &longth, std::string &operators);

	void OpenExcelFile(std::wstring excelname);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrlHis;
	CListCtrl m_ListCtrlDetail;

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
	CComboBox m_comSearchOption;
	CEdit m_editSearchWord;
	CButton m_open_inprogram;
};
