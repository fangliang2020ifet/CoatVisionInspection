// CProductInfo.cpp: 实现文件
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
		m_bSave_Parameter = TRUE;

		SaveAll();

		CString cstr = L"修改产品信息";
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
	}
	else m_bSave_Parameter = FALSE;


	//else {
	//	CString cvalue;
	//	GetDlgItem(IDC_COMBO_ID)->GetWindowTextW(cvalue);
	//	m_ctrID = cvalue;

	//	GetDlgItem(IDC_COMBO_WIDTH)->GetWindowTextW(cvalue);
	//	m_ctrWIDTH = cvalue;

	//	GetDlgItem(IDC_COMBO_BASE)->GetWindowTextW(cvalue);
	//	m_ctrBASE = cvalue;

	//	GetDlgItem(IDC_COMBO_COATING)->GetWindowTextW(cvalue);
	//	m_ctrCOATING = cvalue;

	//	GetDlgItem(IDC_COMBO_OPERATOR)->GetWindowTextW(cvalue);
	//	m_ctrOPERATOR = cvalue;

	//	GetDlgItem(IDC_EDIT_USER)->GetWindowTextW(cvalue);
	//	m_ctrUSER = cvalue;

	//	GetDlgItem(IDC_EDIT_NUMBER)->GetWindowTextW(cvalue);
	//	m_ctrNUMBER = cvalue;

	//	GetDlgItem(IDC_EDIT_ADDITION)->GetWindowTextW(cvalue);
	//	m_ctrADDITION = cvalue;

	//}

	CDialogEx::OnClose();
}

void CProductInfo::LoadInifile()
{
	LPWSTR ReturnedString = new wchar_t[STRINGLENGTH];
	CString ckeyname, cvalue;
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
}

void CProductInfo::SaveAll()
{
	UpdateData(true);

	CString ckeyname, cvalue;
	int index;

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
