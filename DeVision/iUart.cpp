#include "stdafx.h"
#include "iUart.h"

//#include "DeConsole.h"

#define XON 0x11
#define XOFF 0x13

iUart::~iUart()
{
	if (isConnected()) CloseHandle(hUartCom);
}


// ���ļ���ʽ�򿪴���
bool iUart::OpenCom(bool isBlockMode){
	HANDLE hCom;

	// ��10���ϵĴ���
	ComName = L"\\\\.\\" + ComName;
	if (isBlockMode)
	{
		hCom = CreateFile(ComName,// COM��
			GENERIC_READ | GENERIC_WRITE, // �������д
			0, // ��ռ��ʽ
			NULL,
			OPEN_EXISTING, // �򿪶����Ǵ���
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // OVERLAPPED ���첽��ʽ
			NULL);

		// ���÷Ƕ�������
		memset(&m_osRead, 0, sizeof(OVERLAPPED));
		m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		// ���÷Ƕ�������
		memset(&m_osWrite, 0, sizeof(OVERLAPPED));
		m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	else
	{
		hCom = CreateFile(ComName,// COM��
			GENERIC_READ | GENERIC_WRITE, // �������д
			0, // ��ռ��ʽ
			NULL,
			OPEN_EXISTING, // �򿪶����Ǵ���
			0, // ͬ����ʽ
			NULL);
	}


	// ��ȡʧ��
	if (hCom == INVALID_HANDLE_VALUE)
	{
		TRACE("Open %s Fail\n",ComName);
		return false;
	}
	// �����¼�����������
	SetCommMask(hCom, EV_RXCHAR | EV_TXEMPTY | EV_CTS | EV_DSR);
	SetupComm(hCom, IUART_BUFF_MAX, IUART_BUFF_MAX); // ���뻺����������������Ĵ�С����1024

	COMMTIMEOUTS TimeOuts;
	// �趨����ʱ
	TimeOuts.ReadIntervalTimeout = MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier = 0;
	TimeOuts.ReadTotalTimeoutConstant = 0;
	// �ڶ�һ�����뻺���������ݺ���������������أ�
	// �������Ƿ������Ҫ����ַ�
	// �趨д��ʱ
	TimeOuts.WriteTotalTimeoutMultiplier = 100;
	TimeOuts.WriteTotalTimeoutConstant = 500;
	SetCommTimeouts(hCom, &TimeOuts); // ���ó�ʱ
	hUartCom=hCom;

	/* �������ýṹ�� */
	DCB dcb;
	GetCommState(hCom, &dcb);
	// ��������ο� DCB ���ڽṹ��
	dcb.BaudRate = uartConfig.BaudRate;
	dcb.ByteSize = uartConfig.ByteSize;
	dcb.Parity = uartConfig.Parity;
	dcb.StopBits = uartConfig.StopBits;

	TRACE("open: baud:%d, size:%d, stop:%d, ecc:%d\n", uartConfig.BaudRate,
		uartConfig.ByteSize, uartConfig.StopBits, uartConfig.Parity);

	// �Ƿ��� CTS(clear-to-send) �ź������������
	// ������Ϊ true ʱ�� �� CTS Ϊ�͵�ƽ�������ݷ��ͽ�������ֱ��CTS��Ϊ��
	dcb.fOutxCtsFlow = false;

	// ���� DSR <- �Է� DTR
	// ���� DTR -> �Է� DSR
	// ���� RTS -> �Է� CTS
	// ���� CTS <- �Է� RTS
	// RTS_CONTROL_DISABLE/ENABLE ���豸ʱ��RTS�ź�Ϊʲô��ƽ
	// ���Ե��� EscapeCommFunction �������ı�RTS�ߵ�ƽ״̬
	// RTS_CONTROL_HANDSHAKE ���� RTS �ź����֣���ʱӦ�ó����ܵ��� EscapeCommFunction ����
	// �пռ��������ʱ������������ RTS Ϊ���Խ��գ������õ�
	// RTS_CONTROL_TOGGLE ���ֽ�Ҫ����ʱ RTS ��ߣ������л����ֽ��ѱ�������Ϻ�RTS ���
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	// RTS �ĸı�Ӱ��Է��� CTS
	// ������Ϊ true ʱ�� CTS Ϊ�͵�ƽ(�Է� RTS ��)�����ݷ��ͽ�������ֱ�� CTS ��Ϊ��
	dcb.fOutxCtsFlow = false;

	//���� DTR -> �Է� DSR��ʹ�ú� RTS �� RTS һ��
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fOutxDsrFlow = false;

	// �����������ź� SETRTS,CLRRTS,SETDTR,CLRDTR
	// EscapeCommFunction(mUart.GetHandle(), SETRTS);
	// ��ȡ��Ҫ GetCommMask ���ã�Ȼ�� WaitCommEvent �ȴ�����Ҫ��һ���߳�


	// ָ���Ƿ����������ģʽ��Win32 API ��֧�ַǶ�����ģʽ���䣬Ӧ�� true
	dcb.fBinary = true;
	// ����0�ַ���Ϊ true ʱ������ʱ�Զ�ȥ���գ�0ֵ���ֽ�
	dcb.fNull = false;

	// �ص���������� XON/XOFF ��������
	dcb.fInX = false;
	dcb.fOutX = false;
	// dcb.XonChar = XON; // ָ�� XON�������ͷ���������ʱ���ַ� 0x11
	// dcb.XoffChar = XOFF; // ָ�� XOFF�������ͷ�ֹͣ����ʱ���ַ� 0x13
	// dcb.XonLim = 50;
	// dcb.XoffLim = 50;

	if (SetCommState(hUartCom, &dcb) == 0)
	{
		TRACE("Com Config fail\n");
		ClosePort();
		return false;
	}

	// ������պͷ��ͻ�����
	PurgeComm(hUartCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	return true;
}

bool iUart::ConfigUart(CString comName,DCB mConfig)
{
	ComName = comName;

	uartConfig.BaudRate = mConfig.BaudRate;
	uartConfig.ByteSize = mConfig.ByteSize;
	uartConfig.Parity = mConfig.Parity;
	uartConfig.StopBits = mConfig.StopBits;

	return true;
}

// ��ȡ�����б������б�ؼ�
void iUart::GetComList(CComboBox *cblist)
{
	HKEY hKey;
	int   i = 0;
	TCHAR   portName[256], commName[256];
	DWORD   dwLong, dwSize;
	// ����ע�����ˢ��
	// �򿪴���ע���
	int rtn = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Hardware\\DeviceMap\\SerialComm", NULL, KEY_READ, &hKey);
	if (rtn == ERROR_SUCCESS)
	{
		while (TRUE)
		{
			dwLong = dwSize = sizeof(portName);
			memset(portName, 0, sizeof(portName));
			memset(commName, 0, sizeof(commName));

			rtn = RegEnumValue(hKey, i, portName, &dwLong, NULL, NULL, (PUCHAR)commName, &dwSize);
			// ö�ٴ���
			if (rtn == ERROR_NO_MORE_ITEMS)
				break;
			// �����б�
			cblist->AddString(commName);
			i++;
		}

		RegCloseKey(hKey);
	}
	// ���ˢ�´���ѡ��
	cblist->AddString(L"ˢ�´���");
}

int iUart::WriteCString(const CString &cBuffer)
{
	unsigned long dwBytesWrite;
	BOOL bWriteStat;

	if (!isConnected())
		return false;

	bWriteStat = WriteFile(hUartCom, cBuffer, cBuffer.GetAllocLength(), &dwBytesWrite, NULL);
	// ���󷵻� -1
	if (!bWriteStat)
		return -1;
	// ���ط����ֽ�
	return dwBytesWrite;
}


CString iUart::ReadCString(void)
{
	unsigned long dwBytesRead;
	BOOL bReadStat;
	COMSTAT ComStat;
	DWORD dwErrorFlags;

	CString dataStr;
	dataStr.Empty();

	if (!isConnected())
	{
		TRACE("Read:no connect\n");
		return dataStr;
	}

	ClearCommError(hUartCom, &dwErrorFlags, &ComStat);
	// cbInQue �����ڴ�������������������е��ַ���
	dwBytesRead = ComStat.cbInQue;

	if (dwBytesRead == 0)
	{
		TRACE("Read:no data\n");
		return dataStr;
	}

	dataStr.GetBufferSetLength(dwBytesRead);
	bReadStat = ReadFile(hUartCom, dataStr.GetBuffer(0), dwBytesRead, &dwBytesRead, NULL);
	if (!bReadStat)
	{
		TRACE("Read:read failed\n");
		dataStr.Empty();
		return dataStr;
	}

	PurgeComm(hUartCom, PURGE_RXABORT | PURGE_RXCLEAR);
	return dataStr;
}

// ���ڼ����̣߳��������ݷ���Ϣ֪ͨ�������ȡ��������״̬�仯����Ϣ������������
UINT RxThreadFunc(LPVOID mThreadPara)
{
	OVERLAPPED os;
	DWORD dwMask, dwTrans;
	DWORD CommEvent;
	COMSTAT ComStat;
	DWORD dwErrorFlags;

	ThreadPara *pPara = (ThreadPara *)mThreadPara;
	HANDLE hComm = *(pPara->commHandle);
	HWND hwParent = pPara->hwParent;
	HWND hwTopParent = pPara->hwTopParent;
	if (hComm == NULL)
	{
		TRACE("Handle not correct\n");
		return -1;
	}

	memset(&os, 0, sizeof(OVERLAPPED));
	os.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (os.hEvent == NULL)
	{
		TRACE("hEvent Failed\n");
		return (UINT)-1;
	}
	TRACE("Rx Listenner Thread Start\n");

	while (1)
	{
		ClearCommError(hComm, &dwErrorFlags, &ComStat);

		if (ComStat.cbInQue)
		{
			// �����ݷ�����Ϣ��UI�߳�
			// ������Ϣ�����д����߳��˳�������?
			::PostMessage(hwTopParent, WM_MYONRECVMSG, W_UART_RECV, 0);
		}

		dwMask = 0;
		// �ȴ��¼�
		if (!WaitCommEvent(hComm, &dwMask, &os))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				GetOverlappedResult(hComm, &os, &dwTrans, TRUE);
			}
			else
			{
				CloseHandle(os.hEvent);
				return(UINT)-1;
			}
		}
		// ���ض��¼�����ʱ��
		if (dwMask&EV_CTS&EV_DSR)
		{
			// �� UartDlg ���������Ʊ仯��Ϣ
			GetCommModemStatus(hComm, &CommEvent);
			::SendMessage(hwParent, WM_UARTDLG_MSG, dwMask, CommEvent);
		}
		// GetCommMask(hComm, &CommEvent);
	}
	CloseHandle(os.hEvent);
	TRACE("Rx Listenner Thread Stop\n");
	return EXIT_SUCCESS;
}


