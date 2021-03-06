// CHistoryDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "CHistoryDlg.h"
#include "afxdialogex.h"

#include <algorithm>
#include <math.h>
#include <direct.h>
#include <io.h>
#include "Log.h"

//using namespace std;
bool cmp(const std::string &str1, const std::string &str2)
{
	auto pos1 = str1.find_first_of("_");
	std::string sub1 = str1.substr(1, pos1 - 1);
	double NO1 = std::stod(sub1);

	auto pos2 = str2.find_first_of("_");
	std::string sub2 = str2.substr(1, pos2 - 1);
	double NO2 = std::stod(sub2);

	return NO1 < NO2;
}

//查找目录下的所有文件
void getFiles(std::string path, std::vector<std::string>& files)
{
	//文件句柄
	intptr_t   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("*.bmp").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
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

// CHistoryDlg 对话框

IMPLEMENT_DYNAMIC(CHistoryDlg, CDialogEx)

CHistoryDlg::CHistoryDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HISTORY, pParent)
{
	m_pages = 0;
	m_bPagging = false;

}

CHistoryDlg::~CHistoryDlg()
{
}

void CHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HISTORY_IMG1, m_img1);
	DDX_Control(pDX, IDC_HISTORY_IMG2, m_img2);
	DDX_Control(pDX, IDC_HISTORY_IMG3, m_img3);
	DDX_Control(pDX, IDC_HISTORY_IMG4, m_img4);
	DDX_Control(pDX, IDC_HISTORY_IMG5, m_img5);
	DDX_Control(pDX, IDC_HISTORY_IMG6, m_img6);
	DDX_Control(pDX, IDC_HISTORY_IMG7, m_img7);
	DDX_Control(pDX, IDC_HISTORY_IMG8, m_img8);
	DDX_Control(pDX, IDC_HISTORY_IMG9, m_img9);
	DDX_Control(pDX, IDC_BUTTON_PRE_PAGE, m_btn_pre_page);
	DDX_Control(pDX, IDC_BUTTON_NEXT_PAGE, m_btn_next_page);
}

BEGIN_MESSAGE_MAP(CHistoryDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_PRE_PAGE, &CHistoryDlg::OnBnClickedButtonPrePage)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_PAGE, &CHistoryDlg::OnBnClickedButtonNextPage)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_OPENHISTORYPATH, &CHistoryDlg::OnBnClickedButtonOpenhistorypath)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CHistoryDlg 消息处理程序

BOOL CHistoryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	pwnd1 = GetDlgItem(IDC_HISTORY_IMG1);
	pwnd2 = GetDlgItem(IDC_HISTORY_IMG2);
	pwnd3 = GetDlgItem(IDC_HISTORY_IMG3);
	pwnd4 = GetDlgItem(IDC_HISTORY_IMG4);
	pwnd5 = GetDlgItem(IDC_HISTORY_IMG5);
	pwnd6 = GetDlgItem(IDC_HISTORY_IMG6);
	pwnd7 = GetDlgItem(IDC_HISTORY_IMG7);
	pwnd8 = GetDlgItem(IDC_HISTORY_IMG8);
	pwnd9 = GetDlgItem(IDC_HISTORY_IMG9);

	m_pages = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CHistoryDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
}

void CHistoryDlg::OnDestroy()
{
	CDialogEx::OnDestroy();


	// TODO: 在此处添加消息处理程序代码
	m_vImage_name.clear();

	m_bThreadAlive = false;

}

void CHistoryDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码

		// bShow = true 显示， bShow = false 隐藏
	if (bShow == TRUE) {
		m_pRefrushThread = AfxBeginThread(autoRefrush, this);
	}
	else {
		m_bThreadAlive = false;
	}
}


void CHistoryDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1) {
		KillTimer(1);
		m_pages = 0;
		m_bPagging = false;
	}

	CDialogEx::OnTimer(nIDEvent);
}

//上一页
void CHistoryDlg::OnBnClickedButtonPrePage()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_pages > 0)
		m_pages -= 1;

	m_bPagging = true;
	SetTimer(1, 5147, NULL);

	CString cstr;
	cstr.Format(_T("历史图像：第 %d 页"), m_pages);
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

