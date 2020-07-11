//////////////////////////////////////////////////////////////////////
// ExeWordAndTypeCmd.h: interface for the CExeWordAndTypeCmd class.
// Ejecuta los comodines contenidos en las palabras de la oración
// o en los tipos gramaticales de las palabras de la oración
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXEWORDANDTYPECMD_H__05DFAA46_FBF1_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_EXEWORDANDTYPECMD_H__05DFAA46_FBF1_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"
#include "Cmd.h"

class CExeWordAndTypeCmd : public CStepInSentenceWithXX  
{
private:
    bool m_bMatch;
	bool m_bAddWordToArray;  // pregunta si se adicionan las palabras al arreglo de busqueda en el dcc
	bool m_bAddTypeToArray;  // pregunta si se adicionan los tipos entre $ al arreglo de busqueda en el dcc
	POSITION     m_Pos;	     // posición de la palabra que contiene el comodin dentro de la oración
	WORD m_OraLenght;
	WORD				m_PosWordInSentence;    // indice de la palabra en la oracion
public:
	CExeWordAndTypeCmd(	CTranslationUser* ptrTranslationUser=NULL,
						LPCORACION ptrSentence=NULL,
						bool bAddWordToArray = true,
						bool bAddTypeToArray = true):CStepInSentenceWithXX(ptrTranslationUser,ptrSentence)
		{
		m_ptrKeyWord = NULL;
		SetMatch(false);
		SetAddWordToArray(bAddWordToArray);
		SetAddTypeToArray(bAddTypeToArray);
		m_OraLenght = GetPtrSentence()->GetLength();
		}

  bool IsMatch() { return m_bMatch;}

	virtual void Process()
		{
		ASSERT(GetPtrSentence()->m_Ora.IsEmpty() == FALSE);
		ResetWordArray(m_ptrInitialNode);
    
		for(POSITION ptrNode = m_ptrInitialNode;
			  ptrNode != m_ptrFinalNode && ptrNode != NULL;
			  GetNextPos(ptrNode))
			{
			LPCWORD ptrWord = GetWordAt(ptrNode);   // Palabra que se analiza 

			if (ptrWord != NULL)
				{
				ptrNode = Execute(ptrNode, ptrWord);
				}

			m_posWordInSentence++;
			}
		}

protected:

	CKeyWordEntry *m_ptrKeyWord;

    void SetMatch(bool bMatch)
        {m_bMatch = bMatch;}
    void SetAddWordToArray(bool bAddWordToArray)
        {m_bAddWordToArray = bAddWordToArray;}
    void SetAddTypeToArray(bool bAddTypeToArray)
        {m_bAddTypeToArray = bAddTypeToArray;}
    bool GetAddWordToArray()
        {return m_bAddWordToArray;}
    bool GetAddTypeToArray()
        {return m_bAddTypeToArray;}

	///////////////////////////////////
	//  llena un CStrArray con las posibles variantes de la llave a buscar en el diccionario de comodines
	///////////////////////////////////
	virtual void FillWordsToFindArray(CStrArray &aWordsToFind, LPCWORD ptrWord)
		{
		aWordsToFind.RemoveAll();

		if(GetAddWordToArray() == true)
			{
			if(ptrWord->m_Orig.IsEmpty() == FALSE)
				aWordsToFind.Add(ptrWord->m_Orig);
			if(ptrWord->m_Origlwr != ptrWord->m_Orig && ptrWord->m_Origlwr.IsEmpty() == FALSE)
				aWordsToFind.Add(ptrWord->m_Origlwr);
			if(ptrWord->m_Key != ptrWord->m_Origlwr && ptrWord->m_Key != ptrWord->m_Orig && ptrWord->m_Key.IsEmpty() == FALSE)
				aWordsToFind.Add(ptrWord->m_Key);
			}

		if(GetAddTypeToArray() && !ptrWord->m_sTipo.IsEmpty())
			{
			//, sSigno("$")
			CStringA sKey(ptrWord->m_sTipo);
			sKey.Enclose('$');
			//sKey = sSigno + sKey + sSigno;
			aWordsToFind.Add(sKey);
			}
//		sKey.MakeLower();
//		aFindInDcc.Add(sKey);

		}

