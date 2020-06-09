// CTableDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "DeVisionDlg.h"
#include "CTableDlg.h"
#include "afxdialogex.h"
#include "Log.h"

#include <afxdisp.h>
#include <algorithm>
#include <math.h>
#include <direct.h>
#include <io.h>
#include <fstream>


using namespace ATL;

// CTableDlg 对话框

IMPLEMENT_DYNAMIC(CTableDlg, CDialogEx)

CTableDlg::CTableDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TABLE, pParent)
{
	m_pvDFT = NULL;
	InitializeCriticalSection(&m_csvec);


	//初始化画笔
	m_pen[0].CreatePen(PS_SOLID, 3, RGB(0, 0, 0));           //黑色实线，1像素宽---参数：样式、宽度、颜色
	m_pen[1].CreatePen(PS_SOLID, 3, RGB(255, 35, 15));       //红色实线，6像素宽
	m_pen[2].CreatePen(PS_SOLID, 3, RGB(25, 255, 35));       //绿色虚线，必须为一个像素宽
	m_pen[3].CreatePen(PS_SOLID, 3, RGB(35, 55, 225));       //蓝色点线，必须为一个像素宽
	m_pen[4].CreatePen(PS_SOLID, 3, RGB(255, 255, 0));       //黄色点线，必须为一个像素宽
	m_pen[5].CreatePen(PS_SOLID, 3, RGB(255, 0, 255));       //黄色点线，必须为一个像素宽
	m_pen[6].CreatePen(PS_DOT, 1, RGB(88, 88, 88));          //灰色虚线，必须为一个像素宽
}

CTableDlg::~CTableDlg()
{
	DeleteCriticalSection(&m_csvec);

}

void CTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_HISTORY_REPORT, m_ListCtrlHis);
	DDX_Control(pDX, IDC_LIST_DETAIL, m_ListCtrlDetail);
	DDX_Control(pDX, IDC_COMBO_SEARCHOPTION, m_comSearchOption);
	DDX_Control(pDX, IDC_EDIT_SEARCHWORDS, m_editSearchWord);
	DDX_Control(pDX, IDC_BUTTON_OPENINPROGRAM, m_open_inprogram);
}


BEGIN_MESSAGE_MAP(CTableDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_REFRUSH, &CTableDlg::OnBnClickedBtnRefrush)
	ON_WM_PAINT()
	ON_WM_PAINTCLIPBOARD()
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CTableDlg::OnBnClickedButtonSearch)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_SHOWALL, &CTableDlg::OnBnClickedButtonShowall)
	ON_BN_CLICKED(IDC_BUTTON_DELSELECT, &CTableDlg::OnBnClickedButtonDelselect)
	ON_BN_CLICKED(IDC_BUTTON_DELALL, &CTableDlg::OnBnClickedButtonDelall)
	ON_BN_CLICKED(IDC_BUTTON_PRINT, &CTableDlg::OnBnClickedButtonPrint)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, &CTableDlg::OnBnClickedButtonSaveas)
	ON_BN_CLICKED(IDC_BUTTON_OPENEXCEL, &CTableDlg::OnBnClickedButtonOpenexcel)
	ON_BN_CLICKED(IDC_BUTTON_OPENINPROGRAM, &CTableDlg::OnBnClickedButtonOpeninprogram)
	ON_BN_CLICKED(IDC_BUTTON_OPENEXCELPATH, &CTableDlg::OnBnClickedButtonOpenexcelpath)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DETAIL, &CTableDlg::OnNMDblclkListDetail)
END_MESSAGE_MAP()


// CTableDlg 消息处理程序
BOOL CTableDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	m_font.CreatePointFont(80, _T("Times New Roman"));
	m_current_position = 0.0f;

	InitialHistoryList();

	InitialDetailList();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CTableDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_vecDFT.clear();
}

void CTableDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	CWnd *pwnd = GetDlgItem(IDC_STATIC_REPORT);
	CBrush brush(red_color);

	OnPaintClipboard(pwnd, brush);
}

void CTableDlg::OnPaintClipboard(CWnd* pClipAppWnd, HGLOBAL hPaintStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	pClipAppWnd->GetClientRect(&rect);
	CDC *dc = pClipAppWnd->GetDC();
	int wnd_width = rect.Width();
	int wnd_height = rect.Height();
	scale_x = wnd_width / (IMAGE_WIDTH * 4 * HORIZON_PRECISION);
	if (m_current_position != 0)
		scale_y = wnd_height / m_current_position;
	else
		scale_y = 1;

	//dc->SelectObject(&m_font);
	//FillRect(*dc, &rect, CBrush(RGB(255, 255, 255)));     //填充白色
	//DrawTable(dc, rect, 1650.0f, 10.0f);
	//DrawAllFlag(dc, wnd_width, wnd_height);
	//DrawSelectDFT(dc, m_selected_x, m_selected_y);

	CBrush brush(RGB(255, 255, 255));
	// 定义一个内存显示设备对象
	CDC MemDC; 
	// 定义一个位图对象
	CBitmap MemBitmap; 
	//建立与屏幕显示兼容的内存显示设备
	MemDC.CreateCompatibleDC(dc);
	//建立一个与屏幕显示兼容的位图，位图的大小可选用窗口客户区的大小
	MemBitmap.CreateCompatibleBitmap(dc, wnd_width, wnd_height);
	//将位图选入到内存显示设备中，只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
	MemDC.SelectObject(&MemBitmap);
	//CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);

	//绘图部分
	MemDC.SelectObject(&m_font);
	MemDC.FillRect(&rect, &brush);     //填充白色

	DrawTable(&MemDC, rect, std::stof(m_wstr_width), 10.0f);
	DrawAllFlag(&MemDC, wnd_width, wnd_height);
	DrawSelectDFT(&MemDC, m_selected_x, m_selected_y);
	
	//将内存中的图拷贝到屏幕上进行显示
	dc->BitBlt(0, 0, wnd_width, wnd_height, &MemDC, 0, 0, SRCCOPY);
	//绘图完成后的清理
	//MemDC.SelectObject(pOldBit);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();

	//保存图像， 窗口必须切换至 tab3
	SaveDistributeImage();
	CDialogEx::OnPaintClipboard(pClipAppWnd, hPaintStruct);
}

BOOL CTableDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//return CDialogEx::OnEraseBkgnd(pDC);
	return TRUE;
}

void CTableDlg::RefrushDistributeWnd()
{
	CWaitCursor wait;

	//清除列表
	m_ListCtrlDetail.DeleteAllItems();

	Invalidate();
	UpdateWindow();
}

void CTableDlg::GetDetectResult(int rank0, int rank1, int rank2, int rank3, int rank4)
{
	m_DFT_rank[0] = rank0;
	m_DFT_rank[1] = rank1;
	m_DFT_rank[2] = rank2;
	m_DFT_rank[3] = rank3;
	m_DFT_rank[4] = rank4;

}

void CTableDlg::InitialHistoryList()
{
	LONG styles;
	styles = GetWindowLong(m_ListCtrlHis.m_hWnd, GWL_STYLE);//获取窗口风格
	SetWindowLong(m_ListCtrlHis.m_hWnd, GWL_STYLE, styles | LVS_REPORT);
	DWORD dwStyle = m_ListCtrlHis.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;  //选中某行使整行高亮（只适用与report风格）
	dwStyle |= LVS_EX_GRIDLINES;      //行与行之间有分割线
	//dwStyle = LVS_NOCOLUMNHEADER;      //隐藏表头
	m_ListCtrlHis.SetExtendedStyle(dwStyle);

	m_ListCtrlHis.InsertColumn(
		0,              // Rank/order of item 
		L"时间",          // Caption for this header 
		LVCFMT_CENTER,    // Relative position of items under header 
		120);           // Width of items under header

	m_ListCtrlHis.InsertColumn(1, L"批号", LVCFMT_CENTER, 120);
	m_ListCtrlHis.InsertColumn(2, L"型号", LVCFMT_CENTER, 120);
	m_ListCtrlHis.InsertColumn(3, L"长度", LVCFMT_CENTER, 80);
	m_ListCtrlHis.InsertColumn(4, L"操作员", LVCFMT_CENTER, 80);
}

void CTableDlg::InitialDetailList()
{
	LONG styles;
	styles = GetWindowLong(m_ListCtrlDetail.m_hWnd, GWL_STYLE);//获取窗口风格
	SetWindowLong(m_ListCtrlDetail.m_hWnd, GWL_STYLE, styles | LVS_REPORT);
	DWORD dwStyle = m_ListCtrlDetail.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;  //选中某行使整行高亮（只适用与report风格）
	dwStyle |= LVS_EX_GRIDLINES;      //行与行之间有分割线
	//dwStyle = LVS_NOCOLUMNHEADER;      //隐藏表头
	m_ListCtrlDetail.SetExtendedStyle(dwStyle);

	m_ListCtrlDetail.InsertColumn(
		0,              // Rank/order of item 
		L"序号",          // Caption for this header 
		LVCFMT_LEFT,    // Relative position of items under header 
		40);           // Width of items under header

	m_ListCtrlDetail.InsertColumn(1, L"类型", LVCFMT_LEFT, 40);
	m_ListCtrlDetail.InsertColumn(2, L"位置", LVCFMT_LEFT, 40);
	m_ListCtrlDetail.InsertColumn(3, L"直径", LVCFMT_LEFT, 40);
	m_ListCtrlDetail.InsertColumn(4, L"等级", LVCFMT_LEFT, 40);
}

//绘制虚线表格
void CTableDlg::DrawTable(CDC *mDC, CRect rect, float x, float y)
{
	mDC->SelectObject(&m_pen[6]);
	mDC->SetBkColor(RGB(255, 255, 255));

	int high_size = (int)rect.Height() / 11;
	int wide_size = (int)rect.Width() / 11;
	mDC->TextOutW(3, 3, _T("(米)"));

	for (int i = 1; i < 11; i++)
	{
		mDC->MoveTo(0, i*high_size);
		mDC->LineTo(rect.Width(), i*high_size);

		//添加标注
		CString ykey;
		ykey.Format(_T("%.1f"), (11 - i) * (m_current_position / 11.0f));
		mDC->TextOutW(0, i*high_size - 18, ykey);
	}
	
	mDC->MoveTo(0, 0);
	mDC->TextOutW(rect.Width() - 30, rect.Height() - 3, _T("(毫米)"));
	for (int j = 1; j < 11; j++)
	{
		mDC->MoveTo(j*wide_size, 0);
		mDC->LineTo(j*wide_size, rect.Height());

		//添加标注
		CString xkey;
		xkey.Format(_T("%.1f"), j * (x / 11.0f));
		mDC->TextOutW(j*wide_size + 3, rect.Height() - 18, xkey);
	}
}