//下一页
void CHistoryDlg::OnBnClickedButtonNextPage()
{
	// TODO: 在此添加控件通知处理程序代码

	// 向上取整
	int nums = (int)ceil((double)m_vImage_name.size() / 9.0);
	if (m_pages < nums - 1)
		m_pages += 1;

	m_bPagging = true;
	SetTimer(1, 5147, NULL);

	CString cstr;
	cstr.Format(_T("历史图像：第 %d 页"), m_pages);
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

//打开目录
void CHistoryDlg::OnBnClickedButtonOpenhistorypath()
{
	// TODO: 在此添加控件通知处理程序代码
	CString cpath = CA2W(m_file_path.c_str());
	ShellExecute(NULL, L"explore", cpath, NULL, NULL, SW_SHOW);

	CString cstr = L"打开历史图像目录：" + cpath;
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
}

//刷新历史窗口
void CHistoryDlg::RefrushImgWnd(std::string path, std::vector<std::string> vstring)
{
	CString cname;
	CString cimage_name, cposition, cradius, carea, ckind;
	for (int i = 1; i < 10; i++)
	{
		//name = GetImgName(path, i);
		std::string file;
		std::string name, position, radius, area, kind;
		if (vstring.empty())
			return;
		else {
			std::vector<std::string>::reverse_iterator it = vstring.rbegin();
			int vsize = (int)(std::distance(vstring.rbegin(), vstring.rend()));
			if (i + m_pages * 9 <= vsize) {
				name = *(it + i + m_pages * 9 - 1);
				file.append(path).append(name);
			}
			//从图像文件名解析出相应的信息
			ReadDFTInfo(name, kind, position, radius, area);

			cimage_name = name.c_str();
		}

		cname = file.c_str();
		cposition = position.c_str();
		cradius = radius.c_str();
		carea = area.c_str();
		int nKind = 0;
		if (kind.empty())
			return;
		else
			nKind = std::stoi(kind);
		switch (nKind)
		{
		case 0:
			ckind.Format(_T("异物"));
			break;
		case 1:
			ckind.Format(_T("凹凸"));
			break;
		case 2:
			ckind.Format(_T("气泡"));
			break;
		case 3:
			ckind.Format(_T("涂布"));
			break;
		case 4:
			ckind.Format(_T("其它"));
			break;
		}

		CStatic *stext;
		//判断图片名称是否有效
		if (strstr(file.c_str(), "_K") != NULL) {
			switch (i)
			{
			case 1:
				ShowBitmap(pwnd1, cname);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME1);
				stext->SetWindowTextW(cimage_name);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND1);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS1);
				stext->SetWindowTextW(cposition);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS1);
				stext->SetWindowTextW(cradius);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA1);
				stext->SetWindowTextW(carea);
				break;
			case 2:
				ShowBitmap(pwnd2, cname);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME2);
				stext->SetWindowTextW(cimage_name);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND2);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS2);
				stext->SetWindowTextW(cposition);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS2);
				stext->SetWindowTextW(cradius);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA2);
				stext->SetWindowTextW(carea);

				break;
			case 3:
				ShowBitmap(pwnd3, cname);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME3);
				stext->SetWindowTextW(cimage_name);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND3);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS3);
				stext->SetWindowTextW(cposition);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS3);
				stext->SetWindowTextW(cradius);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA3);
				stext->SetWindowTextW(carea);

				break;
			case 4:
				ShowBitmap(pwnd4, cname);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME4);
				stext->SetWindowTextW(cimage_name);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND4);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS4);
				stext->SetWindowTextW(cposition);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS4);
				stext->SetWindowTextW(cradius);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA4);
				stext->SetWindowTextW(carea);

				break;
			case 5:
				ShowBitmap(pwnd5, cname);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME5);
				stext->SetWindowTextW(cimage_name);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND5);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS5);
				stext->SetWindowTextW(cposition);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS5);
				stext->SetWindowTextW(cradius);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA5);
				stext->SetWindowTextW(carea);

				break;
			case 6:
				ShowBitmap(pwnd6, cname);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME6);
				stext->SetWindowTextW(cimage_name);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND6);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS6);
				stext->SetWindowTextW(cposition);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS6);
				stext->SetWindowTextW(cradius);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA6);
				stext->SetWindowTextW(carea);

				break;
			case 7:
				ShowBitmap(pwnd7, cname);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME7);
				stext->SetWindowTextW(cimage_name);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND7);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS7);
				stext->SetWindowTextW(cposition);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS7);
				stext->SetWindowTextW(cradius);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA7);
				stext->SetWindowTextW(carea);

				break;
			case 8:
				ShowBitmap(pwnd8, cname);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME8);
				stext->SetWindowTextW(cimage_name);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND8);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS8);
				stext->SetWindowTextW(cposition);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS8);
				stext->SetWindowTextW(cradius);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA8);
				stext->SetWindowTextW(carea);

				break;
			case 9:
				ShowBitmap(pwnd9, cname);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME9);
				stext->SetWindowTextW(cimage_name);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND9);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS9);
				stext->SetWindowTextW(cposition);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS9);
				stext->SetWindowTextW(cradius);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA9);
				stext->SetWindowTextW(carea);

				break;
			default:
				break;
			}
		}
	}
}

