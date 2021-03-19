// -----------------------------------------------------------------------------------------
// Common Functions for Sapera Examples
// 
//

// Disable the following Visual C++ compiler warning
//    4786 -> 'identifier' : identifier was truncated to 'number' characters in the debug information
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

// Disable deprecated function warnings with Visual Studio 2005
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable: 4995)
#endif

#include "ExampleUtils.h"

// Restore deprecated function warnings with Visual Studio 2005
#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(default: 4995)
#endif

#ifndef SAP_CAMERA_SDK
void XferCallback(SapXferCallbackInfo *pInfo)
{
	SapView *pView= (SapView *) pInfo->GetContext();
	pView->Show();
}
#endif   // SAP_CAMERA_SDK

BOOL GetOptionsFromQuestions(char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName)
{
   //////// Ask questions to user to select acquisition board/device and config file ////////

   // Get total number of boards in the system
   int serverCount = SapManager::GetServerCount();
   if (serverCount == 0)
   {
      printf("No device found!\n");
      return FALSE;
   }
   
   int devicesToSkip = 0;
   for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
   {
      if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0)
      {
         char serverName[CORSERVER_MAX_STRLEN];
         SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
         if (strstr(serverName, CAMERA_LINK_SERVER_NAME_PREFIX) > 0)
            devicesToSkip++;
      }
   }

   printf("\nSelect the acquisition server (or 'q' to quit)");
	printf("\n..............................................\n");

   // Scan the boards to find those that support acquisition
   BOOL serverFound = FALSE;
   BOOL cameraFound = FALSE;

   for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
   {
      if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcq) != 0)
      {
         char serverName[CORSERVER_MAX_STRLEN];
         SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
         printf("%d: %s\n", serverIndex, serverName);
         serverFound = TRUE;
      }
	  else if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0)
      {
         char serverName[CORSERVER_MAX_STRLEN];
         SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
         if (strstr(serverName, CAMERA_LINK_SERVER_NAME_PREFIX) > 0)
            continue;
         printf("%d: %s\n", serverIndex, serverName);
         cameraFound = TRUE;
      }
   }

   // At least one acquisition server must be available
   if (!serverFound && !cameraFound)
   {
      printf("No acquisition server found!\n");
      return FALSE;
   }

   char key = (char)_getch();
   if (key != 0)
   {
	   if (key == 'q')
			return FALSE;

      int serverNum = key - '0'; // char-to-int conversion
      if ((serverNum >= 1) && (serverNum < serverCount))
      {
         // update board name
         SapManager::GetServerName(serverNum, acqServerName, CORSERVER_MAX_STRLEN);
      }
      else
      {
         printf("Invalid selection!\n");
         return FALSE;
      }
   }
   else
   {
      printf("Invalid selection!\n");
      return FALSE;
   }


// Scan all the acquisition devices on that server and show menu to user
int deviceCount = SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq);
int cameraCount = (deviceCount == 0) ? SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcqDevice) : 0;

   printf("\nSelect the acquisition device (or 'q' to quit)");
	printf("\n..............................................\n");

   for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
   {
      char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
      SapManager::GetResourceName(acqServerName, SapManager::ResourceAcq, deviceIndex, deviceName, sizeof(deviceName));
      printf("%d: %s\n", deviceIndex+1, deviceName);
   }

   if (deviceCount == 0)
   {
      for (int cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
      {
         char cameraName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
         SapManager::GetResourceName(acqServerName, SapManager::ResourceAcqDevice, cameraIndex, cameraName, sizeof(cameraName));
         printf("%d: %s\n", cameraIndex+1, cameraName);
      }
   }


   key = (char)_getch();
   if (key != 0)
   {
	   if (key == 'q')
			return FALSE;
      int deviceNum = key - '0'; // char-to-int conversion
      if ((deviceNum >= 1) && (deviceNum <= deviceCount+cameraCount))
      {
         *pAcqDeviceIndex = deviceNum-1;
      }
      else
      {
         printf("Invalid selection!\n");
         return FALSE;
      }
   }
   else
   {
      printf("Invalid selection!\n");
      return FALSE;
   }

   // List all files in the config directory
   char configPath[MAX_PATH];
   configPath[0] = '\0';
	GetEnvironmentVariable("SAPERADIR", configPath, sizeof(configPath));
   configPath[sizeof(configPath) - 1] = '\0';
	CorStrncat(configPath, "\\CamFiles\\User\\", sizeof(configPath));

   char findPath[MAX_PATH];
   CorStrncpy(findPath, configPath, MAX_PATH);
   CorStrncat(findPath, "*.ccf", sizeof(findPath));

	HANDLE fhandle;
	WIN32_FIND_DATA fdata;
	if ((fhandle = FindFirstFile(findPath, &fdata)) == INVALID_HANDLE_VALUE)
	{
		if (cameraCount==0)
		{
		printf("No config file found.\nUse CamExpert to generate a config file before running this example.\n");
		return FALSE;
		}
	}

	fhandle = FindFirstFile(findPath, &fdata);

    // Try to find the last letter to choose

    int configFileMenuCount = 0;
    char lastCharMenu='x';
    do
	{
		// Use numbers 0 to 9, then lowercase letters if there are more than 10 files
		int configFileMenuShow = configFileMenuCount+1;
		if (configFileMenuCount > 9)
            lastCharMenu = (char)(configFileMenuShow - 10 + 'a');
		configFileMenuCount++;
	}
	while (FindNextFile(fhandle, &fdata) && configFileMenuCount < MAX_CONFIG_FILES);
	FindClose(fhandle);

   printf("\nSelect the config file (or 'q' to quit)");
   printf("\n.......................................\n");
	if (deviceCount == 0 && cameraCount != 0)
   {
      printf("0: No config File.\n");
   }
	
    fhandle = FindFirstFile(findPath, &fdata);							//find first file
   BOOL moreFilesAvailable = TRUE;
 	
   int configFileCount = 1;
	while ((fhandle != INVALID_HANDLE_VALUE && moreFilesAvailable) && configFileCount < MAX_CONFIG_FILES)
	{
		// Use numbers 0 to 9, then lowercase letters if there are more than 10 files
		if (configFileCount <= 9)
			printf("%d: %s\n", configFileCount, fdata.cFileName);
		else
			printf("%c: %s\n", configFileCount - 10 + 'a', fdata.cFileName);
		CorStrncpy(configFileNames[configFileCount], fdata.cFileName, sizeof(configFileNames[configFileCount]));
		configFileCount++;
		moreFilesAvailable = FindNextFile(fhandle, &fdata);
	}
	 
	 FindClose(fhandle);

   key = (char)_getch();
   if (key != 0)
   {
	   if (key == '0' && cameraCount != 0)
	   {
			CorStrncpy(configFileName, "NoFile",7); 
			return TRUE;
	   }
		   
		if (key == 'q')
			return FALSE;
      // Use numbers 1 to 9, then lowercase letters if there are more than 9 files
      int configNum;
      if (key >= '0' && key <= '9')
         configNum = key - '0'; // char-to-int conversion
      else
         configNum = key - 'a' + 10; // char-to-int conversion

      if ((configNum >= 0) && (configNum <= configFileCount-1))
      {
		  CorStrncpy(configFileName, configPath, sizeof(configPath));
		  CorStrncat(configFileName, configFileNames[configNum], sizeof(configFileNames[configNum]));
      }
      else
      {
         printf("Invalid selection!\n"); 
         return FALSE;
      }
   }
   else
   {
      printf("Invalid selection!\n");
      return FALSE;
   }

   printf("\n");
   return TRUE;
}


