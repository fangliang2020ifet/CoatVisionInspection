// CAlgorithmDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "CAlgorithmDlg.h"
#include "afxdialogex.h"

// CAlgorithmDlg 对话框

IMPLEMENT_DYNAMIC(CAlgorithmDlg, CDialogEx)

CAlgorithmDlg::CAlgorithmDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ALGORITHM, pParent)
	, m_nRankMethod(0)
	, m_nAlarmA(0)
	, m_nAlarmB(0)
	, m_nAlarmC(0)
	, m_nAlarmD(0)
{
	m_fRankValue1 = 1.0f;
	m_fRankValue2 = 5.0f;
	m_fRankValue3 = 10.0f;
}

CAlgorithmDlg::~CAlgorithmDlg()
{
}

void CAlgorithmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_GLOBAL_THRESHOLD, m_combo_global_threshold);
	DDX_Control(pDX, IDC_COMBO_SELECT_THRESHOLD, m_combo_select_threshold);
	DDX_Radio(pDX, IDC_RADIO_AREA, m_nRankMethod);
	DDX_Check(pDX, IDC_CHECK_ALARM_A, m_nAlarmA);
	DDX_Check(pDX, IDC_CHECK_ALARM_B, m_nAlarmB);
	DDX_Check(pDX, IDC_CHECK_ALARM_C, m_nAlarmC);
	DDX_Check(pDX, IDC_CHECK_ALARM_D, m_nAlarmD);
}

BEGIN_MESSAGE_MAP(CAlgorithmDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CAlgorithmDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_RADIO_AREA, &CAlgorithmDlg::OnBnClickedRadioArea)
	ON_BN_CLICKED(IDC_RADIO_RADIUS, &CAlgorithmDlg::OnBnClickedRadioRadius)
	ON_BN_CLICKED(IDC_RADIO_CONTLENGTH, &CAlgorithmDlg::OnBnClickedRadioContlength)
	ON_EN_KILLFOCUS(IDC_EDIT_AREA_VALUE1, &CAlgorithmDlg::OnEnKillfocusEditAreaValue1)
	ON_EN_KILLFOCUS(IDC_EDIT_AREA_VALUE2, &CAlgorithmDlg::OnEnKillfocusEditAreaValue2)
	ON_EN_KILLFOCUS(IDC_EDIT_AREA_VALUE3, &CAlgorithmDlg::OnEnKillfocusEditAreaValue3)
	ON_EN_KILLFOCUS(IDC_EDIT_RADIUS_VALUE1, &CAlgorithmDlg::OnEnKillfocusEditRadiusValue1)
	ON_EN_KILLFOCUS(IDC_EDIT_RADIUS_VALUE2, &CAlgorithmDlg::OnEnKillfocusEditRadiusValue2)
	ON_EN_KILLFOCUS(IDC_EDIT_RADIUS_VALUE3, &CAlgorithmDlg::OnEnKillfocusEditRadiusValue3)
	ON_EN_KILLFOCUS(IDC_EDIT_CONT_VALUE1, &CAlgorithmDlg::OnEnKillfocusEditContValue1)
	ON_EN_KILLFOCUS(IDC_EDIT_CONT_VALUE2, &CAlgorithmDlg::OnEnKillfocusEditContValue2)
	ON_EN_KILLFOCUS(IDC_EDIT_CONT_VALUE3, &CAlgorithmDlg::OnEnKillfocusEditContValue3)
	ON_EN_KILLFOCUS(IDC_EDIT_SELECT_AREA_MIN, &CAlgorithmDlg::OnEnKillfocusEditSelectAreaMin)
	ON_EN_KILLFOCUS(IDC_EDIT_SELECT_AREA_MAX, &CAlgorithmDlg::OnEnKillfocusEditSelectAreaMax)
	ON_CBN_SELCHANGE(IDC_COMBO_GLOBAL_THRESHOLD, &CAlgorithmDlg::OnCbnSelchangeComboGlobalThreshold)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_THRESHOLD, &CAlgorithmDlg::OnCbnSelchangeComboSelectThreshold)
END_MESSAGE_MAP()

// CAlgorithmDlg 消息处理程序