void CHistoryDlg::ReadDFTInfo(std::string name, std::string &kind, std::string &position,
	std::string &radius, std::string &area)
{
	auto dis1 = name.find("P");
	auto dis2 = name.find("X");
	auto dis3 = name.find("R");
	auto dis4 = name.find("A");
	auto dis5 = name.find("K");

	kind = name.substr(dis5 + 1, 1);
	position = name.substr(dis1 + 1, dis2 - dis1 - 2);
	radius = name.substr(dis3 + 1, dis4 - dis3 - 2);
	area = name.substr(dis4 + 1, dis5 - dis4 - 2);

	return;
}

void CHistoryDlg::RefrushHistoryWnd(int page_index)
{
	if (m_vImage_name.empty())
		return;

	std::vector<std::string>::reverse_iterator it;
	it = m_vImage_name.rbegin() + 9 * page_index;
	auto current_size = std::distance(it, m_vImage_name.rend());
	int rounds = 0;
	if (current_size >= 9)
		rounds = 9;
	else
		rounds = (int)current_size;
	for (int index = 1; index < rounds + 1; index++) {
		std::string temp = *it;
		it++;
		//从图像文件名解析出相应的信息
		std::string strimgname = m_file_path + temp;
		std::string position, radius, area, kind;
		ReadDFTInfo(temp, kind, position, radius, area);

		int nKind = std::stoi(kind);
		CString ckind;
		switch (nKind)
		{
		case 0:
			ckind.Format(_T("异物"));
			break;
		case 1:
			ckind.Format(_T("凹凸"));
			break;
		case 2:
			ckind.Format(_T("气泡"));
			break;
		case 3:
			ckind.Format(_T("涂布"));
			break;
		case 4:
			ckind.Format(_T("其它"));
			break;
		}

		CStatic *stext;
		//判断图片名称是否有效
		if (strstr(strimgname.c_str(), "_K") != NULL) {
			switch (index)
			{
			case 1:
				ShowBitmap(pwnd1, strimgname.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME1);
				stext->SetWindowTextW((CA2W)temp.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND1);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS1);
				stext->SetWindowTextW((CA2W)position.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS1);
				stext->SetWindowTextW((CA2W)radius.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA1);
				stext->SetWindowTextW((CA2W)area.c_str());
				break;
			case 2:
				ShowBitmap(pwnd2, strimgname.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME2);
				stext->SetWindowTextW((CA2W)temp.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND2);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS2);
				stext->SetWindowTextW((CA2W)position.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS2);
				stext->SetWindowTextW((CA2W)radius.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA2);
				stext->SetWindowTextW((CA2W)area.c_str());
				break;
			case 3:
				ShowBitmap(pwnd3, strimgname.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME3);
				stext->SetWindowTextW((CA2W)temp.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND3);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS3);
				stext->SetWindowTextW((CA2W)position.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS3);
				stext->SetWindowTextW((CA2W)radius.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA3);
				stext->SetWindowTextW((CA2W)area.c_str());
				break;
			case 4:
				ShowBitmap(pwnd4, strimgname.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME4);
				stext->SetWindowTextW((CA2W)temp.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND4);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS4);
				stext->SetWindowTextW((CA2W)position.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS4);
				stext->SetWindowTextW((CA2W)radius.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA4);
				stext->SetWindowTextW((CA2W)area.c_str());
				break;
			case 5:
				ShowBitmap(pwnd5, strimgname.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME5);
				stext->SetWindowTextW((CA2W)temp.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND5);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS5);
				stext->SetWindowTextW((CA2W)position.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS5);
				stext->SetWindowTextW((CA2W)radius.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA5);
				stext->SetWindowTextW((CA2W)area.c_str());
				break;
			case 6:
				ShowBitmap(pwnd6, strimgname.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME6);
				stext->SetWindowTextW((CA2W)temp.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND6);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS6);
				stext->SetWindowTextW((CA2W)position.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS6);
				stext->SetWindowTextW((CA2W)radius.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA6);
				stext->SetWindowTextW((CA2W)area.c_str());
				break;
			case 7:
				ShowBitmap(pwnd7, strimgname.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME7);
				stext->SetWindowTextW((CA2W)temp.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND7);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS7);
				stext->SetWindowTextW((CA2W)position.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS7);
				stext->SetWindowTextW((CA2W)radius.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA7);
				stext->SetWindowTextW((CA2W)area.c_str());
				break;
			case 8:
				ShowBitmap(pwnd8, strimgname.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME8);
				stext->SetWindowTextW((CA2W)temp.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND8);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS8);
				stext->SetWindowTextW((CA2W)position.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS8);
				stext->SetWindowTextW((CA2W)radius.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA8);
				stext->SetWindowTextW((CA2W)area.c_str());
				break;
			case 9:
				ShowBitmap(pwnd9, strimgname.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_NAME9);
				stext->SetWindowTextW((CA2W)temp.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_KIND9);
				stext->SetWindowTextW(ckind);
				stext = (CStatic*)GetDlgItem(IDC_STATIC_POS9);
				stext->SetWindowTextW((CA2W)position.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_RADIUS9);
				stext->SetWindowTextW((CA2W)radius.c_str());
				stext = (CStatic*)GetDlgItem(IDC_STATIC_AREA9);
				stext->SetWindowTextW((CA2W)area.c_str());
				break;
			default:
				break;
			}
		}
	}
}

