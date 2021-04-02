/*********************************************************************
 * BarChart.cpp
 * implementation of
 *						CChartBar,
 *						CChartData,
 *						CChartPrinter,
 *						CChartGCalc,
 *						CGDIGrid,
 *
 * Auther: Hamed.M.
 * EMail : HamedMosavi @ gmail.com
 *
 * CAUTION   Disclaimer:
 *			 This code is provided as is without any warranty of
 *			 any kind. The author is not responsible for whatever
 *			 problem or damages that may occur as aresault of using
 *			 this code. Use this entirely at your own risk.
 *			 So don't try to find me if this code made your dog
 *			 Sound like a frog, or burned your car, or even made your
 *			 girl/boy friend shout on you. I am not responsible ;)
 *
 *
 ********************************************************************/
 /********************************************************************
  * History:
  *
  *			Released : 2007/01/11
  *
  *	BugFixes:
  *			2007/01/11
  *			unsigned long double converted to -> double
  *			Some scaling bugs fixed
  *			Tooltips added
  *
  *			2007/01/15
  *			Some function changed.
  *			Tooltip updated.
  *			Some comments added.
  *
  *			2007/01/20
  *			Tooltips now behave much like tooltips(!)
  *
  *			2007/01/24
  *			Database connectivity added.
  *
  *			2007/01/26
  *			Tooltip bug fixed (SetCapture added).
  ********************************************************************/

#include "stdafx.h"
#include "BarChart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

CBarChart::CBarChart()
{
	m_rctClient = CRect(0, 0, 0, 0);

	SetBKColor(RGB(240, 240, 240));

	m_pBmpBars = NULL;
	m_pBmpOld = NULL;

	ShowLabel(TRUE);
	ShowGrid(TRUE);
	ShowBarText(1, TRUE);
	SetAutoScale(FALSE);
	ShowTooltip(TRUE);

	m_szTitle = L"";
	m_szLastErr = L"";
}

CBarChart::~CBarChart()
{
	if (m_brFrame.GetSafeHandle())
	{
		m_brFrame.DeleteObject();
	}

	if (m_pBmpOld)
	{
		m_dcMem.SelectObject(m_pBmpOld);
	}

	if (m_pBmpBars)
	{
		m_pBmpBars->DeleteObject();
		delete m_pBmpBars;
		m_pBmpBars = NULL;
	}

	if (m_dcMem.GetSafeHdc())
	{
		m_dcMem.DeleteDC();
	}
}

BEGIN_MESSAGE_MAP(CBarChart, CWnd)
	//{{AFX_MSG_MAP(CBarChart)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarChart message handlers

// Creates the child control.
// Parameters:
//				rect		: Defines bounding rectangle of the control
//				pParentWnd	: A pointer to the window on which this control will be shown
//				nControlID	: Identifyer of the control
//				szChartTitle: Defines the title at the bottom of the control, can be a simple description of the chart.
BOOL CBarChart::Create(const RECT &rect, CWnd *pParentWnd, UINT nControlID, CString szChartTitle)
{
	if (GetSafeHwnd())
	{
		return FALSE;
	}

	m_rctClient = rect;

	m_szTitle = szChartTitle;

	BOOL bRes = CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, pParentWnd, nControlID);

	if (TRUE == bRes)
	{
		InitToolTips();
	}

	return bRes;
}

BOOL CBarChart::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CBarChart::OnPaint()
{
	CPaintDC dc(this);

	GetClientRect(&m_rctClient);

	if (m_dcMem.GetSafeHdc())
	{
		if (m_bAutoScale)
		{
			dc.StretchBlt(0, 0, m_rctClient.Width(), m_rctClient.Height(),
				&m_dcMem, 0, 0, m_rctMemPic.Width(), m_rctMemPic.Height(), SRCCOPY);
		}
		else
		{
			dc.BitBlt(0, 0, m_rctClient.Width(), m_rctClient.Height(),
				&m_dcMem, 0, 0, SRCCOPY);
		}
	}
	else
	{
		RedrawChart(&dc);
	}
}
// Prints the content of the child control (WYSIWYG)
// Parameters:
//				bCompatible :
//								Default is set to FALSE
//								Set this TRUE if you have any difficaulty printing your chart.
//								It's defined since some printers might not be able to print DDB
//								Bitmaps, setting this to TRUE force the function to use DIB instead.
void CBarChart::Print(BOOL bCompatible)
{
	CClientDC dc(this);
	m_printer.Print(&dc, &m_dcMem, m_pBmpBars,
		(CString)AfxGetAppName() + (CString)" - " + (CString)m_szTitle,
		m_rctMemPic, m_bAutoScale, bCompatible);
}

void CBarChart::RedrawChart(CDC* pDC)
{
	if (!m_dcMem.GetSafeHdc())
	{
		m_dcMem.CreateCompatibleDC(pDC);
	}

	// Deselect and destroy old bitmap, and create a new one
	ResetBitmap(pDC);

	Refresh();
}

void CBarChart::ResetBitmap(CDC* pDC)
{
	// Deselect and destroy old bitmap
	if (m_pBmpOld)
	{
		m_dcMem.SelectObject(m_pBmpOld);
	}

	if (m_pBmpBars)
	{
		m_pBmpBars->DeleteObject();
		delete m_pBmpBars;
		m_pBmpBars = NULL;
	}

	// Re create bitmap
	m_rctMemPic.top = 0;
	m_rctMemPic.left = 0;

	m_rctMemPic.bottom = m_bars.GetBarHeight();
	m_rctMemPic.right = m_bars.GetBarWidth();

	if (m_rctMemPic.Width() < m_rctClient.Width())
	{
		m_rctMemPic.right = m_rctClient.Width();
	}

	if (m_rctMemPic.Height() < m_rctClient.Height())
	{
		m_rctMemPic.bottom = m_rctClient.Height();
	}

	m_pBmpBars = new CBitmap;

	m_pBmpBars->CreateCompatibleBitmap(pDC, m_rctMemPic.right, m_rctMemPic.bottom);
	m_pBmpOld = (CBitmap*)m_dcMem.SelectObject(m_pBmpBars);
}

// Sets/Resets the scale capability
// Parameters:
//				bAutoScale :
//							If set to TRUE force the chart to scale itself to the parent
//							Window, regardless of it's initial bounding rectangle.
//							If set to FALSE returns the size of the chart to the original
//							Bounding rectangle.
//							Note that chart saves it's initial state of creation.
void CBarChart::SetAutoScale(BOOL bAutoScale)
{
	m_bAutoScale = bAutoScale;
}

// Sets the color of the background rectangle of the chart
// Parameters:
//				colorBK :
//							The color in COLORREF format(eg.RGB(250,250,250))
void CBarChart::SetBKColor(COLORREF colorBK)
{
	// Reset background color
	m_colorBK = colorBK;

	// Reset frame brush
	if (m_brFrame.GetSafeHandle())
	{
		m_brFrame.DeleteObject();
	}
	m_brFrame.CreateSolidBrush(DifColor(colorBK, CHART_BORDER_DARKNESS));

	// Reset grid
	m_grid.SetColor(DifColor(colorBK, 50));

	// Redraw window, if it is created
	if (GetSafeHwnd())
	{
		Invalidate();
	}
}

// Adds a bar to the chart
// Parameters:
//				udValue : Value of the bar
//				szLabel : Label of the bar, will be shown under the bar
//				color	: Color of the bar. This color will be used to create a gradient.
//				bRepaint: If set to TRUE, forces the chart to show changes on the screen
//						  Default is set to false. It is recommended to set it to true just
//						  For last Add bar of the chart, so the chart won't be forced to
//						  Show changes every time a single bar is added.
void CBarChart::AddBar(double udValue, CString szLabel, COLORREF color, BOOL bRepaint)
{
	m_bars.AddBar(udValue, szLabel, color);

	if (bRepaint)
	{
		CClientDC dc(this);
		RedrawChart(&dc);

		Invalidate();
	}
}

// Shows/Hides the labels under the chart control
// Parameters:
//				bShow :	 Set to TRUE to show labels
//				bRedraw: If set to TRUE, forces the chart to show changes on the screen
//						  Default is set to false.
void CBarChart::ShowLabel(BOOL bShow, BOOL bRedraw)
{
	m_bars.ShowLabel(bShow);

	if (bRedraw)
	{
		Refresh();
	}
}

