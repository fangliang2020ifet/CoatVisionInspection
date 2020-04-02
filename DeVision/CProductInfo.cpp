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
}


BEGIN_MESSAGE_MAP(CProductInfo, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ID_CHANGE, &CProductInfo::OnBnClickedButtonIdChange)
	ON_BN_CLICKED(IDC_BUTTON_ID_INSERT, &CProductInfo::OnBnClickedButtonIdInsert)
	ON_BN_CLICKED(IDC_BUTTON_ID_SAVE, &CProductInfo::OnBnClickedButtonIdSave)
	ON_BN_CLICKED(IDC_BUTTON_WIDTH_CHANGE, &CProductInfo::OnBnClickedButtonWidthChange)
	ON_BN_CLICKED(IDC_BUTTON_WIDTH_INSERT, &CProductInfo::OnBnClickedButtonWidthInsert)
	ON_BN_CLICKED(IDC_BUTTON_WIDTH_SAVE, &CProductInfo::OnBnClickedButtonWidthSave)
	ON_BN_CLICKED(IDC_BUTTON_BASE_CHANGE, &CProductInfo::OnBnClickedButtonBaseChange)
	ON_BN_CLICKED(IDC_BUTTON_BASE_INSERT, &CProductInfo::OnBnClickedButtonBaseInsert)
	ON_BN_CLICKED(IDC_BUTTON_BASE_SAVE, &CProductInfo::OnBnClickedButtonBaseSave)
	ON_BN_CLICKED(IDC_BUTTON_COATING_CHANGE, &CProductInfo::OnBnClickedButtonCoatingChange)
	ON_BN_CLICKED(IDC_BUTTON_COATING_INSERT, &CProductInfo::OnBnClickedButtonCoatingInsert)
	ON_BN_CLICKED(IDC_BUTTON_COATING_SAVE, &CProductInfo::OnBnClickedButtonCoatingSave)
	ON_BN_CLICKED(IDC_BUTTON_NUMBER_SAVE, &CProductInfo::OnBnClickedButtonNumberSave)
	ON_BN_CLICKED(IDC_BUTTON_USER_SAVE, &CProductInfo::OnBnClickedButtonUserSave)
	ON_BN_CLICKED(IDC_BUTTON_ADDITION_SAVE, &CProductInfo::OnBnClickedButtonAdditionSave)
	ON_CBN_SELCHANGE(IDC_COMBO_ID, &CProductInfo::OnCbnSelchangeComboId)
	ON_CBN_SELCHANGE(IDC_COMBO_WIDTH, &CProductInfo::OnCbnSelchangeComboWidth)
	ON_CBN_SELCHANGE(IDC_COMBO_BASE, &CProductInfo::OnCbnSelchangeComboBase)
	ON_CBN_SELCHANGE(IDC_COMBO_COATING, &CProductInfo::OnCbnSelchangeComboCoating)
	ON_CBN_SELCHANGE(IDC_COMBO_OPERATOR, &CProductInfo::OnCbnSelchangeComboOperator)
	ON_BN_CLICKED(IDC_BUTTON_OPERATOR_CHANGE, &CProductInfo::OnBnClickedButtonOperatorChange)
	ON_BN_CLICKED(IDC_BUTTON_OPERATOR_INSERT, &CProductInfo::OnBnClickedButtonOperatorInsert)
	ON_BN_CLICKED(IDC_BUTTON_OPERATOR_SAVE, &CProductInfo::OnBnClickedButtonOperatorSave)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CProductInfo::OnBnClickedOk)
END_MESSAGE_MAP()


// CProductInfo 消息处理程序

BOOL CProductInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//加载配置文件
	LoadInifile();

	GetDlgItem(IDC_COMBO_ID)->EnableWindow(false);
	GetDlgItem(IDC_COMBO_WIDTH)->EnableWindow(false);
	GetDlgItem(IDC_COMBO_BASE)->EnableWindow(false);
	GetDlgItem(IDC_COMBO_COATING)->EnableWindow(false);
	GetDlgItem(IDC_COMBO_OPERATOR)->EnableWindow(false);

	GetDlgItem(IDC_BUTTON_ID_INSERT)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_WIDTH_INSERT)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_BASE_INSERT)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_COATING_INSERT)->EnableWindow(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CProductInfo::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (MessageBox(_T("请确认已保存修改，确定退出吗"), _T("提示"), MB_YESNO | MB_ICONWARNING) == IDNO)
		return;
	CDialogEx::OnClose();
}