void CTableDlg::CreateFlag(CDC *mDC, int x, int y, int kind)
{
	switch (kind)
	{
	case 0: {
		mDC->SelectObject(&m_pen[1]);
		mDC->Rectangle(x, y, x+3, y+3);
		break;
	}
	case 1: {
		mDC->SelectObject(&m_pen[2]);
		mDC->Rectangle(x, y, x + 3, y + 3);
		break;
	}
	case 2: {
		mDC->SelectObject(&m_pen[3]);
		mDC->Rectangle(x, y, x + 3, y + 3);
		break;
	}
	case 3: {
		mDC->SelectObject(&m_pen[4]);
		mDC->Rectangle(x, y, x + 3, y + 3);
		break;
	}
	case 4: {
		mDC->SelectObject(&m_pen[5]);
		mDC->Rectangle(x, y, x + 3, y + 3);
		break;
	}
	default: {
		mDC->SelectObject(&m_pen[0]);
		//mDC->Rectangle(0, 0, 5, 5);
	}
	}
}

void CTableDlg::DrawAllFlag(CDC *mDC, int wnd_width, int wnd_height)
{
	if (m_pvDFT == NULL)
		return;
	else {
		if (!m_pvDFT->empty()) {
			
			DefectType dft;
			for (int i = 0; i < m_pvDFT->size(); i++)
			{
				dft = m_pvDFT->at(i);
				int x = (int)(dft.center_x * scale_x);
				int y = (int)(wnd_height - dft.absolute_position * scale_y);
				CreateFlag(mDC, x, y, dft.type);

				AddToDetailList(i + 1, dft.type, dft.absolute_position, dft.circle_radius, dft.rank);
			}
		}
	}
	

	return;
}

void CTableDlg::DrawSelectDFT(CDC *mDC, int x, int y)
{
	CPen pen;
	pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));

	mDC->SelectObject(&pen);
	mDC->Rectangle(x - 5, y - 5, x + 5, y + 5);

}

void CTableDlg::AddToDetailList(int NO, int kind, float position, float radius, int rank)
{
	std::string strNO = std::to_string(NO);
	CString cstrKind;
	switch (kind)
	{
	case 0:
		cstrKind.Format(_T("异物"));
		break;
	case 1:
		cstrKind.Format(_T("凹凸"));
		break;
	case 2:
		cstrKind.Format(_T("擦伤"));
		break;
	case 3:
		cstrKind.Format(_T("晶点"));
		break;
	case 4:
		cstrKind.Format(_T("漏涂"));
		break;
	default:
		break;
	}

	char cpos[16];
	sprintf_s(cpos, "%.3f", position);
	char cradius[16];
	sprintf_s(cradius, "%.3f", radius);

	CString cstrRank;
	switch (kind)
	{
	case 0:
		cstrRank.Format(_T("普通"));
		break;
	case 1:
		cstrRank.Format(_T("一级"));
		break;
	case 2:
		cstrRank.Format(_T("二级"));
		break;
	case 3:
		cstrRank.Format(_T("三级"));
		break;
	case 4:
		cstrRank.Format(_T("严重"));
		break;
	default:
		break;
	}
	   	 
	int nItem;
	nItem = m_ListCtrlDetail.InsertItem(0, CA2W(strNO.c_str()));
	m_ListCtrlDetail.SetItemText(nItem, 1, cstrKind);
	m_ListCtrlDetail.SetItemText(nItem, 2, CA2W(cpos));
	m_ListCtrlDetail.SetItemText(nItem, 3, CA2W(cradius));
	m_ListCtrlDetail.SetItemText(nItem, 4, cstrRank);
	return;
}

//保存瑕疵分布图
void CTableDlg::SaveDistributeImage()
{
	CWnd *pwnd = GetDlgItem(IDC_STATIC_REPORT);
	CRect rect;
	pwnd->GetClientRect(&rect);
	CDC *dc = pwnd->GetDC();

	LPRECT prect = rect;
	m_hbitmap = GetSrcBit(*dc, prect);
	//BOOL saved = SaveBMPToFile(m_hbitmap, "D:\\temp\\saved.bmp");
	BOOL saved = SaveBitmapToFile(m_hbitmap, (LPSTR)_T(".\\temp\\saved.bmp"));
	ReleaseDC(dc);
}

HBITMAP CTableDlg::GetSrcBit(HDC hDC, LPRECT rEct)
{
	HDC hBufDC;
	HBITMAP hBitmap, hBitTemp;
	//创建设备上下文(HDC)
	hBufDC = CreateCompatibleDC(hDC);
	//创建HBITMAP
	hBitmap = CreateCompatibleBitmap(hDC, abs(rEct->right - rEct->left), abs(rEct->bottom - rEct->top));
	hBitTemp = (HBITMAP)SelectObject(hBufDC, hBitmap);
	//得到位图缓冲区
	StretchBlt(hBufDC, 0, 0, abs(rEct->right - rEct->left), abs(rEct->bottom - rEct->top),
		hDC, rEct->left, rEct->top, abs(rEct->right - rEct->left), abs(rEct->bottom - rEct->top), SRCCOPY);
	//得到最终的位图信息
	hBitmap = (HBITMAP)SelectObject(hBufDC, hBitTemp);
	//释放内存
	DeleteObject(hBitTemp);
	::DeleteDC(hBufDC);
	return hBitmap;
}

