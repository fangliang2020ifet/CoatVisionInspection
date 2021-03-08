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
{

}

CAlgorithmDlg::~CAlgorithmDlg()
{
}

void CAlgorithmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_GLOBAL_THRESHOLD, m_combo_global_threshold);
	DDX_Control(pDX, IDC_COMBO_SELECT_THRESHOLD, m_combo_select_threshold);
	DDX_Text(pDX, IDC_EDIT_11, n11);
	DDX_Text(pDX, IDC_EDIT_12, n12);
	DDX_Text(pDX, IDC_EDIT_13, n13);
	DDX_Text(pDX, IDC_EDIT_14, n14);
	DDX_Text(pDX, IDC_EDIT_15, n15);
	DDX_Text(pDX, IDC_EDIT_16, n16);
	DDX_Text(pDX, IDC_EDIT_21, n21);
	DDX_Text(pDX, IDC_EDIT_22, n22);
	DDX_Text(pDX, IDC_EDIT_23, n23);
	DDX_Text(pDX, IDC_EDIT_24, n24);
	DDX_Text(pDX, IDC_EDIT_25, n25);
	DDX_Text(pDX, IDC_EDIT_26, n26);
	DDX_Text(pDX, IDC_EDIT_31, n31);
	DDX_Text(pDX, IDC_EDIT_32, n32);
	DDX_Text(pDX, IDC_EDIT_33, n33);
	DDX_Text(pDX, IDC_EDIT_34, n34);
	DDX_Text(pDX, IDC_EDIT_35, n35);
	DDX_Text(pDX, IDC_EDIT_36, n36);
	DDX_Text(pDX, IDC_EDIT_41, n41);
	DDX_Text(pDX, IDC_EDIT_42, n42);
	DDX_Text(pDX, IDC_EDIT_43, n43);
	DDX_Text(pDX, IDC_EDIT_44, n44);
	DDX_Text(pDX, IDC_EDIT_45, n45);
	DDX_Text(pDX, IDC_EDIT_46, n46);
	DDX_Text(pDX, IDC_EDIT_51, n51);
	DDX_Text(pDX, IDC_EDIT_52, n52);
	DDX_Text(pDX, IDC_EDIT_53, n53);
	DDX_Text(pDX, IDC_EDIT_54, n54);
	DDX_Text(pDX, IDC_EDIT_55, n55);
	DDX_Text(pDX, IDC_EDIT_56, n56);
	DDX_Control(pDX, IDC_COMBO_DFT_TYPE, m_comboDFTselect);
}


BEGIN_MESSAGE_MAP(CAlgorithmDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CAlgorithmDlg::OnBnClickedButtonReset)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CAlgorithmDlg::OnCbnSelchangeCombo2)
	ON_EN_KILLFOCUS(IDC_EDIT_11, &CAlgorithmDlg::OnEnKillfocusEdit11)
	ON_EN_KILLFOCUS(IDC_EDIT_12, &CAlgorithmDlg::OnEnKillfocusEdit12)
	ON_EN_KILLFOCUS(IDC_EDIT_13, &CAlgorithmDlg::OnEnKillfocusEdit13)
	ON_EN_KILLFOCUS(IDC_EDIT_14, &CAlgorithmDlg::OnEnKillfocusEdit14)
	ON_EN_KILLFOCUS(IDC_EDIT_15, &CAlgorithmDlg::OnEnKillfocusEdit15)
	ON_EN_KILLFOCUS(IDC_EDIT_16, &CAlgorithmDlg::OnEnKillfocusEdit16)
	ON_EN_KILLFOCUS(IDC_EDIT_21, &CAlgorithmDlg::OnEnKillfocusEdit21)
	ON_EN_KILLFOCUS(IDC_EDIT_22, &CAlgorithmDlg::OnEnKillfocusEdit22)
	ON_EN_KILLFOCUS(IDC_EDIT_23, &CAlgorithmDlg::OnEnKillfocusEdit23)
	ON_EN_KILLFOCUS(IDC_EDIT_24, &CAlgorithmDlg::OnEnKillfocusEdit24)
	ON_EN_KILLFOCUS(IDC_EDIT_25, &CAlgorithmDlg::OnEnKillfocusEdit25)
	ON_EN_KILLFOCUS(IDC_EDIT_26, &CAlgorithmDlg::OnEnKillfocusEdit26)
	ON_EN_KILLFOCUS(IDC_EDIT_31, &CAlgorithmDlg::OnEnKillfocusEdit31)
	ON_EN_KILLFOCUS(IDC_EDIT_32, &CAlgorithmDlg::OnEnKillfocusEdit32)
	ON_EN_KILLFOCUS(IDC_EDIT_33, &CAlgorithmDlg::OnEnKillfocusEdit33)
	ON_EN_KILLFOCUS(IDC_EDIT_34, &CAlgorithmDlg::OnEnKillfocusEdit34)
	ON_EN_KILLFOCUS(IDC_EDIT_35, &CAlgorithmDlg::OnEnKillfocusEdit35)
	ON_EN_KILLFOCUS(IDC_EDIT_36, &CAlgorithmDlg::OnEnKillfocusEdit36)
	ON_EN_KILLFOCUS(IDC_EDIT_41, &CAlgorithmDlg::OnEnKillfocusEdit41)
	ON_EN_KILLFOCUS(IDC_EDIT_42, &CAlgorithmDlg::OnEnKillfocusEdit42)
	ON_EN_KILLFOCUS(IDC_EDIT_43, &CAlgorithmDlg::OnEnKillfocusEdit43)
	ON_EN_KILLFOCUS(IDC_EDIT_44, &CAlgorithmDlg::OnEnKillfocusEdit44)
	ON_EN_KILLFOCUS(IDC_EDIT_45, &CAlgorithmDlg::OnEnKillfocusEdit45)
	ON_EN_KILLFOCUS(IDC_EDIT_46, &CAlgorithmDlg::OnEnKillfocusEdit46)
	ON_EN_KILLFOCUS(IDC_EDIT_51, &CAlgorithmDlg::OnEnKillfocusEdit51)
	ON_EN_KILLFOCUS(IDC_EDIT_52, &CAlgorithmDlg::OnEnKillfocusEdit52)
	ON_EN_KILLFOCUS(IDC_EDIT_53, &CAlgorithmDlg::OnEnKillfocusEdit53)
	ON_EN_KILLFOCUS(IDC_EDIT_54, &CAlgorithmDlg::OnEnKillfocusEdit54)
	ON_EN_KILLFOCUS(IDC_EDIT_55, &CAlgorithmDlg::OnEnKillfocusEdit55)
	ON_EN_KILLFOCUS(IDC_EDIT_56, &CAlgorithmDlg::OnEnKillfocusEdit56)
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

	m_comboDFTselect.AddString(L"异物");
	m_comboDFTselect.AddString(L"擦伤");
	m_comboDFTselect.AddString(L"气泡");
	m_comboDFTselect.AddString(L"黑点");
	m_comboDFTselect.AddString(L"斑块");
	m_comboDFTselect.AddString(L"凹凸");
	m_comboDFTselect.AddString(L"色差");
	m_comboDFTselect.AddString(L"均匀度");
	m_comboDFTselect.SetCurSel(0);
	GetDlgItem(IDC_EDIT_11)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_12)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_13)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_14)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_15)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_16)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_21)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_22)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_23)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_24)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_25)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_26)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_31)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_32)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_33)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_34)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_35)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_36)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_41)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_42)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_43)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_44)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_45)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_46)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_51)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_52)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_53)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_54)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_55)->SetWindowTextW(L"0.0");
	GetDlgItem(IDC_EDIT_56)->SetWindowTextW(L"0.0");

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