// ����̷߳�ʽ�Ķ�ȡ
int iUart::UnblockRead(CString &dataStr)
{
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	DWORD dwBytesRead;
	BOOL bReadStatus;

	ClearCommError(hUartCom, &dwErrorFlags, &ComStat);

	// cbInQue�����ڴ�������������������е��ַ���
	dwBytesRead = ComStat.cbInQue;
	/* �ر�ʱ���һ�ζ�ȡ �޷���dwBytesRead���ر��GetBufferSetLength���ֲ�������*/
	if ((int)dwBytesRead <= 0) return 0;

	dataStr.GetBufferSetLength(dwBytesRead);
	//��ȡ
	bReadStatus = ReadFile(hUartCom, dataStr.GetBuffer(0), dwBytesRead, &dwBytesRead, &m_osRead);
	if (!bReadStatus)
	{
		//����ص�����δ���,�ȴ�ֱ���������
		if (GetLastError() == ERROR_IO_PENDING)
		{
			GetOverlappedResult(hUartCom, &m_osRead, &dwBytesRead, TRUE);
			m_osRead.Offset = 0;
		}
		else
		{
			dwBytesRead = 0;
		}
	}
	//���ض�ȡ����
	return dwBytesRead;
}

int iUart::UnblockSend(const CStringA &dataStr)
{
	BOOL bWriteStatus;
	COMSTAT ComStat;
	DWORD dwErrorFlags, dwLength;

	ClearCommError(hUartCom, &dwErrorFlags, &ComStat);
	if (dwErrorFlags>0)
	{
		TRACE("Unblock Write Failed\n");
		PurgeComm(hUartCom, PURGE_TXABORT | PURGE_TXCLEAR);
		return 0;
	}
	m_osWrite.Offset = 0;

	//dwLength = dataStr.GetAllocLength();
	//append��ʽ��Ӿ͵�GetLength���ܵĳ�����ȷ������GetAllocLength��bug��������GetLength������'\0'�����
	dwLength = dataStr.GetLength();
	bWriteStatus = WriteFile(hUartCom, dataStr, dwLength, &dwLength, &m_osWrite);

	if (!bWriteStatus)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			//����ص�����δ���,�ȴ�ֱ���������
			GetOverlappedResult(hUartCom, &m_osWrite, &dwLength, TRUE);
		}
		else
			dwLength = 0;
	}
	return dwLength;
}
