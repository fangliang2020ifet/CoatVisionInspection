﻿// CProductInfo.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "CProductInfo.h"
#include "afxdialogex.h"



// CProductInfo 对话框

IMPLEMENT_DYNAMIC(CProductInfo, CDialogEx)

CProductInfo::CProductInfo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PRODUCTINFO, pParent)
{

}

CProductInfo::~CProductInfo()
{


}

void CProductInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ID, m_combox_id);
	DDX_Control(pDX, IDC_COMBO_WIDTH, m_combox_width);
	DDX_Control(pDX, IDC_COMBO_BASE, m_combox_base);
	DDX_Control(pDX, IDC_COMBO_COATING, m_combox_coating);
	DDX_Control(pDX, IDC_COMBO_OPERATOR, m_combox_operator);
}


BEGIN_MESSAGE_MAP(CProductInfo, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CProductInfo::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CProductInfo::OnBnClickedCancel)
END_MESSAGE_MAP()


// CProductInfo 消息处理程序

BOOL CProductInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	//加载配置文件
	LoadInifile();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CProductInfo::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (AfxMessageBox(_T("是否保存？"), MB_YESNO | MB_ICONWARNING) == IDYES) {
		SaveAll();
	}

	CDialogEx::OnClose();
}

void CProductInfo::LoadInifile()
{
	LPWSTR ReturnedString = new wchar_t[STRINGLENGTH];
	CString ckeyname;
	int index;

	//产品型号
	for (int i = 1; i < 11; i++)
	{
		ckeyname.Format(_T("ID%d"), 11 - i);
		GetPrivateProfileStringW(APPNAME, ckeyname, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		m_combox_id.InsertString(0, ReturnedString);
	}
	index = GetPrivateProfileInt(APPNAME, L"ID_SELECT", 0, FILEPATH);
	m_combox_id.SetCurSel(index);

	//薄膜宽度
	for (int i = 1; i < 11; i++)
	{
		ckeyname.Format(_T("WIDTH%d"), 11 - i);
		GetPrivateProfileStringW(APPNAME, ckeyname, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		m_combox_width.InsertString(0, ReturnedString);
	}
	index = GetPrivateProfileInt(APPNAME, L"WIDTH_SELECT", 0, FILEPATH);
	m_combox_width.SetCurSel(index);

	//基材
	for (int i = 1; i < 11; i++)
	{
		ckeyname.Format(_T("BASE%d"), 11 - i);
		GetPrivateProfileStringW(APPNAME, ckeyname, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		m_combox_base.InsertString(0, ReturnedString);
	}
	index = GetPrivateProfileInt(APPNAME, L"BASE_SELECT", 0, FILEPATH);
	m_combox_base.SetCurSel(index);

	//涂层
	for (int i = 1; i < 11; i++)
	{
		ckeyname.Format(_T("COATING%d"), 11 - i);
		GetPrivateProfileStringW(APPNAME, ckeyname, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		m_combox_coating.InsertString(0, ReturnedString);
	}
	index = GetPrivateProfileInt(APPNAME, L"COATING_SELECT", 0, FILEPATH);
	m_combox_coating.SetCurSel(index);

	//操作员
	for (int i = 1; i < 11; i++)
	{
		ckeyname.Format(_T("OPERATOR%d"), 11 - i);
		GetPrivateProfileStringW(APPNAME, ckeyname, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		m_combox_operator.InsertString(0, ReturnedString);
	}
	index = GetPrivateProfileInt(APPNAME, L"OPERATOR_SELECT", 0, FILEPATH);
	m_combox_operator.SetCurSel(index);

	//生产批号
	CEdit* pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NUMBER);
	GetPrivateProfileStringW(APPNAME, L"NUMBER", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	pEdit->SetWindowTextW(ReturnedString);

	//客户
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_USER);
	GetPrivateProfileStringW(APPNAME, L"USER", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	pEdit->SetWindowTextW(ReturnedString);

	//附加信息
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ADDITION);
	GetPrivateProfileStringW(APPNAME, L"ADDITION", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	pEdit->SetWindowTextW(ReturnedString);

	delete[] ReturnedString;
}


void CProductInfo::SaveAll()
{
	UpdateData(true);

	CString ckeyname, cvalue;
	int index;

	//产品型号
	index = m_combox_id.GetCurSel();
	ckeyname.Format(_T("ID%d"), index + 1);
	GetDlgItem(IDC_EDIT_INFO_ID)->GetWindowTextW(cvalue);
	if(cvalue != _T(""))
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);	

	//CString cstr;
	//cstr.Format(_T("index = %d, keyname = %s, value = %s"), index + 1, ckeyname, cvalue);
	//::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

	//薄膜宽度
	index = m_combox_width.GetCurSel();
	ckeyname.Format(_T("WIDTH%d"), index + 1);
	GetDlgItem(IDC_EDIT_INFO_WIDTH)->GetWindowTextW(cvalue);
	if (cvalue != _T(""))
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);

	//基膜材料
	index = m_combox_base.GetCurSel();
	ckeyname.Format(_T("BASE%d"), index + 1);
	GetDlgItem(IDC_EDIT_INFO_BASE)->GetWindowTextW(cvalue);
	if (cvalue != _T(""))
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);

	//涂层材料
	index = m_combox_coating.GetCurSel();
	ckeyname.Format(_T("COATING%d"), index + 1);
	GetDlgItem(IDC_EDIT_INFO_COATING)->GetWindowTextW(cvalue);
	if (cvalue != _T(""))
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);

	//操作员
	index = m_combox_operator.GetCurSel();
	ckeyname.Format(_T("OPERATOR%d"), index + 1);
	GetDlgItem(IDC_EDIT_INFO_OPERATOR)->GetWindowTextW(cvalue);
	if (cvalue != _T(""))
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);

	//生产批号
	GetDlgItem(IDC_EDIT_NUMBER)->GetWindowTextW(cvalue);
	ckeyname.Format(_T("NUMBER"));
	if (cvalue != _T(""))
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);

	//客户信息
	GetDlgItem(IDC_EDIT_USER)->GetWindowTextW(cvalue);
	ckeyname.Format(_T("USER"));
	if (cvalue != _T(""))
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);

	//附加信息
	GetDlgItem(IDC_EDIT_ADDITION)->GetWindowTextW(cvalue);
	ckeyname.Format(_T("ADDITION"));
	if (cvalue != _T(""))
		WritePrivateProfileStringW(APPNAME, ckeyname, cvalue, FILEPATH);
}

void CProductInfo::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	//CDialogEx::OnOK();
}

void CProductInfo::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	CDialogEx::OnCancel();
}
