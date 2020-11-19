#ifndef H_LOGGING_H
#define H_LOGGING_H

/*
[CLASS] 
[
    [NOME] 
    DELog
    
    [DESCRIZIONE]   
    Classe statica utilizzata per la creazione del file di log.
]
*/
class DELog
{
	public:
		//** Variabili **
		static bool enableLogging;
		static char *fileNameLog;

		//** Funzioni **
		DLL_API static void LogStart(char *logFileName, char *fileTitle, char *subTitle);
		DLL_API static void LogInfo(char *lpszText, ...);
		DLL_API static void LogTitle(char *lpszText, ...);
		DLL_API static void LogWarning(char *lpszText, ...);
		DLL_API static void LogError(char *lpszText, ...);
		DLL_API static void LogStop();
};


#endif