	/*int GetWordPos(POSITION ptrPos)
		{
		ASSERT(GetPtrSentence()->m_Ora.IsEmpty() == FALSE);

		int pos = 0;
		for(POSITION ptrNode = m_ptrInitialNode;
		    ptrNode != m_ptrFinalNode && ptrNode != NULL;
				GetNextPos(ptrNode),pos++)
			 {
			 if (ptrNode == ptrPos)
				return pos;
			}

		return -1;
		}*/

	static POSITION m_arrWords[300];
	int m_SentenceLen;
	int m_posWordInSentence;

	void ResetWordArray(POSITION ptrNode)
		{
		m_posWordInSentence = -1;
		m_SentenceLen = 0;

		FOREACH_WORD(GetPtrSentence(), ptrWord->m_Tipo.fromString(ptrWord->m_sTipo.c_str()); \
			/*arrWords.Add(pNode);*/ \
			m_arrWords[m_SentenceLen] = pNode; \
			if (pNode == ptrNode) m_posWordInSentence = m_SentenceLen;\
			m_SentenceLen++; \
			ptrWord->m_NoEnComodin = FALSE);
		}


	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		// Este metodo solo se ejecuta con la oracion completa, es decir:
		// m_ptrInitialNode = GetHeadPos() y  m_ptrFinalNode = NULL
		GetPtrSentence()->View(150);

		COLLECT_INTERVAL("ExeWordAndTypeCmd");
		CStringA sKey;

		//SetMatch(false);

		//if(IsMatch())
		//	TRACE("");
		m_Pos = ptrNode;
		CStrArray aFindInDcc;

		FillWordsToFindArray(aFindInDcc, ptrWord);

		//static POSITION arrWords[300];
		//CArrayValue<POSITION> arrWords;
		//arrWords.PreAllocate(48);

		/*m_posWordInSentence = -1;//GetWordPos(ptrNode);
		m_SentenceLen = 0;//GetPtrSentence()->GetLength();

		FOREACH_WORD(GetPtrSentence(), ptrWord->m_Tipo.fromString(ptrWord->m_sTipo.c_str()); \
			m_arrWords[m_SentenceLen] = pNode; \
			if (pNode == ptrNode) m_posWordInSentence = m_SentenceLen;\
			m_SentenceLen++; \
			ptrWord->m_NoEnComodin = FALSE);
		//COLLECT("SentenceLen", SentenceLen);    //274 maximo registrado
		*/

		CComodin cmd(GetTranslationUserPtr(),GetPtrSentence(),m_Pos,GetRuleTable());