BOOL GetCorAcqDeviceOptionsFromQuestions(char *acqServerName, UINT32 *pAcqDeviceIndex, BOOL showGigEOnly)
{
   int serverCount = SapManager::GetServerCount();
	int acqDeviceCount = 0;
   int deviceCount = 0;
   int GenieIndex = 0;
   int deviceIndex = 0;
   std::vector<std::string> v_ServerNames;
   if (serverCount == 0)
   {
      printf("No device found!\n");
      return FALSE;
   }

   printf("\nSelect one of the camera(s) detected (or 'q' to quit)");
	printf("\n....................................................\n");

   char serverName[CORSERVER_MAX_STRLEN]; 
   for (int serverAcqIndex = 0; serverAcqIndex < serverCount; serverAcqIndex++)
   {
		if (SapManager::GetResourceCount(serverAcqIndex, SapManager::ResourceAcqDevice) != 0)
      {
         SapManager::GetServerName(serverAcqIndex, serverName, sizeof(serverName));
         acqDeviceCount++;
#ifdef GRAB_CAMERA_LINK
         if (strstr(serverName, CAMERA_LINK_SERVER_NAME_PREFIX) == 0 && SapManager::GetResourceCount(serverAcqIndex, SapManager::ResourceAcq) == 0)
            acqDeviceCount--;
#else
         if (showGigEOnly)
         {
            if (strstr(serverName, CAMERA_LINK_SERVER_NAME_PREFIX) != 0 || SapManager::GetResourceCount(serverAcqIndex, SapManager::ResourceAcq) != 0)
               acqDeviceCount--;
         }
#endif
		}
	}

   BOOL serverFound = FALSE;
     
   for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
   {
		if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcqDevice) != 0)
      {
			SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
#ifdef GRAB_CAMERA_LINK
         if (strstr(serverName, CAMERA_LINK_SERVER_NAME_PREFIX) == 0 && SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcq) == 0)
            continue;
#else
         if (showGigEOnly)
         {
            if (strstr(serverName, CAMERA_LINK_SERVER_NAME_PREFIX) != 0 || SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcq) != 0)
               continue;
         }
#endif
			printf("%d: %s\n", GenieIndex+1, serverName);
			GenieIndex++;
			serverFound = TRUE;
			deviceCount = GenieIndex;

			char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
			deviceIndex=0;
			SapManager::GetResourceName(serverName, SapManager::ResourceAcqDevice, deviceIndex, deviceName, sizeof(deviceName));
			printf("    %s%s\n", "User defined Name : ", deviceName);
			printf("........................................\n");			
			v_ServerNames.push_back(serverName);
      }
   }

   // At least one acquisition server must be available
   if (!serverFound)
   {
      printf("No camera found!\n");
      return FALSE;
   }

   char key = (char)_getch();
   if (key != 0)
   {
		if (key == 'q')
			return FALSE;
      int serverNum = key - '0'; // char-to-int conversion
      if ((serverNum >= 1) && (serverNum < GenieIndex+1))
      {
		  std::string sServerName = v_ServerNames[serverNum-1];
		  // Get the Acquisition Server name selected.
		  CorStrncpy(acqServerName, sServerName.c_str(),sServerName.size()+1);
		  *pAcqDeviceIndex = 0;
      }
      else
      {
         printf("Invalid selection!\n");
         return FALSE;
      }
   }
   else
   {
      printf("Invalid selection!\n");
      return FALSE;
   }

   printf("\n");
   return TRUE;
}