//参考：https://blog.csdn.net/laironggui/article/details/8085268
bool CTableDlg::SaveBMPToFile(HBITMAP hBitmap, LPSTR lpFileName) //hBitmap 为刚才的屏幕位图句柄,lpFileName 为位图文件名
{
	HDC hDC; //设备描述表
	int iBits; //当前显示分辨率下每个像素所占字节数
	WORD wBitCount; //位图中每个像素所占字节数
	//定义调色板大小， 位图中像素字节大小 ，  位图文件大小 ， 写入文件字节数
	DWORD dwPaletteSize = 0, dwBmBitsSize, dwDIBSize, dwWritten;
	BITMAP Bitmap;   //位图属性结构
	BITMAPFILEHEADER bmfHdr; //位图文件头结构
	BITMAPINFOHEADER bi; //位图信息头结构
	LPBITMAPINFOHEADER lpbi; //指向位图信息头结构

	HANDLE fh, hDib, hPal;
	HPALETTE hOldPal = NULL; //定义文件，分配内存句柄，调色板句柄

	//计算位图文件每个像素所占字节数
	hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1) wBitCount = 1;
	else if (iBits <= 4) wBitCount = 4;
	else if (iBits <= 8) wBitCount = 8;
	else if (iBits <= 24) wBitCount = 24;
	else wBitCount = 32;
	//计算调色板大小
	if (wBitCount <= 8) dwPaletteSize = (1i64 << wBitCount) * sizeof(RGBQUAD);

	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize           = sizeof(BITMAPINFOHEADER);
	bi.biWidth          = Bitmap.bmWidth;
	bi.biHeight         = Bitmap.bmHeight;
	bi.biPlanes         = 1;
	bi.biBitCount       = wBitCount;
	bi.biCompression    = BI_RGB;
	bi.biSizeImage      = 0;
	bi.biXPelsPerMeter  = 0;
	bi.biYPelsPerMeter  = 0;
	bi.biClrUsed        = 0;
	bi.biClrImportant   = 0;

	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount + 31) / 32) * 4 * Bitmap.bmHeight; //为位图内容分配内存

	/*xxxxxxxx计算位图大小分解一下(解释一下上面的语句)xxxxxxxxxxxxxxxxxxxx
	//每个扫描行所占的字节数应该为4的整数倍，具体算法为:
	int biWidth = (Bitmap.bmWidth*wBitCount) / 32;
	if((Bitmap.bmWidth*wBitCount) % 32)
	biWidth++; //不是整数倍的加1
	biWidth *= 4;//到这里，计算得到的为每个扫描行的字节数。
	dwBmBitsSize = biWidth * Bitmap.bmHeight;//得到大小
	xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/


	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// 处理调色板  
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	// 获取该调色板下新的像素值
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
	//恢复调色板  
	if (hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}
	//创建位图文件   
	fh = CreateFile((LPCWSTR)lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE)
		return false;
	//设置位图文件头
	bmfHdr.bfType = 0x4D42;
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	// 写入位图文件头
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// 写入位图文件其余内容
	WriteFile(fh, (LPSTR)lpbi, sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize, &dwWritten, NULL);
	//清除  
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return true;
}

bool CTableDlg::SaveBitmapToFile(HBITMAP hBitmap, LPSTR lpFileName)
{
	CImage img;
	img.Attach(hBitmap);
	HRESULT hResult = img.Save((LPCWSTR)lpFileName);
	//其它图片格式同理
	DeleteObject(hBitmap);
	if (FAILED(hResult))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CTableDlg::GenerateReportName(std::wstring &wstrname)
{
	//获取日期
	std::wstringstream date;
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	date << sysTime.wYear << std::setw(2) << std::setfill(L'0')
		<< sysTime.wMonth << std::setw(2) << std::setfill(L'0')
		<< sysTime.wDay << L"_" << std::setw(2) << std::setfill(L'0')
		<< sysTime.wHour << std::setw(2) << std::setfill(L'0')
		<< sysTime.wMinute << std::setw(2) << std::setfill(L'0')
		<< sysTime.wSecond;
	std::wstring wdate = date.str();
	//const wchar_t* wname = wdate.c_str();
	//_bstr_t name(wname);

	m_wstr_savetime = wdate;

	wdate.append(L"_N").append(m_wstr_num).append(L"_D").append(m_wstr_id)
		.append(L"_W").append(m_wstr_width).append(L"_U").append(m_wstr_user);

	wstrname = wdate;
}

//保存数据 : 参考地址：https://blog.csdn.net/ywx123_/article/details/77074038
void CTableDlg::SaveToExcel(std::vector<DefectType> vDFT)
{

	//1.创建基本对象
	CApplication App;  //创建应用程序实例
	CWorkbooks Books;  //工作簿，多个Excel文件
	CWorkbook Book;    //单个工作簿
	CWorksheets sheets;//多个sheet页面
	CWorksheet sheet;  //单个sheet页面
	CRange range;      //操作单元格

	//2.打开指定Excel文件，如果不存在就创建
	CString strExcelFile = m_save_path.c_str();
	std::wstring strname;
	GenerateReportName(strname);
	CString strdevName(strname.c_str());
	strdevName += _T(".xlsx");
	strExcelFile += strdevName;
	COleVariant
		covTrue((short)TRUE),
		covFalse((short)FALSE),
		covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	LPDISPATCH lpdisp = NULL;
	//1.创建Excel实例
	if (!App.CreateDispatch(_T("Excel.Application"), NULL))
	{
		Win::log("创建Excel实例失败");
		//pMainDlg->RecordWarning(L"创建Excel实例失败");		
	}
	else
	{
		Win::log("创建报表成功");
		//pMainDlg->RecordLog(L"创建报表成功");
	}
	App.put_Visible(FALSE);	//打开Excel, 也可设置为不打开
	App.put_UserControl(FALSE);
	
	//2. 得到workbooks容器
	Books.AttachDispatch(App.get_Workbooks());
	Book.AttachDispatch(Books.Add(covOptional));
	sheets.AttachDispatch(Book.get_Worksheets());
	sheet.AttachDispatch(sheets.get_Item(COleVariant((short)1)));	//获取sheet1
	sheet.put_Name(_T("瑕疵列表"));	    //设置sheet1名字

	//设置表头格式
	FormatTableHead(sheet, range, TRUE);

	//填充表头内容
	CString cnumber = m_wstr_num.c_str();     //批号
	COleVariant vnumber(cnumber);
	range.put_Item(COleVariant((long)2), COleVariant((long)2), vnumber);

	//时间格式化
	m_wstr_savetime.insert(4, L"年");
	m_wstr_savetime.insert(7, L"月");
	m_wstr_savetime.insert(10, L"日");
	m_wstr_savetime.insert(14, L"时");
	m_wstr_savetime.insert(17, L"分");
	m_wstr_savetime.insert(20, L"秒");
	CString ctotal_time = m_wstr_savetime.c_str();     //检测时间
	//ctotal_time.Format(_T("2020年3月24日"));
	COleVariant vtotal_time(ctotal_time);
	range.put_Item(COleVariant((long)2), COleVariant((long)5), vtotal_time);
	//range.put_UseStandardWidth(COleVariant((long)55));

	CString coperator = m_wstr_user.c_str();     //检测员
	COleVariant voperator(coperator);
	range.put_Item(COleVariant((long)2), COleVariant((long)8), voperator);

	CString cID = m_wstr_id.c_str();     //型号
	COleVariant vID(cID);
	range.put_Item(COleVariant((long)3), COleVariant((long)2), vID);

	CString clongth = m_wstr_width.c_str();     //检测长度
	COleVariant vlongth(clongth);
	range.put_Item(COleVariant((long)3), COleVariant((long)5), vlongth);

	CString cspeed = m_wstr_speed.c_str();     //平均速度
	COleVariant vspeed(cspeed);
	range.put_Item(COleVariant((long)3), COleVariant((long)8), vspeed);

	for (int k = 0; k < 7; k++)
	{		
		if (k < 5) {
			COleVariant vkind_stastic((long)k);   //类型统计
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vkind_stastic);
		}
		if (k == 5) {
			CString cserious;     
			cserious.Format(_T("0000"));
			COleVariant vserious(cserious);   //严重缺陷
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vserious);
		}
		if (k == 6) {
			CString cout;
			cout.Format(_T("一级"));
			COleVariant vout(cout);   //检测结果
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vout);
		}
	}
	
	/*
	//写入瑕疵信息
	for (int i = 0; i < vDFT.size(); i++)
	{
		DefectType dft;
		dft =vDFT.at(i);
		for (int j = 1; j < 9; j++)
		{
			switch (j)
			{
			case 1: {
				COleVariant vResult((long)(i+1));
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 2: {
				CString ckind;
				ckind.Format(_T("%d"), dft.type);
				COleVariant vResult(ckind);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 3: {
				CString cx;
				cx.Format(_T("%.2f"), dft.center_x);
				COleVariant vResult(cx);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 4: {
				CString cy;
				cy.Format(_T("%.2f"), dft.absolute_position);
				COleVariant vResult(cy);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 5: {
				CString cradius;
				cradius.Format(_T("%.2f"), dft.circle_radius);
				COleVariant vResult(cradius);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 6: {
				CString carea;
				carea.Format(_T("%.2f"), dft.area);
				COleVariant vResult(carea);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 7: {
				CString csize;
				csize.Format(_T("%.2f"), dft.contlength);
				COleVariant vResult(csize);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 8: {
				CString ctime;
				ctime.Format(_T("%.2f"), dft.pixel_value);
				COleVariant vResult(ctime);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			default:
				break;
			}
		}
	}
	*/

	range.AttachDispatch(sheet.get_UsedRange());//加载已使用的单元格
	range.put_WrapText(COleVariant((long)1));   //设置文本自动换行

	//5.设置对齐方式
	//水平对齐：默认 1 居中 -4108， 左= -4131，右=-4152
	//垂直对齐：默认 2 居中 -4108， 左= -4160，右=-4107
	range.put_VerticalAlignment(COleVariant((long)-4108));
	range.put_HorizontalAlignment(COleVariant((long)-4108));

	//6.设置字体颜色
	CFont0 ft;
	ft.AttachDispatch(range.get_Font());
	ft.put_Name(COleVariant(_T("宋体")));	//字体
	ft.put_ColorIndex(COleVariant((long)1));//颜色	//黑色
	ft.put_Size(COleVariant((long)12));     //大小

	range.AttachDispatch(sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("H1"))), TRUE);
	ft.AttachDispatch(range.get_Font());
	ft.put_Name(COleVariant(_T("宋体")));
	ft.put_Bold(COleVariant((long)1));
	ft.put_ColorIndex(COleVariant((long)1));    //颜色	
	ft.put_Size(COleVariant((long)18));         //大小


	//*****************************************************创建瑕疵分布图**********************************/
	LPDISPATCH lpDisp;
	LPDISPATCH lpDispLast = sheets.get_Item(COleVariant(sheets.get_Count()));
	lpDisp = sheets.Add(vtMissing, _variant_t(lpDispLast), _variant_t((long)1), vtMissing);
	sheet.AttachDispatch(lpDisp);
	sheet.put_Name(L"瑕疵分布图");

	if (1)
	{
		//设置表头格式
		FormatTableHead(sheet, range, FALSE);

		//填充表头内容
		CString cnumber = m_wstr_num.c_str();     //批号
		COleVariant vnumber(cnumber);
		range.put_Item(COleVariant((long)2), COleVariant((long)2), vnumber);

		CString ctotal_time = m_wstr_savetime.c_str();     //检测时间
		COleVariant vtotal_time(ctotal_time);
		range.put_Item(COleVariant((long)2), COleVariant((long)5), vtotal_time);

		CString coperator = m_wstr_user.c_str();     //检测员
		COleVariant voperator(coperator);
		range.put_Item(COleVariant((long)2), COleVariant((long)8), voperator);

		CString cID = m_wstr_id.c_str();     //型号
		COleVariant vID(cID);
		range.put_Item(COleVariant((long)3), COleVariant((long)2), vID);

		CString clongth = m_wstr_width.c_str();     //检测长度
		COleVariant vlongth(clongth);
		range.put_Item(COleVariant((long)3), COleVariant((long)5), vlongth);

		CString cspeed = m_wstr_speed.c_str();     //平均速度
		COleVariant vspeed(cspeed);
		range.put_Item(COleVariant((long)3), COleVariant((long)8), vspeed);

		for (int k = 0; k < 7; k++)
		{
			if (k < 5) {
				COleVariant vkind_stastic((long)k);   //类型统计
				range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vkind_stastic);
			}
			if (k == 5) {
				CString cserious;
				cserious.Format(_T("0000"));
				COleVariant vserious(cserious);   //严重缺陷
				range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vserious);
			}
			if (k == 6) {
				CString cout;
				cout.Format(_T("一级"));
				COleVariant vout(cout);   //检测结果
				range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vout);
			}
		}

		//添加图片
		char path[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, (TCHAR*)path);//获取当前路径
		CString strBMP = (TCHAR*)path;
		CString imageName("\\temp\\saved.bmp");
		strBMP += imageName;
		std::ifstream fin(strBMP.GetBuffer());
		if (fin) {
			//【2】插入图像
			//获取图像插入的范围
			//从Sheet对象上获得一个Shapes   
			CShapes pShapes;
			pShapes.AttachDispatch(sheet.get_Shapes());
			//获得Range对象，用来插入图片
			range.AttachDispatch(sheet.get_Range(COleVariant(_T("A7")), COleVariant(_T("H51"))), TRUE);
			range.Merge(COleVariant((long)0));  //合并单元格
			VARIANT rLeft = range.get_Left();
			VARIANT rTop = range.get_Top();
			VARIANT rWidth = range.get_Width();
			VARIANT rHeight = range.get_Height();

			//添加图像到 A7 - H51范围区域
			CShape pShape = pShapes.AddPicture(strBMP, TRUE, TRUE,
				(float)rLeft.dblVal + 20.0f, (float)rTop.dblVal + 20.0f,
				(float)rWidth.dblVal - 40.0f, (float)rHeight.dblVal - 40.0f);
			////设置图像所占的宽高
			//CShapeRange shapeRange = pShapes.get_Range(_variant_t(long(1)));
			//shapeRange.put_Height(float(600));
			//shapeRange.put_Width(float(500));
		}


		range.AttachDispatch(sheet.get_UsedRange());//加载已使用的单元格
		range.put_WrapText(COleVariant((long)1));   //设置文本自动换行

		//5.设置对齐方式
		//水平对齐：默认 1 居中 -4108， 左= -4131，右=-4152
		//垂直对齐：默认 2 居中 -4108， 左= -4160，右=-4107
		range.put_VerticalAlignment(COleVariant((long)-4108));
		range.put_HorizontalAlignment(COleVariant((long)-4108));

		//6.设置字体颜色
		CFont0 ft;
		ft.AttachDispatch(range.get_Font());
		ft.put_Name(COleVariant(_T("宋体")));	//字体
		ft.put_ColorIndex(COleVariant((long)1));//颜色	//黑色
		ft.put_Size(COleVariant((long)12));     //大小

		range.AttachDispatch(sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("H1"))), TRUE);
		ft.AttachDispatch(range.get_Font());
		ft.put_Name(COleVariant(_T("宋体")));
		ft.put_Bold(COleVariant((long)1));
		ft.put_ColorIndex(COleVariant((long)1));    //颜色	
		ft.put_Size(COleVariant((long)18));         //大小


	}
	//**************************************************************************************************/

	Book.SaveCopyAs(COleVariant(strExcelFile)); //保存

	Book.put_Saved(TRUE);
	//8.释放资源
	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	Book.ReleaseDispatch();
	Books.ReleaseDispatch();
	Book.Close(covOptional, covOptional, covOptional);//关闭Workbook对象
	Books.Close();           // 关闭Workbooks对象
	App.Quit();          // 退出_Application
	App.ReleaseDispatch();

}

