// StepInSentence.h: interface for the CStepInSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEPINSENTENCE_H__05DFAA41_FBF1_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_STEPINSENTENCE_H__05DFAA41_FBF1_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WORK_WITH_COMPILE_CMD 1

#include "PeekMsg.h"
#include "TranslationDataObject.h"
#include "SentenceDataObject.h"
#include "CompiledCmdArray.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


class CStepInSentence : public CTranslationDataObject , public CSentenceDataObject, public CMFCObject
{
private:
	DWORD				m_SentenceNumber;		// Numero de la oracion dentro del arreglo de oraciones del texto
protected:
	POSITION		m_ptrInitialNode;		// puntero a la posición inicial de trabajo en la oración
	POSITION		m_ptrFinalNode;			// puntero a la posición final de trabajo en la oración
	//CMainDict	  *m_pMainDict;			  // diccionario principal
	CDict       *m_pDict;			      // diccionario de trabajo
public:

	CStepInSentence(CTranslationUser* ptrTranslationUser = NULL,	// puntero a los datos para realizar la traducción
					LPCORACION ptrSentence = NULL,					// puntero a la oracion de trabajo
					POSITION ptrInitialNode = NULL,					// puntero a la posición inicial a analizar de la oración
					POSITION ptrFinalNode = NULL)					// puntero a la posición final a analizar de la oración
					: CTranslationDataObject(ptrTranslationUser),CSentenceDataObject(ptrSentence)
		{
		//m_pMainDict = NULL;
		m_pDict = NULL;
		if(ptrInitialNode != NULL)
			SetInitialNode(ptrInitialNode);
		else
			SetInitialNode(GetHeadPos());

		SetFinalNode(ptrFinalNode);
		m_SentenceNumber = 0;
		}

	~CStepInSentence()
		{}

	virtual void Process()
		{
		ASSERT(GetPtrSentence()->m_Ora.IsEmpty() == FALSE);
    
		for(POSITION ptrNode = m_ptrInitialNode;
			  ptrNode != m_ptrFinalNode && ptrNode != NULL;
			  GetNextPos(ptrNode))
			{
			ASSERT(ptrNode != NULL);

			LPCWORD ptrWord = GetWordAt(ptrNode);   // Palabra que se analiza 
			ASSERT(ptrWord != NULL);

			if (ptrWord != NULL)
				{
				ptrNode = Execute(ptrNode,ptrWord);
				}

//			PEEKMESSAGE();
			}
		}

	virtual void ProcessOneWord()
		{}

	inline void SetSentenceNumber(DWORD pos)
		{m_SentenceNumber = pos;}
	inline DWORD GetSentenceNumber()
		{return m_SentenceNumber;}

protected:
	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		ASSERT(ptrNode != NULL);
		ASSERT(ptrWord != NULL);
		return ptrNode;
		}

	inline void SetInitialNode(POSITION ptrNode)
		{m_ptrInitialNode = ptrNode;}
	inline void SetFinalNode(POSITION ptrNode)
		{m_ptrFinalNode = ptrNode;}
	inline POSITION GetInitialNode()
		{return m_ptrInitialNode;}
	inline POSITION GetFinalNode()
		{return m_ptrFinalNode;}

	/*virtual CMainDict *GetMainDict()
		{return m_pMainDic;}
	inline void SetMainDict(CMainDict *ptrDic)
		{m_pMainDic = (ptrDic != NULL)?ptrDic:GetTranslationUserPtr()->GetPtrTranslationDic();}*/

	virtual CDict *GetDictionary()
		{return m_pDict;}
	inline void SetDictionary(CDictMain *ptrDic)
		{m_pDict = (ptrDic != NULL) ? ptrDic : GetTranslationUserPtr()->GetPtrTranslationDic();}

	void UpdateFrecuency(const CStringA& sKey)
		{
		if(!sKey.IsEmpty())
			{
			if(GetPtrSentence()->m_Dest.IsEmpty() == FALSE)
				GetPtrSentence()->m_Dest += ";";

			GetPtrSentence()->m_Dest += sKey;
			}
		}
	/////////////////////////////////////////////////////////////////////////////
	//  Coloca dentro de la palabra que será el comodín un array con las
	//  palabras de la oración que forman dicho comodín y elimina estas palabras
	//  de la oración
	/////////////////////////////////////////////////////////////////////////////
	POSITION MakeArrayFrase(
			 LPCWORD ptrWordComodin,
			 POSITION ptrInitialPos, 
			 POSITION ptrFinalPos)
		{                                       /* MakeComodin */
		if(ptrWordComodin == NULL || ptrInitialPos == NULL || ptrFinalPos == NULL)
			return NULL;

		POSITION ptrRealFinalPos = ptrFinalPos;
		GetNextPos(ptrRealFinalPos);

		for (POSITION ptrNodo = ptrInitialPos; ptrNodo != ptrRealFinalPos;)
			{
			if(ptrNodo != NULL)
				{
				LPCWORD ptrTempWord = GetWordAt(ptrNodo);

				if(ptrTempWord != NULL)
					{
					POSITION ptrOldNodo = ptrNodo;     
					
					ptrWordComodin->m_ComodinArray.Add(ptrTempWord);
					GetNextPos(ptrNodo);

					if(ptrOldNodo != ptrInitialPos)
						GetWordList().RemoveAt(ptrOldNodo); // cambia de posicion antes de borrar el nodo
					}
				}
			else
				return NULL;
			}

		GetWordList().SetAt(ptrInitialPos,ptrWordComodin);
		//m_Size = ptrWordComodin->m_ComodinArray.GetSize();

		return ptrInitialPos;
		}                                       /* MakeComodin */

};

//////////////////////////////////////////////////////////////////////////////
// La misma clase anterior pero con las XX al inicio y final incorporadas
//////////////////////////////////////////////////////////////////////////////

class CStepInSentenceWithXX : public CStepInSentence
{

public:

	CStepInSentenceWithXX(	CTranslationUser* ptrTranslationUser=NULL,	// puntero alos datos para realizar la traducción
							LPCORACION ptrSentence=NULL,
							POSITION ptrInitialNode=NULL,
							POSITION ptrFinalNode=NULL):CStepInSentence(ptrTranslationUser,ptrSentence,ptrInitialNode, ptrFinalNode)
		{
		if(GetPtrSentence() != NULL)
			GetPtrSentence()->PoneXX();
		if(ptrInitialNode != NULL)
			SetInitialNode(ptrInitialNode);
		else
			SetInitialNode(GetHeadPos());
		}
	virtual ~CStepInSentenceWithXX()
		{
		if(GetPtrSentence() != NULL)
			GetPtrSentence()->QuitaXX();
		}

	///////////////////////////////////
	//  llena un CStrArray con las posibles variantes de la palabra a buscar en el diccionario 
	///////////////////////////////////
	virtual void FillWordsToFindArray(CStrArray &aWordsToFind, LPCWORD ptrWord)
		{
		aWordsToFind.RemoveAll();

    aWordsToFind.Add(ptrWord->m_Orig);

		if(ptrWord->m_Origlwr != ptrWord->m_Orig)
			aWordsToFind.Add(ptrWord->m_Origlwr);

		if(!ptrWord->m_Key.IsEmpty() && ptrWord->m_Key != ptrWord->m_Origlwr && ptrWord->m_Key != ptrWord->m_Orig)
			aWordsToFind.Add(ptrWord->m_Key);
		}

};
#endif // !defined(AFX_STEPINSENTENCE_H__05DFAA41_FBF1_11D2_8926_0060972DBBB5__INCLUDED_)
