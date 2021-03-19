
#include "stdio.h"
#include "conio.h"
#include "sapclassbasic.h"
#include <vector>
#include <string>

// Static Variables
#define GAMMA_FACTOR			10000
#define MAX_CONFIG_FILES   36       // 10 numbers + 26 lowercase letters
#define STRING_LENGTH		256 
static char configFileNames[MAX_CONFIG_FILES][MAX_PATH] = {0};

#define GVSP_PIX_BAYRG8 (0x01000000 | 0x00080000 | 0x0009)

#define CAMERA_LINK_SERVER_NAME_PREFIX "CameraLink_"

#ifndef __clang__
inline double round(double x)
{
	return (x > 0.0 ? x + 0.5 : x - 0.5);
}
#endif

#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#endif

// Functions defined to be used in Grab Examples
BOOL GetOptionsFromQuestions(char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName);
BOOL GetCorAcqOptionsFromQuestions(char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName);
BOOL GetCorAcqDeviceOptionsFromQuestions(char *acqServerName, UINT32 *pAcqDeviceIndex, BOOL showGigEOnly = FALSE);
BOOL GetCorAcquisitionOptionsFromQuestions(char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName);

// Functions defined to be used in LUT Examples
BOOL IsMonoBuffer(SapBuffer* Buffers);
SapData SetDataValue(SapBuffer* Buffers, DWORD *pPrmIndex);
BOOL GetLUTOptionsFromQuestions(SapBuffer* Buffers, SapLut* m_pLut, char *chAcqLutName);
BOOL GetLoadLUTFiles(SapLut* Lut, std::vector<std::string> v_list, BOOL* bLutLoaded);
std::vector<std::string> GetLUTFilesSaved(const char* Dossier, std::vector<std::string> pList, BOOL bPrint);

#ifndef SAP_CAMERA_SDK
BOOL GetLoadDynamicLUTFiles(SapDynamicLut* DynamicLut, int lutIndex, std::vector<std::string> v_list, BOOL* bLutLoaded);
#endif

// Functions defined to be used in the 3D Examples
#ifndef SAP_CAMERA_SDK
const int PROFILE_WINDOW_HEIGHT = 1024;  //Should fit all displays resolutions
const int CLUSTERS_DIVISOR = USHRT_MAX / (PROFILE_WINDOW_HEIGHT - 1) + 1;  //To clusterize the range from 0-65535 to 0-ProfileWindowHeight

enum PIXEL_FORMAT { C, CR, AC, ACRW };
enum BUFFER_LAYOUT { Profile, Range};

bool GetParametersFrom3DBuffer(SapBuffer *Buffer, int* Invalid_Flag_C, double* Invalid_Value_C,
   double* Scale_C, double* Offset_C, BUFFER_LAYOUT* Buffer_Layout);
void DisplayRange(SapView * View, SapBuffer * Buffer);
void DisplayProfile(SapView * View, SapBuffer * Buffer);
void ProcessingRange(SapBuffer * Buffer, int Invalid_Flag_C, double Invalid_Value_C,
   double Scale_C, double Offset_C, int * Pixel_Mean,int * World_Mean);
void ProcessingProfile(SapBuffer * Buffer, int Invalid_Flag_C, double Invalid_Value_C,
   double Scale_C, double Offset_C, int * Pixel_Mean,int * World_Mean);
bool GetSelectionFromConsoleCR(int* pIndex_DisplayedComponent);
bool GetSelectionFromConsoleAC(int* pIndex_DisplayedComponent);
bool GetSelectionFromConsoleACRW(int* pIndex_DisplayedComponent);
#endif

// General Functions
int GetKeyCharIndex(char key);
BOOL GetFileOptions( int argc, char *argv[], char *fileDirectory, const char *fileList[], int fileListSize, char *filename, int size);
BOOL AddSaperaPath(char *filename, int size);
void FixFileName(char *filename, int size);