//使用模板保存
void CTableDlg::SaveToExcelUseDefault(CString &name)
{
	//打开模板文件
	TCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	CString cpath = path;
	cpath = cpath + L"\\temp\\example.xlsx";
	std::ifstream fexist(CT2A(cpath.GetBuffer()));
	if (!fexist) {
		CString cstr = L"打开Excel模板失败，请检查.temp\\example.xlsx文件是否存在";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return;
	}

	CApplication app;
	CWorkbooks books;
	CWorkbook book;

	LPDISPATCH lpDisp;
	COleVariant vResult;
	COleVariant  covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (!app.CreateDispatch(L"Excel.Application"))
	{
		AfxMessageBox(L"无法打开Excel应用", MB_OK | MB_ICONWARNING);
		return;
	}
	books.AttachDispatch(app.get_Workbooks());

	lpDisp = books.Open(cpath, covOptional, covOptional, covOptional, covOptional, covOptional
		, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional);
	//得到得到视图workbook
	book.AttachDispatch(lpDisp);

	//另存为
	CString strExcelFile = m_save_path.c_str();
	std::wstring strname;
	GenerateReportName(strname);
	CString strdevName(strname.c_str());
	strdevName += _T(".xlsx");
	strExcelFile += strdevName;
	COleVariant cvname(strExcelFile);
	book.SaveAs(cvname, covOptional, covOptional, covOptional, covOptional, covOptional
		, 1, covOptional, covOptional, covOptional, covOptional, covOptional);

	book.ReleaseDispatch();
	books.ReleaseDispatch();
	book.Close(covOptional, covOptional, covOptional);//关闭Workbook对象
	books.Close();           // 关闭Workbooks对象
	//app.Quit();          // 退出_Application
	//app.ReleaseDispatch();


	name = cpath;
}

//使用模板并插入散点图
void CTableDlg::SaveScatterPlotUseDefault()
{
	TCHAR excel_path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, excel_path);
	CString cpath = excel_path;
	cpath = L"D:\\temp\\test.xlsx";
	std::ifstream fexist(CT2A(cpath.GetBuffer()));
	if (!fexist) {
		CString cstr = L"打开Excel模板失败，请检查\\temp\\目录下文件example.xlsx是否存在";
		::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return;
	}

	//1.创建基本对象
	CApplication App;                //创建应用程序实例
	CWorkbooks Books;                //工作簿，多个Excel文件
	CWorkbook Book_example, Book;    //单个工作簿
	CWorksheets sheets;              //多个sheet页面
	CWorksheet sheet;                //单个sheet页面
	CRange range;                    //操作单元格

	LPDISPATCH lpDisp;
	COleVariant vResult;
	COleVariant  covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (!App.CreateDispatch(L"Excel.Application")) {
		CString cstr = L"打开Excel失败，请检查Excel状态";
		::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return;
	}
	Books.AttachDispatch(App.get_Workbooks());
	//打开模板文件
	lpDisp = Books.Open(cpath, covOptional, covOptional, covOptional, covOptional, covOptional
		, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional);
	//得到得到视图workbook
	Book_example.AttachDispatch(lpDisp);

	//另存为
	CString strExcelFile = m_save_path.c_str();
	std::wstring strname;
	GenerateReportName(strname);
	CString strdevName(strname.c_str());
	strdevName += _T(".xlsx");
	strExcelFile += strdevName;
	COleVariant cvname(strExcelFile);
	Book_example.SaveAs(cvname, covOptional, covOptional, covOptional, covOptional, covOptional
		, 1, covOptional, covOptional, covOptional, covOptional, covOptional);

	//打开另存的文件
	lpDisp = Books.Open(strExcelFile, covOptional, covOptional, covOptional,
		covOptional, covOptional, covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional, covOptional, covOptional, covOptional);
	//得到得到视图workbook
	Book.AttachDispatch(lpDisp);
	//得到worksheets
	sheets.AttachDispatch(Book.get_Worksheets());
	//得到sheet1
	sheet.AttachDispatch(sheets.get_Item(COleVariant((short)1)));	//获取sheet1
	range.AttachDispatch(sheet.get_UsedRange());

	range.AttachDispatch(sheet.get_UsedRange());//加载已使用的单元格
	range.put_WrapText(COleVariant((long)1));   //设置文本自动换行

	//获取图表绘制的数据区域
	LPDISPATCH lpDispXY = sheet.get_Range(COleVariant(_T("C7")), COleVariant(_T("D70")));

	//写入瑕疵分布图 sheet2
	sheet.AttachDispatch(sheets.get_Item(COleVariant((short)2)));	//获取sheet2
	//取得用户区
	range.AttachDispatch(sheet.get_UsedRange());//加载已使用的单元格
	range.AttachDispatch(sheet.get_Range(COleVariant(_T("A7")), COleVariant(_T("H52"))), TRUE);
	VARIANT rLeft = range.get_Left();
	VARIANT rTop = range.get_Top();
	VARIANT rWidth = range.get_Width();
	VARIANT rHeight = range.get_Height();
	double fleft = (double)rLeft.dblVal + 10.0;
	double ftop = (double)rTop.dblVal + 10.0;
	double fwidth = (double)rWidth.dblVal - 20.0;
	double fheight = (double)rHeight.dblVal - 20.0;

	CChartObjects chartobjects;       //chart对象所在的容器
	CChartObject  chartobject;
	CChart        chart;                  //图表

	lpDisp = sheet.ChartObjects(covOptional);
	ASSERT(lpDisp);
	chartobjects.AttachDispatch(lpDisp);
	if (chartobjects.get_Count() != 0) //当excel中存在原有图表时，删除之
	{
		chartobjects.Delete();
	}

	chartobject = chartobjects.Add(fleft, ftop, fwidth, fheight);
	//设置图表区边框
	LPDISPATCH lpDispBorder = chartobject.get_Border();
	CBorder border;
	border.AttachDispatch(lpDispBorder);
	border.put_LineStyle(COleVariant((short)-4142));       // No line

	//散点图样式设置
	chart.AttachDispatch(chartobject.get_Chart());
	chart.put_ChartType(-4169);
	VARIANT var;
	var.vt = VT_DISPATCH;
	var.pdispVal = lpDispXY;
	chart.ChartWizard(var,// Source.
		COleVariant((short)-4169),// Gallery, 散点图
		COleVariant((short)3),// Format: 1~6.
		COleVariant((short)2),// PlotBy: 指定系列中的数据是来自行(1)还是来自列(2).
		COleVariant((short)1),// CategoryLabels.类别：第一行是分类标签
		COleVariant((short)2),// SeriesLabels.系列：第一列是系列标签
		COleVariant((short)FALSE),// HasLegend.
		COleVariant(L"缺陷分布图"),// Title.
		covOptional,// CategoryTitle.
		covOptional,// ValueTitles.
		covOptional  // ExtraTitle.
	);
	chart.put_ChartStyle(COleVariant((short)1));
	//设置网格线
	CAxis axis;
	CGridlines gridlines;
	lpDisp = chart.Axes(COleVariant((short)1), 1);  //Axis displays categories, X轴
	axis.AttachDispatch(lpDisp);
	axis.get_HasMajorGridlines();
	lpDisp = axis.get_MajorGridlines();
	gridlines.AttachDispatch(lpDisp);
	lpDisp = gridlines.get_Border();
	border.AttachDispatch(lpDisp);
	border.put_LineStyle(COleVariant((short)-4115));  //Dashed line
	//border.put_TintAndShade(COleVariant((short)-0.55));  //设置透明度会出现未知错误

	lpDisp = chart.Axes(COleVariant((short)2), 1);  //Axis displays values ， Y轴
	axis.AttachDispatch(lpDisp);
	axis.get_HasMajorGridlines();
	lpDisp = axis.get_MajorGridlines();
	gridlines.AttachDispatch(lpDisp);
	lpDisp = gridlines.get_Border();
	border.AttachDispatch(lpDisp);
	border.put_LineStyle(COleVariant((short)-4115));  //Dashed line

	Book.Save(); //保存
	Book.put_Saved(TRUE);

	//8.释放资源
	border.ReleaseDispatch();
	gridlines.ReleaseDispatch();
	axis.ReleaseDispatch();
	chart.ReleaseDispatch();
	chartobject.ReleaseDispatch();
	chartobjects.ReleaseDispatch();
	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	Book.ReleaseDispatch();
	Book_example.ReleaseDispatch();
	Books.ReleaseDispatch();
	Book.Close(covOptional, covOptional, covOptional);//关闭Workbook对象
	Book_example.Close(covOptional, covOptional, covOptional);
	Books.Close();           // 关闭Workbooks对象
	App.Quit();          // 退出_Application
	App.ReleaseDispatch();

	CString cstr = L"报表已保存: " + strExcelFile;
	::SendMessage(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

}