// Shows/Hides the grid behind the chart control
// Parameters:
//				bShow :	 Set to TRUE to show labels
//				bRedraw: If set to TRUE, forces the chart to show changes on the screen
//						  Default is set to false.
void CBarChart::ShowGrid(BOOL bShow, BOOL bRedraw)
{
	m_bShowGrid = bShow;
	if (bRedraw)
	{
		Refresh();
	}
}

// Shows/Hides the values at the top of the bars of the chart control
// Parameters:
//				nType :  This parameter can be 0 or 1. 0 indicates the chart should
//						 Display percentage of it's value as the top text which is also
//						 The default behavior. Set it to 1 to force the chart to show
//						 The exact VALUE instead of the percentage.
//				bShow :	 Set to TRUE to show labels or FALSE to hide them
//				bRedraw: If set to TRUE, forces the chart to show changes on the screen
//						  Default is set to false.
void CBarChart::ShowBarText(int nType, BOOL bShow, BOOL bRedraw)
{
	m_bars.ShowBarText(nType, bShow);
	if (bRedraw)
	{
		Refresh();
	}
}

// Activates/Deactivates tooltips for bars
// Parameters:
//				bShow :	 Set to TRUE to Activate tooltips
void CBarChart::ShowTooltip(BOOL bShow)
{
	m_bShowTip = bShow;
}
// Draw bar chart
void CBarChart::DrawBars(CDC* pDC, CRect& rcBound)
{
	m_bars.SetBkColor(m_colorBK);
	m_bars.Draw(&m_dcMem, rcBound);
}

void CBarChart::DrawTitle(CDC* pDC)
{
	//pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(m_colorBK);

	CRect rcTitle = CRect(m_rctMemPic.left,
		m_rctMemPic.bottom - CHART_TITLE_HEIGHT,
		m_rctMemPic.right, m_rctMemPic.bottom);

	pDC->FillSolidRect(rcTitle, m_colorBK);

	// UNDONE : DEFAULT DEVICE CONTEXT FONT IS SELECTED, SHOUD CHANGE TO A CUSTOM DEFINED FONT
	pDC->SetTextColor(RGB(200, 200, 200));
	pDC->DrawText(m_szTitle,
		CRect(rcTitle.left + 2, rcTitle.top + 2, rcTitle.right, rcTitle.bottom)
		, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	pDC->SetTextColor(RGB(0, 0, 0));

	pDC->DrawText(m_szTitle, rcTitle, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

// Sets the text(Title/Label) of the chart control. This is the text that will be diplayed
// At the bottom of the chart.
// Parameters:
//				szNewTitle : A title string
void CBarChart::SetTitle(CString szNewTitle)
{
	m_szTitle = szNewTitle;
}

// Forces chart to be redrawn. Redrawing does not affect the chart's bars or etc.
// It just makes the chart to redraw all it's content items.
void CBarChart::Refresh()
{
	if (!GetSafeHwnd())
	{
		return;
	}

	// Draw background
	if (m_dcMem.GetSafeHdc())
	{
		m_dcMem.FillSolidRect(&m_rctMemPic, m_colorBK);
	}
	else
	{
		return;
	}

	// Draw gridlines
	if (m_bShowGrid)
	{
		m_rctMemPic.bottom -= (CHART_TITLE_HEIGHT +
			(m_bars.IsLabelVisible() * BAR_TXT_LABEL_HEIGHT) + 3);

		m_grid.Draw(&m_dcMem, m_rctMemPic);
		m_rctMemPic.bottom += (CHART_TITLE_HEIGHT +
			(m_bars.IsLabelVisible() * BAR_TXT_LABEL_HEIGHT) + 3);
	}

	// Draw bar chart
	DrawBars(&m_dcMem, m_rctMemPic);

	// Draw the chart title
	DrawTitle(&m_dcMem);

	// Draw a frame around the window
	m_dcMem.FrameRect(&m_rctMemPic, &m_brFrame);

	Invalidate();
}

// Removes all bars from the chart. The background  remains unchanges
void CBarChart::RemoveAll()
{
	m_bars.RemoveAll();
	Refresh();
}

// Removes a bar with the zero index identified. Removing a bar does not force the other bars to
// Re calculate height or percentage. There is, however, workaround this as a new feature.
// Parameters:
//				nIndex: Zero based index of the bar to remove.
void CBarChart::RemoveAt(int nIndex)
{
	m_bars.RemoveAt(nIndex);
	Refresh();
}

// Removes all bars, and forces the chart to be redrawn.
void CBarChart::Reset()
{
	m_bars.RemoveAll();
	m_rctMemPic = CRect(0, 0, 0, 0);

	CClientDC dc(this);
	RedrawChart(&dc);
}

void CBarChart::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	//	m_rctClient.bottom = cy - m_rctClient.top;
	//	m_rctClient.right = cx - m_rctClient.left;
}

void CBarChart::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);

	if ((m_bAutoScale && PtInRect(m_rctClient, point)) ||
		(!m_bAutoScale&& PtInRect(m_rctMemPic, point)))
	{
		if (this != GetCapture())
		{
			SetCapture();
		}

		m_bars.DisplayToolTip(point, m_rctClient, m_rctMemPic, m_bAutoScale, m_szTip);
		if (!m_szTip.IsEmpty())
		{
			m_toolTip.Pop();
		}
		else
		{
			m_toolTip.Hide();
		}
	}
	else
	{
		m_toolTip.Hide();

		if (this == GetCapture())
		{
			if (!ReleaseCapture())
			{
				// Retry
				ReleaseCapture();
			};
		}
	}
}

void CBarChart::InitToolTips()
{
	m_toolTip.Create(this, &m_bShowTip, &m_szTip, 4000);
}

// Does nothing at the moment
// {
//		//Empty body
// }
void CBarChart::RedrawAll()
{
	// UNDONE : FORCE CHART TO RE CALCULATE ALL BAR HEIGHTS AND %
	CClientDC dc(this);
	RedrawChart(&dc);

}
// WARNING : ODBC ONLY
// This function tryes to read chart data from a Table
BOOL CBarChart::ReadFromDatabase(CString szDSN, CString szTable, CString szFilter,
	CString szLabelFieldName, CString szValueFieldName,
	COLORREF colorBars,
	CString szUsername, CString szPass)
{
	CChartDatabase cDB;
	if (!cDB.OpenTable(szDSN, szTable, szLabelFieldName, szValueFieldName,
		szFilter, szUsername, szPass))
	{
		m_szLastErr = cDB.GetLastErrorMessage();
		return FALSE;
	};

	CString szLabel;
	double dVal;

	while (!cDB.IsEOF())
	{
		cDB.GetRow(szLabel, dVal);
		if (colorBars == RGB(0, 0, 0))
		{
			AddBar(dVal, szLabel, RGB(rand() % 255, rand() % 255, rand() % 255));
		}
		else
		{
			AddBar(dVal, szLabel, colorBars);
		}

		cDB.MoveNext();
	}

	Refresh();

	return TRUE;
}
// This function tryes to read chart data from an Stored Procedure or Query
BOOL CBarChart::ReadFromDatabase(CString szDSN, CString szSPName, CString szParameterList,
	COLORREF colorBars, CString szUsername, CString szPass)
{
	CChartDatabase cSP;

	if (!cSP.OpenProc(szDSN, szSPName, szParameterList, szUsername, szPass))
	{
		m_szLastErr = cSP.GetLastErrorMessage();
		return FALSE;
	};

	CString szLabel;
	double dVal;

	while (!cSP.IsEOF())
	{
		cSP.GetRow(szLabel, dVal);
		if (colorBars == RGB(0, 0, 0))
		{
			AddBar(dVal, szLabel, RGB(rand() % 255, rand() % 255, rand() % 255));
		}
		else
		{
			AddBar(dVal, szLabel, colorBars);
		}

		cSP.MoveNext();
	}

	Refresh();
	return TRUE;
}

CString CBarChart::GetLastDatabaseErrMessage()
{
	return m_szLastErr;
}

