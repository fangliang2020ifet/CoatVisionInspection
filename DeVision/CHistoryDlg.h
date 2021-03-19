#pragma once
#include <string>
#include <vector>

// CHistoryDlg 对话框

class CHistoryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CHistoryDlg)

public:
	CHistoryDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CHistoryDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HISTORY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	HWND hMainWnd;                           //主窗口句柄

	//std::string m_strPath;	                           //历史图像路径

	//std::vector<std::string> m_vImages;
	BOOL LoadHistoryImage();

	int m_pages;
	std::string m_file_path = "D:\\DetectRecords\\HistoryImages";

protected:
	CWnd *pwnd1;
	CWnd *pwnd2;
	CWnd *pwnd3;
	CWnd *pwnd4;
	CWnd *pwnd5;
	CWnd *pwnd6;
	CWnd *pwnd7;
	CWnd *pwnd8;
	CWnd *pwnd9;

private:
	std::vector<std::string> m_vImage_name;
	bool m_bPagging;

	CWinThread *m_pRefrushThread;
	bool m_bThreadAlive;
	static UINT autoRefrush(LPVOID pParam);


	void RefrushImgWnd(std::string path, std::vector<std::string> vstring);
	void RefrushHistoryWnd(int page_index);
	void ReadDFTInfo(std::string name, std::string &kind, std::string &position, std::string &radius, std::string &area);
	void ShowBitmap(CWnd *pWnd, CString BmpName);
	void ShowBitmap(CWnd *pWnd, const char* BmpName);


public:
	afx_msg void OnBnClickedButtonPrePage();
	afx_msg void OnBnClickedButtonNextPage();
	CStatic m_img1;
	CStatic m_img2;
	CStatic m_img3;
	CStatic m_img4;
	CStatic m_img5;
	CStatic m_img6;
	CStatic m_img7;
	CStatic m_img8;
	CStatic m_img9;
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	CButton m_btn_pre_page;
	CButton m_btn_next_page;
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonOpenhistorypath();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
