// TranslationProcess.h: interface for the CTrdProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSLATIONPROCESS_H__31F3A961_03C1_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_TRANSLATIONPROCESS_H__31F3A961_03C1_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TranslationData.h"

#define PATH_TRAD   0x00000001
#define PATH_LANG   0x00000002

enum eTranslationType
	{
	TT_NoTranslated,
	TT_RuleBase,
	TT_MemoryToolOK,
	TT_MemoryComodin,
	TT_StatisticsBase,
	TT_StatisticsBaseAception,
	TT_ExampleBase,
	TT_BadSourceLang
	};
// Cuenta el numero de ocurrencias en una cadena de un caracter dado
int CharacterCount(const CStringA& str, char c);


////////////////////////////////////////////////////////////////////////
// Clase para el manejo de Usuarios de la CAT
// almacena todos los datos necesarios para el trabajo de un usuario
////////////////////////////////////////////////////////////////////////

class CTranslationUser : public CMFCObject
{
private:
	CTranslationData   *m_ptrTranslationData;			// puntero a los datos de la traducción
	LPCORACION			m_ptrSentence;					// oracion que se traduce

	CStringA				m_NotFoundInitialMark;			// Marca inicial para las palabras que no encontró en el dcc
//	CStringA				m_NoFoundBeginMark;				// Marca de inicio en palabras no encontradas
	CStringA				m_NotFoundFinalMark;			// Marca Final para las palabras que no encontró en el dcc
//	CStringA				m_NoFoundEndMark;				// Marca al final en palabras no encontradas
	CStringA				m_NotTranslationInitialMark;	// Marca inicial para las palabras a no traducir
	CStringA				m_NotTranslationFinalMark;		// Marca Final para las palabras a no traducir en el dcc
	CStringA				m_strNotFoundWordList;			// Palabras no encontradas en la oracion que se traduce
	CStringA				m_strWordList;					// Lista de todas las palabras de la oracion que se traduce
	CStringA				m_strFoundPhraseList;			// Lista de frases idiomaticas encontradas durante el proceso de traducción
	CStringA				m_strDocNotFoundWordList;		// Palabras no encontradas en el documento que se traduce
	CStringA				m_strDocWordList;				// Lista de todas las palabras del documento que se traduce
	CStringA				m_strDocFoundPhraseList;		// Lista de frases idiomaticas encontradas durante el proceso de traducción del documento
	eTranslationType	m_iTranslationType;				// 0- por algoritmo de reglas; 1 Memory tool 100%; 2- Memory tool con comodines; 3- por metodos estadisticos 4- Traducción mediante ejemplos
	int					m_WordCount;					// Cantidad de palabras en la oración
	int					m_PhraseCount;					// Cantidad de frases en la oración
	int					m_DocWordCount;					// Cantidad de palabras del documento
	int					m_DocPhraseCount;				// Cantidad de frases del documento
	int					m_DocByteCount;					// Cantidad de Bytes traducidos en el documento
	bool				m_InformalPronoun;				// Dice si se usa el tú o el usted
	DWORD				m_TotalNGram;					// Porciento de ngram en la oracion de los posibles
	DWORD				m_MatchNGram;					// Porciento de ngram en la oracion de los posibles
	CStringA				m_InitialTypes;					// Tipos despues de buscar palabras en el dcc
	CStringA				m_MiddleTypes;					// Tipos en el medio de la traduccion antes de lenguaje de dcc para resolver tipos
	CStringA				m_FinalTypes;					// Tipos al final de la traduccion
	CStringA				m_SentenceTrace;				// Traza de la oracion traducida de la forma (FO (FS;key;trd;G;N;P;C)(VT;....))
	CStringA				m_StatisticalSentences;			// Traza de la oracion traducida de la forma (FO (FS;key;trd;G;N;P;C)(VT;....))
public:
	CStringA             m_sBuffer;						// Buffer para el retorno de parametros de la DLL

public:
	CTranslationUser(CTranslationData *ptrTranslationData = NULL)
		{
		ASSERT(ptrTranslationData != NULL);

		SetTranslationDataPtr(ptrTranslationData);
		SetPtrSentence(NULL);

		SetTranslationType(TT_NoTranslated);
		m_NotTranslationInitialMark = "[";
		m_NotTranslationFinalMark = "]";
		Clear();
		}
	virtual ~CTranslationUser()
		{
		FreePtrSentence();
		}

