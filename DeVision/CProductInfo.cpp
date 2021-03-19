// CProductInfo.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "DeVisionDlg.h"
#include "CProductInfo.h"
#include "afxdialogex.h"
#include <string>
#include <cstring>


// CProductInfo 对话框

IMPLEMENT_DYNAMIC(CProductInfo, CDialogEx)

CProductInfo::CProductInfo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PRODUCTINFO, pParent)
{
	m_nBatchIndex = 0;

}

CProductInfo::~CProductInfo()
{
}

void CProductInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProductInfo, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CProductInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CProductInfo::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_INDEX_CHANGE, &CProductInfo::OnBnClickedBtnIndexChange)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, &CProductInfo::OnEnKillfocusEditInfoName)
	ON_EN_KILLFOCUS(IDC_EDIT_INFO_NO, &CProductInfo::OnEnKillfocusEditInfoNo)
	ON_EN_KILLFOCUS(IDC_EDIT_ADDITION, &CProductInfo::OnEnKillfocusEditAddition)
	ON_EN_KILLFOCUS(IDC_EDIT_SCHEDULE, &CProductInfo::OnEnKillfocusEditSchedule)
	ON_EN_KILLFOCUS(IDC_EDIT_NO_INDEX, &CProductInfo::OnEnKillfocusEditNoIndex)
END_MESSAGE_MAP()

// CProductInfo 消息处理程序

BOOL CProductInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	ASSERT(pSysMenu != NULL);
	pSysMenu->EnableMenuItem(SC_CLOSE, MF_DISABLED);


	//加载配置文件
	//LoadInifile();


	UpdateControls();
	   
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CProductInfo::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (AfxMessageBox(_T("请确定卷号等信息，确定切卷？"), MB_YESNO | MB_ICONWARNING) == IDNO) {
	//	//m_bSave_Parameter = FALSE;
	//	return;
	//}
	//else {
	//	m_bSave_Parameter = TRUE;
	//	m_cstrBatch = GenerateBatchNumber();

	//	CString cstr = L"当前卷：" + m_cstrBatch;
	//	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	//}

	CDialogEx::OnClose();
}


void CProductInfo::UpdateControls()
{
	UpdateData(false);
	   
	GetDlgItem(IDC_EDIT_INFO_NO)->SetWindowTextW(m_cstrBatchCode);
	CString ctext;
	ctext.Format(_T("%.4d"), m_nBatchIndex);
	GetDlgItem(IDC_EDIT_NO_INDEX)->SetWindowTextW(ctext);
	GetDlgItem(IDC_EDIT_NAME)->SetWindowTextW(m_cstrName);
	GetDlgItem(IDC_EDIT_SCHEDULE)->SetWindowTextW(m_cstrSchedule);
	GetDlgItem(IDC_EDIT_ADDITION)->SetWindowTextW(m_cstrAddition);

	ctext = GenerateBatchNumber();
	GetDlgItem(IDC_STATIC_EXAMPLE)->SetWindowTextW(ctext);

	UpdateData(true);
}


