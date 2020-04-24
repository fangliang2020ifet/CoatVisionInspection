// CSetupDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "CSetupDlg.h"
#include "afxdialogex.h"


// CSetupDlg 对话框

IMPLEMENT_DYNAMIC(CSetupDlg, CDialogEx)

CSetupDlg::CSetupDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SETUP, pParent)
{
	ACCEPTED = FALSE;
}

CSetupDlg::~CSetupDlg()
{
}

void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetupDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SETUP_LOGIN, &CSetupDlg::OnBnClickedButtonSetupLogin)
	ON_BN_CLICKED(IDC_BUTTON_ADD_USER, &CSetupDlg::OnBnClickedButtonAddUser)
	ON_BN_CLICKED(IDC_BUTTON_SETUP_CHANGE, &CSetupDlg::OnBnClickedButtonSetupChange)
	ON_BN_CLICKED(IDC_BUTTON_SETUP_DELETE, &CSetupDlg::OnBnClickedButtonSetupDelete)
END_MESSAGE_MAP()


// CSetupDlg 消息处理程序


BOOL CSetupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	m_wnd1_range = 20.0f;
	CString ctext1, ctext2;
	ctext1.Format(_T("%.2f"), m_wnd1_range);
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_WND1_RANGE);
	pedit->SetWindowTextW(ctext1);

	m_wnd2_range = 3.0f;
	ctext2.Format(_T("%.2f"), m_wnd2_range);
	pedit = (CEdit*)GetDlgItem(IDC_EDIT_WND2_RANGE);
	pedit->SetWindowTextW(ctext2);



	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CSetupDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::DestroyWindow();
}


void CSetupDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (AfxMessageBox(_T("是否保存？"), MB_YESNO | MB_ICONWARNING) == IDYES) {
		m_wnd1_range = GetWnd1DisplayRange();

		m_wnd2_range = GetWnd2DisplayRange();

		CString cstr;
		cstr.Format(_T("修改视窗 1 的显示范围：%.2f"), m_wnd1_range);
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr, NULL);

		CString cstr2;
		cstr2.Format(_T("修改视窗 2 的显示范围：%.2f"), m_wnd2_range);
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstr2, NULL);

	}

	if (m_pConnection != NULL) {
		if (m_pConnection->State)
			m_pConnection->Close();
		m_pConnection = NULL;
	}

	CDialogEx::OnClose();
}


void CSetupDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	//CDialogEx::OnOK();
}


float CSetupDlg::GetWnd1DisplayRange()
{
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_WND1_RANGE);
	float range;
	CString str_edit;
	pedit->GetWindowTextW(str_edit);
	range = (float)_ttof(str_edit);

	return range;
}

float CSetupDlg::GetWnd2DisplayRange()
{
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_WND2_RANGE);
	float range;
	CString str_edit;
	pedit->GetWindowTextW(str_edit);
	range = (float)_ttof(str_edit);

	return range;
}


BOOL CSetupDlg::ConnectAccess()
{
	//首先确认安装的office是否是64位版本，之后安装AccessDatabaseEngine_english_X64.exe，第三步在odbc 64中建立 系统dsn
	try {
		//连接数据库
		::CoInitialize(NULL);
		HRESULT hr = m_pConnection.CreateInstance(_uuidof(Connection));
		if (SUCCEEDED(hr)) {
			_bstr_t strConnect = "Provider=Microsoft.ACE.OLEDB.12.0;\
							  Data Source=.\\system\\UserManageL.accdb;\
							  Persist Security Info=False;\
							  Jet OLEDB:Database Password='qazwsxedc'";
			m_pConnection->Open(strConnect, "", "", adModeUnknown);

		}
	}
	catch (_com_error e) {
		CString errormessage;
		errormessage.Format(L"连接数据库失败！\r\n错误信息：%s", e.ErrorMessage());
		AfxMessageBox(errormessage, MB_ICONEXCLAMATION);//显示错误信息
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&errormessage, NULL);
		if (m_pConnection->State)
			m_pConnection->Close();
		return FALSE;
	}

	return TRUE;
}

//用户消息处理