		for(int i = 0; i < aFindInDcc.GetSize(); i++)
			{
			DeleteRule();

			sKey = aFindInDcc[i];
			//TRACE("buscando %s", sKey.c_str());
			bool bFind = FindRule(sKey);

			if( bFind == true && m_ptrKeyWord != NULL)
				{
				//TRACE("testing : %s", sKey.c_str());

				/* // solo produce una mejora muy pequeña
				if (m_ptrKeyWord->GetTestMainWordPos() >= 0)
					{
					bool bTypesOk = true;
					CArray<CGramTypeSet>* TestTypeSet = &m_ptrKeyWord->m_MatchTypeTest;
					int startPos = posWordInSentence - m_ptrKeyWord->GetTestMainWordPos();
					if (startPos < 0)
						bTypesOk = false;
					else
						{
						int endPos = startPos + TestTypeSet->GetCount() - 1;
						if (endPos >= SentenceLen)
							bTypesOk = false;
						else
							{
							//COUNTER("badEnding");
							//COUNTER("badStart");
							for(int idxType = 0; idxType<TestTypeSet->GetCount(); idxType++)
								{
								CGramTypeSet* pTypeSet = TestTypeSet->GetAtPtr(idxType);
								if (!pTypeSet->IsFull())
									{
									LPCWORD ptrWord = GetWordAt(arrWords[startPos + idxType]);
									bTypesOk = pTypeSet->ContainType(ptrWord->m_Tipo);
									if (bTypesOk == false)
										{
										//TRACE("%s = %s", ptrWord->m_Tipo.toString(), pTypeSet->toString().c_str());
										break;
										}
									}
								} // for
							} // if (endPos >= SentenceLen)
						} // if (startPos < 0)

					if (bTypesOk == false)
						{
						//COUNTER("match excluded");
						if (IsMatch())
							break;
						else
							continue;
						}
					} // if (m_ptrKeyWord->GetTestMainWordPos() >= 0)*/

				for(int j = 0; j < m_ptrKeyWord->m_MatchArray.GetSize(); j++)
					{
					//TRACE(" match# %d", j);
					CMatchItem *ptrMatchItem = (CMatchItem *)m_ptrKeyWord->m_MatchArray.GetAtPtr(j);
					
					// chequear que haya espacio en la oracion para ejecutar el match, segun la
					// palabra central

					// chequear posicion inicial
					int startPos = m_posWordInSentence - ptrMatchItem->GetMainWordPos();
					if (startPos < 0)
						continue;

					// chequear posicion final
					int endPos = startPos + ptrMatchItem->GetKeyWordCount() - 1;
					if (endPos >= m_SentenceLen)
						continue;

					if(cmd.Math(ptrMatchItem, m_arrWords[startPos], m_arrWords[endPos]) == true)
						{
						//TRACE("%s matched", sKey.c_str());
						//TRACE("%s matched, #%d", sKey.c_str(), j);
						//COLLECT_INTERVAL("Matched"); // 10%
#if defined(_DEBUG)						
						CStringA fn = GetFileName();
						cmd.SetFileName(fn);
#endif						
						cmd.ProccessDataFunction();
						GetPtrSentence()->View(200);
						if(cmd.GetError() == true)
							goto G_ERROR;

						/*bool entre =*/ cmd.ProccessDataTranslation();
						GetPtrSentence()->View(201);
						if(cmd.GetError() == true)
							goto G_ERROR;

						m_Pos = cmd.GetFinalPos();
						SetMatch(true);

						ResetWordArray(m_Pos);
						break;
						}
					}

				if(IsMatch() == true)
					break;
 				}
			}
		
		DeleteRule();
		
		return m_Pos;
		
G_ERROR:
		DeleteRule();
		
		return NULL;
		}

	inline void DeleteRule()
		{
		m_ptrKeyWord = NULL;
		/*if (m_ptrKeyWord)
			{
			//delete m_ptrKeyWord;
			m_ptrKeyWord = NULL;
			}*/
		}
	

	virtual bool FindRule(const CStringA& ptrKey)
		{
		m_ptrKeyWord = GetPtrSourceLanguageData()->GetCmdFindGrFunctionDic()->Find(ptrKey);
		return (m_ptrKeyWord != NULL)?true:false;
		}
	virtual CCompiledRuleTable *GetRuleTable()
		{
		return GetPtrSourceLanguageData()->GetCmdFindGrFunctionDic()->GetRuleTable();
		}
	virtual const CStringA& GetFileName()
		{
		return GetPtrSourceLanguageData()->GetCmdFindGrFunctionDic()->GetFileName();
		}

};

class CExeWordAndTypeQCmd : public CExeWordAndTypeCmd
{
public:
	CExeWordAndTypeQCmd(CTranslationUser* ptrTranslationUser=NULL,
						LPCORACION ptrSentence=NULL,
						bool bAddWordToArray = true,
						bool bAddTypeToArray = true):CExeWordAndTypeCmd(ptrTranslationUser,ptrSentence,bAddWordToArray,bAddTypeToArray)
		{}

protected:

