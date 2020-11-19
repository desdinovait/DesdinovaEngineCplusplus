#include "DEGeneralIncludes.h"

bool DELog::enableLogging=false;
char *DELog::fileNameLog="DesdinovaLog.htm";

DLL_API void DELog::LogStart(char *logFileName, char *fileTitle, char *subTitle)
{
	FILE* pFile = NULL;

	if (strcmp(logFileName,"")!=0)
	{
		//Open the file and clear the contens
		fileNameLog = logFileName;
		pFile = fopen(fileNameLog, "wb");
		if(pFile != NULL)
		{
			//Write start html to log
			fprintf(pFile, "<html><head><title>");
			fprintf(pFile, fileTitle);
			fprintf(pFile, "</title></head><body>\n\r");
			fprintf(pFile, "<font face=\"Verdana\" size=\"5\" color=\"#0000FF\"><b><u>");
			fprintf(pFile, fileTitle);
			fprintf(pFile, "</b></u></font><br>\n\r");
			fprintf(pFile, "<font face=\"Verdana\" size=\"3\" color=\"#0000FF\"><b>");
			fprintf(pFile, subTitle);
			fprintf(pFile, "</b></font><br>\n\r");

			//Close the file
			fclose(pFile);

			enableLogging = true;
		}
		else
		{
			enableLogging = false;
		}
	}
	else
	{
		enableLogging = false;
	}
}

DLL_API void DELog::LogInfo(char *lpszText, ...)
{
	if(enableLogging)
	{
		va_list argList;
		FILE *pFile = NULL;

		//Initialize variable argument list
		va_start(argList, lpszText);

		//Open the log file for appending
		pFile = fopen(fileNameLog, "a+");

		if(pFile != NULL)
		{
			//Write the error to the log file
			fprintf(pFile, "<font face=\"Arial\" size=\"2\" color=\"#000000\">");
			vfprintf(pFile, lpszText, argList);
			fprintf(pFile, "</font><br>\n");

			//Close the file
			fclose(pFile);
		}

		va_end(argList);
	}
}


DLL_API void DELog::LogTitle(char *lpszText, ...)
{
	if(enableLogging)
	{
		va_list argList;
		FILE *pFile = NULL;

		//Initialize variable argument list
		va_start(argList, lpszText);

		//Open the log file for appending
		pFile = fopen(fileNameLog, "a+");

		if(pFile != NULL)
		{
			//Write the error to the log file
			fprintf(pFile, "<font face=\"Arial\" size=\"3\" color=\"#0000FF\"><b><u>");
			vfprintf(pFile, lpszText, argList);
			fprintf(pFile, "</b></u></font><br>\n");

			//Close the file
			fclose(pFile);
		}

		va_end(argList);
	}
}


DLL_API void DELog::LogWarning(char *lpszText, ...)
{
	if(enableLogging)
	{
		va_list argList;
		FILE *pFile = NULL;

		//Initialize variable argument list
		va_start(argList, lpszText);

		//Open the log file for appending
		pFile = fopen(fileNameLog, "a+");

		if(pFile != NULL)
		{
			//Write the error to the log file
			fprintf(pFile, "<font face=\"Arial\" size=\"2\" color=\"#0000FF\">");
			vfprintf(pFile, lpszText, argList);
			fprintf(pFile, "</font><br>\n");

			//Close the file
			fclose(pFile);
		}

		va_end(argList);
	}
}


DLL_API void DELog::LogError(char *lpszText, ...)
{
	if(enableLogging)
	{
		va_list argList;
		FILE *pFile = NULL;

		//Initialize variable argument list
		va_start(argList, lpszText);

		//Open the log file for appending
		pFile = fopen(fileNameLog, "a+");

		if(pFile != NULL)
		{
			//Write the error to the log file
			fprintf(pFile, "<font face=\"Arial\" size=\"2\" color=\"#FF0000\">");
			vfprintf(pFile, lpszText, argList);
			fprintf(pFile, "</font><br>\n");

			//Close the file
			fclose(pFile);
		}

		va_end(argList);
	}
}



DLL_API void DELog::LogStop()
{
	if(enableLogging)
	{
		FILE *pFile = NULL;

		//Open the log file for appending
		pFile = fopen(fileNameLog, "a+");
		
		if(pFile != NULL)
		{
			//Write end html to log
			fprintf(pFile, "</body></html>");

			//Close the file
			fclose(pFile);
		}

		enableLogging = false;
	}
}