//登录
void CSetupDlg::OnBnClickedButtonSetupLogin()
{
	// TODO: 在此添加控件通知处理程序代码
	this->UpdateData(true);
	CString cname, cpassword;
	GetDlgItem(IDC_EDIT_SETUP_USERNAME)->GetWindowTextW(cname);
	GetDlgItem(IDC_EDIT_SETUP_USERPASSWORD)->GetWindowTextW(cpassword);

	if (ConnectAccess()) {
		m_pRecordset.CreateInstance(_uuidof(Recordset));
		try {
			m_pRecordset->Open("SELECT * FROM UserTab",
				_variant_t((IDispatch*)m_pConnection, true),
				adOpenDynamic,
				adLockOptimistic,
				adCmdText);

			//处理数据
			_variant_t varID, varname, varpassword;
			CString access_ID, access_name, access_password;
			long index = 0;//注意：必须是long类型
			int countItem = 0;
			while (!m_pRecordset->adoEOF)
			{
				index = 0;
				//读ID号
				varID = m_pRecordset->GetCollect(_variant_t(index));
				if (varID.vt != VT_NULL) {
					access_ID.Format(L"%d", varID.lVal);
				}
				//读ID后面的字段的数据
				varname = m_pRecordset->GetCollect(_variant_t(index + 1));
				if (varname.vt != VT_NULL) {
					access_name = (LPCTSTR)(_bstr_t)varname;
					varpassword = m_pRecordset->GetCollect(_variant_t(index + 2));
					if (varpassword.vt != VT_NULL) {
						access_password = (LPCTSTR)(_bstr_t)varpassword;
						if (cname == access_name && cpassword == access_password) {
							//用户名和密码均匹配
							ACCEPTED = TRUE;
							m_current_logged_name = cname;
							break;
						}
					}
				}
				m_pRecordset->MoveNext();
				countItem++;
			}
			m_pRecordset->Close();
		}
		catch (_com_error e) {
			CString errormessage;
			errormessage.Format(L"数据库操作失败\r\n错误信息：%s", e.ErrorMessage());
			AfxMessageBox(errormessage, MB_ICONERROR);//显示错误信息
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&errormessage, NULL);
			if (m_pConnection->State)
				m_pConnection->Close();
			return;
		}
	}

	if (ACCEPTED == TRUE) {
		GetDlgItem(IDC_BUTTON_SETUP_LOGIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CHANGE_PASSWORD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CHANGE_REPASSWORD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SETUP_CHANGE)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUTTON_ADD_USER)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SETUP_NEWNAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SETUP_NEWPASSWORD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SETUP_NEWREPASSWORD)->EnableWindow(TRUE);	

		GetDlgItem(IDC_BUTTON_SETUP_DELETE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SETUP_DELETENAME)->EnableWindow(TRUE);		
	}
	else
		AfxMessageBox(L"用户名或密码错误", MB_RETRYCANCEL | MB_ICONSTOP);
}

//修改密码
void CSetupDlg::OnBnClickedButtonSetupChange()
{
	// TODO: 在此添加控件通知处理程序代码
	this->UpdateData(true);
	CString cpassword, crepassword;
	GetDlgItem(IDC_EDIT_CHANGE_PASSWORD)->GetWindowTextW(cpassword);
	if (cpassword == _T("")) {
		AfxMessageBox(L"密码输入错误", MB_ICONERROR);
		return;
	}
	GetDlgItem(IDC_EDIT_CHANGE_REPASSWORD)->GetWindowTextW(crepassword);
	if (cpassword != crepassword) {
		AfxMessageBox(L"两次输入不一致", MB_ICONERROR);
		return;
	}

	GetDlgItem(IDC_EDIT_CHANGE_PASSWORD)->SetWindowTextW(L"");
	GetDlgItem(IDC_EDIT_CHANGE_REPASSWORD)->SetWindowTextW(L"");

	if (AfxMessageBox(L"      确认修改？", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		_variant_t RecordsAffected;
		try {
			//CString strCmd = L"UPDATE UserTab SET [USERPASSWORD]='xxxx' WHERE [USERNAME]='User1'";
			CString strCmd = L"UPDATE UserTab SET [USERPASSWORD]='" +
				cpassword + L"' WHERE [USERNAME]='" +
				m_current_logged_name + L"'";

			m_pConnection->Execute(strCmd.AllocSysString(), &RecordsAffected, adCmdText);

		}
		catch (_com_error e) {
			CString errormessage;
			errormessage.Format(L"修改失败\r\n错误信息：%s", e.ErrorMessage());
			AfxMessageBox(errormessage, MB_ICONERROR);//显示错误信息
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&errormessage, NULL);
			if (m_pConnection->State)
				m_pConnection->Close();
			return;
		}
		CString cstrsuccess = L"密码修改成功\r\n用户名：" + m_current_logged_name;
		AfxMessageBox(cstrsuccess, MB_ICONINFORMATION);
		::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrsuccess, NULL);
	}
}