	virtual bool FindRule(const CStringA& ptrKey)
		{
		m_ptrKeyWord = GetPtrSourceLanguageData()->GetCmdFindQGrFunctionDic()->Find(ptrKey);
		return m_ptrKeyWord != NULL;
		}
	virtual CCompiledRuleTable *GetRuleTable()
		{
		return GetPtrSourceLanguageData()->GetCmdFindQGrFunctionDic()->GetRuleTable();
		}
	virtual const CStringA& GetFileName()
		{
		return GetPtrSourceLanguageData()->GetCmdFindQGrFunctionDic()->GetFileName();
		}
};

class CCoordinateAndConjugateSentence : public CExeWordAndTypeCmd
{
public:
	CCoordinateAndConjugateSentence(CTranslationUser* ptrTranslationUser=NULL,
									LPCORACION ptrSentence=NULL)
									:CExeWordAndTypeCmd(ptrTranslationUser,ptrSentence)
		{}

protected:

	virtual bool FindRule(const CStringA& ptrKey)
		{
		m_ptrKeyWord = GetPtrTranslationData()->GetCmdCoorConjDic()->Find(ptrKey);
		return (m_ptrKeyWord != NULL)?true:false;
		}
	virtual CCompiledRuleTable *GetRuleTable()
		{
		return GetPtrTranslationData()->GetCmdCoorConjDic()->GetRuleTable();
		}
	virtual const CStringA& GetFileName()
		{
		return GetPtrTranslationData()->GetCmdCoorConjDic()->GetFileName();
		}
};

class CTranslationWildCard : public CExeWordAndTypeCmd
{
public:
	CTranslationWildCard(CTranslationUser* ptrTranslationUser=NULL,
									LPCORACION ptrSentence=NULL,
						bool bAddWordToArray = true,
						bool bAddTypeToArray = true):CExeWordAndTypeCmd(ptrTranslationUser,ptrSentence,bAddWordToArray,bAddTypeToArray)
		{}

protected:
	virtual bool FindRule(const CStringA& ptrKey)
		{
		m_ptrKeyWord = GetPtrTranslationData()->GetCmdFindTranslationDic()->Find(ptrKey);
		return (m_ptrKeyWord != NULL)?true:false;
		}
	virtual CCompiledRuleTable *GetRuleTable()
		{
		return GetPtrTranslationData()->GetCmdFindTranslationDic()->GetRuleTable();
		}
	virtual const CStringA& GetFileName()
		{
		return GetPtrTranslationData()->GetCmdFindTranslationDic()->GetFileName();
		}
};

class CTranslationQWildCard : public CExeWordAndTypeCmd
{
public:
	CTranslationQWildCard(CTranslationUser* ptrTranslationUser=NULL,
						LPCORACION ptrSentence=NULL,
						bool bAddWordToArray = true,
						bool bAddTypeToArray = true):CExeWordAndTypeCmd(ptrTranslationUser,ptrSentence,bAddWordToArray,bAddTypeToArray)
		{}

protected:
	virtual bool FindRule(const CStringA& ptrKey)
		{
		m_ptrKeyWord = GetPtrTranslationData()->GetCmdFindQTranslationDic()->Find(ptrKey);
		return (m_ptrKeyWord != NULL)?true:false;
		}
	virtual CCompiledRuleTable *GetRuleTable()
		{
		return GetPtrTranslationData()->GetCmdFindQTranslationDic()->GetRuleTable();
		}
	virtual const CStringA& GetFileName()
		{
		return GetPtrTranslationData()->GetCmdFindQTranslationDic()->GetFileName();
		}
};

