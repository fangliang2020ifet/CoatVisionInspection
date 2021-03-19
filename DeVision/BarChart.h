// BarChart.h : header file

#if !defined(AFX_BARCHART_H__24B6B328_2855_4810_8F81_DC2EA0217FBD__INCLUDED_)
#define AFX_BARCHART_H__24B6B328_2855_4810_8F81_DC2EA0217FBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sql.h>
#include <sqlext.h>

#ifndef _AFX_NOFORCE_LIBS

#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")

#endif //!_AFX_NOFORCE_LIBS

 ///////////////////////////////////////////////////////////////////////
 // Some definitions for more customization
 //
 //		The following definitions can be used to customize the chart.
 //		Many values and variables in chart considered to have a default
 //		Value to make it easier to use the chart.
 //		But in some situations you might decide to customize them.
 //
 ///////////////////////////////////////////////////////////////////////

#define CHART_TITLE_HEIGHT		20	// This reserves enough room for the title at the bottom of the chart. If you don't need a title, set it to zero

#define GRID_MARGIN_WIDTH		0	// Distance between Top & Right side of the control and the Grid box
#define GRID_DEFAULT_SIZE		10	// WARNING: Should not set to 0 - This is the size of the grid rectangles

#define BAR_DEFAULT_HEIGHT		100	// Default height for each bar. Almost neutral since it will be replaced by the height of the control
#define BAR_DEFAULT_WIDTH		24	// Default width of each bar, increase it to make bars fat
#define BAR_DEFAULT_GAP			4	// Distance between bars
#define BAR_REFLECT_AMOUNT		32	// Decrease to make WHITE shines more

#define BAR_TXT_PERCENT_HEIGHT	10	// Room for a Value/Percent at the top of each bar
#define BAR_TXT_LABEL_HEIGHT	10	// Room for the label at the buttom of each bar
#define BAR_TXT_VLABEL_MARGIN	30	// The right margin, room for a value at the right side and in front of each bar in % mode

#define TIP_BK_COLOR			RGB(255, 255, 220)	// Background color for tooltip
#define TIP_FRAME_COLOR			RGB(240, 210, 80)	// Frame color for tooltip
#define TIP_TITLE_COLOR			RGB(255, 100, 0)	// Color of the tip's top most text
#define TIP_TEXT_COLOR			RGB(10, 100, 10)	// Color of the tip's middle text
#define TIP_PERCENT_COLOR		RGB(150, 150, 150)	// Color of the tip's bottom most text

#define TIP_LEFT_MARGIN			6					// Left margin for text in the tip
#define TIP_TOP_MARGIN			2					// Top margin for text in the tip

#define TIP_FRAME_THICKNESS		2					// Thickness of the frame line around the tooltip
#define TIP_TEXT_MAX_WIDTH		210					// Maximum width of the text within the tip
#define TIP_TEXT_MAX_HEIGHT		26					// Maximum height of the text within the tip
#define TIP_RGN_ROUND_CORNERS	10					// Increase to round tip corners more
#define TIP_SHADOW_SIZE			4					// NOT USED YET
#define TIP_SHADOW_COLOR		RGB(155, 155, 155)	// NOT USED YET
#define TIP_TOP_OFFSET			20
#define TIP_RIGHT_OFFSET		10

#define TIP_DELAY_TIMER			1	// Don't change this, please. It does not have any influence.
#define CHART_BORDER_DARKNESS	100 // Don't change this, please.
#define GRID_FRAME_HEIGHT		2	// Don't change this, please.

/////////////////////////////////////////////////////////////////////////////

 ///////////////////////////////////////////////////////////////////////
 // CChartGCalc Class
 //
 //
 //			A set of handy GDI functions for manipulating
 //			pixel colors, etc.
 //
 ///////////////////////////////////////////////////////////////////////
class CChartGCalc
{
protected:
	UINT GABS(const int& nmbr);
	COLORREF DifColor(COLORREF color, UINT nDist);
	COLORREF OrColor(COLORREF colFore, COLORREF colBK, UINT nForePercent);

	// Refer to the following article in 'CodeProject.com' By 'Roger Allen'
	// 'Printing tips and tricks from the trenches'
	// And goto topic: 'Use DIB's instead of DDB's'
	HANDLE DDBToDIB(CBitmap* bitmap, DWORD dwCompression, CPalette* pPal);
}; // end of GDICalc

 ///////////////////////////////////////////////////////////////////////
 // CChartTip Class
 //
 //
 //				Adds tooltip capabilities for bar chart.
 //				Tooltip should show: value, Label and percentage
 //
 ///////////////////////////////////////////////////////////////////////
class CChartTip : public CWnd, CChartGCalc
{
	// Construction
public:
	CChartTip();

	// Attributes
public:

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CChartTip)
		//}}AFX_VIRTUAL

	// Implementation
public:
	void Hide();
	void Pop();
	BOOL Create(CWnd* pParentWnd, BOOL* pbEnable, CString* lpszTitle, int nDelay);
	virtual ~CChartTip();

	// Generated message map functions
