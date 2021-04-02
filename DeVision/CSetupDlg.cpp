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
	ACCEPTED = false;
	m_bSave_Parameter = false;
	m_wnd1_range = 0.0f;
	m_wnd2_range = 0.0f;
	m_k_speed = 0.0f;
	m_threadnum = 0;
	m_bSaveRefImg = false;
	m_strDeffect_Path = "";
	m_strTable_Path = "";
	m_strUartCOM = "";
}

CSetupDlg::~CSetupDlg()
{
}

void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_THREADNUM, m_combo_threadnum);
	DDX_Control(pDX, IDC_CHECK_SAVE_REF, m_save_reference_image);
	DDX_Control(pDX, IDC_COMBO_ALARM, m_comboAlarm);
}


BEGIN_MESSAGE_MAP(CSetupDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_SETUP_LOGIN, &CSetupDlg::OnBnClickedButtonSetupLogin)
	ON_BN_CLICKED(IDC_BUTTON_ADD_USER, &CSetupDlg::OnBnClickedButtonAddUser)
	ON_BN_CLICKED(IDC_BUTTON_SETUP_CHANGE, &CSetupDlg::OnBnClickedButtonSetupChange)
	ON_BN_CLICKED(IDC_BUTTON_SETUP_DELETE, &CSetupDlg::OnBnClickedButtonSetupDelete)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_DEFFECT_PATH, &CSetupDlg::OnBnClickedButtonSelectDeffectPath)
	ON_BN_CLICKED(IDC_BUTTON_TABLE_PATH, &CSetupDlg::OnBnClickedButtonTablePath)
	ON_BN_CLICKED(IDC_BUTTON_SYSTEM_RESET, &CSetupDlg::OnBnClickedButtonSystemReset)
	ON_EN_KILLFOCUS(IDC_EDIT_WND1_RANGE, &CSetupDlg::OnEnKillfocusEditWnd1Range)
	ON_EN_KILLFOCUS(IDC_EDIT_WND2_RANGE, &CSetupDlg::OnEnKillfocusEditWnd2Range)
	ON_EN_KILLFOCUS(IDC_EDIT_K_SPEED, &CSetupDlg::OnEnKillfocusEditKSpeed)
	ON_CBN_SELCHANGE(IDC_COMBO_THREADNUM, &CSetupDlg::OnCbnSelchangeComboThreadnum)
	ON_BN_CLICKED(IDC_CHECK_SAVE_REF, &CSetupDlg::OnBnClickedCheckSaveRef)
	ON_CBN_SELCHANGE(IDC_COMBO_ALARM, &CSetupDlg::OnCbnSelchangeComboAlarm)
END_MESSAGE_MAP()


// CSetupDlg 消息处理程序

BOOL CSetupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return FALSE;

	loadInitialParameters();

	CString ctext1, ctext2, ctext3;
	ctext1.Format(_T("%.2f"), m_wnd1_range);
	GetDlgItem(IDC_EDIT_WND1_RANGE)->SetWindowTextW(ctext1);
	ctext2.Format(_T("%.2f"), m_wnd2_range);
	GetDlgItem(IDC_EDIT_WND2_RANGE)->SetWindowTextW(ctext2);
	ctext3.Format(_T("%.2f"), m_k_speed);
	GetDlgItem(IDC_EDIT_K_SPEED)->SetWindowTextW(ctext3);

	CString cthread;
	for (int i = 1; i < 6; i++){
		cthread.Format(_T("并行处理线程数  %d"), 6 - i);
		m_combo_threadnum.InsertString(0, cthread);
	}
	m_combo_threadnum.SetCurSel(m_threadnum - 1);
	m_save_reference_image.SetCheck(m_bSaveRefImg);
		
	CString path = (CA2W)m_strDeffect_Path.c_str();
	SetDlgItemText(IDC_EDIT_DEFFECT_PATH, path);
	path = (CA2W)m_strTable_Path.c_str();
	SetDlgItemText(IDC_EDIT_TABLE_PATH, path);

	CString ctext;
	int select_index = 0;
	for (int i = 0; i < 9; i++) {
		ctext.Format(_T("COM%d"), i + 1);
		m_comboAlarm.AddString(ctext);
		if (!ctext.CompareNoCase((CA2W)m_strUartCOM.c_str()))
			select_index = i;
	}
	m_comboAlarm.SetCurSel(select_index);

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
		m_bSave_Parameter = TRUE;
		saveParameters();
	}
	else m_bSave_Parameter = FALSE;

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