CString CBarChart::GetLastErrorMessage()
{
	return GetLastDatabaseErrMessage();
}

BOOL CBarChart::SaveToFile(CString szPath)
{
	// to return results
	BOOL bRes = TRUE;

	// If chart is not ready
	if (m_dcMem.GetSafeHdc() == NULL)
	{
		return FALSE;
	}

	// If no path specified, ask for a path
	if (szPath == "")
	{
		if (!PromptForFile(szPath, _T("Bitmap file"), _T("*.Bmp")))
		{
			m_szLastErr = "Canceled by user";
			return FALSE;
		}
	}

	BITMAPINFOHEADER    *pBMI;
	BITMAPFILEHEADER	hdr;
	DWORD		dwTotal;              // total count of bytes
	DWORD		cb;                   // incremental count of bytes
	CPalette	pal;
	DWORD		dwWritten = 0;
	LPBYTE		hp;

	// Prepare for converting DDB to DIB
	UINT        nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
	LOGPALETTE* pLP = (LOGPALETTE*)new BYTE[nSize];
	pLP->palVersion = 0x300;
	pLP->palNumEntries = (unsigned short)GetSystemPaletteEntries(
		m_dcMem.GetSafeHdc(), 0, 255,
		pLP->palPalEntry);

	// Create the palette
	pal.CreatePalette(pLP);

	// Free memory
	delete[] pLP;

	// Do convert DDB to DIB
	HANDLE hDib = DDBToDIB(m_pBmpBars, BI_RGB, &pal);

	// Prepare Bitmap Info Header
	pBMI = (BITMAPINFOHEADER*)GlobalLock(hDib);
	int nColors = 0;
	if (pBMI->biBitCount <= 8)
	{
		nColors = (1 << pBMI->biBitCount);
	}

	// Prepare FileHeader
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the size of the entire file.
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
		pBMI->biSize + pBMI->biClrUsed
		* sizeof(RGBQUAD) + pBMI->biSizeImage);

	// Compute the offset to the array of color indices.
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
		pBMI->biSize + pBMI->biClrUsed
		* sizeof(RGBQUAD);

	// Create the .BMP file.
	HANDLE hf = CreateFile(szPath,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	if (hf == INVALID_HANDLE_VALUE)
	{
		m_szLastErr = "Cannot create file";
		bRes = FALSE;
		goto CLEANUP;
	}

	// Copy the BITMAPFILEHEADER into the .BMP file.
	if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)&dwWritten, NULL))
	{
		m_szLastErr = "Cannot write to file";
		bRes = FALSE;
		goto CLEANUP;
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
	if (!WriteFile(hf, (LPVOID)pBMI, sizeof(BITMAPINFOHEADER)
		+ pBMI->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)&dwWritten, (NULL)))
	{
		m_szLastErr = "Cannot write to file";
		bRes = FALSE;
		goto CLEANUP;
	}

	// Copy the array of color indices into the .BMP file.
	dwTotal = cb = pBMI->biSizeImage;
	hp = (LPBYTE)pBMI + (pBMI->biSize + nColors * sizeof(RGBQUAD));
	if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwWritten, NULL))
	{
		m_szLastErr = "Cannot write to file";
		bRes = FALSE;
		goto CLEANUP;
	}

CLEANUP:;

	// free resources
	GlobalUnlock(hDib);
	GlobalFree(hDib);

	// Close the .BMP file.
	if (hf != INVALID_HANDLE_VALUE)
	{
		if (!CloseHandle(hf))
		{
			m_szLastErr = "Cannot close file";
			return FALSE;
		}
	}

	return bRes;
}

BOOL CBarChart::PromptForFile(CString& szPath, CString szFilterName, CString szFilter)
{
	CString szFltr;
	szFltr.Format(_T("%s|%s||"), szFilterName, szFilter);

	// If path did not set, ask for a path
	CFileDialog fileSDlg(
		FALSE,				// Not an open file dialog
		_T("bmp"),			// This is to save bitmap files
		NULL,				// No default file name
		OFN_ENABLESIZING |
		//OFN_FORCESHOWHIDDEN |
		OFN_LONGNAMES |
		OFN_OVERWRITEPROMPT |
		OFN_PATHMUSTEXIST,
		szFltr);

	if (fileSDlg.DoModal() != IDOK)
	{
		return FALSE;
	}

	szPath = fileSDlg.GetPathName();

	return TRUE;
}

void CBarChart::SetGridLines(int nHorLineCount, int nVerLineCount,
	BOOL bFixedSize, int nFixedSize)
{
	m_grid.SetLineCount(nVerLineCount, nHorLineCount, bFixedSize, nFixedSize);
}
///////////////////////////////////////////////////////////////////////
//	  CGDIGrid Class
//
//	 		Draws a grid behind the chart.
//	 		UNDONE: The grid class should act better. instead of
//	 		drawing horizontal lines from top, it should start
//	 		from the line under the bars. (Logical y = 0 axe)
//
///////////////////////////////////////////////////////////////////////
void CBarChart::CGDIGrid::Draw(CDC* pDC, CRect& rcBound)
{
	if (m_penGrid.GetSafeHandle()) {
		m_pPenOld = pDC->SelectObject(&m_penGrid);

		// |||
		int nGapSize = (m_nVLineCount == 0 ? m_nFixedSize :
			(rcBound.Width() - 2 * GRID_MARGIN_WIDTH) / m_nVLineCount);
		if (nGapSize <= 0)
		{
			nGapSize = 1;
		}
		for (i = rcBound.left + GRID_MARGIN_WIDTH; i < rcBound.right - 2 * GRID_MARGIN_WIDTH;
			i += nGapSize)
		{
			pDC->MoveTo(i, GRID_MARGIN_WIDTH);
			pDC->LineTo(i, rcBound.Height());
		}

		// ===
		nGapSize = (m_nHLineCount == 0 ? m_nFixedSize :
			(rcBound.Height() - GRID_MARGIN_WIDTH) / m_nHLineCount);
		if (nGapSize <= 0)
		{
			nGapSize = 1;
		}
		for (i = rcBound.bottom; i >= rcBound.top + GRID_MARGIN_WIDTH;
			i -= nGapSize)
		{
			pDC->MoveTo(GRID_MARGIN_WIDTH, i);
			pDC->LineTo(rcBound.Width() - 2 * GRID_MARGIN_WIDTH, i);
		}
	}

	// Cleanup
	pDC->SelectObject(m_pPenOld);
};

CBarChart::CGDIGrid::CGDIGrid()
{
	m_nVLineCount = 0;
	m_nHLineCount = 0;
	m_nFixedSize = GRID_DEFAULT_SIZE;
	m_pPenOld = NULL;
};

CBarChart::CGDIGrid::~CGDIGrid()
{
	if (m_penGrid.GetSafeHandle())
	{
		m_penGrid.DeleteObject();
	}
};

void CBarChart::CGDIGrid::SetLineCount(UINT nVLineCnt, UINT nHLineCnt, BOOL bFixedSize, UINT nFixedSize)
{
	if (bFixedSize)
	{
		if (nFixedSize == 0)
		{
			m_nFixedSize = GRID_DEFAULT_SIZE;
		}
		else
		{
			m_nFixedSize = nFixedSize;
		}
		m_nVLineCount = 0;
		m_nHLineCount = 0;
	}
	else
	{
		if (nVLineCnt != 0) m_nVLineCount = nVLineCnt;
		if (nHLineCnt != 0) m_nHLineCount = nHLineCnt;
	}
};

void CBarChart::CGDIGrid::SetColor(COLORREF color)
{
	// Reset grid pen
	if (m_penGrid.GetSafeHandle())
	{
		m_penGrid.DeleteObject();
	}
	m_penGrid.CreatePen(PS_SOLID, 1, color);
}

///////////////////////////////////////////////////////////////////////
// CChartBar Class
//
//	 		This class is responsible for for drawing bars and
//	 		performing required calculation for the job.
//
///////////////////////////////////////////////////////////////////////