	void SetInitialTypes(const CStringA& ptrString)
		{m_InitialTypes = ptrString;}
	void SetMiddleTypes(const CStringA& ptrString)
		{m_MiddleTypes = ptrString;}
	void SetFinalTypes(const CStringA& ptrString)
		{m_FinalTypes = ptrString;}
	CStringA &GetInitialTypes()
		{return m_InitialTypes;}
	CStringA &GetMiddleTypes()
		{return m_MiddleTypes;}
	CStringA &GetFinalTypes()
		{return m_FinalTypes;}
	void AddStatisticalSentences(const CStringA& ptrString)
		{
		if(m_StatisticalSentences.IsEmpty() == FALSE)
			{
			m_StatisticalSentences += "|";
			}
		m_StatisticalSentences += ptrString;
		}
	CStringA &GetStatisticalSentences()
		{return m_StatisticalSentences;}
	////////////////////////////////////////////////////////////////////////
	// Funciones para el manejo de las propiedades de la clase
	////////////////////////////////////////////////////////////////////////
	
	CDictMain *GetPtrTranslationDic()
		{
		return GetTranslationDataPtr()->GetPtrTranslationDic();
		}

	CTranslationData *GetTranslationDataPtr()
		{return m_ptrTranslationData;}

	void SetTranslationDataPtr(CTranslationData *ptrTranslationData)
		{
		ASSERT(ptrTranslationData != NULL);
		m_ptrTranslationData = ptrTranslationData;
		}

	// Borra todos los campos de retorno de las funciones de traducción
	void Clear()
		{
		m_strNotFoundWordList.Empty();
		m_strWordList.Empty();
		m_strFoundPhraseList.Empty();
		m_strDocNotFoundWordList.Empty();
		m_strDocWordList.Empty();
		m_strDocFoundPhraseList.Empty();
		SetTranslationType(TT_NoTranslated);
		m_WordCount = 0;
		m_PhraseCount = 0;
		m_DocWordCount = 0;
		m_DocPhraseCount = 0;
		m_DocByteCount = 0;
		m_InformalPronoun = false;
		m_TotalNGram = 0;
		m_MatchNGram = 0;
		m_InitialTypes.Empty();
		m_MiddleTypes.Empty();
		m_FinalTypes.Empty();
		m_StatisticalSentences.Empty();
		}

	// Borra todos los campos de retorno de las funciones de traducción
	void ClearSentence()
		{
		m_strNotFoundWordList.Empty();
		m_strWordList.Empty();
		m_strFoundPhraseList.Empty();
		SetTranslationType(TT_NoTranslated);
		m_WordCount = 0;
		m_PhraseCount = 0;
		m_TotalNGram  = 0;
		m_MatchNGram  = 0;
		m_InitialTypes.Empty();
		m_MiddleTypes.Empty();
		m_FinalTypes.Empty();
		m_StatisticalSentences.Empty();
		}
		
	bool GetInformalPronoun()
		{return m_InformalPronoun;}

	void SetInformalPronoun(bool InformalPronoun)
		{m_InformalPronoun = InformalPronoun;	}

	// Tipo de algoritmo que se utilizo en la traducción
	eTranslationType GetTranslationType()
		{return m_iTranslationType;}

	void SetTranslationType(eTranslationType iTranslationType)
		{m_iTranslationType = iTranslationType;	}

	// Cantidad de palabras traduccidas
	int GetWordCount()
		{return m_WordCount-m_PhraseCount;}

	// Cantidad de bytes traducidos
	int GetByteCount()
		{
//		int i = GetPtrSentence()->m_Orig.GetLength();
		return GetPtrSentence()->m_Orig.GetLength();
		}
	// Lista de frases encontradas en el diccionario separadas por ;
	const CStringA& GetFoundPhraseList()
		{return m_strFoundPhraseList;}

	void AddPhraseToList(const CStringA& str)
		{
#ifdef _DEBUG
		//ASSERT(str != NULL);
		m_PhraseCount++;
		if(m_strFoundPhraseList.IsEmpty() == false)
			{
			m_strFoundPhraseList += "|";
			}
		m_strFoundPhraseList += str;
#endif
		}