protected:
	static LPCTSTR		m_lpszClass;
	BOOL		*m_pbEnable,
		m_bTimer;
	CString		*m_lpszTitle,
		m_szText,
		m_szOldTitle;
	CPoint		m_pointOld;
	int			m_nDelay;
	CRect		m_rcClient;
	CBrush		m_brFrame,
		m_brBK,
		m_brShadow;
	CFont		m_fontBold,
		m_fontNorm;

	CRgn		m_rgnTip, m_rgnCopy, m_rgnShadow;

	//{{AFX_MSG(CChartTip)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void HideWindow();
};

///////////////////////////////////////////////////////////////////////
// CBarChart Class
//
//				Represents a chart control. The chart shows some bars
//				Each bar has a value. Bars heights will be calculated
//				Regarding other bars. The bars are able to show a percentage
//				Instead of a value. The bar with maximum value has 100% and
//				Percentage of other bars will be calculated in accordance to
//				This maximum value.
//
///////////////////////////////////////////////////////////////////////
class CBarChart : public CWnd, CChartGCalc
{
	// Construction
public:
	CBarChart();

	// Operations
protected:

	/*----------------------------------------------------------*
	 | CChartData Class											|
	 |															|
	 |		Holds data of a chart and let the chart to do		|
	 |		reqauered operations on it							|
	 |															|
	 *----------------------------------------------------------*/
	class CChartData
	{
		struct data
		{
			CString m_szLabel;
			double m_udValue;
			COLORREF m_color;
		};

		CObArray m_arr;
		double m_udMax;

	protected:
		void CalculateMaximum();

	public:
		CChartData();
		void RemoveAll();
		void RemoveAt(int nIndex);
		int GetCount();
		void Add(CString& szLabel, double udValue, COLORREF color);

		double GetMaxValue();
		double GetValue(int nIndex);
		CString GetLabel(int nIndex);
		COLORREF GetColor(int nIndex);
	};	// end of CChartData

	/*-----------------------------------------------------------*
	 | CGDIGrid Class											 |
	 |															 |
	 |		Draws a grid behind the chart.						 |
	 |		UNDONE: The grid class should act better. instead of |
	 |		drawing horizontal lines from top, it should start	 |
	 |		from the line under the bars. (Logical y = 0 axe)	 |
	 |															 |
	 *-----------------------------------------------------------*/
	class CGDIGrid
	{
		// Attributes
		UINT	m_nVLineCount,
			m_nHLineCount,
			m_nFixedSize;
		int i;

		CPen	m_penGrid,
			*m_pPenOld;

	public:
		// Construction
		CGDIGrid();
		~CGDIGrid();

		void SetLineCount(UINT nVLineCnt, UINT nHLineCnt, BOOL bFixedSize = FALSE, UINT nFixedSize = 0);
		void Draw(CDC* pDC, CRect& rcBound);
		void SetColor(COLORREF color);
	}; // end of CGDIGrid

	/*-----------------------------------------------------------*
	 | CChartBar Class											 |
	 |															 |
	 |		This class is responsible for for drawing bars and   |
	 |		performing required calculation for the job.         |
	 |															 |
	 *-----------------------------------------------------------*/
	class CChartBar : public CChartGCalc
	{
		CChartData
			m_chartData;

		CSize	m_sBarPicSize;

		BOOL	m_bShowLabel,
			m_bShowBarText;

		int		m_nBarTextType;
		int		m_nBarWidth, m_nBarGap;
		double	m_dAlpha, m_dIndex;

		COLORREF m_colorChartBK;

		CFont	m_font, *m_pFontOld;
		LOGFONT m_lf;

		double m_rMargin;

		void DrawBars(CDC* pDC, CRect& rcBound);
		void CreateLabelFont();

	public:
		BOOL IsLabelVisible();

		void DisplayToolTip(CPoint& point, CRect& rcBound,
			CRect& rcMemPic, BOOL bScale, CString& szTip);

		CChartBar();
		~CChartBar();

		void SetBkColor(COLORREF color);
		int GetBarHeight();
		int GetBarWidth();

		void RemoveAll();
		void RemoveAt(int nIndex);
		void AddBar(double udValue, CString szLabel, COLORREF color);

		void Draw(CDC* pDC, CRect& rcBound);
		void ShowLabel(BOOL bShow);
		void ShowBarText(int nType = 0 /* 0 or 1, Default: 0=Percent, 1=value*/, BOOL bShow = TRUE/*Default:Show*/);
	};

	/*----------------------------------------------------------*
	| CChartPrinter Class										 |
	|															 |
	|		No need to description(!) This class should print the|
	|		Chart using print dialog.							 |
	|															 |
	*----------------------------------------------------------*/
	class CChartPrinter : public CChartGCalc
	{
	protected:
		CDC		*m_pMemDC;
		CRect	m_rcMemPic;
		CBitmap *m_pBmpMem;

		BOOL	m_bScale;

		void OnPrint(CDC *pDC, CPrintInfo* pInfo);
		void OnPrintCompatible(CDC *pDC, CPrintInfo* pInfo);