#ifndef SAP_CAMERA_SDK
BOOL IsMonoBuffer(SapBuffer* Buffers)
{
	SapFormat format = Buffers->GetFormat();

	if(CORDATA_FORMAT_IS_MONO(format))
		return TRUE;
	else
		return FALSE;
}

SapData SetDataValue(SapBuffer* Buffers, DWORD *pPrmIndex)
{
	if(IsMonoBuffer(Buffers) == TRUE)
	{
		SapDataMono mono(128);
		return mono;
	}
	else
	{
		SapDataRGB rgb;
		//SapDataRGB rgb(m_pInfoList->GetValueAt(*pPrmIndex), m_pInfoList->GetValueAt(*pPrmIndex+1), m_pInfoList->GetValueAt(*pPrmIndex+2));
		*pPrmIndex = *pPrmIndex+3;
		return rgb;
	}
}

BOOL GetLUTOptionsFromQuestions(SapBuffer* Buffers, SapLut* m_pLut, char *chAcqLutName)
{
   //////// Ask questions to user to select LUT mode ////////
	DWORD prmIndex = 1;
	char acqLutFileName[STRING_LENGTH];

   printf("\nSelect the LookUpTable mode you want to apply: \n");

   printf("%s: %s\n", "a", "Normal mode");
   printf("%s: %s\n", "b", "Arithmetic mode");
   printf("%s: %s\n", "c", "Binary mode");
   printf("%s: %s\n", "d", "Boolean mode");
   printf("%s: %s\n", "e", "Gamma mode");
   printf("%s: %s\n", "f", "Reverse mode");
   printf("%s: %s\n", "g", "Roll mode");
   printf("%s: %s\n", "h", "Shift mode");
   printf("%s: %s\n", "i", "Slope mode");
   printf("%s: %s\n", "j", "Threshold single mode");
   printf("%s: %s\n", "k", "Threshold double mode");

   char key = (char)_getch();
   if (key != 0)
   {
      if (key == 'a' || key == 'b' || key == 'c' || key == 'd' || key == 'e' || key == 'f' || key == 'g' || key == 'h' || key == 'i' || key == 'j' || key == 'k')
		{

			switch (::tolower(key))
			{
					case 'a':
					{
						m_pLut->Normal();
						CorStrncpy(acqLutFileName, "Normal_Lut_Mode.lut", sizeof("Normal_Lut_Mode.lut"));
						CorStrncpy(chAcqLutName, "Normal Lut", sizeof("Normal Lut"));
						break;
					}
					case 'b':
					{
						int operationMode = 0;//Linear plus offset with clip
						/*
							Others operations available
						*/
						//int operation = 1;//Linear minus offset(absolute)
						//int operation = 2;//Linear minus offset(with clip)
						//int operation = 3;//Linear with lower clip
						//int operation = 4;//Linear with upper clip
						//int operation = 5;//Scale to maximum limit
						
						SapData offSet;
						offSet = SetDataValue(Buffers, &prmIndex);
						m_pLut->Arithmetic((SapLut::ArithmeticOp)operationMode, offSet);
						CorStrncpy(acqLutFileName, "Arithmetic_Lut_Mode.lut", sizeof("Arithmetic_Lut_Mode.lut"));
						CorStrncpy(chAcqLutName, "Arithmetic Lut", sizeof("Arithmetic Lut"));
						break;
					}
					case 'c':
					{
						SapData clipValue;
						clipValue = SetDataValue(Buffers, &prmIndex);
						m_pLut->BinaryPattern(0, clipValue);
						CorStrncpy(acqLutFileName, "Binary_Lut_Mode.lut", sizeof("Binary_Lut_Mode.lut"));
						CorStrncpy(chAcqLutName, "Binary Lut", sizeof("Binary Lut"));
						break;
					}
					case 'd':
					{
						SapData booleanFunction;
						booleanFunction = SetDataValue(Buffers, &prmIndex);
						m_pLut->Boolean((SapLut::BooleanOp)0, booleanFunction);
						/*
							Others operations available
						*/
						// AND
						//m_pLut->Boolean((SapLut::BooleanOp)1, booleanFunction);
						// OR
						//m_pLut->Boolean((SapLut::BooleanOp)2, booleanFunction);
						// XOR
						CorStrncpy(acqLutFileName, "Boolean_Lut_Mode.lut", sizeof("Boolean_Lut_Mode.lut"));
						CorStrncpy(chAcqLutName, "Boolean Lut", sizeof("Boolean Lut"));
						break;
					}
					case 'e':
					{
						int gammaFactor = (int)(2*GAMMA_FACTOR);
						m_pLut->Gamma((float)gammaFactor/GAMMA_FACTOR);
						CorStrncpy(acqLutFileName, "Gamma_Lut_Mode.lut", sizeof("Gamma_Lut_Mode.lut"));
						CorStrncpy(chAcqLutName, "Gamma Lut", sizeof("Gamma Lut"));
						break;
					}
					case 'f':
					{
						m_pLut->Reverse();
						CorStrncpy(acqLutFileName, "Reverse_Lut_Mode.lut", sizeof("Reverse_Lut_Mode.lut"));
						CorStrncpy(chAcqLutName, "Reverse Lut", sizeof("Reverse Lut"));
						break;
					}
					case 'g':
					{
						int numEntries = 128;
						m_pLut->Roll(numEntries);
						CorStrncpy(acqLutFileName, "Roll_Lut_Mode.lut", sizeof("Roll_Lut_Mode.lut"));
						CorStrncpy(chAcqLutName, "Roll Lut", sizeof("Roll Lut"));
						break;
					}
					case 'h':
					{
						int bitsToShift = 3;
						m_pLut->Shift(bitsToShift);
						CorStrncpy(acqLutFileName, "Shift_Lut_Mode.lut", sizeof("Shift_Lut_Mode.lut"));
						CorStrncpy(chAcqLutName, "Shift Lut", sizeof("Shift Lut"));
						break;
					}
					case 'i':
					{
						int startIndex1 = 76;
						int endIndex1 = 179;
						BOOL clipOutSide = FALSE;//TRUE
						SapData minValue;
						SapData maxValue;
						minValue = SetDataValue(Buffers, &prmIndex);
						maxValue = SetDataValue(Buffers, &prmIndex);
						m_pLut->Slope(startIndex1, endIndex1, minValue, maxValue, clipOutSide);
						CorStrncpy(acqLutFileName, "Slope_With_Range_Lut_Mode.lut", sizeof("Slope_With_Range_Lut_Mode.lut"));
						CorStrncpy(chAcqLutName, "Slope With Range Lut", sizeof("Slope With Range Lut"));
						break;
					}
					case 'j':
					{
						SapData treshValue;
						treshValue = SetDataValue(Buffers, &prmIndex);
						m_pLut->Threshold(treshValue);
						CorStrncpy(acqLutFileName, "Threshold_Single_Mode.lut", sizeof("Threshold_Single_Mode.lut"));
						CorStrncpy(chAcqLutName, "Threshold Single Lut", sizeof("Threshold Single Lut"));
						break;
					}
					case 'k':
					{
						SapData treshValue1;
						SapData treshValue2;
						treshValue1 = SetDataValue(Buffers, &prmIndex);
						treshValue2 = SetDataValue(Buffers, &prmIndex);
						m_pLut->Threshold(treshValue1, treshValue2);
						CorStrncpy(acqLutFileName, "Threshold_Double_Mode.lut", sizeof("Threshold_Double_Mode.lut"));
						CorStrncpy(chAcqLutName, "Threshold Double Lut", sizeof("Threshold Double Lut"));
						break;
					}
			}
		}         
      else
      {
         printf("\nInvalid selection!\n");
         return FALSE;
      }
   }
   else
   {
      printf("\nInvalid selection!\n");
      return FALSE;
   }

	m_pLut->Save(acqLutFileName);		// Save LUT to file (can be reloaded in the main demo)
   printf("\n");
   return TRUE;
}
#endif   // SAP_CAMERA_SDK