	// Lista de palabras que conforman la oración
	const CStringA& GetWordList()
		{
		if(m_strFoundPhraseList.IsEmpty() == false)
			{
			CStringA s("|");
			s += GetFoundPhraseList();
			int i = m_strWordList.Find(s);
			if(i != -1)
				m_strWordList = m_strWordList.Left(i);
			}
		return m_strWordList;
		}

	void AddWordToList(const CStringA& str)
		{
#ifdef _DEBUG
		//ASSERT(str != NULL);
		m_WordCount++;
		if(m_strWordList.IsEmpty() == false)
			{
			m_strWordList += "|";
			}
		m_strWordList += str;
#endif
		}

	/*const CStringA& GetDocWordList()
		{return m_strDocWordList;}
	const CStringA& GetDocNotFoundWordList()
		{return m_strDocNotFoundWordList;}
	int GetDocWordCount()
		{return CharacterCount(m_strDocWordList,'|')+1;}
	int GetDocByteCount()
		{return m_DocByteCount;}
	DWORD GetTotalNGram()
		{return m_TotalNGram;}
	DWORD GetMatchNGram()
		{return m_MatchNGram;}

	const CStringA& GetDocFoundPhraseList()
		{return m_strDocFoundPhraseList;}

	// Lista de palabras no encontradas en el diccionario
	const CStringA& GetNotFoundWordList()
		{return m_strNotFoundWordList;}*/

	void AddNotFoundWordToList(const CStringA& str)
		{
#ifdef _DEBUG
		if(m_strNotFoundWordList.IsEmpty() == false)
			{
			m_strNotFoundWordList += ";";
			}
		m_strNotFoundWordList += str;
#endif
		}

	virtual void SetPtrSentence(LPCORACION ptrSentence)
		{
    m_ptrSentence = ptrSentence;
    }

	void FreePtrSentence();

	LPCORACION GetPtrSentence()
		{return m_ptrSentence;}

	//////////////////////////////////////////////////////////////////////
	// Dice que oración traducir y crea el objeto con ella
	//////////////////////////////////////////////////////////////////////
	void SetSentence(const CStringA& ptrSource,int iFormat)
		{
		//ASSERT(ptrSource != NULL);
		FreePtrSentence();
		ClearSentence();
		SetPtrSentence((LPCORACION) new COracion(ptrSource,iFormat));
		ASSERT(GetPtrSentence() != NULL);
		}

	virtual void SetSentenceNumber(DWORD pos)
		{
		ASSERT(GetPtrSentence()!=NULL);
		GetPtrSentence()->SetSentenceNumber(pos);
		}
	virtual DWORD GetSentenceNumber()
		{
		ASSERT(GetPtrSentence()!=NULL);
		return GetPtrSentence()->GetSentenceNumber();
		}

	const CStringA& GetTranslatedSentence()
		{
		ASSERT(GetPtrSentence()!=NULL);
/***********
		int byte = GetByteCount();
		int word = GetWordCount();
		CStringA s;

		s = GetNotFoundWordList();
		s = GetFoundPhraseList();
		s = GetWordList();
***********/
		if(GetPtrSentence() != NULL)
			return GetPtrSentence()->m_Dest;
		return CANull;
		}
	/*const CStringA& GetNotFoundWordInSentence()
		{
		ASSERT(GetPtrSentence()!=NULL);
		if(GetPtrSentence() != NULL)
			return GetNotFoundWordList();
		return CANull;
		}
	const CStringA& GetNotFoundWordInDoc()
		{
		ASSERT(GetPtrSentence()!=NULL);
		if(GetPtrSentence() != NULL)
			return GetDocNotFoundWordList();
		return CANull;
		}
	int GetTranslatedBytes()
		{
		ASSERT(GetPtrSentence()!=NULL);
		if(GetPtrSentence() != NULL)
			return GetPtrSentence()->m_Dest.GetLength();
		return 0;
		}*/
	/*int GetTranslatedWords()
		{
		ASSERT(GetPtrSentence()!=NULL);
		//if(GetPtrSentence() != NULL)
		//	return GetPtrSentence()->m_Dest;
		return GetWordCount();
		}*/
	////////////////////////////////////////////////////////////////////////
	// Funciones para el trabajo con marcas a las palabras no encontradas
	////////////////////////////////////////////////////////////////////////