void CProductInfo::LoadInifile()
{
	CComboBox* pComBox;
	//LPWSTR KeyName = new wchar_t[STRINGLENGTH];
	LPWSTR ReturnedString = new wchar_t[STRINGLENGTH];
	LPWSTR KeyName;
	//LPWSTR ReturnedString;

	//产品型号
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_ID);
	for (int i = 1; i < 11; i++)
	{
		std::wstring strkeyname = L"ID";
		std::wstring tempt = std::to_wstring(11 - i);
		strkeyname = strkeyname + tempt;
		KeyName = const_cast<wchar_t *>(strkeyname.c_str());
		GetPrivateProfileStringW(L"ProductInfo", KeyName, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		pComBox->InsertString(0, ReturnedString);
	}
	//GetPrivateProfileStringW(L"ProductInfo", L"ID_SELECT", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	//pComBox->SetCurSel(std::stoi(ReturnedString));
	int selectID = GetPrivateProfileInt(L"ProductInfo", L"ID_SELECT", 0, FILEPATH);
	pComBox->SetCurSel(selectID);

	//薄膜宽度
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_WIDTH);
	for (int i = 1; i < 11; i++)
	{
		std::wstring strkeyname = L"WIDTH";
		std::wstring tempt = std::to_wstring(11 - i);
		strkeyname = strkeyname + tempt;
		KeyName = const_cast<wchar_t *>(strkeyname.c_str());
		GetPrivateProfileStringW(L"ProductInfo", KeyName, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		pComBox->InsertString(0, ReturnedString);
	}
	int selectWIDTH = GetPrivateProfileInt(L"ProductInfo", L"WIDTH_SELECT", 0, FILEPATH);
	pComBox->SetCurSel(selectWIDTH);


	//基材
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_BASE);
	for (int i = 1; i < 11; i++)
	{
		std::wstring strkeyname = L"BASE";
		std::wstring tempt = std::to_wstring(11 - i);
		strkeyname = strkeyname + tempt;
		KeyName = const_cast<wchar_t *>(strkeyname.c_str());
		GetPrivateProfileStringW(L"ProductInfo", KeyName, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		pComBox->InsertString(0, ReturnedString);
	}
	int selectBASE = GetPrivateProfileInt(L"ProductInfo", L"BASE_SELECT", 0, FILEPATH);
	pComBox->SetCurSel(selectBASE);


	//涂层
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_COATING);
	for (int i = 1; i < 11; i++)
	{
		std::wstring strkeyname = L"COATING";
		std::wstring tempt = std::to_wstring(11 - i);
		strkeyname = strkeyname + tempt;
		KeyName = const_cast<wchar_t *>(strkeyname.c_str());
		GetPrivateProfileStringW(L"ProductInfo", KeyName, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		pComBox->InsertString(0, ReturnedString);
	}
	int selectCOATING = GetPrivateProfileInt(L"ProductInfo", L"COATING_SELECT", 0, FILEPATH);
	pComBox->SetCurSel(selectCOATING);

	//操作员
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_OPERATOR);
	for (int i = 1; i < 11; i++)
	{
		std::wstring strkeyname = L"OPERATOR";
		std::wstring tempt = std::to_wstring(11 - i);
		strkeyname = strkeyname + tempt;
		KeyName = const_cast<wchar_t *>(strkeyname.c_str());
		GetPrivateProfileStringW(L"ProductInfo", KeyName, L"", ReturnedString, STRINGLENGTH, FILEPATH);
		pComBox->InsertString(0, ReturnedString);
	}
	int selectOPERATOR = GetPrivateProfileInt(L"ProductInfo", L"OPERATOR_SELECT", 0, FILEPATH);
	pComBox->SetCurSel(selectOPERATOR);


	//生产批号
	CEdit* pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NUMBER);
	GetPrivateProfileStringW(L"ProductInfo", L"NUMBER", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	pEdit->SetWindowTextW(ReturnedString);

	//客户
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_USER);
	GetPrivateProfileStringW(L"ProductInfo", L"USER", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	pEdit->SetWindowTextW(ReturnedString);

	//附加信息
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ADDITION);
	GetPrivateProfileStringW(L"ProductInfo", L"ADDITION", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	pEdit->SetWindowTextW(ReturnedString);


	delete[] ReturnedString;
}

//产品型号
void CProductInfo::OnBnClickedButtonIdChange()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_COMBO_ID)->EnableWindow(true);

}


void CProductInfo::OnBnClickedButtonIdInsert()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComBox;
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_ID);
	CString newstring;
	pComBox->GetWindowTextW(newstring);
	pComBox->InsertString(0, newstring);
}


void CProductInfo::OnBnClickedButtonIdSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComBox;
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_ID);
	int selectnum = GetPrivateProfileInt(L"ProductInfo", L"ID_SELECT", 0, FILEPATH);
	std::wstring strkeyname = L"ID";
	std::wstring tempt = std::to_wstring(selectnum + 1);
	strkeyname = strkeyname + tempt;
	LPCWSTR KeyName = const_cast<wchar_t *>(strkeyname.c_str());

	CStringW selectwstr;
	pComBox->GetWindowTextW(selectwstr);
	LPCWSTR WriteString = (LPCWSTR)(LPCTSTR)selectwstr;
	WritePrivateProfileStringW(L"ProductInfo", KeyName, WriteString, FILEPATH);

	GetDlgItem(IDC_COMBO_ID)->EnableWindow(false);
}

//薄膜宽度
void CProductInfo::OnBnClickedButtonWidthChange()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_COMBO_WIDTH)->EnableWindow(true);

}


void CProductInfo::OnBnClickedButtonWidthInsert()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComBox;
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_WIDTH);
	CString newstring;
	pComBox->GetWindowTextW(newstring);
	pComBox->InsertString(0, newstring);

}


void CProductInfo::OnBnClickedButtonWidthSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComBox;
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_WIDTH);
	int selectnum = GetPrivateProfileInt(L"ProductInfo", L"WIDTH_SELECT", 0, FILEPATH);
	std::wstring strkeyname = L"WIDTH";
	std::wstring tempt = std::to_wstring(selectnum + 1);
	strkeyname = strkeyname + tempt;
	LPCWSTR KeyName = const_cast<wchar_t *>(strkeyname.c_str());

	CStringW selectwstr;
	pComBox->GetWindowTextW(selectwstr);
	LPCWSTR WriteString = (LPCWSTR)(LPCTSTR)selectwstr;
	WritePrivateProfileStringW(L"ProductInfo", KeyName, WriteString, FILEPATH);

	GetDlgItem(IDC_COMBO_WIDTH)->EnableWindow(false);

}

//基膜材料
void CProductInfo::OnBnClickedButtonBaseChange()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_COMBO_BASE)->EnableWindow(true);

}


void CProductInfo::OnBnClickedButtonBaseInsert()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComBox;
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_BASE);
	CString newstring;
	pComBox->GetWindowTextW(newstring);
	pComBox->InsertString(0, newstring);

}


void CProductInfo::OnBnClickedButtonBaseSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComBox;
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_BASE);
	int selectnum = GetPrivateProfileInt(L"ProductInfo", L"BASE_SELECT", 0, FILEPATH);
	std::wstring strkeyname = L"BASE";
	std::wstring tempt = std::to_wstring(selectnum + 1);
	strkeyname = strkeyname + tempt;
	LPCWSTR KeyName = const_cast<wchar_t *>(strkeyname.c_str());

	CStringW selectwstr;
	pComBox->GetWindowTextW(selectwstr);
	LPCWSTR WriteString = (LPCWSTR)(LPCTSTR)selectwstr;
	WritePrivateProfileStringW(L"ProductInfo", KeyName, WriteString, FILEPATH);

	GetDlgItem(IDC_COMBO_BASE)->EnableWindow(false);

}

//涂层材料
void CProductInfo::OnBnClickedButtonCoatingChange()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_COMBO_COATING)->EnableWindow(true);

}


void CProductInfo::OnBnClickedButtonCoatingInsert()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComBox;
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_COATING);
	CString newstring;
	pComBox->GetWindowTextW(newstring);
	pComBox->InsertString(0, newstring);

}


void CProductInfo::OnBnClickedButtonCoatingSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComBox;
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_COATING);
	int selectnum = GetPrivateProfileInt(L"ProductInfo", L"COATING_SELECT", 0, FILEPATH);
	std::wstring strkeyname = L"COATING";
	std::wstring tempt = std::to_wstring(selectnum + 1);
	strkeyname = strkeyname + tempt;
	LPCWSTR KeyName = const_cast<wchar_t *>(strkeyname.c_str());

	CStringW selectwstr;
	pComBox->GetWindowTextW(selectwstr);
	LPCWSTR WriteString = (LPCWSTR)(LPCTSTR)selectwstr;
	WritePrivateProfileStringW(L"ProductInfo", KeyName, WriteString, FILEPATH);

	GetDlgItem(IDC_COMBO_COATING)->EnableWindow(false);

}

//生产批号
void CProductInfo::OnBnClickedButtonNumberSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit* pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NUMBER);
	CStringW wstr;
	pEdit->GetWindowTextW(wstr);
	LPCWSTR WriteString = (LPCWSTR)(LPCTSTR)wstr;
	WritePrivateProfileStringW(L"ProductInfo", L"NUMBER", WriteString, FILEPATH);

	GetDlgItem(IDC_EDIT_NUMBER)->EnableWindow(false);
}