BOOL GetLoadLUTFiles(SapLut* Lut, std::vector<std::string> v_list, BOOL* bLutLoaded)
{
   //////// Ask questions to user to select LUT file ////////
	std::string fileName;
	std::string fullFileName;

	v_list = GetLUTFilesSaved(".", v_list, FALSE);
	int numVect = (int)v_list.size();

	if (numVect!=0)
	{
			printf("\nDo you want to load an existing LUT file? y/n (Yes/No)  \n");
			char keyQuestion = (char)_getch();
			if (keyQuestion != 0)
			{
				if (keyQuestion == 'n')
				{
					*bLutLoaded = FALSE;
					return TRUE;
				}
				else if (keyQuestion == 'y')
					goto select_lut;
				else
				{
					printf("\nInvalid selection!\n");
					return FALSE;
				}
			}
			else
			{
				printf("\nInvalid selection!\n");
				return FALSE;
			}

	select_lut:
		printf("\nSelect the LUT file available: \n");

		v_list = GetLUTFilesSaved(".", v_list, TRUE);

			char key = (char)_getch();
			if (key != 0)
			{
				int fileNum = GetKeyCharIndex(key);
				if ((fileNum != -1) && (fileNum >= 0) && (fileNum < (int)v_list.size() ))
				{
					fileName = v_list[fileNum];
					fullFileName = fileName + ".lut";
					// Load LUT (saved before in the main demo)
					if (Lut->Load(fullFileName.c_str()))
						*bLutLoaded=TRUE;			

				printf("\n");
				if (bLutLoaded)
					printf("%s%s\n", fullFileName.c_str(), " loaded.");

				}         
				else
				{
					printf("Invalid selection!\n");
					return FALSE;
				}
			}
			else
			{
				printf("Invalid selection!\n");
				return FALSE;
			}
	}

return TRUE;
}


