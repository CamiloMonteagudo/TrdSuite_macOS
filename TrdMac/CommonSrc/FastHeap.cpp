
#include "WinUtil.h"

/*
#define GENERAL_HEAP_BIG 3000
#define GENERAL_HEAP_MEDIUM 1800
*/

#define GENERAL_HEAP_BIG 6000
#define GENERAL_HEAP_MEDIUM 3000

#define WORD_HEAP_BIG    (GENERAL_HEAP_BIG/6)
#define WORD_HEAP_MEDIUM     (GENERAL_HEAP_MEDIUM/6)

CFastHeap* CFastHeap::gFirstHeap = NULL;
CFastHeap* CFastHeap::gCurrHeap = NULL;
BOOL CFastHeap::gEnabled = FALSE;

CFastHeap* CFastHeap::NewHeap()
	{
	// El primer heap se crea grande, los demas son mas pequeños
	if (gFirstHeap == NULL)
		gCurrHeap = new CFastHeap(GENERAL_HEAP_BIG*1024, WORD_HEAP_BIG*1024);
	else
		gCurrHeap = new CFastHeap(GENERAL_HEAP_MEDIUM*1024, WORD_HEAP_MEDIUM*1024);

	return gCurrHeap;
	}