//客户信息
void CProductInfo::OnBnClickedButtonUserSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit* pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_USER);
	CStringW wstr;
	pEdit->GetWindowTextW(wstr);
	LPCWSTR WriteString = (LPCWSTR)(LPCTSTR)wstr;
	WritePrivateProfileStringW(L"ProductInfo", L"USER", WriteString, FILEPATH);

	GetDlgItem(IDC_EDIT_USER)->EnableWindow(false);

}

//附加信息
void CProductInfo::OnBnClickedButtonAdditionSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit* pEdit;
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ADDITION);
	CStringW wstr;
	pEdit->GetWindowTextW(wstr);
	LPCWSTR WriteString = (LPCWSTR)(LPCTSTR)wstr;
	WritePrivateProfileStringW(L"ProductInfo", L"ADDITION", WriteString, FILEPATH);

	GetDlgItem(IDC_EDIT_ADDITION)->EnableWindow(false);

}

//操作员
void CProductInfo::OnBnClickedButtonOperatorChange()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_COMBO_OPERATOR)->EnableWindow(true);

}


void CProductInfo::OnBnClickedButtonOperatorInsert()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CProductInfo::OnBnClickedButtonOperatorSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComBox;
	pComBox = (CComboBox*)GetDlgItem(IDC_COMBO_OPERATOR);
	int selectnum = GetPrivateProfileInt(L"ProductInfo", L"OPERATOR_SELECT", 0, FILEPATH);
	std::wstring strkeyname = L"OPERATOR";
	std::wstring tempt = std::to_wstring(selectnum + 1);
	strkeyname = strkeyname + tempt;
	LPCWSTR KeyName = const_cast<wchar_t *>(strkeyname.c_str());

	CStringW selectwstr;
	pComBox->GetWindowTextW(selectwstr);
	LPCWSTR WriteString = (LPCWSTR)(LPCTSTR)selectwstr;
	WritePrivateProfileStringW(L"ProductInfo", KeyName, WriteString, FILEPATH);

}


void CProductInfo::OnCbnSelchangeComboId()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* m_pComboBox;
	m_pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_ID);
	int nIndex = m_pComboBox->GetCurSel();
	int nCount = m_pComboBox->GetCount();
	std::wstring tempt = std::to_wstring(nIndex);
	LPCWSTR WriteString = const_cast<wchar_t *>(tempt.c_str());
	WritePrivateProfileStringW(L"ProductInfo", L"ID_SELECT", WriteString, FILEPATH);
}


void CProductInfo::OnCbnSelchangeComboWidth()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* m_pComboBox;
	m_pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_WIDTH);
	int nIndex = m_pComboBox->GetCurSel();
	int nCount = m_pComboBox->GetCount();
	std::wstring tempt = std::to_wstring(nIndex);
	LPCWSTR WriteString = const_cast<wchar_t *>(tempt.c_str());
	WritePrivateProfileStringW(L"ProductInfo", L"WIDTH_SELECT", WriteString, FILEPATH);

}


void CProductInfo::OnCbnSelchangeComboBase()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* m_pComboBox;
	m_pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_BASE);
	int nIndex = m_pComboBox->GetCurSel();
	int nCount = m_pComboBox->GetCount();
	std::wstring tempt = std::to_wstring(nIndex);
	LPCWSTR WriteString = const_cast<wchar_t *>(tempt.c_str());
	WritePrivateProfileStringW(L"ProductInfo", L"BASE_SELECT", WriteString, FILEPATH);

}


void CProductInfo::OnCbnSelchangeComboCoating()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* m_pComboBox;
	m_pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_COATING);
	int nIndex = m_pComboBox->GetCurSel();
	int nCount = m_pComboBox->GetCount();
	std::wstring tempt = std::to_wstring(nIndex);
	LPCWSTR WriteString = const_cast<wchar_t *>(tempt.c_str());
	WritePrivateProfileStringW(L"ProductInfo", L"COATING_SELECT", WriteString, FILEPATH);

}


void CProductInfo::OnCbnSelchangeComboOperator()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* m_pComboBox;
	m_pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_OPERATOR);
	int nIndex = m_pComboBox->GetCurSel();
	int nCount = m_pComboBox->GetCount();
	std::wstring tempt = std::to_wstring(nIndex);
	LPCWSTR WriteString = const_cast<wchar_t *>(tempt.c_str());
	WritePrivateProfileStringW(L"ProductInfo", L"OPERATOR_SELECT", WriteString, FILEPATH);

}

void CProductInfo::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox(_T("请确认已保存修改，确定退出吗"), _T("提示"), MB_YESNO | MB_ICONWARNING) == IDNO)
		return;

	CDialogEx::OnOK();
}