	void SetNotFoundInitialMark(const CStringA& sMark)
		{
		//ASSERT(sMark != NULL);
		m_NotFoundInitialMark = sMark;
		}
	void SetNotFoundFinalMark(const CStringA& sMark)
		{
		//ASSERT(sMark != NULL);
		m_NotFoundFinalMark = sMark;
		}
	const CStringA& GetNotFoundInitialMark()
		{return m_NotFoundInitialMark;}
	const CStringA& GetNotFoundFinalMark()
		{return m_NotFoundFinalMark;}

	////////////////////////////////////////////////////////////////////////
	// Funciones para el trabajo con marcas para no traducir
	////////////////////////////////////////////////////////////////////////

	void SetNoTranslationInitialMark(const CStringA& sMark)
		{
		//ASSERT(sMark != NULL);
		m_NotTranslationInitialMark = sMark;
		}
	void SetNotTranslationFinalMark(const CStringA& sMark)
		{
		//ASSERT(sMark != NULL);
		m_NotTranslationFinalMark = sMark;
		}
	const CStringA& GetNotTranslationInitialMark()
		{return m_NotTranslationInitialMark;}
	const CStringA& GetNotTranslationFinalMark()
		{return m_NotTranslationFinalMark;}

	////////////////////////////////////////////////////////////////////////
	// Funciones para la traduccion o busquedad de informacion en la oracion
	////////////////////////////////////////////////////////////////////////

	void TranslateSentence();
	};

////////////////////////////////////////////////////////////////////////
// Clase para el el control del proceso de traducción en la CAT
////////////////////////////////////////////////////////////////////////

class CTrdProc : public CMFCObject
{
private:
	CTranslationData	m_TranslationData;				// Datos para realizar la traducción
protected:
	CTranslationUser   *m_ptrDefaultTranslationUser;	// Puntero a los datos del usuario default

public:
	CTrdProc()
		{
		m_ptrDefaultTranslationUser = NULL;
		}
	virtual ~CTrdProc()
		{
		if(m_ptrDefaultTranslationUser != NULL)
			delete m_ptrDefaultTranslationUser;
		}

		
	////////////////////////////////////////////////////////////////////////
	// Funciones para el manejo de las propiedades de la clase
	////////////////////////////////////////////////////////////////////////
	
	CTranslationUser *GetDefaultTranslationUserPtr()
		{return m_ptrDefaultTranslationUser;}
	void SetDefaultTranslationUserPtr(CTranslationUser *ptrDefaultTranslationUser)
		{
		ASSERT(ptrDefaultTranslationUser != NULL);
		m_ptrDefaultTranslationUser = ptrDefaultTranslationUser;
		}
	CTranslationData *GetPtrTranslationData()
		{return &m_TranslationData;}

	void SetRootDir(CStringF ptrRootDir, int sw=(PATH_TRAD|PATH_LANG) )
		{
    if( sw & PATH_TRAD )
      m_TranslationData.SetRootDir(ptrRootDir);

    if( sw & PATH_LANG )
      {
      m_TranslationData.GetSourceLanguage().SetRootDir(ptrRootDir);
      m_TranslationData.GetTargetLanguage().SetRootDir(ptrRootDir);
      }
    }

  void SetTrdDir( int src, int des)
    {
    m_TranslationData.SetDir( src, des );
    
    m_TranslationData.GetSourceLanguage().SetLang(src);
    m_TranslationData.GetTargetLanguage().SetLang(des);
    }

	bool LoadTranslationData()
		{
		{
		//COLLECT_INTERVAL("OpenConjAndRed");
    m_TranslationData.GetSourceLanguage().OpenConjAndRed();
    m_TranslationData.GetTargetLanguage().OpenConjAndRed();
		}

		bool ret = GetPtrTranslationData()->Load();
		return ret;
		}

#ifdef ACTIVE_SAVE
	bool SaveTranslationData()
		{
    //m_TranslationData.GetSourceLanguage().SaveConjAndRed();
    //m_TranslationData.GetTargetLanguage().SaveConjAndRed();

		bool ret = GetPtrTranslationData()->Save();
		return ret;
		}
#endif

public:

};


#endif // !defined(AFX_TRANSLATIONPROCESS_H__31F3A961_03C1_11D3_8926_0060972DBBB5__INCLUDED_)
