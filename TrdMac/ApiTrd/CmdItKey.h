/////////////////////////////////////////////////////////////////////
// Clase para manipular los comodines de la llave
/////////////////////////////////////////////////////////////////////
#if !defined(_CCOMODINTEMKEY_)
#define _CCOMODINTEMKEY_

class CComodinItemKey: public CStepInSentence
	{
private:
	POSITION		m_ptrActualNode;
	CStringA		m_ptrPhrase;		// Cadena con la Frase
	POSITION		m_InitialPos;		// Posición donde comienza la frase dentro de la oración
	POSITION		m_FinalPos;			// Posición donde termina la frase dentro de la oración
	int         m_CentralPos;		// Posicion en indice de la palabra central de la frase
	CCompiledRuleTable *m_ptrCompiledRuleTable;
#if defined(_DEBUG)
	CStringA			m_StrKeyWord;
#endif

public:
#if defined(_DEBUG)
	CComodinItemKey(LPCORACION ptrSentence  = NULL,
		            POSITION ptrNode = NULL,
		            CCompiledRuleTable *ptrCompiledRuleTable = NULL,
		            const CStringA& ptrStrKeyWord = CANull):CStepInSentence(NULL,ptrSentence)

#else
	CComodinItemKey(LPCORACION ptrSentence  = NULL,
		            POSITION ptrNode = NULL,
		            CCompiledRuleTable *ptrCompiledRuleTable = NULL):CStepInSentence(NULL,ptrSentence)

#endif
		{
		SetInitialPosition(NULL);
		SetFinalPosition(NULL);

		ASSERT(ptrNode != NULL);
		SetActualPosition(ptrNode);
	  ASSERT(GetWordAt(GetActualPosition())!=NULL);
		ASSERT(ptrCompiledRuleTable != NULL);
		m_ptrCompiledRuleTable = ptrCompiledRuleTable;
#if defined(_DEBUG)
		//if(ptrStrKeyWord != NULL)
		//	{
			m_StrKeyWord = ptrStrKeyWord;
		//	}
#endif
		}

	bool StrFindBetweenMark3(LPCSTR strMain, LPCSTR aStr1, LPCSTR aStr2, LPCSTR aStr3, char cMark);

	bool StrFindBetweenMark(const CStringA& strMain, const CStringA& strFind,char cMark)
		{
		//char s1[4500];
		//char s2[255];

	//	if(!*strFind || !*strMain)
		if(strFind.IsEmpty() || strMain.IsEmpty())
			return false;
		//int lenStrMain = strlen(strMain);
		//int lenStrFind = strlen(strFind);
		CStringA s1 = cMark + strMain + cMark; 
		/*s1[0] = cMark;
		strcpy(s1+1,strMain);
		s1[1+lenStrMain] = cMark;
		s1[2+lenStrMain] = '\0';*/

		CStringA s2 = cMark + strFind + cMark; 
		/*s2[0] = cMark;
		strcpy(s2+1,strFind);
		s2[1+lenStrFind] = cMark;
		s2[2+lenStrFind] = '\0';*/

		//if(strstr(s1,s2) != NULL)
		if(s1.Find(s2) != -1)
			return true;
		return false;
		}

	bool TypeComparation(LPCSTR tipos,	LPCSTR tipo)
		{return ((strstr(tipos,tipo)!= NULL)?true:false);}

  bool SemanticComparation(const CStringA& sWordField, LPCSTR sSemanticPatter);

	bool SuffixComparation(const CStringA& sWordField, LPCSTR sSuffixes);

	bool PreffixComparation(const CStringA& sWordField, LPCSTR sPreffixes);

	bool MatchItemFunction(CItemKeyWord* itemKeyWord, LPCWORD ptrWord);
	

	POSITION GetActualPosition()
		{return m_ptrActualNode;}

	void SetActualPosition(POSITION ptrNode)
		{
		ASSERT(ptrNode != NULL);
		m_ptrActualNode = ptrNode;
		}

	void SetPtrPhrase(const CStringA& ptrPhrase)
		{
		//ASSERT(ptrPhrase != NULL);
		m_ptrPhrase = ptrPhrase;
		}

	POSITION FindInitialPosition(int aMainWordPos)
		{
		POSITION posStart = GetActualPosition();

		for(int i=aMainWordPos; posStart != NULL && i > 0; GetPrevPos(posStart), i--);

		return posStart;
		}

	POSITION FindFinalPosition(int aMainWordPos, int aMatchLen)
		{
		POSITION posEnd = GetActualPosition();

		for(int i=aMainWordPos; posEnd != NULL && i < aMatchLen-1; GetNextPos(posEnd), i++);

		return posEnd;
		}

	public:

	const CStringA& GetPtrPhrase()
		{return m_ptrPhrase;}

	bool Match(CMatchItem *ptrMatchItem, POSITION nodeStart, POSITION nodeEnd);

	/*virtual*/ POSITION &GetInitialPosition()
		{return m_InitialPos;}

	/*virtual*/ POSITION &GetFinalPosition()
		{return m_FinalPos;}

protected:
	/*virtual*/ void SetInitialPosition(POSITION ptrNode)
		{m_InitialPos = ptrNode;}

	/*virtual*/ void SetFinalPosition(POSITION ptrNode)
		{m_FinalPos = ptrNode;}

	CCompiledRuleTable *GetRuleTable()
		{return m_ptrCompiledRuleTable;}
	};
#endif
/////////////////////////////////////////////////////////////////////
