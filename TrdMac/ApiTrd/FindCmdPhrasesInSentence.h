// FindCmdPhrasesInSentence.h: interface for the CFindCmdPhrasesInSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDCMDPHRASESINSENTENCE_H__F23C1DE1_0D05_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_FINDCMDPHRASESINSENTENCE_H__F23C1DE1_0D05_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FindPhrasesInSentence.h"

class CFindCmdPhrasesInSentence : public CFindPhrasesInSentence  
{
private:
	int m_iIndex;
	CStringA m_sIndex;

public:
	CFindCmdPhrasesInSentence(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL,int iIndex=0):CFindPhrasesInSentence(ptrTranslationUser,ptrSentence)
		{
		SetIndex(iIndex);
		}

protected:
	virtual int GetStringArraySize()
		{return 1;}

	virtual CCompiledRuleTable *GetRuleTable()
		{
		return GetPtrTranslationData()->GetCmdCoorConj()->GetRuleTable();
		}

	CKeyWordEntry* Find(const CStringA& ptrStr)
		{
		//return NULL;
		return GetPtrTranslationData()->GetCmdCoorConj()->GetRuleTable()->FindKeyWordEntryInDcc(ptrStr);
		}

	/*virtual*/ void FillWordsToFindArray(CStrArray &aWordsToFind, LPCWORD ptrWord)
		{
		CStringA tmp(ptrWord->m_sTipo);
		tmp.MakeLower();
    aWordsToFind.Add(tmp);
		}

	/*virtual*/ void SetIndex(int iIndex)
		{
		m_iIndex = iIndex;
	  m_sIndex.Format("p%i ", iIndex);
		}

	/*virtual*/ int GetIndex()
		{return m_iIndex;}

	/*virtual*/ CStringA FormatPhrase(const CStringA& ptrPhrase)
		{
		CStringA sFormatedPhrase;
	  //sFormatedPhrase.Format("p%i ",GetIndex());
    if(m_sIndex.Compare(0, 2, ptrPhrase) != 0)
			sFormatedPhrase = m_sIndex;

		sFormatedPhrase += ptrPhrase;
		sFormatedPhrase.MakeLower();
		return sFormatedPhrase;
		}

	/*virtual*/ CStringA CreatePhrase(const CStrArray &aPhrase, int size)
		{
		CStringA sFormatedPhrase;
		
		for(int i = 0; i < size; i++)
			{
			if(!sFormatedPhrase.IsEmpty())
				{
				sFormatedPhrase.AppendChar(' ');
				}
			sFormatedPhrase += aPhrase[i];
			}

		return sFormatedPhrase;
		}
		
/////////////////////////////////////////////////////////////////////////////

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		//COLLECT_INTERVAL("CFindCmdPh__::Execute");
		//if(GetRuleTable()->m_Table.GetSize() == 0)
		//	return ptrNode;

    CStringA sFraseAnterior;			// Última frase buscada en el diccionario
    CStringA sLastPhrase;			// Última frase formada para buscar en el diccionario
    CStringA sNewPhrase;				// Nueva frase formada para buscarla en el diccionario
    CStringA sData;					// Datos asociados a la frase buscada en el diccionario

	//bool bEntre = false;			// Si se encontró una frase en el diccionario y llegó a concretarse

    POSITION ptrIniNode = ptrNode;	// nodo inicio de la frase 
    POSITION ptrEndPhrase = NULL;	// nodo posible fin de frase 
       // nodo posible
    //bool bEncontre = false;

    CStrArray aPhrase;

		for (POSITION ptrPos = ptrIniNode; ptrPos != NULL; GetNextPos(ptrPos))
			{
			LPCWORD w = GetWordAt(ptrPos);
			FillWordsToFindArray(aPhrase,w);
			if(ptrPos != NULL)
				ptrEndPhrase = ptrPos;
      }

		for(int iPhraseSize = aPhrase.GetSize(); iPhraseSize > 0; iPhraseSize--)
			{
			CStringA sPhrase = CreatePhrase(aPhrase,iPhraseSize);
			if(!sPhrase.IsEmpty())
				{
				CStringA sFormatedPhrase = FormatPhrase(sPhrase);
				CKeyWordEntry* keyWord = Find(sFormatedPhrase);
				if(keyWord != NULL)
					{
					// encontre la frase
					//POSITION inifrase = ptrIniNode;
					POSITION finfrase = ptrEndPhrase;

					IfFoundPhrase2(sFormatedPhrase, keyWord, ptrIniNode, finfrase, iPhraseSize, ptrWord);
					ptrNode = ptrIniNode;
					break;
					}
				}
			GetPrevPos(ptrEndPhrase);
			}

	return ptrNode;
	}
	//////////////////////////////////////////////////////////////////////
	//	Si la frase es encontrada en el diccionario
	//	LPCWORD ptrTmpWord,	// Nodo temporal donde se almacena la frase
	//	POSITION ptrIniNode,// Nodo de inicio de la frase
	//	POSITION ptrPos,    // Nodo final de la frase
	//	int iPhraseSize,	// Tamaño de la frase
	//	LPCWORD ptrWord)	// Palabra de trabajo
	//////////////////////////////////////////////////////////////////////

	void IfFoundPhrase2(
			const CStringA& aKey,
			CKeyWordEntry* aKeyWord,
			POSITION ptrIniNode,// Nodo de inicio de la frase
			POSITION ptrPos,    // Nodo final de la frase
			int /*iPhraseSize*/,	// Tamaño de la frase
			LPCWORD /*ptrWord*/)	// Palabra de trabajo
		{
		// ya que la CCompiledRuleTable::m_Table siempre esta vacia este codigo no hace nada
		//return;

		if (aKeyWord != NULL)
			{
			CComodinPhrase objCmdCoorCnj(GetTranslationUserPtr(),GetPtrSentence(),ptrIniNode,ptrPos, aKey,GetRuleTable());
			CMatchItem *ptr = (CMatchItem *)aKeyWord->m_MatchArray.GetAtPtr(0);
			objCmdCoorCnj.ProccessDataFunction(ptr);
			}
		}
};

#endif // !defined(AFX_FINDCMDPHRASESINSENTENCE_H__F23C1DE1_0D05_11D3_8926_0060972DBBB5__INCLUDED_)
