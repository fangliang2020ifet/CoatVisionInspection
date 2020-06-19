// CLogin.cpp: 实现文件
//

#include "stdafx.h"
#include "DeVision.h"
#include "CLogin.h"
#include "afxdialogex.h"

#include <atlstr.h>

// CLogin 对话框

IMPLEMENT_DYNAMIC(CLogin, CDialogEx)

CLogin::CLogin(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOGIN, pParent)
{
	ACCEPTED = FALSE;
}

CLogin::~CLogin()
{
}

void CLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_edit_name);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_edit_password);
}


BEGIN_MESSAGE_MAP(CLogin, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLogin::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLogin::OnBnClickedCancel)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL CLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	 //TODO:  在此添加额外的初始化

	//首先确认安装的office是否是64位版本，之后安装AccessDatabaseEngine_english_X64.exe，第三步在odbc 64中建立 系统dsn
	try	{
		//连接数据库
		::CoInitialize(NULL);
		HRESULT hr = m_pConnection.CreateInstance(_uuidof(Connection));
		if (SUCCEEDED(hr)) {
			_bstr_t strConnect = "Provider=Microsoft.ACE.OLEDB.12.0;\
							  Data Source=.\\system\\UserManageL.accdb;\
							  Persist Security Info=False;\
							  Jet OLEDB:Database Password='qazwsxedc'";
			m_pConnection->Open(strConnect, "", "", adModeUnknown);

			m_pCommand.CreateInstance(_uuidof(Command));
			m_pCommand->CommandTimeout = 5;
			m_pCommand->ActiveConnection = m_pConnection;
		}
	}catch (_com_error e){
		CString errormessage;
		errormessage.Format(L"连接数据库失败！\r\n错误信息：%s", e.ErrorMessage());
		AfxMessageBox(errormessage, MB_ICONERROR);//显示错误信息
		if (m_pConnection->State)
			m_pConnection->Close();
		return FALSE;
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CLogin::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	//如果数据库连接有效, 则关闭数据库连接
	if (m_pConnection != NULL) {
		if (m_pConnection->State)
			m_pConnection->Close();
		m_pConnection = NULL;
	}

}

// CLogin 消息处理程序

//登录
void CLogin::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	this->UpdateData(true);
	CString cname, cpassword;
	m_edit_name.GetWindowTextW(cname);
	m_strname = CW2A(cname);
	m_edit_password.GetWindowTextW(cpassword);
	m_strpassword = CW2A(cpassword);


	m_pRecordset.CreateInstance(_uuidof(Recordset));
	//m_pRecordset.CreateInstance("ADODB.Recordset");
	try {
		m_pRecordset->Open("SELECT * FROM UserTab",
			_variant_t((IDispatch*)m_pConnection,true),
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
			if (varID.vt != VT_NULL){
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
						ACCEPTED = TRUE;
						m_logo_name = cname;
						break;
					}
				}
			}
			m_pRecordset->MoveNext();
			countItem++;
		}
		m_pRecordset->Close();
	}
	catch (_com_error *e) {
		CString errormessage;
		errormessage.Format(L"数据库操作失败\r\n错误信息：%s", e->ErrorMessage());
		AfxMessageBox(errormessage, MB_ICONERROR);//显示错误信息
		if (m_pConnection->State)
			m_pConnection->Close();
		return;
	}

	if (m_pRecordset->State)
		m_pRecordset->Close();


	if (ACCEPTED == TRUE) {
		CDialog::OnOK();
	}
	else
		AfxMessageBox(L"用户名或密码错误", MB_RETRYCANCEL | MB_ICONSTOP);
}

//退出
void CLogin::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	ACCEPTED = FALSE;

	CDialogEx::OnCancel();
}