#ifndef SAP_CAMERA_SDK
BOOL GetLoadDynamicLUTFiles(SapDynamicLut* DynamicLut, int lutIndex, std::vector<std::string> v_list, BOOL* bLutLoaded)
{
   //////// Ask questions to user to select LUT file ////////
	std::string fileName;
	std::string fullFileName;

	v_list = GetLUTFilesSaved(".", v_list, FALSE);
	int numVect = (int)v_list.size();

	if (numVect!=0)
	{
			printf("\nSelect one of the Dynamic LUT files available: \n");

			// Remove vector content.
			v_list.erase(v_list.begin(), v_list.end());

			v_list = GetLUTFilesSaved(".", v_list, TRUE);

			int listSize = (int)v_list.size();

			char key = (char)_getch();
			if (key != 0)
			{
				int fileNum = GetKeyCharIndex(key);
				if ((fileNum != -1) && (fileNum >= 0) && (fileNum < listSize ))
				{
					fileName = v_list[fileNum];
					fullFileName = fileName + ".lut";
					// Load LUT (saved before in the main demo)
					if (DynamicLut->Load(lutIndex, fullFileName.c_str()))
						*bLutLoaded=TRUE;			

				printf("\n");
				if (bLutLoaded)
					printf("%s%s\n", fullFileName.c_str(), " loaded.");

				}         
				else
				{
					printf("Invalid selection!\n");
					return FALSE;
				}
			}
			else
			{
				printf("Invalid selection!\n");
				return FALSE;
			}
	}
	else
	{
		printf("\nNo Dynamic LUT file available.\n\n");
		return FALSE;
	}

return TRUE;
}
#endif   // SAP_CAMERA_SDK

std::vector<std::string> GetLUTFilesSaved(const char* Dossier, std::vector<std::string> pList, BOOL bPrint)
{ 
HANDLE hFind; 
WIN32_FIND_DATA FindData; 

// Set the directory
SetCurrentDirectory (Dossier); 

// Beginning of search
hFind=FindFirstFile ("*.*", &FindData); 
//plist.empty();
int fileIndex=0;

if (hFind!=INVALID_HANDLE_VALUE) 
{ 
	// Next files
	while (FindNextFile (hFind, &FindData)) 
	{ 
		if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
		{ 
		char* file_name = FindData.cFileName;
		char * pch;
      char *nextToken = NULL;

		pch = CorStrtok(file_name, ".", &nextToken);

		char* extension_file = pch;
		while (pch != NULL)
		{
			extension_file = pch;

         pch = CorStrtok(NULL, ".", &nextToken);
			if (pch == NULL) 
            break;
		}

         if (extension_file != NULL && strlen(extension_file))
         {
				if(strcmp(extension_file,"lut") == 0)
				{
		         if (bPrint)
					{
						switch(fileIndex)
						{
							case 0:
							{
								printf("%s: %s\n", "a", file_name);
								break;
							}
							case 1:
							{
								printf("%s: %s\n", "b", file_name);
								break;
							}
							case 2:
							{
								printf("%s: %s\n", "c", file_name);
								break;
							}
							case 3:
							{
								printf("%s: %s\n", "d", file_name);
								break;
							}
							case 4:
							{
								printf("%s: %s\n", "e", file_name);
								break;
							}
							case 5:
							{
								printf("%s: %s\n", "f", file_name);
								break;
							}
							case 6:
							{
								printf("%s: %s\n", "g", file_name);
								break;
							}
							case 7:
							{
								printf("%s: %s\n", "h", file_name);
								break;
							}
							case 8:
							{
								printf("%s: %s\n", "i", file_name);
								break;
							}
							case 9:
							{
								printf("%s: %s\n", "j", file_name);
								break;
							}
							case 10:
							{
								printf("%s: %s\n", "k", file_name);
								break;
							}
							case 11:
							{
								printf("%s: %s\n", "l", file_name);
								break;
							}
							case 12:
							{
								printf("%s: %s\n", "m", file_name);
								break;
							}
						//printf("%d: %s\n", fileIndex, file_name);
						}
					}
					fileIndex++;
					pList.push_back(file_name);
				}
			} 

		} 
	}//end while 
}
// End of search
FindClose (hFind); 
return pList;
} 




