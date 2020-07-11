// ConjugateByPattern.cpp: implementation of the CConjugateByPattern class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExeWordAndTypeCmd.h"
#include "ConjugateByPattern.h"
#include "Cmd.h"
#include "FindCmdPhrasesInSentence.h"

/////////////////////////////////////////////////////////////////////////////
// Coordinación y Conjugación de verbos mediante comodines
/////////////////////////////////////////////////////////////////////////////
void CConjugateByPattern::Process()
	{
//	GetPtrSentence()->PoneXX();

	for(int i=0; i < MAX_CNJ_CICLE; i++)
		{
		CStringA sKey,sData;
		//POSITION ptrIni = NULL, ptrFin = NULL;
			{	// para que elimine los xx
			CCoordinateAndConjugateSentence objCoordinateAndConjugateSentence(GetTranslationUserPtr(),GetPtrSentence());
			objCoordinateAndConjugateSentence.Process();
			}
			{	// para que elimine los xx
			CFindCmdPhrasesInSentence objFindCmdPhrasesInSentence(GetTranslationUserPtr(),GetPtrSentence(),i);
			objFindCmdPhrasesInSentence.Process();
			}
		/************
		for (int j = 0; j < 100; j++)
			{
			bool bFind = false;

			//Translate->m_Idioma->FindCoorConjFrase(i, this,sKey,sData,ptrIni,ptrFin);

			if( bFind == true)
				{
				if(sData.IsEmpty() == false)
					{
					CComodinPhrase objCmdCoorCnj(GetPtrTranslationData(),GetPtrSentence(),ptrIni,ptrFin, sKey,sData);

					objCmdCoorCnj.ProccessDataFunction();
					}
				else
					break;
				}
			else
				break;
			}
		********/
		}
	GetPtrSentence()->ExpandeFrases();
//	GetPtrSentence()->QuitaXX();
	}
/////////////////////////////////////////////////////////////////////////////