void CProductInfo::LoadInifile()
{
	//LPWSTR ReturnedString = new wchar_t[STRINGLENGTH];
	//CString ckeyname, cvalue;
	//int index;

	/*
	//产品型号
	for (int i = 1; i < 11; i++)
	{
		ckeyname.Format(_T("ID%d"), 11 - i);
		GetPrivateProfileStringW(APPNAME, ckeyname, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		m_combox_id.InsertString(0, ReturnedString);
	}
	index = GetPrivateProfileInt(APPNAME, L"ID_SELECT", 0, FILEPATH);
	m_combox_id.SetCurSel(index);
	m_combox_id.GetWindowTextW(cvalue);
	m_ctrID = cvalue;

	//薄膜宽度
	for (int i = 1; i < 11; i++)
	{
		ckeyname.Format(_T("WIDTH%d"), 11 - i);
		GetPrivateProfileStringW(APPNAME, ckeyname, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		m_combox_width.InsertString(0, ReturnedString);
	}
	index = GetPrivateProfileInt(APPNAME, L"WIDTH_SELECT", 0, FILEPATH);
	m_combox_width.SetCurSel(index);
	m_combox_width.GetWindowTextW(cvalue);
	m_ctrWIDTH = cvalue;

	//基材
	for (int i = 1; i < 11; i++)
	{
		ckeyname.Format(_T("BASE%d"), 11 - i);
		GetPrivateProfileStringW(APPNAME, ckeyname, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		m_combox_base.InsertString(0, ReturnedString);
	}
	index = GetPrivateProfileInt(APPNAME, L"BASE_SELECT", 0, FILEPATH);
	m_combox_base.SetCurSel(index);
	m_combox_base.GetWindowTextW(cvalue);
	m_ctrBASE = cvalue;

	//涂层
	for (int i = 1; i < 11; i++)
	{
		ckeyname.Format(_T("COATING%d"), 11 - i);
		GetPrivateProfileStringW(APPNAME, ckeyname, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		m_combox_coating.InsertString(0, ReturnedString);
	}
	index = GetPrivateProfileInt(APPNAME, L"COATING_SELECT", 0, FILEPATH);
	m_combox_coating.SetCurSel(index);
	m_combox_coating.GetWindowTextW(cvalue);
	m_ctrCOATING = cvalue;

	//操作员
	for (int i = 1; i < 11; i++)
	{
		ckeyname.Format(_T("OPERATOR%d"), 11 - i);
		GetPrivateProfileStringW(APPNAME, ckeyname, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		m_combox_operator.InsertString(0, ReturnedString);
	}
	index = GetPrivateProfileInt(APPNAME, L"OPERATOR_SELECT", 0, FILEPATH);
	m_combox_operator.SetCurSel(index);
	m_combox_operator.GetWindowTextW(cvalue);
	m_ctrOPERATOR = cvalue;

	//生产批号
	GetPrivateProfileStringW(APPNAME, L"NUMBER", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	GetDlgItem(IDC_EDIT_NUMBER)->SetWindowTextW(ReturnedString);
	m_ctrNUMBER = ReturnedString;

	//客户
	GetPrivateProfileStringW(APPNAME, L"USER", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	GetDlgItem(IDC_EDIT_USER)->SetWindowTextW(ReturnedString);
	m_ctrUSER = ReturnedString;

	//附加信息
	GetPrivateProfileStringW(APPNAME, L"ADDITION", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	GetDlgItem(IDC_EDIT_ADDITION)->SetWindowTextW(ReturnedString);
	m_ctrADDITION = ReturnedString;

	delete[] ReturnedString;
	*/
}


void CProductInfo::SaveAll()
{
	UpdateData(true);

	//CString ckeyname, cvalue;
	//int index;

	/*
	//产品型号
	index = m_combox_id.GetCurSel();
	cvalue.Format(_T("%d"), index);
	WritePrivateProfileStringW(APPNAME, L"ID_SELECT", cvalue, FILEPATH);
	ckeyname.Format(_T("ID%d"), index + 1);
	GetDlgItem(IDC_EDIT_INFO_ID)->GetWindowTextW(cvalue);
	if (cvalue != _T("")) {
		m_ctrID = cvalue;
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);
	}
	else {
		m_combox_id.GetWindowTextW(cvalue);
		m_ctrID = cvalue;
	}

	//CString cstr;
	//cstr.Format(_T("index = %d, keyname = %s, value = %s"), index + 1, ckeyname, cvalue);
	//::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	//薄膜宽度
	index = m_combox_width.GetCurSel();
	cvalue.Format(_T("%d"), index);
	WritePrivateProfileStringW(APPNAME, L"WIDTH_SELECT", cvalue, FILEPATH);
	ckeyname.Format(_T("WIDTH%d"), index + 1);
	GetDlgItem(IDC_EDIT_INFO_WIDTH)->GetWindowTextW(cvalue);
	if (cvalue != _T("")) {
		m_ctrWIDTH = cvalue;
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);
	}
	else {
		m_combox_width.GetWindowTextW(cvalue);
		m_ctrWIDTH = cvalue;
	}

	//基膜材料
	index = m_combox_base.GetCurSel();
	cvalue.Format(_T("%d"), index);
	WritePrivateProfileStringW(APPNAME, L"BASE_SELECT", cvalue, FILEPATH);
	ckeyname.Format(_T("BASE%d"), index + 1);
	GetDlgItem(IDC_EDIT_INFO_BASE)->GetWindowTextW(cvalue);
	if (cvalue != _T("")) {
		m_ctrBASE = cvalue;
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);
	}
	else {
		m_combox_base.GetWindowTextW(cvalue);
		m_ctrBASE = cvalue;
	}

	//涂层材料
	index = m_combox_coating.GetCurSel();
	cvalue.Format(_T("%d"), index);
	WritePrivateProfileStringW(APPNAME, L"COATING_SELECT", cvalue, FILEPATH);
	ckeyname.Format(_T("COATING%d"), index + 1);
	GetDlgItem(IDC_EDIT_INFO_COATING)->GetWindowTextW(cvalue);
	if (cvalue != _T("")) {
		m_ctrCOATING = cvalue;
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);
	}
	else {
		m_combox_coating.GetWindowTextW(cvalue);
		m_ctrCOATING = cvalue;
	}

	//操作员
	index = m_combox_operator.GetCurSel();
	cvalue.Format(_T("%d"), index);
	WritePrivateProfileStringW(APPNAME, L"OPERATOR_SELECT", cvalue, FILEPATH);
	ckeyname.Format(_T("OPERATOR%d"), index + 1);
	GetDlgItem(IDC_EDIT_INFO_OPERATOR)->GetWindowTextW(cvalue);
	if (cvalue != _T("")) {
		m_ctrOPERATOR = cvalue;
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);
	}
	else {
		m_combox_operator.GetWindowTextW(cvalue);
		m_ctrOPERATOR = cvalue;
	}

	//生产批号
	GetDlgItem(IDC_EDIT_NUMBER)->GetWindowTextW(cvalue);
	m_ctrNUMBER = cvalue;
	ckeyname.Format(_T("NUMBER"));
	if (cvalue != _T(""))
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);

	//客户信息
	GetDlgItem(IDC_EDIT_USER)->GetWindowTextW(cvalue);
	m_ctrUSER = cvalue;
	ckeyname.Format(_T("USER"));
	if (cvalue != _T(""))
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);

	//附加信息
	GetDlgItem(IDC_EDIT_ADDITION)->GetWindowTextW(cvalue);
	m_ctrADDITION = cvalue;
	ckeyname.Format(_T("ADDITION"));
	if (cvalue != _T(""))
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);
	*/
}