BOOL GetCorAcquisitionOptionsFromQuestions(char *acqServerName, UINT32 *pAcqDeviceIndex, char *configFileName)
{
   //////// Ask questions to user to select acquisition board/device and config file ////////

   // Get total number of boards in the system
   int serverCount = SapManager::GetServerCount();
   if (serverCount == 0)
   {
      printf("No device found!\n");
      return FALSE;
   }

   printf("\nSelect the acquisition server (or 'q' to quit)");
	printf("\n..............................................\n");

   // Scan the boards to find those that support acquisition
   BOOL serverFound = FALSE;

   for (int serverIndex = 0; serverIndex < serverCount; serverIndex++)
   {
      if (SapManager::GetResourceCount(serverIndex, SapManager::ResourceAcq) != 0)
      {
         char serverName[CORSERVER_MAX_STRLEN];
         SapManager::GetServerName(serverIndex, serverName, sizeof(serverName));
         printf("%d: %s\n", serverIndex, serverName);
         serverFound = TRUE;
      }
   }

   // At least one acquisition server must be available
   if (!serverFound)
   {
      printf("No acquisition server found!\n");
      return FALSE;
   }

   char key = (char)_getch();
   if (key != 0)
   {
	   if (key == 'q')
			return FALSE;

      int serverNum = key - '0'; // char-to-int conversion
      if ((serverNum >= 1) && (serverNum < serverCount))
      {
         // update board name
         SapManager::GetServerName(serverNum, acqServerName, CORSERVER_MAX_STRLEN);
      }
      else
      {
         printf("Invalid selection!\n");
         return FALSE;
      }
   }
   else
   {
      printf("Invalid selection!\n");
      return FALSE;
   }


// Scan all the acquisition devices on that server and show menu to user
int deviceCount = SapManager::GetResourceCount(acqServerName, SapManager::ResourceAcq);
int cameraCount = 0;

#ifndef GRAB_CAMERA_LINK
   printf("\nSelect the acquisition device (or 'q' to quit)");
	printf("\n..............................................\n");
#else
   printf("\nSelect the device you wish to use on this server (or 'q' to quit)");
	printf("\n.................................................................\n");
#endif

   for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
   {
      char deviceName[CORPRM_GETSIZE(CORACQ_PRM_LABEL)];
      SapManager::GetResourceName(acqServerName, SapManager::ResourceAcq, deviceIndex, deviceName, sizeof(deviceName));
      printf("%d: %s\n", deviceIndex+1, deviceName);
   }


   key = (char)_getch();
   if (key != 0)
   {
	   if (key == 'q')
			return FALSE;
      int deviceNum = key - '0'; // char-to-int conversion
      if ((deviceNum >= 1) && (deviceNum <= deviceCount))
      {
         *pAcqDeviceIndex = deviceNum-1;
      }
      else
      {
         printf("Invalid selection!\n");
         return FALSE;
      }
   }
   else
   {
      printf("Invalid selection!\n");
      return FALSE;
   }

   // List all files in the config directory
   char configPath[MAX_PATH];
   configPath[0] = '\0';
	GetEnvironmentVariable("SAPERADIR", configPath, sizeof(configPath));
   configPath[sizeof(configPath) - 1] = '\0';
	CorStrncat(configPath, "\\CamFiles\\User\\", sizeof(configPath));

   char findPath[MAX_PATH];
   CorStrncpy(findPath, configPath, MAX_PATH);
   CorStrncat(findPath, "*.ccf", sizeof(findPath));


	HANDLE fhandle;
	WIN32_FIND_DATA fdata;
	if ((fhandle = FindFirstFile(findPath, &fdata)) == INVALID_HANDLE_VALUE)
	{
		if (cameraCount==0)
		{
		printf("No config file found.\nUse CamExpert to generate a config file before running this example.\n");
		return FALSE;
		}
	}


   int configFileCount = 0;
	do
	{
	  configFileCount++;
	}
	while (FindNextFile(fhandle, &fdata) && configFileCount < MAX_CONFIG_FILES);
	FindClose(fhandle);
	fhandle = FindFirstFile(findPath, &fdata);

    // Try to find the last letter to choose

    int configFileMenuCount = 0;
    char lastCharMenu='x';
    do
	{
		// Use numbers 0 to 9, then lowercase letters if there are more than 10 files
		int configFileMenuShow = configFileMenuCount+1;
		if (configFileMenuCount > 9)
            lastCharMenu = (char)(configFileMenuShow - 10 + 'a');
		//CorStrncpy(configFileNames[configFileCount], fdata.cFileName, sizeof(configFileNames[configFileCount]));
		configFileMenuCount++;
	}
	while (FindNextFile(fhandle, &fdata) && configFileMenuCount < MAX_CONFIG_FILES);
	FindClose(fhandle);

   printf("\nSelect the config file (or 'q' to quit)");
	printf("\n.......................................\n");

   configFileCount = 0;
   fhandle = FindFirstFile(findPath, &fdata);
	 do
	 {
		  // Use numbers 0 to 9, then lowercase letters if there are more than 10 files
		  int configFileShow = configFileCount+1;
		  if (configFileCount < 9)
				printf("%d: %s\n", configFileShow, fdata.cFileName);
		  else
				printf("%c: %s\n", configFileShow - 10 + 'a', fdata.cFileName);
		  CorStrncpy(configFileNames[configFileCount], fdata.cFileName, sizeof(configFileNames[configFileCount]));
		  configFileCount++;
	 }
	 while (FindNextFile(fhandle, &fdata) && configFileCount < MAX_CONFIG_FILES);
	 FindClose(fhandle);

   key = (char)_getch();
   if (key != 0)
   {
	   if (key == '1' && cameraCount != 0)
			return TRUE;
		if (key == 'q')
			return FALSE;
      // Use numbers 1 to 9, then lowercase letters if there are more than 9 files
      int configNum;
      if (key >= '1' && key <= '9')
         configNum = key - '1'; // char-to-int conversion
      else
         configNum = key - 'a' + 9; // char-to-int conversion

      if ((configNum >= 0) && (configNum <= configFileCount))
      {
		  CorStrncpy(configFileName, configPath, sizeof(configPath));
		  CorStrncat(configFileName, configFileNames[configNum], sizeof(configFileNames[configNum]));
      }
      else
      {
         printf("Invalid selection!\n"); 
         return FALSE;
      }
   }
   else
   {
      printf("Invalid selection!\n");
      return FALSE;
   }

   printf("\n");
   return TRUE;
}


#ifndef SAP_CAMERA_SDK
bool GetParametersFrom3DBuffer(SapBuffer *Buffer, int* Invalid_Flag_C, double* Invalid_Value_C,
   double* Scale_C, double* Offset_C, BUFFER_LAYOUT* Buffer_Layout)
{
	if(Buffer->GetHeight() > 1)
		*Buffer_Layout = Range;
	else
		*Buffer_Layout = Profile;

	if (!Buffer->GetParameter(CORBUFFER_PRM_SCAN3D_INVALID_DATA_VALUE_C, Invalid_Value_C))
		return false;
	if (!Buffer->GetParameter(CORBUFFER_PRM_SCAN3D_INVALID_DATA_FLAG_C, Invalid_Flag_C))
		return false;
	if (!Buffer->GetParameter(CORBUFFER_PRM_SCAN3D_COORD_SCALE_C, Scale_C))
		return false;
	if (!Buffer->GetParameter(CORBUFFER_PRM_SCAN3D_COORD_OFFSET_C, Offset_C))
		return false;

	return true;
}