BOOL CHistoryDlg::LoadHistoryImage()
{
	////std::string file_path = m_strPath.substr(0, m_strPath.length() - 2);
	//std::string _path = path;
	//m_file_path = path;

	//排序
	std::vector<std::string> vstring;
	getFiles(m_file_path, vstring);
	sort(vstring.begin(), vstring.end(), cmp);
	m_vImage_name = vstring;

	m_pages = 0;
	RefrushImgWnd(m_file_path, m_vImage_name);

	return TRUE;
}

//显示 .bmp 图像文件
void CHistoryDlg::ShowBitmap(CWnd *pWnd, CString BmpName)
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
	if (m_hBitmap == NULL) {
		CString cstr = L"加载历史图像失败，图像名：" + BmpName;
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return;
	}

	if (m_bitmap.m_hObject)
	{
		m_bitmap.Detach();           //切断CWnd和窗口联系
	}
	m_bitmap.Attach(m_hBitmap);      //将句柄HBITMAP m_hBitmap与CBitmap m_bitmap关联

	//定义并创建一个内存设备环境DC
	CDC dcBmp;
	if (!dcBmp.CreateCompatibleDC(pDC)) {
		CString cstr = L"加载历史图像失败，创建内存DC失败";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return;
	}

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

void CHistoryDlg::ShowBitmap(CWnd *pWnd, const char* BmpName)
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
	m_hBitmap = (HBITMAP)LoadImage(NULL, (CA2W)BmpName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	if (m_hBitmap == NULL) {
		CString cstr = L"加载历史图像失败，图像名：" + CString(BmpName);
		//::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return;
	}

	if (m_bitmap.m_hObject)
	{
		m_bitmap.Detach();           //切断CWnd和窗口联系
	}
	m_bitmap.Attach(m_hBitmap);      //将句柄HBITMAP m_hBitmap与CBitmap m_bitmap关联

	//定义并创建一个内存设备环境DC
	CDC dcBmp;
	if (!dcBmp.CreateCompatibleDC(pDC)) {
		CString cstr = L"加载历史图像失败，创建内存DC失败";
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return;
	}

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

UINT CHistoryDlg::autoRefrush(LPVOID pParam)
{
	CHistoryDlg* pThis = (CHistoryDlg*)pParam;
	pThis->m_bThreadAlive = true;

	std::string strpath = pThis->m_file_path;
	while (pThis->m_bThreadAlive)
	{
		if (!pThis->m_bPagging) {
			pThis->m_vImage_name.clear();
			std::vector<std::string> vstring;
			getFiles(strpath, vstring);        //查找目录下的所有文件
			sort(vstring.begin(), vstring.end(), cmp);    //排序
			pThis->m_vImage_name = vstring;
		}
		
		pThis->RefrushHistoryWnd(pThis->m_pages);
		if (pThis->m_pages == 0)
			pThis->GetDlgItem(IDC_BUTTON_PRE_PAGE)->EnableWindow(false);
		else
			pThis->GetDlgItem(IDC_BUTTON_PRE_PAGE)->EnableWindow(true);


		if (!pThis->m_bPagging)
			Sleep(1543);
		else
			Sleep(100);
	}

	pThis->m_bThreadAlive = false;

	return 0;
}