void CTableDlg::FormatTableHead(CWorksheet &sheet, CRange &range, BOOL bhead)
{
	//3. 加载要合并的单元格
	range.AttachDispatch(sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("H1"))), TRUE);
	range.Merge(COleVariant((long)0));  //合并单元格

	range.AttachDispatch(sheet.get_Range(COleVariant(_T("B2")), COleVariant(_T("C2"))), TRUE);
	range.Merge(COleVariant((long)0));  //合并单元格

	range.AttachDispatch(sheet.get_Range(COleVariant(_T("E2")), COleVariant(_T("F2"))), TRUE);
	range.Merge(COleVariant((long)0));  //合并单元格

	range.AttachDispatch(sheet.get_Range(COleVariant(_T("B3")), COleVariant(_T("C3"))), TRUE);
	range.Merge(COleVariant((long)0));  //合并单元格

	range.AttachDispatch(sheet.get_Range(COleVariant(_T("E3")), COleVariant(_T("F3"))), TRUE);
	range.Merge(COleVariant((long)0));  //合并单元格

	range.AttachDispatch(sheet.get_Range(COleVariant(_T("A4")), COleVariant(_T("A5"))), TRUE);
	range.Merge(COleVariant((long)0));  //合并单元格


	//4. 设置表格内容
	range.AttachDispatch(sheet.get_Cells(), TRUE);				//加载所有单元格
	//写入单元格：行号；列号；内容
	range.put_Item(COleVariant((long)1), COleVariant((long)1), COleVariant(_T("产品瑕疵检测报告")));

	range.put_Item(COleVariant((long)2), COleVariant((long)1), COleVariant(_T("产品批号")));
	range.put_Item(COleVariant((long)2), COleVariant((long)4), COleVariant(_T("检测时间")));
	range.put_Item(COleVariant((long)2), COleVariant((long)7), COleVariant(_T("检测员")));

	range.put_Item(COleVariant((long)3), COleVariant((long)1), COleVariant(_T("产品型号")));
	range.put_Item(COleVariant((long)3), COleVariant((long)4), COleVariant(_T("检测长度(m)")));
	range.put_Item(COleVariant((long)3), COleVariant((long)7), COleVariant(_T("平均速度(m/min)")));

	range.put_Item(COleVariant((long)4), COleVariant((long)1), COleVariant(_T("瑕疵统计(数量：个)")));
	range.put_Item(COleVariant((long)4), COleVariant((long)2), COleVariant(_T("类型1")));
	range.put_Item(COleVariant((long)4), COleVariant((long)3), COleVariant(_T("类型2")));
	range.put_Item(COleVariant((long)4), COleVariant((long)4), COleVariant(_T("类型3")));
	range.put_Item(COleVariant((long)4), COleVariant((long)5), COleVariant(_T("类型4")));
	range.put_Item(COleVariant((long)4), COleVariant((long)6), COleVariant(_T("类型5")));
	range.put_Item(COleVariant((long)4), COleVariant((long)7), COleVariant(_T("严重缺陷")));
	range.put_Item(COleVariant((long)4), COleVariant((long)8), COleVariant(_T("检测结果")));

	if (bhead) {
		range.put_Item(COleVariant((long)6), COleVariant((long)1), COleVariant(_T("序号")));
		range.put_Item(COleVariant((long)6), COleVariant((long)2), COleVariant(_T("瑕疵类型")));
		range.put_Item(COleVariant((long)6), COleVariant((long)3), COleVariant(_T("横向(mm)")));
		range.put_Item(COleVariant((long)6), COleVariant((long)4), COleVariant(_T("纵向(m)")));
		range.put_Item(COleVariant((long)6), COleVariant((long)5), COleVariant(_T("直径(mm)")));
		range.put_Item(COleVariant((long)6), COleVariant((long)6), COleVariant(_T("面积(m2)")));
		range.put_Item(COleVariant((long)6), COleVariant((long)7), COleVariant(_T("尺寸(mm)")));
		range.put_Item(COleVariant((long)6), COleVariant((long)8), COleVariant(_T("检出时间")));
	}

}

//保存报表
void CTableDlg::BeginSaveTable()
{
	if (m_SaveTable != NULL) {
		//Win::log("报表保存失败，上一个保存线程尚未结束");
		m_SaveTable = NULL;
	}
	//使用默认模板另存为
	//SaveToExcelUseDefault(m_current_excel_name);

	if (!(m_SaveTable = AfxBeginThread(SaveTableThreadDefault, this))) {
		CString cstr = L"报表保存失败,保存线程创建失败";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return;
	}

	return;
}

//线程函数：保存excel
UINT CTableDlg::SaveTableThread(LPVOID pParam)
{
	//解决多线程打开excel的错误
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		CoInitialize(NULL);
	AfxEnableControlContainer();	

	CTableDlg *pThis = (CTableDlg *)pParam;

	//1.创建基本对象
	CApplication App;  //创建应用程序实例
	CWorkbooks Books;  //工作簿，多个Excel文件
	CWorkbook Book;    //单个工作簿
	CWorksheets sheets;//多个sheet页面
	CWorksheet sheet;  //单个sheet页面
	CRange range;      //操作单元格
	
	//2.打开指定Excel文件，如果不存在就创建
	CString strExcelFile = pThis->m_save_path.c_str();

	std::wstring strname;
	pThis->GenerateReportName(strname);
	CString strdevName(strname.c_str());
	strdevName += _T(".xlsx");
	strExcelFile += strdevName;

	COleVariant
		covTrue((short)TRUE),
		covFalse((short)FALSE),
		covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	LPDISPATCH lpdisp = NULL;
	//1.创建Excel实例
	if (!App.CreateDispatch(_T("Excel.Application"), NULL))
	{
		CString cstr = L"创建Excel失败，请检查Excel是否正常";
		::SendNotifyMessageW(pThis->hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
	}

	App.put_Visible(FALSE);	//打开Excel, 也可设置为不打开
	App.put_UserControl(FALSE);

	//2. 得到workbooks容器
	Books.AttachDispatch(App.get_Workbooks());
	Book.AttachDispatch(Books.Add(covOptional));
	sheets.AttachDispatch(Book.get_Worksheets());
	sheet.AttachDispatch(sheets.get_Item(COleVariant((short)1)));	//获取sheet1
	sheet.put_Name(_T("瑕疵列表"));	    //设置sheet1名字

	//设置表头格式
	pThis->FormatTableHead(sheet, range, TRUE);
	//填充表头内容
	CString cnumber = pThis->m_wstr_num.c_str();     //批号
	COleVariant vnumber(cnumber);
	range.put_Item(COleVariant((long)2), COleVariant((long)2), vnumber);

	//时间格式化
	pThis->m_wstr_savetime.insert(4, L"年");
	pThis->m_wstr_savetime.insert(7, L"月");
	pThis->m_wstr_savetime.insert(10, L"日");
	pThis->m_wstr_savetime.insert(14, L"时");
	pThis->m_wstr_savetime.insert(17, L"分");
	pThis->m_wstr_savetime.insert(20, L"秒");
	CString ctotal_time = pThis->m_wstr_savetime.c_str();     //检测时间
	COleVariant vtotal_time(ctotal_time);
	range.put_Item(COleVariant((long)2), COleVariant((long)5), vtotal_time);

	CString coperator = pThis->m_wstr_user.c_str();     //检测员
	COleVariant voperator(coperator);
	range.put_Item(COleVariant((long)2), COleVariant((long)8), voperator);

	CString cID = pThis->m_wstr_id.c_str();     //型号
	COleVariant vID(cID);
	range.put_Item(COleVariant((long)3), COleVariant((long)2), vID);

	CString clongth = pThis->m_wstr_width.c_str();     //检测长度
	COleVariant vlongth(clongth);
	range.put_Item(COleVariant((long)3), COleVariant((long)5), vlongth);

	CString cspeed = pThis->m_wstr_speed.c_str();     //平均速度
	COleVariant vspeed(cspeed);
	range.put_Item(COleVariant((long)3), COleVariant((long)8), vspeed);

	for (int k = 0; k < 7; k++)
	{
		if (k < 5) {
			COleVariant vkind_stastic((long)k);   //类型统计
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vkind_stastic);
		}
		if (k == 5) {
			CString cserious;
			cserious.Format(_T("0000"));
			COleVariant vserious(cserious);   //严重缺陷
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vserious);
		}
		if (k == 6) {
			CString cout;
			cout.Format(_T("一级"));
			COleVariant vout(cout);   //检测结果
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vout);
		}
	}

	//写入瑕疵信息
	EnterCriticalSection(&pThis->m_csvec);
	for (int i = 0; i < (int)pThis->m_vecDFT.size(); i++)
	{
		DefectType dft;
		dft = pThis->m_vecDFT.at(i);
		for (int j = 1; j < 9; j++)
		{
			switch (j)
			{
			case 1: {
				COleVariant vResult((long)(i + 1));
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 2: {
				CString ckind;
				ckind.Format(_T("%d"), dft.type);
				COleVariant vResult(ckind);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 3: {
				CString cx;
				cx.Format(_T("%.2f"), dft.center_x);
				COleVariant vResult(cx);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 4: {
				CString cy;
				cy.Format(_T("%.2f"), dft.absolute_position);
				COleVariant vResult(cy);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 5: {
				CString cradius;
				cradius.Format(_T("%.2f"), dft.circle_radius);
				COleVariant vResult(cradius);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 6: {
				CString carea;
				carea.Format(_T("%.2f"), dft.area);
				COleVariant vResult(carea);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 7: {
				CString csize;
				csize.Format(_T("%.2f"), dft.contlength);
				COleVariant vResult(csize);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 8: {
				CString ctime;
				ctime.Format(_T("%.2f"), dft.pixel_value);
				COleVariant vResult(ctime);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			default:
				break;
			}
		}
	}
	LeaveCriticalSection(&pThis->m_csvec);

	range.AttachDispatch(sheet.get_UsedRange());//加载已使用的单元格
	range.put_WrapText(COleVariant((long)1));   //设置文本自动换行

	//5.设置对齐方式
	range.put_VerticalAlignment(COleVariant((long)-4108));
	range.put_HorizontalAlignment(COleVariant((long)-4108));
	//6.设置字体颜色
	CFont0 ft;
	ft.AttachDispatch(range.get_Font());
	ft.put_Name(COleVariant(_T("宋体")));	//字体
	ft.put_ColorIndex(COleVariant((long)1));//颜色	//黑色
	ft.put_Size(COleVariant((long)12));     //大小

	range.AttachDispatch(sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("H1"))), TRUE);
	ft.AttachDispatch(range.get_Font());
	ft.put_Name(COleVariant(_T("宋体")));
	ft.put_Bold(COleVariant((long)1));
	ft.put_ColorIndex(COleVariant((long)1));    //颜色	
	ft.put_Size(COleVariant((long)18));         //大小

	//*****************************************************创建瑕疵分布图**********************************/
	LPDISPATCH lpDisp;
	LPDISPATCH lpDispLast = sheets.get_Item(COleVariant(sheets.get_Count()));
	lpDisp = sheets.Add(vtMissing, _variant_t(lpDispLast), _variant_t((long)1), vtMissing);
	sheet.AttachDispatch(lpDisp);
	sheet.put_Name(L"瑕疵分布图");

	if (1)
	{
		//设置表头格式
		pThis->FormatTableHead(sheet, range, FALSE);

		//填充表头内容
		CString cnumber = pThis->m_wstr_num.c_str();     //批号
		COleVariant vnumber(cnumber);
		range.put_Item(COleVariant((long)2), COleVariant((long)2), vnumber);

		CString ctotal_time = pThis->m_wstr_savetime.c_str();     //检测时间
		COleVariant vtotal_time(ctotal_time);
		range.put_Item(COleVariant((long)2), COleVariant((long)5), vtotal_time);

		CString coperator = pThis->m_wstr_user.c_str();     //检测员
		COleVariant voperator(coperator);
		range.put_Item(COleVariant((long)2), COleVariant((long)8), voperator);

		CString cID = pThis->m_wstr_id.c_str();     //型号
		COleVariant vID(cID);
		range.put_Item(COleVariant((long)3), COleVariant((long)2), vID);

		CString clongth = pThis->m_wstr_width.c_str();     //检测长度
		COleVariant vlongth(clongth);
		range.put_Item(COleVariant((long)3), COleVariant((long)5), vlongth);

		CString cspeed = pThis->m_wstr_speed.c_str();     //平均速度
		COleVariant vspeed(cspeed);
		range.put_Item(COleVariant((long)3), COleVariant((long)8), vspeed);

		for (int k = 0; k < 7; k++)
		{
			if (k < 5) {
				COleVariant vkind_stastic((long)k);   //类型统计
				range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vkind_stastic);
			}
			if (k == 5) {
				CString cserious;
				cserious.Format(_T("0000"));
				COleVariant vserious(cserious);   //严重缺陷
				range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vserious);
			}
			if (k == 6) {
				CString cout;
				cout.Format(_T("一级"));
				COleVariant vout(cout);   //检测结果
				range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vout);
			}
		}

		//添加图片
		char path[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, (TCHAR*)path);//获取当前路径
		CString strBMP = (TCHAR*)path;
		CString imageName("\\temp\\saved.bmp");
		strBMP += imageName;
		std::ifstream fin(strBMP.GetBuffer());
		if (fin) {
			//【2】插入图像
			//获取图像插入的范围
			//从Sheet对象上获得一个Shapes   
			CShapes pShapes;
			pShapes.AttachDispatch(sheet.get_Shapes());
			//获得Range对象，用来插入图片
			range.AttachDispatch(sheet.get_Range(COleVariant(_T("A6")), COleVariant(_T("H50"))), TRUE);
			//range.Merge(COleVariant((long)0));  //合并单元格
			VARIANT rLeft = range.get_Left();
			VARIANT rTop = range.get_Top();
			VARIANT rWidth = range.get_Width();
			VARIANT rHeight = range.get_Height();

			//添加图像到 H1 - K10范围区域
			CShape pShape = pShapes.AddPicture(strBMP, TRUE, TRUE,
				(float)rLeft.dblVal, (float)rTop.dblVal, (float)rWidth.dblVal, (float)rHeight.dblVal);
			//设置图像所占的宽高
			CShapeRange shapeRange = pShapes.get_Range(_variant_t(long(1)));
			shapeRange.put_Height(float(600));
			shapeRange.put_Width(float(450));
		}

		range.AttachDispatch(sheet.get_UsedRange());//加载已使用的单元格
		range.put_WrapText(COleVariant((long)1));   //设置文本自动换行

		//5.设置对齐方式
		//水平对齐：默认 1 居中 -4108， 左= -4131，右=-4152
		//垂直对齐：默认 2 居中 -4108， 左= -4160，右=-4107
		range.put_VerticalAlignment(COleVariant((long)-4108));
		range.put_HorizontalAlignment(COleVariant((long)-4108));

		//6.设置字体颜色
		CFont0 ft;
		ft.AttachDispatch(range.get_Font());
		ft.put_Name(COleVariant(_T("宋体")));	//字体
		ft.put_ColorIndex(COleVariant((long)1));//颜色	//黑色
		ft.put_Size(COleVariant((long)12));     //大小

		range.AttachDispatch(sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("H1"))), TRUE);
		ft.AttachDispatch(range.get_Font());
		ft.put_Name(COleVariant(_T("宋体")));
		ft.put_Bold(COleVariant((long)1));
		ft.put_ColorIndex(COleVariant((long)1));    //颜色	
		ft.put_Size(COleVariant((long)18));         //大小


	}
	//**************************************************************************************************/

	Book.SaveCopyAs(COleVariant(strExcelFile)); //保存

	Book.put_Saved(TRUE);
	//8.释放资源
	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	Book.ReleaseDispatch();
	Books.ReleaseDispatch();
	Book.Close(covOptional, covOptional, covOptional);//关闭Workbook对象
	Books.Close();           // 关闭Workbooks对象
	App.Quit();          // 退出_Application
	App.ReleaseDispatch();

	pThis->m_vecDFT.clear();

	Win::log("报表已保存");

	return 0;
}