void DisplayRange(SapView * View, SapBuffer * Buffer)
{
	SapBuffer *Buffer_View = (View->GetBuffer());
	Buffer_View->Clear();
	Buffer_View->Copy(Buffer);
	View->Show();
}

void DisplayProfile(SapView * View, SapBuffer * Buffer)
{
	SapBuffer *Buffer_View = (View->GetBuffer());
	void *pData_viewbuf;
	void *pData_buf;

	Buffer_View->Clear();
	Buffer_View->GetAddress(&pData_viewbuf);
	Buffer->GetAddress(&pData_buf);

	unsigned short* begin_viewbuf = (unsigned short*)pData_viewbuf;
	unsigned short* begin_buf = (unsigned short*)pData_buf;

	for (int i = 0; i < Buffer_View->GetWidth(); ++i)
	{
		int value = *(begin_buf + i) / CLUSTERS_DIVISOR; //Convert [0-MAX_USHORT[ to  [0, WINDOWS_HEIGHT[
		*(begin_viewbuf + (Buffer_View->GetWidth() * (PROFILE_WINDOW_HEIGHT - value - 1) + i)) = USHRT_MAX;
	}
	View->Show();
}

//Process (Mean) the C component of each line and does a mean of mean. Get World and Pixel Mean
void ProcessingRange(SapBuffer * Buffer, int Invalid_Flag_C, double Invalid_Value_C,
   double Scale_C, double Offset_C, int * Pixel_Mean,int * World_Mean)
{
	int Nb_Pixels = 0;
	int Pixel_Mean_Line = 0;
	void *pData_buf;

	Buffer->GetAddress(&pData_buf);
	unsigned short *begin = (unsigned short*)pData_buf;

	if (Invalid_Flag_C != 0) //Check for Invalid Values only if the flag is true.
	{
		for (int i = 0; i < Buffer->GetWidth() *  Buffer->GetHeight(); ++i)
		{
			if (*(begin + i) != Invalid_Value_C)
			{
				Pixel_Mean_Line += *(begin + i);
				Nb_Pixels++;
			}
			if (i % Buffer->GetWidth() == 0)
			{
            if (Nb_Pixels)
				   *Pixel_Mean += Pixel_Mean_Line / Nb_Pixels;
				Pixel_Mean_Line = 0;
				Nb_Pixels = 0;
			}
		}
	}
	else
	{
		for (int i = 0; i <Buffer->GetWidth()* Buffer->GetHeight(); ++i)
		{
			Pixel_Mean_Line += *(begin + i);
			if (i % Buffer->GetWidth() == 0)
			{
				*Pixel_Mean += Pixel_Mean_Line / Buffer->GetWidth();
				Pixel_Mean_Line = 0;
			}
		}
		Nb_Pixels = Buffer->GetWidth() * Buffer->GetHeight();
	}

	*Pixel_Mean = *Pixel_Mean / Buffer->GetHeight();

	// Pixel to World : (Position Pixel X * Scale X) + Offset X = Position Pixel World
	*World_Mean = (int) ((*Pixel_Mean * Scale_C) + Offset_C);
}

//Process (Mean) the C component of a single line. Get World and Pixel Mean
void ProcessingProfile(SapBuffer * Buffer, int Invalid_Flag_C, double Invalid_Value_C,
   double Scale_C, double Offset_C, int * Pixel_Mean,int * World_Mean)
{
	int Nb_Pixels = 0;
	void *pData_buf;

	Buffer->GetAddress(&pData_buf);
	unsigned short *begin = (unsigned short*)pData_buf;

	if (Invalid_Flag_C != 0) //Check for Invalid Values only if the flag is true.
	{
		for (int i = 0; i < Buffer->GetWidth(); ++i)
		{
			if (*(begin + i) != Invalid_Value_C)
			{
				*Pixel_Mean += *(begin + i);
				Nb_Pixels++;
			}
		}
	}
	else
	{
		for (int i = 0; i < Buffer->GetWidth(); ++i)
		{
			*Pixel_Mean += *(begin + i);
		}
		Nb_Pixels = Buffer->GetWidth();
	}
   if (Nb_Pixels)
   	*Pixel_Mean = *Pixel_Mean / Nb_Pixels;

	// Pixel to World : (Position Pixel X * Scale X) + Offset X = Position Pixel World
	*World_Mean = (int) ((*Pixel_Mean * Scale_C) + Offset_C);

}

//Ask the user for input to display a component. A(X) = 0, C(Z) = 1. R(Reflectance) = 2 and 0(unused) = 3
bool GetSelectionFromConsoleCR(int* pIndex_DisplayedComponent)
{
	printf("\nSelect the component to display (or 'q' to quit)");
	printf("\n..............................................\n");
	printf("1: Z (C)\n");
	printf("2: Reflectance (R)\n");
	char c = (char)CorGetch();
	if (!(c == 'q' || c == '1' || c == '2'))
	{
		printf("\nInvalid selection!\n");
		return false;
	}
	else
	{
		if (c == 'q')
		{
			printf("\nQuitting!\n");
			return false;
		}
		else
		{
			*pIndex_DisplayedComponent = c - '0';
		}
	}
	return true;
}