CBarChart::CChartBar::CChartBar()
{
	m_nBarWidth = BAR_DEFAULT_WIDTH;
	m_nBarGap = BAR_DEFAULT_GAP;
	m_sBarPicSize = CSize(0, 0);
	m_sBarPicSize.cy = 0;
	m_colorChartBK = RGB(255, 255, 255);

	m_pFontOld = NULL;
	ZeroMemory(&m_lf, sizeof(m_lf));

	CreateLabelFont();
}

CBarChart::CChartBar::~CChartBar()
{
	m_chartData.RemoveAll();

	if (m_font.GetSafeHandle())
	{
		m_font.DeleteObject();
	}
}

void CBarChart::CChartBar::RemoveAll()
{
	m_chartData.RemoveAll();
	m_sBarPicSize.cx = 0;
	m_sBarPicSize.cy = 0;
}

void CBarChart::CChartBar::RemoveAt(int nIndex)
{
	m_chartData.RemoveAt(nIndex);
	m_sBarPicSize.cx = m_chartData.GetCount() * (m_nBarWidth + m_nBarGap);
}

void CBarChart::CChartBar::CreateLabelFont()
{
	if (m_font.GetSafeHandle())
	{
		m_font.DeleteObject();
	}

	strcpy_s((CW2A)m_lf.lfFaceName, sizeof("Tahoma"), "Tahoma");
	m_lf.lfHeight = -10;
	m_lf.lfWidth = 0;
	m_lf.lfEscapement = 0;
	m_lf.lfOrientation = 0;
	m_lf.lfItalic = 0;
	m_lf.lfUnderline = 0;
	m_lf.lfStrikeOut = 0;
	m_lf.lfWeight = 400;
	m_lf.lfCharSet = ARABIC_CHARSET;
	m_lf.lfOutPrecision = OUT_STROKE_PRECIS;
	m_lf.lfClipPrecision = 2;
	m_lf.lfQuality = 1;
	m_lf.lfPitchAndFamily = 2;

	// Create font
	m_font.CreateFontIndirect(&m_lf);
}

void CBarChart::CChartBar::AddBar(double udValue, CString szLabel, COLORREF color)
{
	m_chartData.Add(szLabel, udValue, color);
	m_sBarPicSize.cx = m_chartData.GetCount() * (m_nBarWidth + m_nBarGap);
}

void CBarChart::CChartBar::Draw(CDC* pDC, CRect& rcBound)
{
	m_sBarPicSize.cy = rcBound.Height() - GRID_FRAME_HEIGHT;

	// Draw all bars and labels one by one
	DrawBars(pDC, rcBound);
}

void CBarChart::CChartBar::SetBkColor(COLORREF color)
{
	m_colorChartBK = color;
}

int CBarChart::CChartBar::GetBarHeight()
{
	return m_sBarPicSize.cy;
}

int CBarChart::CChartBar::GetBarWidth()
{
	return m_sBarPicSize.cx;
}