BOOL CAlgorithmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	loadInitialParameters();

	CString ctext;
	for (int i = 0; i < 18; i++) {
		ctext.Format(_T("%d"), i + 3);
		m_combo_global_threshold.AddString(ctext);
	}
	m_combo_global_threshold.SetCurSel(m_normal_distribution - 3);

	m_combo_select_threshold.AddString(L"3");
	m_combo_select_threshold.AddString(L"5");
	m_combo_select_threshold.AddString(L"7");
	m_combo_select_threshold.AddString(L"9");
	m_combo_select_threshold.AddString(L"11");
	m_combo_select_threshold.AddString(L"13");
	m_combo_select_threshold.AddString(L"15");
	m_combo_select_threshold.SetCurSel(m_filter_size - 1);

	ctext.Format(_T("%.2f"), m_min_radius);
	GetDlgItem(IDC_EDIT_SELECT_AREA_MIN)->SetWindowText(ctext);
	ctext.Format(_T("%.2f"), m_max_radius);
	GetDlgItem(IDC_EDIT_SELECT_AREA_MAX)->SetWindowText(ctext);

	ctext.Format(_T("%.2f"), m_fRankValue1);
	GetDlgItem(IDC_EDIT_AREA_VALUE1)->SetWindowText(ctext);
	GetDlgItem(IDC_EDIT_RADIUS_VALUE1)->SetWindowText(ctext);
	GetDlgItem(IDC_EDIT_CONT_VALUE1)->SetWindowText(ctext);
	GetDlgItem(IDC_STATIC_AREA_VALUE1)->SetWindowText(ctext);
	GetDlgItem(IDC_STATIC_RADIUS_VALUE1)->SetWindowText(ctext);
	GetDlgItem(IDC_STATIC_CONT_VALUE1)->SetWindowText(ctext);
	ctext.Format(_T("%.2f"), m_fRankValue2);
	GetDlgItem(IDC_EDIT_AREA_VALUE2)->SetWindowText(ctext);
	GetDlgItem(IDC_EDIT_RADIUS_VALUE2)->SetWindowText(ctext);
	GetDlgItem(IDC_EDIT_CONT_VALUE2)->SetWindowText(ctext);
	GetDlgItem(IDC_STATIC_AREA_VALUE2)->SetWindowText(ctext);
	GetDlgItem(IDC_STATIC_RADIUS_VALUE2)->SetWindowText(ctext);
	GetDlgItem(IDC_STATIC_CONT_VALUE2)->SetWindowText(ctext);
	ctext.Format(_T("%.2f"), m_fRankValue3);
	GetDlgItem(IDC_EDIT_AREA_VALUE3)->SetWindowText(ctext);
	GetDlgItem(IDC_EDIT_RADIUS_VALUE3)->SetWindowText(ctext);
	GetDlgItem(IDC_EDIT_CONT_VALUE3)->SetWindowText(ctext);
	GetDlgItem(IDC_STATIC_AREA_VALUE3)->SetWindowText(ctext);
	GetDlgItem(IDC_STATIC_RADIUS_VALUE3)->SetWindowText(ctext);
	GetDlgItem(IDC_STATIC_CONT_VALUE3)->SetWindowText(ctext);


	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CAlgorithmDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

void CAlgorithmDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}

void CAlgorithmDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnCancel();
}

void CAlgorithmDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (AfxMessageBox(_T("是否保存？"), MB_YESNO | MB_ICONWARNING) == IDYES) {
		m_bSave_Parameter = TRUE;
		saveParameters();
	}
	else m_bSave_Parameter = FALSE;

	CDialogEx::OnClose();
}

