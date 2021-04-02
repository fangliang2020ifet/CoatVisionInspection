#pragma once

#include "SerialPort.h"
#include "iUart.h"
#include "Text.h"
#include <string>
#include <sstream>


class CAlarm
{
public:
	CAlarm();
	~CAlarm();

public:
	HWND hMainWnd;                           //主窗口句柄

	CString m_cstrCOM;

	bool OpenUart();
	void closeUart();
	bool getUartState();
	bool sendControlSignal(int code);
	void clearAlarm();
	void signalRun();
	void signalPause();
	void signalStop();
	void signalAlarm();

private:

	//串口类
	bool isOpened = false;
	iUart mUart;
	HANDLE hRxThread;	// receive thread HANDLE
	TextBank DataRx, DataTx;
	
	void InitialAlarm();
	DCB GetUartConfigDCB(void);
	int AsyncSend(const CString& dataStr) {
		if (!isOpened)
			return -1;
		return mUart.UnblockSend(dataStr);
	}
	int AsyncRead(CString& dataStr) {
		if (!isOpened)
			return -1;
		return mUart.UnblockRead(dataStr);
	}





};