void CBarChart::CChartBar::DrawBars(CDC* pDC, CRect& rcBound)
{
	double nHeight = 0;
	int r1, g1, b1;
	int r2, g2, b2;
	int r, g, b;
	COLORREF colorG, colorEnd, colorSmooth, colorBorder;
	int x = 0, y = 0;
	CString str;

	int cy = m_sBarPicSize.cy;
	//  If there is a title
	//	if ()
	//	{
	cy -= CHART_TITLE_HEIGHT;
	//	}

	if (m_bShowBarText)
	{
		cy -= BAR_TXT_PERCENT_HEIGHT;
	}
	if (m_bShowLabel)
	{
		cy -= BAR_TXT_LABEL_HEIGHT;

		pDC->FillSolidRect(rcBound.left,
			rcBound.bottom - BAR_TXT_LABEL_HEIGHT - CHART_TITLE_HEIGHT - 1,
			rcBound.right,
			rcBound.bottom - CHART_TITLE_HEIGHT, m_colorChartBK);
	}

	for (int i = 0; i < m_chartData.GetCount(); i++)
	{
		colorBorder = DifColor(m_chartData.GetColor(i), CHART_BORDER_DARKNESS);
		colorEnd = DifColor(m_chartData.GetColor(i), CHART_BORDER_DARKNESS - 14);
		colorSmooth = OrColor(colorBorder, m_colorChartBK, 40);	// A correction color to smooth top 2 rounded edges of the border

		nHeight = (m_chartData.GetValue(i) * cy) /
			(m_chartData.GetMaxValue());

		x = i * (m_nBarWidth + m_nBarGap);
		y = m_sBarPicSize.cy - (int)nHeight -
			(m_bShowLabel*BAR_TXT_LABEL_HEIGHT) - CHART_TITLE_HEIGHT;

		// Center bars
		m_rMargin = (double)(rcBound.Width() - m_sBarPicSize.cx) / 2.0;
		x += (int)m_rMargin;

		pDC->FillSolidRect(x, y,
			m_nBarWidth, (int)nHeight,
			colorBorder);

		// Draw a gradient, thanks to Nishant.S
		r1 = GetRValue(colorEnd);
		g1 = GetGValue(colorEnd);
		b1 = GetBValue(colorEnd);

		r2 = GetRValue(m_chartData.GetColor(i));
		g2 = GetGValue(m_chartData.GetColor(i));
		b2 = GetBValue(m_chartData.GetColor(i));

		// Don't corrupt border by over painting the gradient
		nHeight -= 2;

		// Draw gradient
		for (int j = 1; j <= m_nBarWidth - 2; j++)
		{
			r = r1 + (j * (r2 - r1) / m_nBarWidth);
			g = g1 + (j * (g2 - g1) / m_nBarWidth);
			b = b1 + (j * (b2 - b1) / m_nBarWidth);

			if (j < m_nBarWidth / 2) {
				colorG = OrColor(RGB(r, g, b), RGB(255, 255, 255), (BAR_REFLECT_AMOUNT + 5 * j) > 100 ?
					100 : (BAR_REFLECT_AMOUNT + 5 * j));
			}
			else {
				colorG = RGB(r, g, b);
			}

			pDC->FillSolidRect(j + x, y + 1, 1, (int)nHeight, colorG);
		}

		// Return borders to usual
		nHeight += 2;

		// Do some Corrections
		pDC->SetPixel(x, y, m_colorChartBK);
		pDC->SetPixel(x + m_nBarWidth - 1, y, m_colorChartBK);

		// Smooth these pixels
		pDC->SetPixel(x + 1, y, colorSmooth);
		pDC->SetPixel(x, y + 1, colorSmooth);

		pDC->SetPixel(x + m_nBarWidth - 2, y, colorSmooth);
		pDC->SetPixel(x + m_nBarWidth - 1, y + 1, colorSmooth);

		// Repair these pixels
		pDC->SetPixel(x + m_nBarWidth - 2, y + 1, colorBorder);
		pDC->SetPixel(x + 1, y + 1, colorBorder);

		// Add a Percentage/Value at the top of each bar
		if (m_bShowBarText || m_bShowLabel)
		{
			pDC->SetBkMode(TRANSPARENT);
			m_pFontOld = (CFont*)pDC->SelectObject(&m_font);

			if (m_bShowBarText)
			{
				// If we shall display percentage
				if (m_nBarTextType == 0)
				{
					str.Format(L"%0.01f%%", (m_chartData.GetValue(i) * 100) / m_chartData.GetMaxValue());
					pDC->DrawText(str, CRect(x, y - BAR_TXT_PERCENT_HEIGHT, x + m_nBarWidth + m_nBarGap - 1, y),
						DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				}
				else if (m_nBarTextType == 1)		// Draw value at the top of the bar
				{
					str.Format(L"%0.02f", m_chartData.GetValue(i));
					pDC->DrawText(str, CRect(x, y - BAR_TXT_PERCENT_HEIGHT, x + m_nBarWidth + m_nBarGap - 1, y),
						DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				}
			}

			if (m_bShowLabel)
			{
				str = m_chartData.GetLabel(i);
				pDC->DrawText(str, CRect(x, y + (int)nHeight, x + m_nBarWidth + m_nBarGap - 1, y + (int)nHeight + BAR_TXT_LABEL_HEIGHT),
					DT_LEFT | DT_SINGLELINE | DT_VCENTER);

				// If displaying a % at the top of each bar
			/*	if (m_nBarTextType==0)
				{
					str.Format("%0.02f", m_chartData.GetValue(i));
					pDC->DrawText(str, CRect(0, y - BAR_TXT_PERCENT_HEIGHT, BAR_TXT_VLABEL_MARGIN, y),
						DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				}*/
			}

			pDC->SelectObject(m_pFontOld);
		}
	}
}

void CBarChart::CChartBar::ShowLabel(BOOL bShow)
{
	m_bShowLabel = bShow;
}

void CBarChart::CChartBar::ShowBarText(int nType, BOOL bShow)
{
	m_bShowBarText = bShow;
	m_nBarTextType = nType;
}

void CBarChart::CChartBar::DisplayToolTip(CPoint& point, CRect& rcBound,
	CRect& rcMemPic, BOOL bScale, CString& szTip)
{
	if (bScale)
	{
		m_dAlpha = (double)rcBound.Width() / (double)(m_sBarPicSize.cx + m_rMargin * 2);
	}
	else
	{
		m_dAlpha = (double)rcMemPic.Width() / (double)(m_sBarPicSize.cx + m_rMargin * 2);
	}
	m_dIndex = (int)((double)(point.x - m_rMargin * m_dAlpha) /
		(double)((m_nBarWidth*m_dAlpha) + (m_nBarGap*m_dAlpha)));

	if (m_dIndex < 0.0 || (int)m_dIndex >= m_chartData.GetCount())
	{
		szTip = "";
	}
	else
	{
		szTip.Format(L"%s\n%0.04f\n%0.02f%%\n",
			m_chartData.GetLabel((int)m_dIndex), m_chartData.GetValue((int)m_dIndex),
			(m_chartData.GetValue((int)m_dIndex) * 100) / m_chartData.GetMaxValue());
	}
}

BOOL CBarChart::CChartBar::IsLabelVisible()
{
	return m_bShowLabel;
}
///////////////////////////////////////////////////////////////////////
// CChartGCalc Class
//
//
//			A set of handy GDI functions for manipulating
//			pixel colors, etc.
//
///////////////////////////////////////////////////////////////////////

UINT CChartGCalc::GABS(const int& nmbr)
{
	return (nmbr > 0) ? nmbr : -nmbr;
}

COLORREF CChartGCalc::DifColor(COLORREF color, UINT nDist)
{
	return RGB(
		GABS(GetRValue(color) - nDist),
		GABS(GetGValue(color) - nDist),
		GABS(GetBValue(color) - nDist)
	);
}

COLORREF CChartGCalc::OrColor(COLORREF colFore, COLORREF colBK, UINT nForePercent)
{
	return RGB(
		(GetRValue(colBK) * 100 + (nForePercent*(GetRValue(colFore) - GetRValue(colBK)))) / 100,
		(GetGValue(colBK) * 100 + (nForePercent*(GetGValue(colFore) - GetGValue(colBK)))) / 100,
		(GetBValue(colBK) * 100 + (nForePercent*(GetBValue(colFore) - GetBValue(colBK)))) / 100);
}

// Refer to the following article in 'CodeProject.com' By 'Roger Allen'
// 'Printing tips and tricks from the trenches'
// And goto topic: 'Use DIB's instead of DDB's'

// DDBToDIB        - Creates a DIB from a DDB
// bitmap        - Device dependent bitmap
// dwCompression    - Type of compression - see BITMAPINFOHEADER
// pPal            - Logical palette
HANDLE CChartGCalc::DDBToDIB(CBitmap* bitmap, DWORD dwCompression, CPalette* pPal)
{
	BITMAP            bm;
	BITMAPINFOHEADER    bi;
	LPBITMAPINFOHEADER     lpbi;
	DWORD            dwLen;
	HANDLE            hDIB;
	HANDLE            handle;
	HDC             hDC;
	HPALETTE        hPal;

	ASSERT(bitmap->GetSafeHandle());

	// The function has no arg for bitfields
	if (dwCompression == BI_BITFIELDS)
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE)pPal->GetSafeHandle();
	if (hPal == NULL)
		hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap->GetObject(sizeof(bm), (LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = (unsigned short)(bm.bmPlanes * bm.bmBitsPixel);
	bi.biCompression = dwCompression;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// Compute the size of the  infoheader and the color table
	int nColors = 0;
	if (bi.biBitCount <= 8)
	{
		nColors = (1 << bi.biBitCount);
	}
	dwLen = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = ::GetDC(NULL);
	hPal = SelectPalette(hDC, hPal, FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED, dwLen);

	if (!hDIB) {
		SelectPalette(hDC, hPal, FALSE);
		::ReleaseDC(NULL, hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver
	// will calculate the biSizeImage field
	GetDIBits(hDC, (HBITMAP)bitmap->GetSafeHandle(), 0L, (DWORD)bi.biHeight,
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0) {
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8)
			* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE);
	if (handle != NULL)
		hDIB = handle;
	else
	{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC, hPal, FALSE);
		::ReleaseDC(NULL, hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits(hDC, (HBITMAP)bitmap->GetSafeHandle(),
		0L,                      // Start scan line
		(DWORD)bi.biHeight,      // # of scan lines
		(LPBYTE)lpbi             // address for bitmap bits
		+ (bi.biSize + nColors * sizeof(RGBQUAD)),
		(LPBITMAPINFO)lpbi,      // address of bitmapinfo
		(DWORD)DIB_RGB_COLORS);  // Use RGB for color table

	if (!bGotBits)
	{
		GlobalFree(hDIB);

		SelectPalette(hDC, hPal, FALSE);
		::ReleaseDC(NULL, hDC);
		return NULL;
	}

	SelectPalette(hDC, hPal, FALSE);
	::ReleaseDC(NULL, hDC);
	return hDIB;
}

///////////////////////////////////////////////////////////////////////
// CChartData Class
//
//		Holds data of a chart and let the chart to do
//			reqauered operations on it
//
///////////////////////////////////////////////////////////////////////
CBarChart::CChartData::CChartData()
{
	m_udMax = 0;
}

void CBarChart::CChartData::CalculateMaximum()
{
	for (int i = 0; i < m_arr.GetSize(); i++)
	{
		if (m_udMax < ((data*)m_arr.GetAt(i))->m_udValue)
		{
			m_udMax = ((data*)m_arr.GetAt(i))->m_udValue;
		}
	}
}

void CBarChart::CChartData::RemoveAll()
{
	data* pData;
	for (int i = 0; i < m_arr.GetSize(); i++) {
		if ((pData = (data*)m_arr.GetAt(i)) != NULL) {
			delete pData;
		}
	}

	m_arr.RemoveAll();
	m_udMax = 0;
}

void CBarChart::CChartData::RemoveAt(int nIndex)
{
	if (m_arr.GetSize() == 0)
		return;

	if (nIndex >= m_arr.GetSize() || nIndex < 0)
	{
		return;
	}

	delete (data*)m_arr.GetAt(nIndex);
	m_arr.RemoveAt(nIndex);

	CalculateMaximum();
}

int CBarChart::CChartData::GetCount()
{
	return (int)m_arr.GetSize();
}

void CBarChart::CChartData::Add(CString& szLabel, double udValue, COLORREF color)
{
	data *pData;

	pData = new data;
	pData->m_szLabel = szLabel;
	pData->m_udValue = udValue;
	pData->m_color = color;

	m_arr.Add((CObject*)pData);

	if (m_udMax < udValue)
	{
		m_udMax = udValue;
	}
}

double CBarChart::CChartData::GetMaxValue()
{
	if (m_udMax != 0)
	{
		return m_udMax;
	}
	else
	{
		// Eliminate probability of division by zero
		return 1;
	}
}

double CBarChart::CChartData::GetValue(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_arr.GetSize())
	{
		return -1;
	}

	return ((data*)m_arr.GetAt(nIndex))->m_udValue;
}

CString CBarChart::CChartData::GetLabel(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_arr.GetSize())
	{
		return L"";
	}
	return ((data*)m_arr.GetAt(nIndex))->m_szLabel;
}

COLORREF CBarChart::CChartData::GetColor(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_arr.GetSize())
	{
		return RGB(0, 0, 0);
	}

	return ((data*)m_arr.GetAt(nIndex))->m_color;
}
///////////////////////////////////////////////////////////////////////
// CChartPrinter Class
//
//			No need to description(!) This class should print the
//			Chart using print dialog.
//
///////////////////////////////////////////////////////////////////////

// Many of the following code, had been stolen shamelessly from ChrisMaunder's Best work: MFCGrid
// Find it in 'www.codeproject.com'
void CBarChart::CChartPrinter::Print(CDC* pDC, CDC *pMemDC, CBitmap* pBmp,
	CString szTitle, CRect& rcMemPic, BOOL bFit,
	BOOL bCompatible)
{
	CDC dc;

	CPrintDialog printDlg(FALSE);
	if (printDlg.DoModal() != IDOK)             // Get printer settings from user
		return;

	dc.Attach(printDlg.GetPrinterDC());         // attach a printer DC

	m_rcMemPic = rcMemPic;
	m_pMemDC = pMemDC;
	m_pBmpMem = pBmp;
	m_bScale = bFit;

	DOCINFO di;                                 // Initialise print doc details
	memset(&di, 0, sizeof(DOCINFO));
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = szTitle;

	BOOL bPrintingOK = dc.StartDoc(&di);        // Begin a new print job
	CPrintInfo info;
	info.m_rectDraw.SetRect(0, 0, dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));

	dc.StartPage();                         // begin new page

	if (bCompatible)
	{
		OnPrintCompatible(&dc, &info);
	}
	else
	{
		OnPrint(&dc, &info);
	}

	bPrintingOK = (dc.EndPage() > 0);       // end page

	if (bPrintingOK)
		dc.EndDoc();                            // end a print job
	else
		dc.AbortDoc();                          // abort job.

	dc.Detach();                                // detach the printer DC
}

void CBarChart::CChartPrinter::OnPrint(CDC *pDC, CPrintInfo* pInfo)
{
	// Do some calculations
	pDC->SetMapMode(MM_ANISOTROPIC);

	pInfo->m_nCurPage = 1;
	pInfo->SetMaxPage(1);
	//	UNDONE : SET TRUE MAPPING MODE, AND CALCULATIONS, IN CASE OF NON-SCALED CHART PRINTING
	//	if (m_bScale)
	//	{
	pDC->StretchBlt(pInfo->m_rectDraw.left,
		pInfo->m_rectDraw.top, pInfo->m_rectDraw.Width(), pInfo->m_rectDraw.Height(),
		m_pMemDC, 0, 0, m_rcMemPic.Width(), m_rcMemPic.Height(),
		SRCCOPY);
	//	}
	//	else
	//	{
	//		pDC->BitBlt(pInfo->m_rectDraw.left,
	//			pInfo->m_rectDraw.top, pInfo->m_rectDraw.Width(), pInfo->m_rectDraw.Height(),
	//			m_pMemDC, 0, 0, SRCCOPY);
	//	}
}
// Refered to the following article in 'CodeProject.com' By 'Roger Allen'
// 'Printing tips and tricks from the trenches'
// And in topic: 'Use DIB's instead of DDB's'
// Print function might not work is a DDB blotting is done
void CBarChart::CChartPrinter::OnPrintCompatible(CDC *pDC, CPrintInfo* pInfo)
{
	// Do some calculations
	pDC->SetMapMode(MM_ANISOTROPIC);

	pInfo->m_nCurPage = 1;
	pInfo->SetMaxPage(1);

	CPalette    pal;
	UINT        nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
	LOGPALETTE* pLP = (LOGPALETTE*)new BYTE[nSize];
	pLP->palVersion = 0x300;
	pLP->palNumEntries = (unsigned short)GetSystemPaletteEntries(m_pMemDC->GetSafeHdc(), 0, 255,
		pLP->palPalEntry);

	// Create the palette
	pal.CreatePalette(pLP);

	// Free memory
	delete[] pLP;

	// Convert the bitmap to a DIB
	HANDLE hDib = DDBToDIB(m_pBmpMem, BI_RGB, &pal);

	BITMAPINFOHEADER    *pBMI;
	pBMI = (BITMAPINFOHEADER*)GlobalLock(hDib);
	int nColors = 0;
	if (pBMI->biBitCount <= 8)
	{
		nColors = (1 << pBMI->biBitCount);
	}
	// print the correct image
	::StretchDIBits(pDC->m_hDC,
		pInfo->m_rectDraw.left,
		pInfo->m_rectDraw.top,
		pInfo->m_rectDraw.Width(),
		pInfo->m_rectDraw.Height(),
		0,
		0,
		pBMI->biWidth,
		pBMI->biHeight,
		(LPBYTE)pBMI + (pBMI->biSize + nColors * sizeof(RGBQUAD)),
		(BITMAPINFO*)pBMI,
		DIB_RGB_COLORS,
		SRCCOPY);
	// free resources
	GlobalUnlock(hDib);
	GlobalFree(hDib);
}

///////////////////////////////////////////////////////////////////////
// CChartTip Class
//
//
//				Adds tooltip capabilities for bar chart.
//				Tooltip should show: value, Label and percentage
//
///////////////////////////////////////////////////////////////////////

CChartTip::CChartTip()
{
	if (m_lpszClass == NULL) m_lpszClass = AfxRegisterWndClass(CS_SAVEBITS);
	if (m_lpszClass == NULL) m_lpszClass = AfxRegisterWndClass(CS_SAVEBITS);
}

CChartTip::~CChartTip()
{
	if (m_brShadow.GetSafeHandle())
	{
		m_brShadow.DeleteObject();
	}

	if (m_brFrame.GetSafeHandle())
	{
		m_brFrame.DeleteObject();
	}

	if (m_brBK.GetSafeHandle())
	{
		m_brBK.DeleteObject();
	}

	if (m_fontBold.GetSafeHandle())
	{
		m_fontBold.DeleteObject();
	}

	if (m_fontNorm.GetSafeHandle())
	{
		m_fontNorm.DeleteObject();
	}

	// Shall not do delete m_rgnTip
	// System will delete this rgn whenever doesnot need it any more

	if (m_rgnCopy.GetSafeHandle())
	{
		m_rgnCopy.DeleteObject();
	}

	if (m_bTimer)
	{
		if (m_hWnd != NULL)
		{
			m_bTimer = FALSE;
			KillTimer(TIP_DELAY_TIMER);
		}
	}

	if (m_hWnd)
	{
		if (IsWindowVisible())
		{
			HideWindow();
		}
	}

	if (m_hWnd != NULL) DestroyWindow();
}

BEGIN_MESSAGE_MAP(CChartTip, CWnd)
	//{{AFX_MSG_MAP(CChartTip)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChartTip message handlers
LPCTSTR CChartTip::m_lpszClass = NULL;

BOOL CChartTip::Create(CWnd *pParentWnd, BOOL *pbEnable, CString* lpszTitle, int nDelay)
{
	m_rcClient = CRect(0, 0, 0, 0);

	DWORD dwStyleEx = WS_EX_TOPMOST;
	if (!CWnd::CreateEx(dwStyleEx, m_lpszClass, NULL, WS_POPUP,
		m_rcClient, pParentWnd, 0, NULL)) return FALSE;

	SetOwner(pParentWnd);

	m_pbEnable = pbEnable;
	m_lpszTitle = lpszTitle;
	m_nDelay = nDelay;
	m_bTimer = FALSE;

	m_rcClient = CRect(0, 0, 120, 100);

	// create a brush
	m_brFrame.CreateSolidBrush(TIP_FRAME_COLOR);
	m_brShadow.CreateSolidBrush(TIP_SHADOW_COLOR);
	m_brBK.CreateSolidBrush(TIP_BK_COLOR);

	// create fonts
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(lf));

	strcpy_s((CW2A)lf.lfFaceName, sizeof("Tahoma"), "Tahoma");
	lf.lfHeight = -20;
	lf.lfWeight = 700;
	lf.lfCharSet = ARABIC_CHARSET;
	lf.lfOutPrecision = OUT_STROKE_PRECIS;
	lf.lfClipPrecision = 2;
	lf.lfQuality = 1;
	lf.lfPitchAndFamily = 2;

	m_fontBold.CreateFontIndirect(&lf);

	lf.lfWeight = 400;
	m_fontNorm.CreateFontIndirect(&lf);

	if (m_rgnTip.CreateRoundRectRgn(m_rcClient.left,
		m_rcClient.top, m_rcClient.right, m_rcClient.bottom,
		TIP_RGN_ROUND_CORNERS, TIP_RGN_ROUND_CORNERS))
	{
		// Window region
		SetWindowRgn((HRGN)m_rgnTip.GetSafeHandle(), FALSE);
	};

	// Region for drawing bounding frame
	m_rgnCopy.CreateRoundRectRgn(m_rcClient.left,
		m_rcClient.top, m_rcClient.right, m_rcClient.bottom,
		TIP_RGN_ROUND_CORNERS, TIP_RGN_ROUND_CORNERS);
	/*
		m_rgnShadow.CreateRoundRectRgn(m_rcClient.left +2,
			m_rcClient.top +2, m_rcClient.right , m_rcClient.bottom ,
			TIP_RGN_ROUND_CORNERS, TIP_RGN_ROUND_CORNERS);
	*/
	return TRUE;
}

