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
END_MESSAGE_MAP()


BOOL CLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	 //TODO:  在此添加额外的初始化
	try	{
		//HRESULT hr = m_pConnection.CreateInstance("ADODB.Connection");//创建Connection对象
		//if (SUCCEEDED(hr)){
		if(1){
			//连接数据库
			::CoInitialize(NULL);
			//m_pConnection = _ConnectionPtr(__uuidof(Connection));
			m_pConnection.CreateInstance(_uuidof(Connection));
			_bstr_t strConnect = "Provider=Microsoft.ACE.OLEDB.12.0; Data Source=UserManage.accdb";
			m_pConnection->Open(strConnect, "", "", adModeUnknown);
		}
	}catch (_com_error e){
		CString errormessage;
		errormessage.Format(L"连接数据库失败！\r\n错误信息：%s", e.ErrorMessage());
		AfxMessageBox(errormessage);//显示错误信息
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

// CLogin 消息处理程序

//登录
void CLogin::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	this->UpdateData(true);

	
	//m_pRecordset.CreateInstance("ADODB.Recordset");
	//m_pRecordset->Open("SELECT* FROM news WHERE id = 174",
	//	m_pConnection.GetInterfacePtr(),
	//	adOpenDynamic,
	//	adLockOptimistic,
	//	adCmdText);
	//_variant_t value = m_pRecordset->GetCollect("content");




	CString cname, cpassword;
	m_edit_name.GetWindowTextW(cname);
	m_strname = CW2A(cname);
	m_edit_password.GetWindowTextW(cpassword);
	m_strpassword = CW2A(cpassword);

	if (this->m_strname == "1" && this->m_strpassword == "1") {
		ACCEPTED = TRUE;
		CDialog::OnOK();
	}
	else
		AfxMessageBox(L"用户名或密码错误", MB_RETRYCANCEL | MB_ICONSTOP);

	//CDialogEx::OnOK();
}

//退出
void CLogin::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	ACCEPTED = FALSE;

	//如果数据库连接有效
	if (m_pConnection->State)
		m_pConnection->Close();
	m_pConnection = NULL;


	CDialogEx::OnCancel();
}


