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
{
	m_fRankValue1 = 10.0f;
	m_fRankValue2 = 20.0f;
	m_fRankValue3 = 50.0f;


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
END_MESSAGE_MAP()


// CAlgorithmDlg 消息处理程序


BOOL CAlgorithmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	m_combo_global_threshold.AddString(L"3");
	m_combo_global_threshold.AddString(L"4");
	m_combo_global_threshold.AddString(L"5");
	m_combo_global_threshold.AddString(L"6");
	m_combo_global_threshold.AddString(L"7");
	m_combo_global_threshold.AddString(L"8");
	m_combo_global_threshold.AddString(L"9");
	m_combo_global_threshold.AddString(L"10");
	m_combo_global_threshold.AddString(L"11");
	m_combo_global_threshold.AddString(L"12");
	m_combo_global_threshold.AddString(L"13");
	m_combo_global_threshold.AddString(L"14");
	m_combo_global_threshold.AddString(L"15");
	m_combo_global_threshold.AddString(L"16");
	m_combo_global_threshold.AddString(L"17");
	m_combo_global_threshold.AddString(L"18");
	m_combo_global_threshold.AddString(L"19");
	m_combo_global_threshold.AddString(L"20");
	m_combo_global_threshold.SetCurSel(m_normal_distribution - 3);
	
	m_combo_select_threshold.AddString(L"3");
	m_combo_select_threshold.AddString(L"5");
	m_combo_select_threshold.AddString(L"7");
	m_combo_select_threshold.AddString(L"9");
	m_combo_select_threshold.AddString(L"11");
	m_combo_select_threshold.AddString(L"13");
	m_combo_select_threshold.AddString(L"15");
	m_combo_select_threshold.SetCurSel(m_filter_size - 1);

	CString cstr;
	cstr.Format(_T("%.2f"), m_min_radius);
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_SELECT_AREA_MIN);
	pedit->SetWindowTextW(cstr);

	cstr.Format(_T("%.2f"), m_max_radius);
	pedit = (CEdit*)GetDlgItem(IDC_EDIT_SELECT_AREA_MAX);
	pedit->SetWindowTextW(cstr);

	GetDlgItem(IDC_EDIT_AREA_VALUE1)->SetWindowText(L"10.0");
	GetDlgItem(IDC_EDIT_AREA_VALUE2)->SetWindowText(L"20.0");
	GetDlgItem(IDC_EDIT_AREA_VALUE3)->SetWindowText(L"50.0");
	GetDlgItem(IDC_EDIT_RADIUS_VALUE1)->SetWindowText(L"10.0");
	GetDlgItem(IDC_EDIT_RADIUS_VALUE2)->SetWindowText(L"20.0");
	GetDlgItem(IDC_EDIT_RADIUS_VALUE3)->SetWindowText(L"50.0");
	GetDlgItem(IDC_EDIT_CONT_VALUE1)->SetWindowText(L"10.0");
	GetDlgItem(IDC_EDIT_CONT_VALUE2)->SetWindowText(L"20.0");
	GetDlgItem(IDC_EDIT_CONT_VALUE3)->SetWindowText(L"50.0");

	GetDlgItem(IDC_STATIC_AREA_VALUE1)->SetWindowText(L"10.0");
	GetDlgItem(IDC_STATIC_AREA_VALUE2)->SetWindowText(L"20.0");
	GetDlgItem(IDC_STATIC_AREA_VALUE3)->SetWindowText(L"50.0");
	GetDlgItem(IDC_STATIC_RADIUS_VALUE1)->SetWindowText(L"10.0");
	GetDlgItem(IDC_STATIC_RADIUS_VALUE2)->SetWindowText(L"20.0");
	GetDlgItem(IDC_STATIC_RADIUS_VALUE3)->SetWindowText(L"50.0");
	GetDlgItem(IDC_STATIC_CONT_VALUE1)->SetWindowText(L"10.0");
	GetDlgItem(IDC_STATIC_CONT_VALUE2)->SetWindowText(L"20.0");
	GetDlgItem(IDC_STATIC_CONT_VALUE3)->SetWindowText(L"50.0");

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
		int index_normal_distribution = m_combo_global_threshold.GetCurSel() + 3;
		if (m_normal_distribution != index_normal_distribution) {
			m_normal_distribution = index_normal_distribution;
			CString cstr;
			cstr.Format(_T("设置检测算法概率密度为： %d"), m_normal_distribution);
			::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
		}

		int index_filter_size = m_combo_select_threshold.GetCurSel() + 1;
		if (m_filter_size != index_filter_size) {
			m_filter_size = index_filter_size;
			CString cstr;
			cstr.Format(_T("设置检测算法图像滤波器大小为： %d"), 2 * m_filter_size + 1);
			::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
		}

		float value_selectarea_min = GetSelectAreaValueMin();
		if (m_min_radius != value_selectarea_min) {
			m_min_radius = value_selectarea_min;
			CString cstr;
			cstr.Format(_T("设置最小检测半径： %.2f 毫米"), m_min_radius);
			::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
		}

		float value_selectarea_max = GetSelectAreaValueMax();
		if (m_max_radius != value_selectarea_max) {
			m_max_radius = value_selectarea_max;
			CString cstr;
			cstr.Format(_T("设置最大检测半径： %.2f 毫米"), m_max_radius);
			::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);
		}


	}
	else m_bSave_Parameter = FALSE;


	CDialogEx::OnClose();
}

float CAlgorithmDlg::GetSelectAreaValueMin()
{
	float range;
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_SELECT_AREA_MIN);
	CString str_edit;
	pedit->GetWindowTextW(str_edit);
	std::string str = (CW2A)str_edit;
	range = std::stof(str);
	return range;
}

float CAlgorithmDlg::GetSelectAreaValueMax()
{
	float range;
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_SELECT_AREA_MAX);
	CString str_edit;
	pedit->GetWindowTextW(str_edit);
	std::string str = (CW2A)str_edit;
	range = std::stof(str);
	return range;
}

//恢复默认设置
void CAlgorithmDlg::OnBnClickedButtonReset()
{
	// TODO: 在此添加控件通知处理程序代码
	m_normal_distribution = 5;
	m_combo_global_threshold.SetCurSel(2);

	m_filter_size = 1;
	m_combo_select_threshold.SetCurSel(0);

	m_min_radius = 0.05f;
	CString cstr;
	cstr.Format(_T("%.2f"), m_min_radius);
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_SELECT_AREA_MIN);
	pedit->SetWindowTextW(cstr);

	m_max_radius = 50.00f;
	cstr.Format(_T("%.2f"), m_max_radius);
	pedit = (CEdit*)GetDlgItem(IDC_EDIT_SELECT_AREA_MAX);
	pedit->SetWindowTextW(cstr);

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
		m_fRankValue1 = value;

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
		m_fRankValue1 = value;

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