void CChartTip::Pop()
{
	if (!m_hWnd)
	{
		return;
	}
	if (!*m_pbEnable)
	{
		return;
	}

	if (IsWindowVisible() && m_szOldTitle == *m_lpszTitle)
	{
		/*		m_pointOld = point;
				SetWindowPos( 0,
					point.x+TIP_RIGHT_OFFSET, point.y+TIP_TOP_OFFSET, 0, 0,
					SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOSIZE );*/
		return;
	}
	else
	{
		m_szOldTitle = *m_lpszTitle;
	}

	CPoint point;
	GetCursorPos(&point);

	if (m_pointOld == point)
	{
		return;
	}

	if (!IsWindowVisible() ||
		(IsWindowVisible() && m_pointOld != point))
	{
		m_pointOld = point;
		m_bTimer = FALSE;
		KillTimer(TIP_DELAY_TIMER);

		CRect rc(m_pointOld.x + TIP_RIGHT_OFFSET, m_pointOld.y + TIP_TOP_OFFSET,
			m_pointOld.x + TIP_RIGHT_OFFSET + m_rcClient.right,
			m_pointOld.y + TIP_TOP_OFFSET + m_rcClient.bottom);

		CRect rcMonitor(0, 0, 0, 0);
		rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
		rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);

		if (rc.right >= rcMonitor.right)
		{
			rc.OffsetRect(rcMonitor.right - rc.right - 4, 0);
		}

		if (rc.bottom >= rcMonitor.bottom)
		{
			rc.OffsetRect(0, (rcMonitor.bottom - rc.bottom));
		}
		SetWindowPos(&wndTopMost,
			rc.left, rc.top, rc.Width(), rc.Height(),
			SWP_SHOWWINDOW | SWP_NOACTIVATE);

		Invalidate(TRUE);
		UpdateWindow();

		SetTimer(TIP_DELAY_TIMER, m_nDelay, NULL);
		m_bTimer = TRUE;
		return;
	}
}

