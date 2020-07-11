// ProcOptions.h: interface for the ProcOptions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCOPTIONS_H__548B1E13_A475_4A79_9C6F_442D9B7DC534__INCLUDED_)
#define AFX_PROCOPTIONS_H__548B1E13_A475_4A79_9C6F_442D9B7DC534__INCLUDED_

#include "GTypes.h"
#include "GString.h"

class ProcOptions  
{
public:
    GString* ownerPassword;
    GString* userPassword;

    int firstPage;
    int lastPage;
    GBool bImages,bJPG,bPNG,bBinary;
		//HWND hWnd;
		//UINT Message;
		long pageNum;
		long pageTotal;

    ProcOptions();
    virtual ~ProcOptions();

    void setPasswords(char* aOwnerPassword,char* aUserPassword);
    void setRange(int firstPage, int lastPage);
		void loadImages(GBool bImagesA, GBool bJPGA, GBool bPNGA, GBool bBinaryA);
		//void setProgress(HWND hWndA, UINT MessageA);
		void updateProgress(long pageNumA, long pageTotalA);
		//void getProgress(long *page, long *total);
};

#endif // !defined(AFX_PROCOPTIONS_H__548B1E13_A475_4A79_9C6F_442D9B7DC534__INCLUDED_)