void CAlgorithmDlg::loadInitialParameters()
{
	LPWSTR ReturnedString = new wchar_t[STRINGLENGTH];
	CString ckeyname, cvalue;

	std::string strvalue;
	GetPrivateProfileStringW(APPNAME, L"NormalDistribution", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_normal_distribution = std::stoi(strvalue);

	GetPrivateProfileStringW(APPNAME, L"FilterSize", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_filter_size = std::stoi(strvalue);

	GetPrivateProfileStringW(APPNAME, L"RadiusMin", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_min_radius = std::stof(strvalue);

	GetPrivateProfileStringW(APPNAME, L"RadiusMax", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_max_radius = std::stof(strvalue);

	GetPrivateProfileStringW(APPNAME, L"RankMethod", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_nRankMethod = std::stoi(strvalue);

	GetPrivateProfileStringW(APPNAME, L"RankValue1", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_fRankValue1 = std::stof(strvalue);

	GetPrivateProfileStringW(APPNAME, L"RankValue2", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_fRankValue2 = std::stof(strvalue);

	GetPrivateProfileStringW(APPNAME, L"RankValue3", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_fRankValue3 = std::stof(strvalue);

	GetPrivateProfileStringW(APPNAME, L"AlarmA", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_nAlarmA = std::stoi(strvalue);

	GetPrivateProfileStringW(APPNAME, L"AlarmB", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_nAlarmB = std::stoi(strvalue);

	GetPrivateProfileStringW(APPNAME, L"AlarmC", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_nAlarmC = std::stoi(strvalue);

	GetPrivateProfileStringW(APPNAME, L"AlarmD", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_nAlarmD = std::stoi(strvalue);

	delete[] ReturnedString;
}


void CAlgorithmDlg::saveParameters()
{
	UpdateData(true);
	CString cstrparam, ckeyname;

	cstrparam.Format(_T("%d"), m_normal_distribution);
	WritePrivateProfileStringW(APPNAME, L"NormalDistribution", cstrparam, FILEPATH);
	
	cstrparam.Format(_T("%d"), m_filter_size);
	WritePrivateProfileStringW(APPNAME, L"FilterSize", cstrparam, FILEPATH);

	cstrparam.Format(_T("%.2f"), m_min_radius);
	WritePrivateProfileStringW(APPNAME, L"RadiusMin", cstrparam, FILEPATH);

	cstrparam.Format(_T("%.2f"), m_max_radius);
	WritePrivateProfileStringW(APPNAME, L"RadiusMax", cstrparam, FILEPATH);

	cstrparam.Format(_T("%d"), m_nRankMethod);
	WritePrivateProfileStringW(APPNAME, L"RankMethod", cstrparam, FILEPATH);

	cstrparam.Format(_T("%.2f"), m_fRankValue1);
	WritePrivateProfileStringW(APPNAME, L"RankValue1", cstrparam, FILEPATH);

	cstrparam.Format(_T("%.2f"), m_fRankValue2);
	WritePrivateProfileStringW(APPNAME, L"RankValue2", cstrparam, FILEPATH);

	cstrparam.Format(_T("%.2f"), m_fRankValue3);
	WritePrivateProfileStringW(APPNAME, L"RankValue3", cstrparam, FILEPATH);

	cstrparam.Format(_T("%d"), m_nAlarmA);
	WritePrivateProfileStringW(APPNAME, L"AlarmA", cstrparam, FILEPATH);

	cstrparam.Format(_T("%d"), m_nAlarmB);
	WritePrivateProfileStringW(APPNAME, L"AlarmB", cstrparam, FILEPATH);

	cstrparam.Format(_T("%d"), m_nAlarmC);
	WritePrivateProfileStringW(APPNAME, L"AlarmC", cstrparam, FILEPATH);

	cstrparam.Format(_T("%d"), m_nAlarmD);
	WritePrivateProfileStringW(APPNAME, L"AlarmD", cstrparam, FILEPATH);
}


void CAlgorithmDlg::OnCbnSelchangeComboGlobalThreshold()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	m_normal_distribution = m_combo_global_threshold.GetCurSel() + 3;
	UpdateData(false);
}


void CAlgorithmDlg::OnCbnSelchangeComboSelectThreshold()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	m_filter_size = m_combo_select_threshold.GetCurSel() + 1;
	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEditSelectAreaMin()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	CString text;
	GetDlgItem(IDC_EDIT_SELECT_AREA_MIN)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_min_radius = value;

	UpdateData(false);
}


void CAlgorithmDlg::OnEnKillfocusEditSelectAreaMax()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	CString text;
	GetDlgItem(IDC_EDIT_SELECT_AREA_MAX)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_max_radius = value;

	UpdateData(false);
}

//恢复默认设置
void CAlgorithmDlg::OnBnClickedButtonReset()
{
	// TODO: 在此添加控件通知处理程序代码
	m_normal_distribution = 5;
	m_combo_global_threshold.SetCurSel(2);

	m_filter_size = 1;
	m_combo_select_threshold.SetCurSel(0);

	m_min_radius = 0.1f;
	CString cstr;
	cstr.Format(_T("%.2f"), m_min_radius);
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_SELECT_AREA_MIN);
	pedit->SetWindowTextW(cstr);

	m_max_radius = 10.0f;
	cstr.Format(_T("%.2f"), m_max_radius);
	pedit = (CEdit*)GetDlgItem(IDC_EDIT_SELECT_AREA_MAX);
	pedit->SetWindowTextW(cstr);

	m_nRankMethod = 1;
	m_fRankValue1 = 15.0f;


	m_fRankValue2 = 50.0f;

	m_fRankValue3 = 100.0f;

	UpdateData(FALSE);

}

void CAlgorithmDlg::OnBnClickedRadioArea()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	switch (m_nRankMethod)
	{
	case 0:
		AfxMessageBox(L"按瑕疵面积进行等级划分");
		break;
	case 1:
		AfxMessageBox(L"按瑕疵直径进行等级划分");
		break;
	case 2:
		AfxMessageBox(L"按瑕疵周长进行等级划分");
		break;
	default:
		break;
	}

	UpdateData(FALSE);
}

void CAlgorithmDlg::OnBnClickedRadioRadius()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	switch (m_nRankMethod)
	{
	case 0:
		AfxMessageBox(L"按瑕疵面积进行等级划分");
		break;
	case 1:
		AfxMessageBox(L"按瑕疵直径进行等级划分");
		break;
	case 2:
		AfxMessageBox(L"按瑕疵周长进行等级划分");
		break;
	default:
		break;
	}

	UpdateData(FALSE);
}

void CAlgorithmDlg::OnBnClickedRadioContlength()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	switch (m_nRankMethod)
	{
	case 0:
		AfxMessageBox(L"按瑕疵面积进行等级划分");
		break;
	case 1:
		AfxMessageBox(L"按瑕疵直径进行等级划分");
		break;
	case 2:
		AfxMessageBox(L"按瑕疵周长进行等级划分");
		break;
	default:
		break;
	}

	UpdateData(FALSE);
}

void CAlgorithmDlg::OnEnKillfocusEditAreaValue1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (m_nRankMethod != 0)
		return;

	CString text;
	GetDlgItem(IDC_EDIT_AREA_VALUE1)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_fRankValue1 = value;

	UpdateData(false);
}

void CAlgorithmDlg::OnEnKillfocusEditAreaValue2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (m_nRankMethod != 0)
		return;

	CString text;
	GetDlgItem(IDC_EDIT_AREA_VALUE2)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_fRankValue2 = value;

	UpdateData(false);
}

void CAlgorithmDlg::OnEnKillfocusEditAreaValue3()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (m_nRankMethod != 0)
		return;

	CString text;
	GetDlgItem(IDC_EDIT_AREA_VALUE3)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_fRankValue3 = value;

	UpdateData(false);
}

void CAlgorithmDlg::OnEnKillfocusEditRadiusValue1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (m_nRankMethod != 1)
		return;

	CString text;
	GetDlgItem(IDC_EDIT_RADIUS_VALUE1)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_fRankValue1 = value;

	UpdateData(false);
}

void CAlgorithmDlg::OnEnKillfocusEditRadiusValue2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (m_nRankMethod != 1)
		return;

	CString text;
	GetDlgItem(IDC_EDIT_RADIUS_VALUE2)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_fRankValue2 = value;

	UpdateData(false);
}

void CAlgorithmDlg::OnEnKillfocusEditRadiusValue3()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (m_nRankMethod != 1)
		return;

	CString text;
	GetDlgItem(IDC_EDIT_RADIUS_VALUE3)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_fRankValue3 = value;

	UpdateData(false);
}

void CAlgorithmDlg::OnEnKillfocusEditContValue1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (m_nRankMethod != 2)
		return;

	CString text;
	GetDlgItem(IDC_EDIT_CONT_VALUE1)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_fRankValue1 = value;

	UpdateData(false);
}

void CAlgorithmDlg::OnEnKillfocusEditContValue2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (m_nRankMethod != 2)
		return;

	CString text;
	GetDlgItem(IDC_EDIT_CONT_VALUE2)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_fRankValue2 = value;

	UpdateData(false);
}

void CAlgorithmDlg::OnEnKillfocusEditContValue3()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (m_nRankMethod != 2)
		return;

	CString text;
	GetDlgItem(IDC_EDIT_CONT_VALUE3)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_fRankValue3 = value;

	UpdateData(false);
}