class CTranslationVWildCard : public CExeWordAndTypeCmd
{
public:
	CTranslationVWildCard(CTranslationUser* ptrTranslationUser=NULL,
						LPCORACION ptrSentence=NULL,
						bool bAddWordToArray = true,
						bool bAddTypeToArray = true):CExeWordAndTypeCmd(ptrTranslationUser,ptrSentence,bAddWordToArray,bAddTypeToArray)
		{}

protected:
	virtual bool FindRule(const CStringA& ptrKey)
		{
		m_ptrKeyWord = GetPtrTranslationData()->GetCmdFindVTranslationDic()->Find(ptrKey);
		return (m_ptrKeyWord != NULL)?true:false;
		}
	virtual CCompiledRuleTable *GetRuleTable()
		{
		return GetPtrTranslationData()->GetCmdFindVTranslationDic()->GetRuleTable();
		}
	virtual const CStringA& GetFileName()
		{
		return GetPtrTranslationData()->GetCmdFindVTranslationDic()->GetFileName();
		}
};

class CTranslationIWildCard : public CExeWordAndTypeCmd
{
public:
	CTranslationIWildCard(CTranslationUser* ptrTranslationUser=NULL,
						LPCORACION ptrSentence=NULL,
						bool bAddWordToArray = true,
						bool bAddTypeToArray = true):CExeWordAndTypeCmd(ptrTranslationUser,ptrSentence,bAddWordToArray,bAddTypeToArray)
		{}

protected:
	virtual bool FindRule(const CStringA& ptrKey)
		{
		m_ptrKeyWord = GetPtrTranslationData()->GetCmdFindITranslationDic()->Find(ptrKey);
		return (m_ptrKeyWord != NULL)?true:false;
		}
	virtual CCompiledRuleTable *GetRuleTable()
		{
		return GetPtrTranslationData()->GetCmdFindITranslationDic()->GetRuleTable();
		}
	virtual const CStringA& GetFileName()
		{
		return GetPtrTranslationData()->GetCmdFindITranslationDic()->GetFileName();
		}
};

class CTranslationFWildCard : public CExeWordAndTypeCmd
{
public:
	CTranslationFWildCard(CTranslationUser* ptrTranslationUser=NULL,
						LPCORACION ptrSentence=NULL,
						bool bAddWordToArray = true,
						bool bAddTypeToArray = true):CExeWordAndTypeCmd(ptrTranslationUser,ptrSentence,bAddWordToArray,bAddTypeToArray)
		{}

protected:
	virtual bool FindRule(const CStringA& ptrKey)
		{
		m_ptrKeyWord = GetPtrTranslationData()->GetCmdFindFTranslationDic()->Find(ptrKey);
		return (m_ptrKeyWord != NULL)?true:false;
		}
	virtual CCompiledRuleTable *GetRuleTable()
		{
		return GetPtrTranslationData()->GetCmdFindFTranslationDic()->GetRuleTable();
		}
	virtual const CStringA& GetFileName()
		{
		return GetPtrTranslationData()->GetCmdFindFTranslationDic()->GetFileName();
		}
};

class CProperNounWildCard : public CExeWordAndTypeCmd
{
public:
	CProperNounWildCard(CTranslationUser* ptrTranslationUser=NULL,
									LPCORACION ptrSentence=NULL,
						bool bAddWordToArray = true,
						bool bAddTypeToArray = true):CExeWordAndTypeCmd(ptrTranslationUser,ptrSentence,bAddWordToArray,bAddTypeToArray)
		{}

protected:
	virtual bool FindRule(const CStringA& ptrKey)
		{
		m_ptrKeyWord = GetPtrSourceLanguageData()->GetCmdFindProperNounDic()->Find(ptrKey);
		return (m_ptrKeyWord != NULL)?true:false;
		}
	virtual CCompiledRuleTable *GetRuleTable()
		{
		return GetPtrSourceLanguageData()->GetCmdFindProperNounDic()->GetRuleTable();
		}
	virtual const CStringA& GetFileName()
		{
		return GetPtrSourceLanguageData()->GetCmdFindProperNounDic()->GetFileName();
		}
};
#endif // !defined(AFX_EXEWORDANDTYPECMD_H__05DFAA46_FBF1_11D2_8926_0060972DBBB5__INCLUDED_)