CString CProductInfo::GetTimeStamp()
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
	const std::wstring wdate = date.str();
	const wchar_t* wname = wdate.c_str();
	_bstr_t name(wname);
	std::string str_time = name;
	std::string str_subtime = str_time.substr(2);
	CString time = (CA2W)str_subtime.c_str();

	return time;
}


CString CProductInfo::GenerateBatchNumber()
{
	UpdateData(false);
	CString cstrTime = GetTimeStamp();

	GetDlgItem(IDC_EDIT_INFO_NO)->GetWindowTextW(m_cstrBatchCode);

	CString cstrIndex;
	GetDlgItem(IDC_EDIT_NO_INDEX)->GetWindowTextW(cstrIndex);
	m_nBatchIndex = std::stoi(cstrIndex.GetBuffer());

	CString cstrBatch = cstrTime + L"_" + m_cstrBatchCode + L"_" + cstrIndex;

	return cstrBatch;
}

//  确定
void CProductInfo::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	if (AfxMessageBox(_T("请确定卷号等信息，确定切卷？"), MB_YESNO | MB_ICONWARNING) == IDNO) {
		//m_bSave_Parameter = FALSE;
		return;
	}
	else {
		m_bSave_Parameter = TRUE;
		m_cstrBatch = GenerateBatchNumber();

		CString cstr = L"当前卷：" + m_cstrBatch;
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}
	   	 

	CDialogEx::OnOK();
}

//  取消
void CProductInfo::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bSave_Parameter = FALSE;
	//AfxMessageBox(L"cancle");

	CDialogEx::OnCancel();
}

// 批号  修改序号
void CProductInfo::OnBnClickedBtnIndexChange()
{
	// TODO: 在此添加控件通知处理程序代码

	GetDlgItem(IDC_EDIT_NO_INDEX)->EnableWindow(true);

	CString ctext = GenerateBatchNumber();
	GetDlgItem(IDC_STATIC_EXAMPLE)->SetWindowTextW(ctext);
}

// 批号   修改代码
void CProductInfo::OnEnKillfocusEditInfoNo()
{
	// TODO: 在此添加控件通知处理程序代码

	GetDlgItem(IDC_EDIT_INFO_NO)->GetWindowTextW(m_cstrBatchCode);

}

//        序号
void CProductInfo::OnEnKillfocusEditNoIndex()
{
	// TODO: 在此添加控件通知处理程序代码
	CString ctext;
	GetDlgItem(IDC_EDIT_NO_INDEX)->GetWindowTextW(ctext);
	m_nBatchIndex = std::stoi(ctext.GetBuffer());

	ctext.Format(_T("%.4d"), m_nBatchIndex);
	GetDlgItem(IDC_EDIT_NO_INDEX)->SetWindowTextW(ctext);
}

//  产品名称
void CProductInfo::OnEnKillfocusEditInfoName()
{
	// TODO: 在此添加控件通知处理程序代码

	GetDlgItem(IDC_EDIT_NAME)->GetWindowTextW(m_cstrName);

}

//  班次
void CProductInfo::OnEnKillfocusEditSchedule()
{
	// TODO: 在此添加控件通知处理程序代码

	GetDlgItem(IDC_EDIT_SCHEDULE)->GetWindowTextW(m_cstrSchedule);

}


void CProductInfo::OnEnKillfocusEditAddition()
{
	// TODO: 在此添加控件通知处理程序代码

	GetDlgItem(IDC_EDIT_ADDITION)->GetWindowTextW(m_cstrAddition);

}