//新增用户
void CSetupDlg::OnBnClickedButtonAddUser()
{
	// TODO: 在此添加控件通知处理程序代码
	this->UpdateData(true);
	CString cname, cpassword, crepassword;
	GetDlgItem(IDC_EDIT_SETUP_NEWNAME)->GetWindowTextW(cname);
	if (cname == _T("")) {
		AfxMessageBox(L"用户名输入错误", MB_ICONERROR);
		return;
	}
	GetDlgItem(IDC_EDIT_SETUP_NEWPASSWORD)->GetWindowTextW(cpassword);
	if (cpassword == _T("")) {
		AfxMessageBox(L"密码输入错误", MB_ICONERROR);
		return;
	}
	GetDlgItem(IDC_EDIT_SETUP_NEWREPASSWORD)->GetWindowTextW(crepassword);
	if (cpassword != crepassword) {
		AfxMessageBox(L"两次输入不一致", MB_ICONERROR);
		return;
	}

	GetDlgItem(IDC_EDIT_SETUP_NEWNAME)->SetWindowTextW(L"");
	GetDlgItem(IDC_EDIT_SETUP_NEWPASSWORD)->SetWindowTextW(L"");
	GetDlgItem(IDC_EDIT_SETUP_NEWREPASSWORD)->SetWindowTextW(L"");

	_variant_t RecordsAffected;
	try	{
		m_pRecordset->Open("SELECT * FROM UserTab",
			_variant_t((IDispatch*)m_pConnection, true),
			adOpenDynamic,
			adLockOptimistic,
			adCmdText);

		long index = 0;//注意：必须是long类型
		_variant_t varID, varname;
		CString access_ID, access_name;
		while (!m_pRecordset->adoEOF)
		{
			//读 USERNAME 字段的数据
			varname = m_pRecordset->GetCollect(_variant_t(index + 1));
			if (varname.vt != VT_NULL) {
				access_name = (LPCTSTR)(_bstr_t)varname;
				if (cname == access_name) {
					//重复添加
					CString cstr = L"重复添加!\r\n用户名：" + cname + L" 已存在";
					AfxMessageBox(cstr, MB_ICONERROR);
					m_pRecordset->Close();
					return;
				}
			}
			m_pRecordset->MoveNext();
		}
		m_pRecordset->Close();

		//CString strCmd = L"INSERT INTO UserTab(USERNAME,USERPASSWORD) VALUES('User5','55')";
		CString strCmd = L"INSERT INTO UserTab(USERNAME,USERPASSWORD) VALUES('" +
			cname + L"','" +
			cpassword + L"')";
		m_pConnection->Execute(strCmd.AllocSysString(), &RecordsAffected, adCmdText);

	}catch (_com_error e){
		CString errormessage;
		errormessage.Format(L"用户添加失败\r\n错误信息：%s", e.ErrorMessage());
		AfxMessageBox(errormessage, MB_ICONERROR);//显示错误信息
		::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&errormessage, NULL);
		if (m_pConnection->State)
			m_pConnection->Close();
		return;
	}
	CString cstrsuccess = L"用户添加成功\r\n用户名：" + cname;
	AfxMessageBox(cstrsuccess, MB_ICONINFORMATION);
	::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrsuccess, NULL);
}

//删除
void CSetupDlg::OnBnClickedButtonSetupDelete()
{
	// TODO: 在此添加控件通知处理程序代码
	this->UpdateData(true);
	CString cname;
	GetDlgItem(IDC_EDIT_SETUP_DELETENAME)->GetWindowTextW(cname);
	if (cname == _T("")) {
		AfxMessageBox(L"用户名输入错误", MB_ICONERROR);
		return;
	}
	GetDlgItem(IDC_EDIT_SETUP_DELETENAME)->SetWindowTextW(L"");

	if (AfxMessageBox(L"      确认删除？", MB_YESNO | MB_ICONQUESTION) == IDYES) {
		try {
			m_pRecordset->Open("SELECT * FROM UserTab",
				_variant_t((IDispatch*)m_pConnection, true),
				adOpenDynamic,
				adLockOptimistic,
				adCmdText);

			long index = 0;//注意：必须是long类型
			_variant_t varID, varname;
			CString access_ID, access_name;
			while (!m_pRecordset->adoEOF)
			{
				//读 USERNAME 字段的数据
				varname = m_pRecordset->GetCollect(_variant_t(index + 1));
				if (varname.vt != VT_NULL) {
					access_name = (LPCTSTR)(_bstr_t)varname;
					if (cname == access_name) {
						//删除表中当前数据
						m_pRecordset->Delete(adAffectCurrent);
						CString cstrsuccess = L"用户已删除\r\n用户名：" + cname;
						AfxMessageBox(cstrsuccess, MB_ICONINFORMATION);
						::SendNotifyMessageW(hMainWnd, WM_LOGGING_MSG, (WPARAM)&cstrsuccess, NULL);

						break;
					}
				}
				m_pRecordset->MoveNext();
			}
			m_pRecordset->Close();
		}
		catch (_com_error e) {
			CString errormessage;
			errormessage.Format(L"删除失败\r\n错误信息：%s", e.ErrorMessage());
			AfxMessageBox(errormessage, MB_ICONERROR);//显示错误信息
			::SendNotifyMessageW(hMainWnd, WM_WARNING_MSG, (WPARAM)&errormessage, NULL);
			if (m_pConnection->State)
				m_pConnection->Close();
			return;
		}
	}
}