void CChartTip::OnTimer(UINT_PTR nIDEvent)
{
	CWnd::OnTimer(nIDEvent);

	if (nIDEvent == TIP_DELAY_TIMER)
	{
		m_bTimer = FALSE;
		KillTimer(TIP_DELAY_TIMER);
		HideWindow();
	}
}

void CChartTip::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	HideWindow();
}

void CChartTip::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);

	if (!PtInRect(&m_rcClient, point))
	{
		return;
	}

	GetCursorPos(&point);
	m_pointOld = point;

	CRect rcMonitor(0, 0, 0, 0);
	rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
	rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);

	CRect rc(m_pointOld.x + TIP_RIGHT_OFFSET, m_pointOld.y + TIP_TOP_OFFSET,
		m_pointOld.x + TIP_RIGHT_OFFSET + m_rcClient.right,
		m_pointOld.y + TIP_TOP_OFFSET + m_rcClient.bottom);

	if (rc.right >= rcMonitor.right)
	{
		rc.OffsetRect(rcMonitor.right - rc.right - 4, 0);
	}

	if (rc.bottom >= rcMonitor.bottom)
	{
		rc.OffsetRect(0, (rcMonitor.bottom - rc.bottom));
	}

	SetWindowPos(0, rc.left, rc.top, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	//HideWindow();
}

BOOL CChartTip::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CChartTip::OnPaint()
{
	if (!IsWindow(GetSafeHwnd()) || !IsWindowVisible()) return;

	CPaintDC dc(this); // device context for painting

	// Draw tip
	dc.FillRgn(&m_rgnCopy, &m_brBK);
	dc.FrameRgn(&m_rgnCopy, &m_brFrame, TIP_FRAME_THICKNESS, TIP_FRAME_THICKNESS);

	CFont* pOldFont;

	int nIndex = 0, nLastIndex = 0;
	nIndex = m_lpszTitle->Find(L"\n", 0);

	dc.SetBkMode(TRANSPARENT);
	if (nIndex != -1)
	{
		nLastIndex = nIndex;
		if (m_szText != m_lpszTitle->Left(nIndex))
		{
			m_szText = m_lpszTitle->Left(nIndex);
		}

		pOldFont = (CFont*)dc.SelectObject(&m_fontBold);

		dc.SetTextColor(TIP_TITLE_COLOR);
		// Draw label
		dc.DrawText(m_szText,
			CRect(m_rcClient.left + TIP_LEFT_MARGIN,
				m_rcClient.top + TIP_TOP_MARGIN,
				m_rcClient.right - TIP_LEFT_MARGIN,
				m_rcClient.top + TIP_TEXT_MAX_HEIGHT),

			DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		dc.SelectObject(pOldFont);
	}

	// Draw rest
	pOldFont = (CFont*)dc.SelectObject(&m_fontNorm);
	int i = 1;
	dc.SetTextColor(TIP_TEXT_COLOR);
	while (1)
	{
		if (nIndex + 1 >= m_lpszTitle->GetLength()) break;
		nIndex = m_lpszTitle->Find(L"\n", nLastIndex + 1);
		if (nIndex != -1)
		{
			m_szText = m_lpszTitle->Mid(nLastIndex + 1, nIndex - nLastIndex - 1);
			nLastIndex = nIndex;
		}
		else
		{
			break;
		}

		dc.DrawText(m_szText,
			CRect(m_rcClient.left + TIP_LEFT_MARGIN,
				m_rcClient.top + TIP_TOP_MARGIN + i * TIP_TEXT_MAX_HEIGHT + 2,
				m_rcClient.right - TIP_LEFT_MARGIN,
				m_rcClient.top + i * TIP_TEXT_MAX_HEIGHT + 50),

			DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		i++;
		dc.SetTextColor(TIP_PERCENT_COLOR);
	}

	dc.SelectObject(pOldFont);
}

void CChartTip::HideWindow()
{
	GetCursorPos(&m_pointOld);
	if (m_bTimer)
	{
		m_bTimer = FALSE;
		KillTimer(TIP_DELAY_TIMER);
	}

	ShowWindow(SW_HIDE);
}

int CChartTip::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bTimer = FALSE;

	return 0;
}

void CChartTip::Hide()
{
	HideWindow();
}

///////////////////////////////////////////////////////////////////////
// CChartDatabase Class
//
//			This class gives the chart the ability to connect to an
//			ODBC data source. Then it is able to read data of a table
//			or use results of a stored procedure.
//			The class uses ODBC Version 3.
//
///////////////////////////////////////////////////////////////////////
CBarChart::CChartDatabase::CChartDatabase()
{
	// Some initializations...
	m_hDbConn = SQL_NULL_HANDLE;
	m_hOdbcEnv = SQL_NULL_HANDLE;
	m_hstmt = SQL_NULL_HANDLE;

	memset(&m_row, NULL, sizeof(m_row));

	m_sqlRet = SQL_SUCCESS;

	m_bEOF = FALSE;
	m_szError = _T("");
}

CBarChart::CChartDatabase::~CChartDatabase()
{
	Close();
}
/*****************************************************************************************
 * Opens requested table or executes procedure.
 * If this is a call to a procedure, then 'szTable' is name of the procedure
 * And 'szFilter' parameter will be used as input parameter(s) of the procedure
 *
 * CAUTION : If you are using this to call a stored procedure, then LabelFieldName and
 * ValueFieldName will be ignored. The Stored procedure is considered to have at least 2
 * parameters. The fist parameter will be considered as Label and the second one as value.
 *****************************************************************************************/
BOOL CBarChart::CChartDatabase::OpenTable(CString szDSN, CString szTable,
	CString szLabelFieldName, CString szValueFieldName,
	CString szFilter,
	CString szUsername, CString szPass,
	BOOL bStoredProc, ULONG ulODBCVer)
{
	// Already opened, can't call open twice
	ASSERT(!m_hOdbcEnv);
	if (m_hOdbcEnv != SQL_NULL_HANDLE)
	{
		return FALSE;
	}

	// Lest prepare environment, etc.
	if (!Prepare(ulODBCVer))
	{
		ASSERT(FALSE);
		return FALSE;
	};

	// Connect to ODBC
	if (!Connect(szDSN, szUsername, szPass))
	{
		ASSERT(FALSE);
		return FALSE;
	};

	// We are not at the end of records
	m_bEOF = FALSE;

	// Execute query or run a stored procedure
	if (bStoredProc == FALSE)
	{
		if (!ExecuteQuery(szTable, szFilter, szLabelFieldName, szValueFieldName))
		{
			ASSERT(FALSE);
			return FALSE;
		}
		else
		{
			// Fetch first row, ready for GetRow
			MoveNext();
		}
	}
	else
	{
		if (!ExecuteProc(szTable, szFilter, szLabelFieldName, szValueFieldName))
		{
			ASSERT(FALSE);
			return FALSE;
		}
		else
		{
			// Fetch first row, ready for GetRow
			MoveNext();
		}
	}

	return TRUE;
}

BOOL CBarChart::CChartDatabase::OpenProc(CString szDSN, CString szStoredProc,
	CString szParameterList,
	CString szUsername, CString szPass,
	ULONG ulODBCVer)
{
	// Already opened, can't call open twice
	ASSERT(!m_hOdbcEnv);
	if (m_hOdbcEnv != SQL_NULL_HANDLE)
	{
		return FALSE;
	}

	return
		OpenTable(szDSN, szStoredProc, L"", L"", szParameterList,
			szUsername, szPass, TRUE, ulODBCVer);
}

BOOL CBarChart::CChartDatabase::Prepare(ULONG ulODBCVer)
{
	// Allocate Environment
	m_sqlRet = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hOdbcEnv);
	if (m_sqlRet != SQL_SUCCESS && m_sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		m_szError = _T("Error in Allocating Environment.");
		return FALSE;
	}

	// Set the App's ODBC Version
	m_sqlRet = SQLSetEnvAttr(m_hOdbcEnv, SQL_ATTR_ODBC_VERSION,
		&ulODBCVer, SQL_IS_INTEGER);
	if (m_sqlRet != SQL_SUCCESS && m_sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		m_szError = _T("Error in Setting ODBC Version.");
		return FALSE;
	}

	// Allocate Connection
	m_sqlRet = SQLAllocHandle(SQL_HANDLE_DBC, m_hOdbcEnv, &m_hDbConn);
	if (m_sqlRet != SQL_SUCCESS && m_sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		m_szError = _T("Error in Allocating Connection.");
		return FALSE;
	}

	return TRUE;
}

