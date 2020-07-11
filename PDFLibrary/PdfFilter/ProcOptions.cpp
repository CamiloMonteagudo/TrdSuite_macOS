// ProcOptions.cpp: implementation of the ProcOptions class.
//
//////////////////////////////////////////////////////////////////////

#include "ProcOptions.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ProcOptions::ProcOptions()
{
  ownerPassword = NULL;
  userPassword = NULL;

  firstPage = 0;
  lastPage = 0;
  bImages = gTrue;
	bJPG = gTrue;
	bPNG = gTrue;
	bBinary = gTrue;
	//hWnd = NULL;
	//Message = 0;
	pageNum = -1;
	pageTotal = 0;
}

ProcOptions::~ProcOptions()
{
  setPasswords( NULL, NULL);
}

void ProcOptions::setPasswords(char* aOwnerPassword,char* aUserPassword)
{
  if (ownerPassword)
      delete ownerPassword;

  if (userPassword)
      delete userPassword;

  ownerPassword = NULL;
  userPassword = NULL;

  if (aOwnerPassword && aOwnerPassword[0] != 0)
  	ownerPassword = new GString(aOwnerPassword);

  if (aUserPassword && aUserPassword[0] != 0)
  	userPassword = new GString(aUserPassword);
}

void ProcOptions::setRange(int firstPageA, int lastPageA)
{
  firstPage = firstPageA;
  lastPage = lastPageA;
}

void ProcOptions::loadImages(GBool bImagesA, GBool bJPGA, GBool bPNGA, GBool bBinaryA)
{
  bImages = bImagesA;
	bJPG = bJPGA;
	bPNG = bPNGA;
	bBinary = bBinaryA;
}

/*void ProcOptions::setProgress(HWND hWndA, UINT MessageA)
{
  hWnd = hWndA;
	Message = MessageA;
}*/

void ProcOptions::updateProgress(long pageNumA, long pageTotalA)
{
	/*if (hWnd == NULL)
		return;

	pageNum = pageNumA;
	pageTotal = pageTotalA;

	SendMessage(hWnd, Message, 0, 0);

	pageNum = -1;*/
}

/*void ProcOptions::getProgress(long *page, long *total)
{
	*page = pageNum;
	*total = pageTotal;
}*/