UINT CTableDlg::SaveTableThreadDefault(LPVOID pParam)
{
	//解决多线程打开excel的错误
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		CoInitialize(NULL);
	AfxEnableControlContainer();

	CTableDlg *pThis = (CTableDlg *)pParam;

	TCHAR excel_path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, excel_path);
	CString cpath = excel_path;
	cpath = cpath + L"\\temp\\example.xlsx";
	std::ifstream fexist(CT2A(cpath.GetBuffer()));
	if (!fexist) {
		CString cstr = L"打开Excel模板失败，请检查\\temp\\目录下文件example.xlsx是否存在";
		::SendMessage(pThis->hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return -1;
	}

	//1.创建基本对象
	CApplication App;                //创建应用程序实例
	CWorkbooks Books;                //工作簿，多个Excel文件
	CWorkbook Book_example, Book;    //单个工作簿
	CWorksheets sheets;              //多个sheet页面
	CWorksheet sheet;                //单个sheet页面
	CRange range;                    //操作单元格
	
	LPDISPATCH lpDisp;
	COleVariant vResult;
	COleVariant  covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (!App.CreateDispatch(L"Excel.Application")){
		CString cstr = L"打开Excel失败，请检查Excel状态";
		::SendMessage(pThis->hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return -1;
	}
	Books.AttachDispatch(App.get_Workbooks());
	//打开模板文件
	lpDisp = Books.Open(cpath, covOptional, covOptional, covOptional, covOptional, covOptional
		, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional);
	//得到得到视图workbook
	Book_example.AttachDispatch(lpDisp);

	//另存为
	CString strExcelFile = pThis->m_save_path.c_str();
	std::wstring strname;
	pThis->GenerateReportName(strname);
	CString strdevName(strname.c_str());
	strdevName += _T(".xlsx");
	strExcelFile += strdevName;
	COleVariant cvname(strExcelFile);
	Book_example.SaveAs(cvname, covOptional, covOptional, covOptional, covOptional, covOptional
		, 1, covOptional, covOptional, covOptional, covOptional, covOptional);

	//打开另存的文件
	lpDisp = Books.Open(strExcelFile, covOptional, covOptional, covOptional,
		covOptional, covOptional , covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional, covOptional,covOptional, covOptional);
	//得到得到视图workbook
	Book.AttachDispatch(lpDisp);
	//得到worksheets
	sheets.AttachDispatch(Book.get_Worksheets());
	//得到sheet
	sheet.AttachDispatch(sheets.get_Item(COleVariant((short)1)));	//获取sheet1
	range.AttachDispatch(sheet.get_UsedRange());

	//填充表头
	CString cnumber = pThis->m_wstr_num.c_str();     //批号
	COleVariant vnumber(cnumber);
	range.put_Item(COleVariant((long)2), COleVariant((long)2), vnumber);

	//时间格式化
	pThis->m_wstr_savetime.insert(4, L"年");
	pThis->m_wstr_savetime.insert(7, L"月");
	pThis->m_wstr_savetime.insert(10, L"日");
	pThis->m_wstr_savetime.insert(14, L"时");
	pThis->m_wstr_savetime.insert(17, L"分");
	pThis->m_wstr_savetime.insert(20, L"秒");
	CString ctotal_time = pThis->m_wstr_savetime.c_str();     //检测时间
	COleVariant vtotal_time(ctotal_time);
	range.put_Item(COleVariant((long)2), COleVariant((long)5), vtotal_time);

	CString coperator = pThis->m_wstr_user.c_str();     //检测员
	COleVariant voperator(coperator);
	range.put_Item(COleVariant((long)2), COleVariant((long)8), voperator);

	CString cID = pThis->m_wstr_id.c_str();     //型号
	COleVariant vID(cID);
	range.put_Item(COleVariant((long)3), COleVariant((long)2), vID);

	CString clongth = pThis->m_wstr_width.c_str();     //检测长度
	COleVariant vlongth(clongth);
	range.put_Item(COleVariant((long)3), COleVariant((long)5), vlongth);

	CString cspeed = pThis->m_wstr_speed.c_str();     //平均速度
	COleVariant vspeed(cspeed);
	range.put_Item(COleVariant((long)3), COleVariant((long)8), vspeed);
	for (int k = 0; k < 7; k++)
	{
		if (k < 5) {
			COleVariant vkind_stastic((long)pThis->m_DFT_rank[k]); 
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vkind_stastic);
		}
		if (k == 5) {
			COleVariant vserious((long)pThis->m_serious_num);  
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vserious);
		}
		if (k == 6) {
			CString cout = pThis->GenerateRankText(pThis->m_product_rank);
			COleVariant vout(cout); 
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vout);
		}
	}

	//写入瑕疵列表 sheet1
	EnterCriticalSection(&pThis->m_csvec);
	for (int i = 0; i < (int)pThis->m_vecDFT.size(); i++)
	{
		DefectType dft;
		dft = pThis->m_vecDFT.at(i);
		for (int j = 1; j < 9; j++)
		{
			switch (j)
			{
			case 1: {
				COleVariant vResult((long)(i + 1));
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 2: {
				CString ckind;
				ckind.Format(_T("%d"), dft.type);
				COleVariant vResult(ckind);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 3: {
				CString cx;
				cx.Format(_T("%.2f"), dft.center_x);
				COleVariant vResult(cx);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 4: {
				CString cy;
				cy.Format(_T("%.2f"), dft.absolute_position);
				COleVariant vResult(cy);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 5: {
				CString cradius;
				cradius.Format(_T("%.2f"), dft.circle_radius);
				COleVariant vResult(cradius);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 6: {
				CString carea;
				carea.Format(_T("%.2f"), dft.area);
				COleVariant vResult(carea);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 7: {
				CString csize;
				csize.Format(_T("%.2f"), dft.contlength);
				COleVariant vResult(csize);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			case 8: {
				CString ctime;
				ctime.Format(_T("%.2f"), dft.rank);
				COleVariant vResult(ctime);
				range.put_Item(COleVariant((long)(i + 7)), COleVariant((long)j), vResult);
				break; }
			default:
				break;
			}
		}
	}
	LeaveCriticalSection(&pThis->m_csvec);

	range.AttachDispatch(sheet.get_UsedRange());//加载已使用的单元格
	range.put_WrapText(COleVariant((long)1));   //设置文本自动换行

	//获取图表绘制的数据区域
	int row_num = (int)pThis->m_vecDFT.size() + 7;
	CString cend;
	cend.Format(_T("D%d"), row_num);
	LPDISPATCH lpDispXY = sheet.get_Range(COleVariant(_T("C7")), COleVariant(cend));

	//写入瑕疵分布图 sheet2
	sheet.AttachDispatch(sheets.get_Item(COleVariant((short)2)));
	//取得用户区
	range.AttachDispatch(sheet.get_UsedRange());
	range.put_Item(COleVariant((long)2), COleVariant((long)2), vnumber);
	range.put_Item(COleVariant((long)2), COleVariant((long)5), vtotal_time);
	range.put_Item(COleVariant((long)2), COleVariant((long)8), voperator);
	range.put_Item(COleVariant((long)3), COleVariant((long)2), vID);
	range.put_Item(COleVariant((long)3), COleVariant((long)5), vlongth);
	range.put_Item(COleVariant((long)3), COleVariant((long)8), vspeed);
	for (int k = 0; k < 7; k++)
	{
		if (k < 5) {
			COleVariant vkind_stastic((long)pThis->m_DFT_rank[k]);
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vkind_stastic);
		}
		if (k == 5) {
			COleVariant vserious((long)pThis->m_serious_num);
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vserious);
		}
		if (k == 6) {
			CString cout = pThis->GenerateRankText(pThis->m_product_rank);
			COleVariant vout(cout);
			range.put_Item(COleVariant((long)5), COleVariant((long)(k + 2)), vout);
		}
	}

	//生成散点图
	range.AttachDispatch(sheet.get_UsedRange());//加载已使用的单元格
	range.AttachDispatch(sheet.get_Range(COleVariant(_T("A7")), COleVariant(_T("H52"))), TRUE);
	VARIANT rLeft = range.get_Left();
	VARIANT rTop = range.get_Top();
	VARIANT rWidth = range.get_Width();
	VARIANT rHeight = range.get_Height();
	double fleft = (double)rLeft.dblVal + 10.0;
	double ftop = (double)rTop.dblVal + 10.0;
	double fwidth = (double)rWidth.dblVal - 20.0;
	double fheight = (double)rHeight.dblVal - 20.0;

	CChartObjects chartobjects;       //chart对象所在的容器
	CChartObject  chartobject;
	CChart        chart;                  //图表

	lpDisp = sheet.ChartObjects(covOptional);
	ASSERT(lpDisp);
	chartobjects.AttachDispatch(lpDisp);
	if (chartobjects.get_Count() != 0) //当excel中存在原有图表时，删除之
	{
		chartobjects.Delete();
	}
	chartobject = chartobjects.Add(fleft, ftop, fwidth, fheight);
	//设置图表区边框
	LPDISPATCH lpDispBorder = chartobject.get_Border();
	CBorder border;
	border.AttachDispatch(lpDispBorder);
	border.put_LineStyle(COleVariant((short)-4142));       // No line

	//散点图样式设置
	chart.AttachDispatch(chartobject.get_Chart());
	chart.put_ChartType(-4169);
	VARIANT var;
	var.vt = VT_DISPATCH;
	var.pdispVal = lpDispXY;
	chart.ChartWizard(var,// Source.
		COleVariant((short)-4169),// Gallery, 散点图
		COleVariant((short)3),// Format: 1~6.
		COleVariant((short)2),// PlotBy: 指定系列中的数据是来自行(1)还是来自列(2).
		COleVariant((short)1),// CategoryLabels.类别：第一行是分类标签
		COleVariant((short)2),// SeriesLabels.系列：第一列是系列标签
		COleVariant((short)FALSE),// HasLegend.
		COleVariant(L"缺陷分布图"),// Title.
		covOptional,// CategoryTitle.
		covOptional,// ValueTitles.
		covOptional  // ExtraTitle.
	);
	chart.put_ChartStyle(COleVariant((short)1));
	//设置网格线
	CAxis axis;
	CGridlines gridlines;
	lpDisp = chart.Axes(COleVariant((short)1), 1);  //Axis displays categories, X轴
	axis.AttachDispatch(lpDisp);
	axis.get_HasMajorGridlines();
	lpDisp = axis.get_MajorGridlines();
	gridlines.AttachDispatch(lpDisp);
	lpDisp = gridlines.get_Border();
	border.AttachDispatch(lpDisp);
	border.put_LineStyle(COleVariant((short)-4115));  //Dashed line

	lpDisp = chart.Axes(COleVariant((short)2), 1);  //Axis displays values ， Y轴
	axis.AttachDispatch(lpDisp);
	axis.get_HasMajorGridlines();
	lpDisp = axis.get_MajorGridlines();
	gridlines.AttachDispatch(lpDisp);
	lpDisp = gridlines.get_Border();
	border.AttachDispatch(lpDisp);
	border.put_LineStyle(COleVariant((short)-4115));  //Dashed line

	/*
	//添加图片
	char path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, (TCHAR*)path);//获取当前路径
	CString strBMP = (TCHAR*)path;
	CString imageName("\\temp\\saved.bmp");
	strBMP += imageName;
	std::ifstream fin(strBMP.GetBuffer());
	if (fin) {
		//【2】插入图像
		//获取图像插入的范围
		//从Sheet对象上获得一个Shapes   
		CShapes pShapes;
		pShapes.AttachDispatch(sheet.get_Shapes());
		//获得Range对象，用来插入图片
		range.AttachDispatch(sheet.get_Range(COleVariant(_T("A7")), COleVariant(_T("H52"))), TRUE);
		range.Merge(COleVariant((long)0));  //合并单元格
		VARIANT rLeft = range.get_Left();
		VARIANT rTop = range.get_Top();
		VARIANT rWidth = range.get_Width();
		VARIANT rHeight = range.get_Height();

		//添加图像到 A7 - H51范围区域
		CShape pShape = pShapes.AddPicture(strBMP, TRUE, TRUE,
			(float)rLeft.dblVal + 20.0f, (float)rTop.dblVal + 20.0f,
			(float)rWidth.dblVal - 40.0f, (float)rHeight.dblVal - 40.0f);
		////设置图像所占的宽高
		//CShapeRange shapeRange = pShapes.get_Range(_variant_t(long(1)));
		//shapeRange.put_Height(float(600));
		//shapeRange.put_Width(float(500));
	}
	else {
		CString cstr = L"插入瑕疵分布图失败，\\temp\\目录下未生成分布图";
		::SendMessage(pThis->hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
	}
	*/
	//*************************************************************

	Book.Save(); //保存
	Book.put_Saved(TRUE);

	//8.释放资源
	border.ReleaseDispatch();
	gridlines.ReleaseDispatch();
	axis.ReleaseDispatch();
	chart.ReleaseDispatch();
	chartobject.ReleaseDispatch();
	chartobjects.ReleaseDispatch();

	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	Book.ReleaseDispatch();
	Book_example.ReleaseDispatch();
	Books.ReleaseDispatch();
	Book.Close(covOptional, covOptional, covOptional);//关闭Workbook对象
	Book_example.Close(covOptional, covOptional, covOptional);
	Books.Close();           // 关闭Workbooks对象
	App.Quit();          // 退出_Application
	App.ReleaseDispatch();

	pThis->m_vecDFT.clear();

	CString cstr = L"报表已保存: " + strExcelFile;
	::SendMessage(pThis->hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	pThis->TableSaved_Event.SetEvent();

	return 0;
}

CString CTableDlg::GenerateRankText(int rank)
{
	CString crank;
	switch (rank)
	{
	case 0:
		crank.Format(_T("合格"));
		break;
	case 1:
		crank.Format(_T("A级"));
		break;
	case 2:
		crank.Format(_T("B级"));
		break;
	case 3:
		crank.Format(_T("C级"));
		break;
	case 4:
		crank.Format(_T("D级"));
		break;
	}
	return crank;
}

//查找目录下的所有文件
void getExcels(std::string path, std::vector<std::string>& files)
{
	//文件句柄
	intptr_t   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("*.xlsx").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getExcels(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				//files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				files.push_back(fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void CTableDlg::GetExcelsInfo(std::string filename, std::string &time, std::string &number, 
	std::string &ID, std::string &longth, std::string &operators)
{
	auto dis1 = filename.find("_N");
	auto dis2 = filename.find("_D");
	auto dis3 = filename.find("_W");
	auto dis4 = filename.find("_U");
	auto dis5 = filename.find(".xlsx");

	time      = filename.substr(0, dis1);
	number    = filename.substr(dis1 + 2, dis2 - dis1 - 2);
	ID        = filename.substr(dis2 + 2, dis3 - dis2 - 2);
	longth    = filename.substr(dis3 + 2, dis4 - dis3 - 2);
	operators = filename.substr(dis4 + 2, dis5 - dis4 - 2);

	return;
}

//打开excel文件
void CTableDlg::OpenExcelFile(std::wstring excelname)
{
		//1.创建基本对象
	CApplication App;  //创建应用程序实例
	CWorkbooks Books;  //工作簿，多个Excel文件
	CWorkbook Book;    //单个工作簿
	CWorksheets sheets;//多个sheet页面
	CWorksheet sheet;  //单个sheet页面
	CRange range;      //操作单元格
	COleVariant
		covTrue((short)TRUE),
		covFalse((short)FALSE),
		covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	LPDISPATCH lpdisp = NULL;
	//1.创建Excel实例
	if (!App.CreateDispatch(_T("Excel.Application"), NULL))
	{
		CString cstr = L"打开Excel失败，请检查Excel软件状态";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return;
	}
	App.put_Visible(TRUE);	//打开Excel, 也可设置为不打开
	App.put_UserControl(FALSE);

	Books.AttachDispatch(App.get_Workbooks());

	//2. 得到workbooks容器
	lpdisp = Books.Open(excelname.c_str(), covOptional, covOptional, covOptional, covOptional, covOptional
		, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional);

	Book.AttachDispatch(lpdisp);
	sheets.AttachDispatch(Book.get_Worksheets());
	lpdisp = Book.get_ActiveSheet();
	sheet.AttachDispatch(lpdisp);

	//取得用户区
	CRange userRange;
	userRange.AttachDispatch(sheet.get_UsedRange());
	//得到用户区的行数
	range.AttachDispatch(userRange.get_Rows());
	long rowNum = range.get_Count();
	//得到用户区的列数
	range.AttachDispatch(userRange.get_Columns());
	long colNum = range.get_Count();
	//得到用户区的开始行和开始列
	long startRow = userRange.get_Row();
	long startCol = userRange.get_Column();

	long startWriteRow = startRow + rowNum;
	long startWriteCol = startCol + colNum;


	//8.释放资源
	userRange.ReleaseDispatch();
	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	Book.ReleaseDispatch();
	Books.ReleaseDispatch();
	//Book.Close(covOptional, covOptional, covOptional);//关闭Workbook对象
	//Books.Close();           // 关闭Workbooks对象
	//App.Quit();          // 退出_Application
	//App.ReleaseDispatch();
}

//刷新
void CTableDlg::OnBnClickedBtnRefrush()
{
	// TODO: 在此添加控件通知处理程序代码
	CWaitCursor wait;

	//清除列表
	m_ListCtrlDetail.DeleteAllItems();

	Invalidate();
	UpdateWindow();

	CString cstr = L"瑕疵分布图已刷新";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

//查询
void CTableDlg::OnBnClickedButtonSearch()
{
	// TODO: 在此添加控件通知处理程序代码
	CWaitCursor wait;

	//SaveToExcel(*m_pvDFT);
	//CString excel_name;
	//SaveToExcelUseDefault(excel_name);

	//SaveScatterPlotUseDefault();

	CString cstr = L"查询到： 0 条记录";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

}

//全部显示
void CTableDlg::OnBnClickedButtonShowall()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ListCtrlHis.DeleteAllItems();
	//m_ListCtrlHis.InsertColumn(0,L"时间",LVCFMT_CENTER,120); 
	//m_ListCtrlHis.InsertColumn(1, L"批号", LVCFMT_CENTER, 120);
	//m_ListCtrlHis.InsertColumn(2, L"型号", LVCFMT_CENTER, 120);
	//m_ListCtrlHis.InsertColumn(3, L"长度", LVCFMT_CENTER, 80);
	//m_ListCtrlHis.InsertColumn(4, L"操作员", LVCFMT_CENTER, 80);

	std::string _path((LPCSTR)CW2A(m_save_path.c_str()));
	//搜索目录下的所有 xlsx 文件
	std::vector<std::string> vstring;
	getExcels(_path, vstring);
	if (vstring.empty()) return;
	m_vstring.clear();
	m_vstring = vstring;

	std::vector<std::string>::reverse_iterator it = vstring.rbegin();
	int vsize = (int)(std::distance(vstring.rbegin(), vstring.rend()));
	std::string excel_name;
	for (; it != vstring.rend(); it++)
	{
		excel_name = *it;
		std::string time, number, ID, longth, operators;
		GetExcelsInfo(excel_name, time, number, ID, longth, operators);
		//时间格式化
		time.insert(4, "-");
		time.insert(7, "-");
		time.insert(13, ":");
		time.insert(16, ":");

		int nItem = m_ListCtrlHis.InsertItem(0, CA2W(time.c_str()));
		m_ListCtrlHis.SetItemText(nItem, 1, CA2W(number.c_str()));
		m_ListCtrlHis.SetItemText(nItem, 2, CA2W(ID.c_str()));
		m_ListCtrlHis.SetItemText(nItem, 3, CA2W(longth.c_str()));
		m_ListCtrlHis.SetItemText(nItem, 4, CA2W(operators.c_str()));
	}

	CString cstr;
	cstr.Format(_T("已找到报表记录： %d 条"), vsize);
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

//程序窗口中打开
void CTableDlg::OnBnClickedButtonOpeninprogram()
{
	// TODO: 在此添加控件通知处理程序代码
}

//打开文件
void CTableDlg::OnBnClickedButtonOpenexcel()
{
	// TODO: 在此添加控件通知处理程序代码
	
		//得到文件路径
	int nIndex = m_ListCtrlHis.GetSelectionMark();
	if (nIndex == -1) return;
	m_ListCtrlHis.DeleteItem(nIndex);

	std::string filename((LPCSTR)CW2A(m_save_path.c_str()));
	std::string name;
	std::vector<std::string>::reverse_iterator it = m_vstring.rbegin();
	if (it + nIndex < m_vstring.rend()) {
		name = *(it + nIndex);
		filename.append(name);
	}
	else return;

	std::wstring excelname((LPCTSTR)CA2W(filename.c_str()));

	//打开 Excel 文件
	OpenExcelFile(excelname);

	CString cfilename = excelname.c_str();
	CString cstr = L"打开报表：" + cfilename;
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

}


//删除选中
void CTableDlg::OnBnClickedButtonDelselect()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_ListCtrlHis.GetSelectionMark();
	if (nIndex == -1) return;
	m_ListCtrlHis.DeleteItem(nIndex);

	std::string filename((LPCSTR)CW2A(m_save_path.c_str()));
	std::string name;
	std::vector<std::string>::reverse_iterator it = m_vstring.rbegin();
	if (it + nIndex < m_vstring.rend()) {
		name = *(it + nIndex);
		filename.append(name);
	}
	else return;

	//删除文件
	DeleteFileA(filename.c_str());

	CString cfilename = CA2W(filename.c_str());
	CString cstr = L"删除报表：" + cfilename;
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

//全部删除
void CTableDlg::OnBnClickedButtonDelall()
{
	// TODO: 在此添加控件通知处理程序代码

	CString cstr = L"全部删除：";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

}

//打印选中
void CTableDlg::OnBnClickedButtonPrint()
{
	// TODO: 在此添加控件通知处理程序代码

	CString cstr = L"打印报表：";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

}

//选中另存为
void CTableDlg::OnBnClickedButtonSaveas()
{
	// TODO: 在此添加控件通知处理程序代码

	CString cstr = L"报表另存为：";
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

}

//打开文件夹
void CTableDlg::OnBnClickedButtonOpenexcelpath()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string path((LPCSTR)CW2A(m_save_path.c_str()));
	CString cpath = CA2W(path.c_str());
	ShellExecute(NULL, L"explore", cpath, NULL, NULL, SW_SHOW);

	CString cstr = L"打开文件夹：" + cpath;
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

//双击ListBox某一瑕疵
void CTableDlg::OnNMDblclkListDetail(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (m_iSystemState == 2) return;

	CString image_name;
	int index = m_ListCtrlDetail.GetSelectionMark();

	if (!m_pvDFT->empty()) {
		int nsize = m_ListCtrlDetail.GetItemCount();
		DefectType dtype = m_pvDFT->at(nsize - index - 1);
		//标记的位置
		CRect rect;
		GetDlgItem(IDC_STATIC_REPORT)->GetClientRect(&rect);
		m_selected_x = (int)(dtype.center_x * scale_x);
		m_selected_y = (int)(rect.Height() - dtype.absolute_position * scale_y);

		//生成图像文件名
		HTuple hv_path = (HTuple)m_DFT_img_path.c_str();
		char cpos[16];
		sprintf_s(cpos, "%.3f", dtype.absolute_position);
		//HTuple hv_position = (HTuple)(std::to_string(dtype.absolute_position).c_str());
		HTuple hv_position = (HTuple)cpos;
		char cX[16];
		sprintf_s(cX, "%.3f", dtype.center_x);
		HTuple hv_X = (HTuple)cX;
		char cradius[16];
		sprintf_s(cradius, "%.3f", dtype.circle_radius);
		HTuple hv_radius = (HTuple)cradius;
		char carea[16];
		sprintf_s(carea, "%.3f", dtype.area);
		HTuple hv_harea = (HTuple)carea;
		HTuple hv_kind = (HTuple)dtype.type;
		HTuple hv_img_name = hv_path + "P" + hv_position
			+ "_X" + hv_X
			+ "_R" + hv_radius
			+ "_A" + hv_harea
			+ "_K" + hv_kind
			+ ".bmp";
		std::string name = hv_img_name.S();
		image_name = name.c_str();
	}

	CWnd *pwnd = GetDlgItem(IDC_STATIC_TABLE_DFT_IMAGE);
	ShowBitmap(pwnd, image_name);

	*pResult = 0;
}

//显示 .bmp 图像文件
void CTableDlg::ShowBitmap(CWnd *pWnd, CString BmpName)
{
	CRect rect;
	pWnd->GetClientRect(&rect);
	int width = rect.Width();
	int height = rect.Height();
	CDC* pDC = pWnd->GetDC();
	CBitmap m_bitmap;

	/*创建位图并调用函数LoadImage装载图标、光标或位图.
	/*定义bitmap指针 调用函数LoadImage装载位图
	/* 1.要装载OEM图像，则设此参数值为0  OBM_ OEM位图 OIC_OEM图标 OCR_OEM光标
	/* 2.BmpName要装载图片的文件名
	/* 3.装载图像类型:
	/*   IMAGE_BITMAP-装载位图 IMAGE_CURSOR-装载光标 IMAGE_ICON-装载图标
	/* 4.指定图标或光标的像素宽度和长度 以像素为单位
	/* 5.加载选项:
	/*   IR_LOADFROMFILE-指明由lpszName指定文件中加载图像
	/*   IR_DEFAULTSIZE-指明使用图像默认大小
	/*   LR_CREATEDIBSECTION-当uType参数为IMAGE_BITMAP时,创建一个DIB项
	/**************************************************************************/
	HBITMAP m_hBitmap;
	m_hBitmap = (HBITMAP)LoadImage(NULL, BmpName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	if (m_hBitmap == NULL)
		return;

	if (m_bitmap.m_hObject)
	{
		m_bitmap.Detach();           //切断CWnd和窗口联系  
	}
	m_bitmap.Attach(m_hBitmap);      //将句柄HBITMAP m_hBitmap与CBitmap m_bitmap关联  

	//定义并创建一个内存设备环境DC  
	CDC dcBmp;
	if (!dcBmp.CreateCompatibleDC(pDC))   //创建兼容性的DC  
		return;

	//定义BITMAP变量,调用函数GetBitmap将图片载入位图中,该定义是为获取图像的长宽等信息
	BITMAP m_bmp;                          //临时bmp图片变量  
	m_bitmap.GetBitmap(&m_bmp);            //将图片载入位图中  

	//调用函数SelectObject将位图选入兼容内存设备环境DC中.
	CBitmap* pbmpOld = dcBmp.SelectObject(&m_bitmap);         //将位图选入临时内存设备环境  

	pDC->SetStretchBltMode(STRETCH_HALFTONE);
	pDC->StretchBlt(0, 0, width, height, &dcBmp, 0, 0, m_bmp.bmWidth, m_bmp.bmHeight, SRCCOPY);

	//恢复临时DC的位图,删除CreateCompatibleDC得到的图片DC,删除内存中的位图及释放系统资源
	dcBmp.SelectObject(pbmpOld);           //恢复临时DC的位图  
	DeleteObject(&m_bitmap);               //删除内存中的位图  
	dcBmp.DeleteDC();                      //删除CreateCompatibleDC得到的图片DC  

}