BOOL CBarChart::CChartDatabase::Connect(CString szDSN, CString szUsername, CString szPass)
{
	// Set Connect Timeout
	m_sqlRet = SQLSetConnectAttr(m_hDbConn, SQL_ATTR_LOGIN_TIMEOUT, (void*)15, 0);
	if (m_sqlRet != SQL_SUCCESS && m_sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		m_szError = _T("Error in Setting Login Timeout.");
		return FALSE;
	}

	// Connect to Data Source
	//m_sqlRet = SQLConnect(m_hDbConn, (UCHAR *)(LPCSTR)szDSN, SQL_NTS,
	//	(UCHAR *)(LPCSTR)szUsername, SQL_NTS,
	//	(UCHAR *)(LPCSTR)szPass, SQL_NTS);
	m_sqlRet = SQLConnect(m_hDbConn, (WCHAR *)(LPCWSTR)szDSN, SQL_NTS,
		(WCHAR *)(LPCWSTR)szUsername, SQL_NTS,
		(WCHAR *)(LPCWSTR)szPass, SQL_NTS);

	if (m_sqlRet != SQL_SUCCESS && m_sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		m_szError = _T("Error in SQLConnect");
		return FALSE;
	}

	return TRUE;
}

BOOL CBarChart::CChartDatabase::ExecuteQuery(CString szTable, CString szFilter,
	CString szLabelFieldName, CString szValueFieldName)
{
	CString	szQuery;
	szQuery.Format(L"SELECT %s, %s FROM %s %s",
		szLabelFieldName, szValueFieldName, szTable, szFilter);

	szQuery.TrimLeft();
	szQuery.TrimRight();

	return ExecuteSQL(szQuery);
}

BOOL CBarChart::CChartDatabase::ExecuteProc(CString szTable, CString szFilter,
	CString szLabelFieldName, CString szValueFieldName)
{
	CString	szQuery;
	szQuery.Format(L"{call %s (%s)}",
		szTable, szFilter);

	return ExecuteSQL(szQuery);
}

BOOL CBarChart::CChartDatabase::ExecuteSQL(CString& szQuery)
{
	// Allocate Statement Handle
	m_sqlRet = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbConn, &m_hstmt);
	if (m_sqlRet != SQL_SUCCESS && m_sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		m_szError = _T("Error in allocating statement");
		return FALSE;
	}

	// Execute SQL statement
	m_sqlRet = SQLExecDirect(m_hstmt, (WCHAR*)(LPCWSTR)szQuery, SQL_NTS);
	if (m_sqlRet != SQL_SUCCESS && m_sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		m_szError = _T("Error executing statement");
		return FALSE;
	}

	// Bind each column
	m_sqlRet = SQLBindCol(m_hstmt, 1, SQL_C_CHAR,
		m_row.szLabel, sizeof(m_row.szLabel), (SQLLEN*)&m_row.nLabelLen);

	if (m_sqlRet != SQL_SUCCESS && m_sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		m_szError = _T("Error in Binding first column");
		return FALSE;
	}

	m_sqlRet = SQLBindCol(m_hstmt, 2, SQL_C_DOUBLE,
		&m_row.dValue, sizeof(m_row.dValue), (SQLLEN*)&m_row.nValueLen);

	if (m_sqlRet != SQL_SUCCESS && m_sqlRet != SQL_SUCCESS_WITH_INFO)
	{
		m_szError = _T("Error in Binding second column");
		return FALSE;
	}

	return TRUE;
}

BOOL CBarChart::CChartDatabase::IsEOF()
{
	return m_bEOF;
}

BOOL CBarChart::CChartDatabase::MoveNext()
{
	if (m_hstmt == NULL || m_bEOF == TRUE)
	{
		return FALSE;
	}

	if (SQLFetch(m_hstmt) != SQL_SUCCESS)
	{
		m_bEOF = TRUE;
		return FALSE;
	}

	return TRUE;
}

void CBarChart::CChartDatabase::GetRow(CString &szLabel, double &dValue)
{
	if (m_row.nLabelLen != 0)
	{
		szLabel = m_row.szLabel;
	}

	if (m_row.nValueLen != 0)
	{
		dValue = m_row.dValue;
	}
}

void CBarChart::CChartDatabase::Close()
{
	if (m_hstmt != SQL_NULL_HANDLE)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
	}

	if (m_hDbConn != SQL_NULL_HANDLE)
	{
		SQLDisconnect(m_hDbConn);

		SQLFreeHandle(SQL_HANDLE_DBC, m_hDbConn);
	}

	if (m_hOdbcEnv != SQL_NULL_HANDLE)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, m_hOdbcEnv);
	}
}

CString CBarChart::CChartDatabase::GetLastErrorMessage()
{
	return m_szError;
}