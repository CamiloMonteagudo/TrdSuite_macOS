// ExpandCmdPhrases.h: interface for the CExpandCmdPhrases class.
// Expande las frases con comodines que existan en la oración
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPANDCMDPHRASES_H__AF8D0AC1_0848_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_EXPANDCMDPHRASES_H__AF8D0AC1_0848_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CExpandCmdPhrases : public CStepInSentence  
{
public:
	CExpandCmdPhrases(LPCORACION ptrSentence=NULL):CStepInSentence(NULL,ptrSentence)
		{}

protected:
	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		if(ptrWord->m_ComodinArray.GetSize() > 0)
			{
			for(int i = 0; i < ptrWord->m_ComodinArray.GetSize(); i++)
				{
				LPCWORD ptrWord2 = (LPCWORD)ptrWord->m_ComodinArray[i];

				if(i == 0)
					GetWordList().SetAt(ptrNode,ptrWord2);
				else
					ptrNode = GetWordList().InsertAfter(ptrNode,ptrWord2);
				}    
			ptrNode = GetWordList().GetHeadPosition(); 
			delete ptrWord;
			} 
		return ptrNode;
		}
};

#endif // !defined(AFX_EXPANDCMDPHRASES_H__AF8D0AC1_0848_11D3_8926_0060972DBBB5__INCLUDED_)