		//HANDLE DDBToDIB( CBitmap* bitmap, DWORD dwCompression, CPalette* pPal );
	public:
		void Print(CDC* pDC, CDC *pMemDC, CBitmap* pBmp,
			CString szTitle, CRect& rcMemPic, BOOL bFit, BOOL bCompatible);
	};

	/*----------------------------------------------------------*
	| CChartDatabase Class										 |
	|															 |
	|		This class provides the ability to connect to an	 |
	|		ODBC database and read chart data from a table or an |
	|		Stored procedure.									 |
	|															 |
	*----------------------------------------------------------*/
	class CChartDatabase
	{
	protected:
		BOOL ExecuteSQL(CString& szQuery);
		void Close();
		BOOL ExecuteProc(CString szTable, CString szFilter, CString szLabelFieldName, CString szValueFieldName);
		BOOL ExecuteQuery(CString szTable, CString szFilter, CString szLabelFieldName, CString szValueFieldName);
		BOOL Connect(CString szDSN, CString szUsername, CString szPass);
		BOOL Prepare(ULONG ulODBCVer);

		SQLRETURN	m_sqlRet;				// To store resaults
		SQLHANDLE	m_hOdbcEnv;				// ODBC Environment handle
		SQLHANDLE	m_hDbConn;				// ODBC Connection handle
		SQLHSTMT	m_hstmt;				// ODBC Statement handle

		BOOL		m_bEOF;					// Set to true when reached the end of database
		CString		m_szError;

		struct scDBRow
		{
			SQLCHAR szLabel[51];
			double	dValue;
			SQLINTEGER nLabelLen;
			SQLINTEGER nValueLen;
		} m_row;

	public:
		CString GetLastErrorMessage();
		void GetRow(CString& szLabel, double& dValue);
		BOOL MoveNext();
		BOOL IsEOF();
		CChartDatabase();
		virtual ~CChartDatabase();

		BOOL OpenTable(CString szDSN, CString szTable,
			CString szLabelFieldName, CString szValueFieldName,
			CString szFilter = L"",
			CString szUsername = L"", CString szPass = L"",
			BOOL bStoredProc = FALSE,
			ULONG ulODBCVer = SQL_OV_ODBC3);

		BOOL OpenProc(CString szDSN, CString szStoredProc,
			CString szParameterList = L"",
			CString szUsername = L"", CString szPass = L"",
			ULONG ulODBCVer = SQL_OV_ODBC3);
	}; // End of chart database

// Attributes
protected:
	COLORREF m_colorBK;
	CRect	m_rctClient;
	CBrush	m_brFrame;
	CGDIGrid m_grid;

	BOOL	m_bShowGrid,
		m_bAutoScale,
		m_bShowTip;

	CChartBar
		m_bars;

	CDC		m_dcMem;

	CBitmap *m_pBmpBars,
		*m_pBmpOld;

	CRect	m_rctMemPic;

	CChartPrinter
		m_printer;

	CString m_szTitle,
		m_szTip;

	CChartTip
		m_toolTip;

	CString m_szLastErr;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CBarChart)
		//}}AFX_VIRTUAL

	// Implementation
protected:

	BOOL PromptForFile(CString& szPath, CString szFilterName, CString szFilter);
	void ResetBitmap(CDC* pDC);
	void DrawBars(CDC* pDC, CRect& rcBound);
	void RedrawChart(CDC* pDC);
	void DrawTitle(CDC* pDC);
	void InitToolTips();

public:
	void RedrawAll();

	void Print(BOOL bCompatible = FALSE);
	void Reset();
	void RemoveAll();
	void RemoveAt(int nIndex);

	void SetAutoScale(BOOL bAutoScale);

	void SetTitle(CString szNewTitle);

	void ShowTooltip(BOOL bShow);
	void ShowLabel(BOOL bShow, BOOL bRedraw = FALSE);
	void ShowGrid(BOOL bShow, BOOL bRedraw = FALSE);
	void ShowBarText(int nType = 0 /* 0 or 1, Default: 0=Percent, 1=value*/, BOOL bShow = TRUE/*Default:Show*/, BOOL bRedraw = FALSE);

	void AddBar(double udValue, CString szLabel, COLORREF color, BOOL bRepaint = FALSE);

	void SetBKColor(COLORREF colorBK);

	void Refresh();

	BOOL ReadFromDatabase(CString szDSN, CString szTable, CString szFilter,
		CString szLabelFieldName, CString szValueFieldName,
		COLORREF colorBars = RGB(0, 0, 0),
		CString szUsername = L"", CString szPass = L"");

	BOOL ReadFromDatabase(CString szDSN, CString szSPName, CString szParameterList = L"",
		COLORREF colorBars = RGB(0, 0, 0),
		CString szUsername = L"", CString szPass = L"");

	CString GetLastDatabaseErrMessage();
	CString GetLastErrorMessage();

	BOOL SaveToFile(CString szPath = L"");

	void SetGridLines(int nHorLineCount, int nVerLineCount, BOOL bFixedSize = FALSE, int nFixedSize = 0);

	BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nControlID, CString szChartTitle = L"");
	virtual ~CBarChart();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBarChart)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BARCHART_H__24B6B328_2855_4810_8F81_DC2EA0217FBD__INCLUDED_)
