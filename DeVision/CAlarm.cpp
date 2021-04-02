
#include "stdafx.h"
#include "DeVision.h"
#include "CAlarm.h"
#include "Log.h"


CAlarm::CAlarm()
{
	m_cstrCOM = L"";

	//InitialAlarm();
}


CAlarm::~CAlarm()
{


}


void CAlarm::InitialAlarm()
{
	hMainWnd = AfxGetMainWnd()->m_hWnd;
	if (hMainWnd == NULL)
		return;

}


bool CAlarm::getUartState()
{
	if (isOpened)
		return true;
	else
		return false;
}


bool CAlarm::OpenUart()
{
	InitialAlarm();

	//���ռ�þ�ȡ��ռ��
	if (mUart.isConnected())
		mUart.ClosePort();

	CString strtmp = m_cstrCOM;
	bool state = mUart.ConfigUart(strtmp, GetUartConfigDCB());
	if (!mUart.OpenCom()){
		CString cstrlog;
		cstrlog.Format(_T("�򿪱����ƴ���ʧ��,���飺\n1.�����Ƿ�����\n2.COM���Ƿ�������ȷ\n"));
		::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		return false;
	}

	//���������߳�
	hRxThread = AfxBeginThread(RxThreadFunc, mUart.GetThreadStartPara(), THREAD_PRIORITY_NORMAL);
	if (hRxThread == NULL){
		CString cstrlog;
		cstrlog.Format(_T("Rx Listenner Thread Created Failed"));
		::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstrlog, NULL);
		return false;
	}

	isOpened = true;

	return true;
}


void CAlarm::closeUart()
{
	TerminateThread(hRxThread, 0);
	//�رմ���
	if (mUart.isConnected())
		mUart.ClosePort();

	isOpened = false;
}

//�ӿؼ��еõ�������Ϣ�ַ���
DCB CAlarm::GetUartConfigDCB(void)
{
	DCB configDCB;
	//CString tmpStr, UartConfig;
	//CComboBox *pCombox;
	////�趨���ڲ���
	//pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_BAUDRATE);
	//pCombox->GetLBText(pCombox->GetCurSel(), tmpStr);
	//configDCB.BaudRate = _ttoi(tmpStr);

	//pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_ECC);
	//configDCB.Parity = pCombox->GetCurSel();

	//pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_BITS);
	//pCombox->GetLBText(pCombox->GetCurSel(), tmpStr);
	//configDCB.ByteSize = _wtoi(tmpStr);

	//pCombox = (CComboBox*)GetDlgItem(IDC_COMBO_STOP);
	//configDCB.StopBits = pCombox->GetCurSel();

	//�趨���ڲ���
	configDCB.BaudRate = (DWORD)9600;
	configDCB.Parity = (BYTE)0;
	configDCB.ByteSize = (BYTE)8;
	configDCB.StopBits = (BYTE)1;

	return configDCB;
}

//-------------------------- �����ƿ���--------------------------//
//   1:����                  4:��˸                 2:�ر�
//   11:��                   41                     21
//   12:��                   42                     22
//   14:��                   44                     24
//   18:������               48                     28
bool CAlarm::sendControlSignal(int code)
{
	if (!isOpened)
		return false;

	// code Ϊ16������
	std::string str_code;
	switch (code)
	{
	case 11:
		str_code = "\x11";
		break;
	case 12:
		str_code = "\x12";
		break;
	case 14:
		str_code = "\x14";
		break;
	case 18:
		str_code = "\x18";
		break;
	case 41:
		str_code = "\x41";
		break;
	case 42:
		str_code = "\x42";
		break;
	case 44:
		str_code = "\x44";
		break;
	case 48:
		str_code = "\x48";
		break;
	case 21:
		str_code = "\x21";
		break;
	case 22:
		str_code = "\x22";
		break;
	case 24:
		str_code = "\x24";
		break;
	case 28:
		str_code = "\x28";
		break;
	}
	if (str_code.empty())
		return false;
	//std::string str_code = "\x11";
	DataTx.ReString((CString)str_code.c_str());
	int send_state = AsyncSend(DataTx.GetCStrData());
	if (send_state == 0) {
		CString cstr = L"�����ƿ���ָ���ʧ��";
		::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
		return false;
	}

	//// ����ֵ�ж�, �������ݳ��Ȳ�Ϊ 0 ����ʾ���óɹ�
	//CString dataStr, infoStr;
	//// AsyncRead ���ض�ȡ�����ֽ���
	//int length = AsyncRead(dataStr);
	//if (length == 0) {
	//	CString cstr = L"�����ƿ���ʧ��";
	//	::SendMessage(hMainWnd, WM_WARNING_MSG, (WPARAM)&cstr, NULL);
	//	return false;
	//}

	return true;
}


void CAlarm::clearAlarm()
{
	sendControlSignal(21);
	sendControlSignal(22); 
	sendControlSignal(24);
	sendControlSignal(28);
}


void CAlarm::signalRun()
{
	clearAlarm();

	sendControlSignal(11);
}


void CAlarm::signalPause()
{
	clearAlarm();

	sendControlSignal(44);
}


void CAlarm::signalStop()
{
	clearAlarm();

	sendControlSignal(14);
}


void CAlarm::signalAlarm()
{
	clearAlarm();

	sendControlSignal(41);
	sendControlSignal(42);
	sendControlSignal(44);
	sendControlSignal(48);
}
