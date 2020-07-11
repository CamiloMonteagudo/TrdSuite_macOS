
#include "WinUtil.h"


char STAnsi::iTempBuffer[TEMP_BUFFER_SIZE];

#ifndef STD_STRING_HAS_REFCOUNT

TStringDataT<STAnsi> NullStringDataT(ENullStrData);

TStringDataT<STAnsi>* STAnsi::m_pNullStrData = &NullStringDataT;

#endif

CStringA CANull;


void InitCStringBuffers()
	{
#ifndef STD_STRING_HAS_REFCOUNT
	TRACE("start NullRefs: %d", (NullStringDataT.GetRefCount()));
#endif
	}

void ResetStringData()
	{
#ifndef STD_STRING_HAS_REFCOUNT
	NullStringDataT.resetNullRefCount();
#endif
	}

void FreeCStringBuffers()
	{
#ifndef STD_STRING_HAS_REFCOUNT
	TRACE("end NullRefs: %d", (NullStringDataT.GetRefCount()));
#endif
	}
