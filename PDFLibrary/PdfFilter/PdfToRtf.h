#ifndef _PDF_TO_RTF_H
#define _PDF_TO_RTF_H

//#include <windows.h>


long InitProcess();

void EndProcess(long idProcess);

void SetRange(long idProcess, int firstPage, int lastPage);

void LoadImages(long idProcess, bool bImages, bool bJPG, bool bPNG, bool bBinary);

//void SetProgress(long idProcess, HWND hWnd, UINT Message);

//void GetProgress(long idProcess, long *page, long *total);

void SetPasswords(long idProcess, char* aOwnerPassword,char* aUserPassword);

int Pdf2Rtf(long idProcess, const char* inPDFFile, char** outRtf, long* sLen);

int FreeRtf(char * sRtf);

#endif