void CSetupDlg::loadInitialParameters()
{
	LPWSTR ReturnedString = new wchar_t[STRINGLENGTH];
	CString ckeyname, cvalue;
	std::string strvalue;

	GetPrivateProfileStringW(APPNAME, L"DisplayWindow1", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_wnd1_range = std::stof(strvalue);

	GetPrivateProfileStringW(APPNAME, L"DisplayWindow2", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_wnd2_range = std::stof(strvalue);

	GetPrivateProfileStringW(APPNAME, L"SpeedK", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_k_speed = std::stof(strvalue);

	GetPrivateProfileStringW(APPNAME, L"ThreadNumber", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_threadnum = std::stoi(strvalue);

	GetPrivateProfileStringW(APPNAME, L"SaveImage", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	strvalue = (CW2A)ReturnedString;
	m_bSaveRefImg = std::stoi(strvalue);

	GetPrivateProfileStringW(APPNAME, L"ImagePath", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	m_strDeffect_Path = (CW2A)ReturnedString;

	GetPrivateProfileStringW(APPNAME, L"TablePath", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	m_strTable_Path = (CW2A)ReturnedString;

	GetPrivateProfileStringW(APPNAME, L"UartCOM", L"", ReturnedString, STRINGLENGTH, FILEPATH);
	m_strUartCOM = (CW2A)ReturnedString;

	delete[] ReturnedString;
}


void CSetupDlg::saveParameters()
{
	UpdateData(true);
	CString cstrparam, ckeyname;

	cstrparam.Format(_T("%.2f"), m_wnd1_range);
	WritePrivateProfileStringW(APPNAME, L"DisplayWindow1", cstrparam, FILEPATH);

	cstrparam.Format(_T("%.2f"), m_wnd2_range);
	WritePrivateProfileStringW(APPNAME, L"DisplayWindow2", cstrparam, FILEPATH);

	cstrparam.Format(_T("%.2f"), m_k_speed);
	WritePrivateProfileStringW(APPNAME, L"SpeedK", cstrparam, FILEPATH);

	cstrparam.Format(_T("%d"), m_threadnum);
	WritePrivateProfileStringW(APPNAME, L"ThreadNumber", cstrparam, FILEPATH);

	cstrparam.Format(_T("%d"), m_bSaveRefImg);
	WritePrivateProfileStringW(APPNAME, L"SaveImage", cstrparam, FILEPATH);

	cstrparam = (CA2W)m_strDeffect_Path.c_str();
	WritePrivateProfileStringW(APPNAME, L"ImagePath", cstrparam, FILEPATH);

	cstrparam = (CA2W)m_strTable_Path.c_str();
	WritePrivateProfileStringW(APPNAME, L"TablePath", cstrparam, FILEPATH);

	cstrparam = (CA2W)m_strUartCOM.c_str();
	WritePrivateProfileStringW(APPNAME, L"UartCOM", cstrparam, FILEPATH);

}


// 链接数据库
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


void CSetupDlg::OnBnClickedButtonSelectDeffectPath()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR			szFolderPath[MAX_PATH] = { 0 };
	CString m_cstrDeffect_Path = TEXT("");

	BROWSEINFO		sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择一个文件夹：");
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			m_cstrDeffect_Path = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}

	m_strDeffect_Path = (CW2A)m_cstrDeffect_Path.GetBuffer();
	//std::string::size_type pos = 0;
	//while ((pos = m_strDeffect_Path.find('\\', pos)) != std::string::npos) {
	//	m_strDeffect_Path.insert(pos, "\\");
	//	pos = pos + 2;
	//}
	CString final_path = (CA2W)m_strDeffect_Path.c_str();

	SetDlgItemText(IDC_EDIT_DEFFECT_PATH, final_path);
}


void CSetupDlg::OnBnClickedButtonTablePath()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR			szFolderPath[MAX_PATH] = { 0 };
	CString m_cstrTable_Path = TEXT("");

	BROWSEINFO		sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择一个文件夹：");
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			m_cstrTable_Path = szFolderPath;
		}
	}
	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}

	m_strTable_Path = (CW2A)m_cstrTable_Path.GetBuffer();
	//std::string::size_type pos = 0;
	//while ((pos = m_strTable_Path.find('\\', pos)) != std::string::npos) {
	//	m_strTable_Path.insert(pos, "\\");
	//	pos = pos + 2;
	//}
	CString final_path = (CA2W)m_strTable_Path.c_str();

	SetDlgItemText(IDC_EDIT_TABLE_PATH, final_path);
}

//恢复默认设置
void CSetupDlg::OnBnClickedButtonSystemReset()
{
	// TODO: 在此添加控件通知处理程序代码
	m_wnd1_range = 50.0f;
	CEdit * pedit = (CEdit*)GetDlgItem(IDC_EDIT_WND1_RANGE);
	CString ctext;
	ctext.Format(_T("%.2f"), m_wnd1_range);
	pedit->SetWindowTextW(ctext);

	m_wnd2_range = 5.0f;
	pedit = (CEdit*)GetDlgItem(IDC_EDIT_WND2_RANGE);
	ctext.Format(_T("%.2f"), m_wnd2_range);
	pedit->SetWindowTextW(ctext);

	m_k_speed = 764.0f;
	pedit = (CEdit*)GetDlgItem(IDC_EDIT_K_SPEED);
	ctext.Format(_T("%.2f"), m_k_speed);
	pedit->SetWindowTextW(ctext);

	m_threadnum = 3;
	m_combo_threadnum.SetCurSel(2);

	m_bSaveRefImg = FALSE;
	m_save_reference_image.SetCheck(0);

	m_strDeffect_Path = "D:\\瑕疵检测数据记录\\2瑕疵图像记录";
	pedit = (CEdit*)GetDlgItem(IDC_EDIT_DEFFECT_PATH);
	CString deffect_path(m_strDeffect_Path.c_str());
	pedit->SetWindowTextW(deffect_path);

	m_strTable_Path = "D:\\瑕疵检测数据记录\\1检测报表记录";
	pedit = (CEdit*)GetDlgItem(IDC_EDIT_TABLE_PATH);
	CString table_path(m_strTable_Path.c_str());
	pedit->SetWindowTextW(table_path);


}


void CSetupDlg::OnEnKillfocusEditWnd1Range()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	CString text;
	GetDlgItem(IDC_EDIT_WND1_RANGE)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_wnd1_range = value;

	UpdateData(false);

}


void CSetupDlg::OnEnKillfocusEditWnd2Range()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	CString text;
	GetDlgItem(IDC_EDIT_WND2_RANGE)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_wnd2_range = value;

	UpdateData(false);

}


void CSetupDlg::OnEnKillfocusEditKSpeed()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	CString text;
	GetDlgItem(IDC_EDIT_K_SPEED)->GetWindowTextW(text);
	float value = (float)_ttof(text);
	if (value > 0)
		m_k_speed = value;

	UpdateData(false);

}


void CSetupDlg::OnCbnSelchangeComboThreadnum()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	m_threadnum = m_combo_threadnum.GetCurSel() + 1;
	UpdateData(false);

}


void CSetupDlg::OnBnClickedCheckSaveRef()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	int state = m_save_reference_image.GetCheck();
	if (state == 0)
		m_bSaveRefImg = false;
	else if (state == 1)
		m_bSaveRefImg = true;

}

// 报警灯 com 口选择
void CSetupDlg::OnCbnSelchangeComboAlarm()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	int index = m_comboAlarm.GetCurSel();
	CString cstr;
	m_comboAlarm.GetLBText(index, cstr);
	m_strUartCOM = (CW2A)cstr.GetBuffer();

	UpdateData(false);
}