bool GetSelectionFromConsoleAC(int* pIndex_DisplayedComponent)
{
	printf("\nSelect the component to display (or 'q' to quit)");
	printf("\n..............................................\n");
	printf("0: X (A)\n");
	printf("1: Z (C)\n");
	char c = (char)CorGetch();;
	if (!(c == 'q' || c == '1' || c == '0'))
	{
		printf("\nInvalid selection!\n");
		return false;
	}
	else
	{
		if (c == 'q')
		{
			printf("\nQuitting!\n");
			return false;
		}
		else
		{
			*pIndex_DisplayedComponent = c - '0';
		}
	}
	return true;
}

bool GetSelectionFromConsoleACRW(int* pIndex_DisplayedComponent)
{
	printf("\nSelect the component to display (or 'q' to quit)");
	printf("\n..............................................\n");
	printf("0: X (A)\n");
	printf("1: Z (C)\n");
	printf("2: Reflectance (R)\n");
	char c = (char)CorGetch();;
	if (!(c == 'q' || c == '1' || c == '0' || c == '2'))
	{
		printf("\nInvalid selection!\n");
		return false;
	}
	else
	{
		if (c == 'q')
		{
			printf("\nQuitting!\n");
			return false;
		}
		else
		{
			*pIndex_DisplayedComponent = c - '0';
		}
	}
	return true;
}
#endif   // SAP_CAMERA_SDK

int GetKeyCharIndex(char key) 
{
   if (key != 0)
   {
			switch (::tolower(key))
			{
					case 'a':
						return 0;
					case 'b':
						return 1;
					case 'c':
						return 2;
					case 'd':
						return 3;
					case 'e':
						return 4;
					case 'f':
						return 5;
					case 'g':
						return 6;
					case 'h':
						return 7;
					case 'i':
						return 8;
					case 'j':
						return 9;
					case 'k':
						return 10;
					case 'l':
						return 11;
					case 'm':
						return 12;
					case 'n':
						return 13;
					case 'o':
						return 14;
					case 'p':
						return 15;
					case 'q':
						return 16;
					case 'r':
						return 17;
					case 's':
						return 18;
					case 't':
						return 19;
					case 'u':
						return 20;
					case 'v':
						return 21;
					case 'w':
						return 22;
					case 'x':
						return 23;
					case 'y':
						return 24;
					case 'z':
						return 25;
			}
		}         
   else
   {
      printf("\nInvalid selection!\n");
      return -1;
   }
return -1;
}

BOOL GetFileOptions( int argc, char *argv[], char *fileDirectory, const char *fileList[], int fileListSize, char *filename, int size)
{
   // Check the command line for user commands
   if(argc >= 2)
   {
      if( (strcmp(argv[1], "/?") == 0) || (strcmp(argv[1], "-?") == 0) )
      {
         // print help
         printf("Usage:\n");
         printf("FileLoadCPP [<image filename>]\n");
         return FALSE;
      }

      // Check if user specified filename
      if(argc == 2)
      {
         // Verify that the specified file exists
        	OFSTRUCT of = {0};
         if (OpenFile(argv[1], &of, OF_EXIST) == HFILE_ERROR)
         {
            printf("Specified image file (%s) is invalid!\n", argv[1]);
            return FALSE;
         }

         CorStrncpy(filename, argv[1], size);

         return TRUE;
      }

      printf("Invalid command line!\n");
      return FALSE;
   }

   /// Ask user to select image file ///

   printf("Choose one image file: \n");
   for (int i=0; i < fileListSize; i++)
      printf("%d: %s\n", i, fileList[i]);

   int key = CorGetch() - '0';

   if (key < 0 || key > fileListSize)
   {
      printf("Invalid selection!\n");
      return FALSE;
   }
 
   CorStrncpy(filename, fileDirectory, size);
   CorStrncat(filename, fileList[key], size - strlen(filename));
   AddSaperaPath(filename, size);
   FixFileName(filename, MAX_PATH);

   return TRUE;
}

BOOL AddSaperaPath(char *filename, int size)
{
	static char sapDir[MAX_PATH];
	static char path[MAX_PATH] = "";

	// Get Sapera installation directory
	sapDir[0] = '\0';
	if (!GetEnvironmentVariable("SAPERADIR", sapDir, sizeof(sapDir)))
      return FALSE;

   // Build user string from executable file name and path
	sapDir[sizeof(sapDir) - 1] = '\0';
   CorStrncpy(path, "\"", sizeof(path));
   CorStrncat(path, sapDir, sizeof(path) - strlen(path));
   CorStrncat(path, "\\", sizeof(path) - strlen(path));
   CorStrncat(path, filename, sizeof(path) - strlen(path));
   CorStrncat(path, "\"", sizeof(path) - strlen(path));

   // Copy back to user string
   CorStrncpy(filename, path, size);
   return TRUE;
}

void FixFileName(char *filename, int size)
{
   // Remove quotes in file names
   static char workBuf[MAX_PATH];
   char *pWorkBuf = workBuf;
   size_t length = min(strlen(filename), MAX_PATH);
   size_t pos;

   for (pos = 0; pos < length; pos++)
   {
      if (filename[pos] != '\"')
         *pWorkBuf++ = filename[pos];
   }

   *pWorkBuf = '\0';
   CorStrncpy(filename, workBuf, size);
}
