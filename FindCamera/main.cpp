// -----------------------------------------------------------------------------------------
// Sapera++ console grab example
// 
//    This program shows how to grab images from a camera into a buffer in the host
//    computer's memory, using Sapera++ Acquisition and Buffer objects, and a Transfer 
//    object to link them.  Also, a View object is used to display the buffer.
//
// -----------------------------------------------------------------------------------------

// Disable deprecated function warnings with Visual Studio 2005
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable: 4995)
#endif

#include "stdio.h"
#include "conio.h"
#include "math.h"
#include "sapclassbasic.h"
#include "ExampleUtils.h"

// Restore deprecated function warnings with Visual Studio 2005
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(default: 4995)
#endif

// Static Functions
static void XferCallback(SapXferCallbackInfo *pInfo);
static BOOL GetOptions(int argc, char *argv[], char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName);
static BOOL GetOptionsFromCommandLine(int argc, char *argv[], char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName);

int main(int argc, char* argv[])
{
	UINT32   acqDeviceNumber;
	char*    acqServerName = new char[CORSERVER_MAX_STRLEN];
	char*    configFilename = new char[MAX_PATH];

	printf("Sapera Console Grab Example (C++ version)\n");

	// Call GetOptions to determine which acquisition device to use and which config
	// file (CCF) should be loaded to configure it.
	// Note: if this were an MFC-enabled application, we could have replaced the lengthy GetOptions 
	// function with the CAcqConfigDlg dialog of the Sapera++ GUI Classes (see GrabMFC example)
	if (!GetOptions(argc, argv, acqServerName, &acqDeviceNumber, configFilename))
	{
		printf("\nPress any key to terminate\n");
		CorGetch();
		return 0;
	}

	SapAcquisition Acq;
	SapAcqDevice AcqDevice;
	SapBufferWithTrash Buffers;
	SapTransfer AcqToBuf = SapAcqToBuf(&Acq, &Buffers);
	SapTransfer AcqDeviceToBuf = SapAcqDeviceToBuf(&AcqDevice, &Buffers);
	SapTransfer* Xfer = NULL;
	SapView View;

	SapLocation loc(acqServerName, acqDeviceNumber);

	if (SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq) > 0)
	{
		Acq = SapAcquisition(loc, configFilename);
		Buffers = SapBufferWithTrash(2, &Acq);
		View = SapView(&Buffers, SapHwndAutomatic);
		AcqToBuf = SapAcqToBuf(&Acq, &Buffers, XferCallback, &View);
		Xfer = &AcqToBuf;

		// Create acquisition object
		if (!Acq.Create())
			goto FreeHandles;

	}

	else if (SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcqDevice) > 0)
	{
		if (strcmp(configFilename, "NoFile") == 0)
			AcqDevice = SapAcqDevice(loc, FALSE);
		else
			AcqDevice = SapAcqDevice(loc, configFilename);

		Buffers = SapBufferWithTrash(2, &AcqDevice);
		View = SapView(&Buffers, SapHwndAutomatic);
		AcqDeviceToBuf = SapAcqDeviceToBuf(&AcqDevice, &Buffers, XferCallback, &View);
		Xfer = &AcqDeviceToBuf;

		// Create acquisition object
		if (!AcqDevice.Create())
			goto FreeHandles;

	}

	// Create buffer object
	if (!Buffers.Create())
		goto FreeHandles;

	// Create transfer object
	if (Xfer && !Xfer->Create())
		goto FreeHandles;

	// Create view object
	if (!View.Create())
		goto FreeHandles;

	// Start continous grab
	Xfer->Grab();

	printf("Press any key to stop grab\n");
	CorGetch();

	// Stop grab
	Xfer->Freeze();
	if (!Xfer->Wait(5000))
		printf("Grab could not stop properly.\n");

FreeHandles:
	printf("Press any key to terminate\n");
	CorGetch();

	//unregister the acquisition callback
	Acq.UnregisterCallback();

	// Destroy view object
	if (!View.Destroy()) return FALSE;

	// Destroy transfer object
	if (Xfer && *Xfer && !Xfer->Destroy()) return FALSE;

	// Destroy buffer object
	if (!Buffers.Destroy()) return FALSE;

	// Destroy acquisition object
	if (!Acq.Destroy()) return FALSE;

	// Destroy acquisition object
	if (!AcqDevice.Destroy()) return FALSE;

	return 0;
}

static void XferCallback(SapXferCallbackInfo *pInfo)
{
	SapView *pView = (SapView *)pInfo->GetContext();

	// refresh view
	pView->Show();

	// refresh framerate
	static float lastframerate = 0.0f;

	SapTransfer* pXfer = pInfo->GetTransfer();
	if (pXfer->UpdateFrameRateStatistics())
	{
		SapXferFrameRateInfo* pFrameRateInfo = pXfer->GetFrameRateStatistics();
		float framerate = 0.0f;

		if (pFrameRateInfo->IsLiveFrameRateAvailable())
			framerate = pFrameRateInfo->GetLiveFrameRate();

		// check if frame rate is stalled
		if (pFrameRateInfo->IsLiveFrameRateStalled())
		{
			printf("Live frame rate is stalled.\n");
		}
		// update FPS only if the value changed by +/- 0.1
		else if ((framerate > 0.0f) && (abs(lastframerate - framerate) > 0.1f))
		{
			printf("Grabbing at %.1f frames/sec\n", framerate);
			lastframerate = framerate;
		}
	}
}

static BOOL GetOptions(int argc, char *argv[], char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName)
{
	// Check if arguments were passed
	if (argc > 1)
		return GetOptionsFromCommandLine(argc, argv, acqServerName, pAcqDeviceIndex, configFileName);
	else
		return GetOptionsFromQuestions(acqServerName, pAcqDeviceIndex, configFileName);
}

static BOOL GetOptionsFromCommandLine(int argc, char *argv[], char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName)
{
	// Check the command line for user commands
	if ((strcmp(argv[1], "/?") == 0) || (strcmp(argv[1], "-?") == 0))
	{
		// print help
		printf("Usage:\n");
		printf("GrabCPP [<acquisition server name> <acquisition device index> <config filename>]\n");
		return FALSE;
	}

	// Check if enough arguments were passed
	if (argc < 4)
	{
		printf("Invalid command line!\n");
		return FALSE;
	}

	// Validate server name
	if (SapManager::GetServerIndex(argv[1]) < 0)
	{
		printf("Invalid acquisition server name!\n");
		return FALSE;
	}

	// Does the server support acquisition?
	int deviceCount = SapManager::GetResourceCount(argv[1], SapManager::ResourceAcq);
	int cameraCount = SapManager::GetResourceCount(argv[1], SapManager::ResourceAcqDevice);

	if (deviceCount + cameraCount == 0)
	{
		printf("This server does not support acquisition!\n");
		return FALSE;
	}

	// Validate device index
	if (atoi(argv[2]) < 0 || atoi(argv[2]) >= deviceCount + cameraCount)
	{
		printf("Invalid acquisition device index!\n");
		return FALSE;
	}

	// Verify that the specified config file exist
	OFSTRUCT of = { 0 };
	if (OpenFile(argv[3], &of, OF_EXIST) == HFILE_ERROR)
	{
		printf("The specified config file (%s) is invalid!\n", argv[3]);
		return FALSE;
	}

	// Fill-in output variables
	CorStrncpy(acqServerName, argv[1], CORSERVER_MAX_STRLEN);
	*pAcqDeviceIndex = atoi(argv[2]);
	CorStrncpy(configFileName, argv[3], MAX_PATH);

	return TRUE;
}