void CAlgorithmDlg::OnCbnSelchangeCombo2()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CAlgorithmDlg::OnEnKillfocusEdit11()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n11);
	if (value > 0)
		m_fPlanValue[0][0] = value;

	UpdateData(false);
}


void CAlgorithmDlg::OnEnKillfocusEdit12()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n12);
	if (value > 0)
		m_fPlanValue[0][1] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit13()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n13);
	if (value > 0)
		m_fPlanValue[0][2] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit14()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n14);
	if (value > 0)
		m_fPlanValue[0][3] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit15()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n15);
	if (value > 0)
		m_fPlanValue[0][4] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit16()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n16);
	if (value > 0)
		m_fPlanValue[0][5] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit21()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n21);
	if (value > 0)
		m_fPlanValue[1][0] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit22()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n22);
	if (value > 0)
		m_fPlanValue[1][1] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit23()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n23);
	if (value > 0)
		m_fPlanValue[1][2] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit24()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n24);
	if (value > 0)
		m_fPlanValue[1][3] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit25()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n25);
	if (value > 0)
		m_fPlanValue[1][4] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit26()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n26);
	if (value > 0)
		m_fPlanValue[1][5] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit31()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n31);
	if (value > 0)
		m_fPlanValue[2][0] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit32()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n32);
	if (value > 0)
		m_fPlanValue[2][1] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit33()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n33);
	if (value > 0)
		m_fPlanValue[2][2] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit34()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n34);
	if (value > 0)
		m_fPlanValue[2][3] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit35()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n35);
	if (value > 0)
		m_fPlanValue[2][4] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit36()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n36);
	if (value > 0)
		m_fPlanValue[2][5] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit41()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n41);
	if (value > 0)
		m_fPlanValue[3][0] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit42()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n42);
	if (value > 0)
		m_fPlanValue[3][1] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit43()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n43);
	if (value > 0)
		m_fPlanValue[3][2] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit44()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n44);
	if (value > 0)
		m_fPlanValue[3][3] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit45()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n45);
	if (value > 0)
		m_fPlanValue[3][4] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit46()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n46);
	if (value > 0)
		m_fPlanValue[3][5] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit51()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n51);
	if (value > 0)
		m_fPlanValue[4][0] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit52()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n52);
	if (value > 0)
		m_fPlanValue[4][1] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit53()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n53);
	if (value > 0)
		m_fPlanValue[4][2] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit54()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n54);
	if (value > 0)
		m_fPlanValue[4][3] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit55()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n55);
	if (value > 0)
		m_fPlanValue[4][4] = value;

	UpdateData(false);

}


void CAlgorithmDlg::OnEnKillfocusEdit56()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	float value = (float)_ttof(n56);
	if (value > 0)
		m_fPlanValue[4][5] = value;

	UpdateData(false);

}
