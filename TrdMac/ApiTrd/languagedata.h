// LanguageData.h: interface for the CLanguageData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LANGUAGEDATA_H__659264E2_FF5D_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_LANGUAGEDATA_H__659264E2_FF5D_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UtilFun.h"
#include "ApiConj.h"
#include "RootLanguageData.h"
#include "LanguageName.h"
#include "CmdDcc.h"
#include "ApiRed.h"
#include "RegExp.h"
#include "Observer.h"

class CLanguageData : public CRootLanguageData, public CObserver
  {
  private:
	  bool		     m_Target;					        // Si es idioma de llegada o de partida
    CDccComodin  m_CmdFindQGrFunction;      // Trabajo con comodines para identificar función gramatical en oraciones interrogativas
    CDccComodin  m_CmdFindIGrFunction;      // Trabajo con comodines para identificar función gramatical en oraciones interrogativas
    CDccComodin  m_CmdFindGrFunction;       // Trabajo con comodines para identificar función gramatical
    CDccComodin  m_CmdFindTGrFunction;      // Trabajo con comodines para identificar función gramatical de frases de tiempo y con guion
    CDccComodin  m_CmdFindProperNoun;       // Trabajo con comodines para Buscar Nombres propios
    CDict       *m_TypeByDictLanguage;      // Búsqueda de la funcion gramatical mediante el lenguage de diccionario
    CDict       *m_TypeAndPattern;          // Diccionario que contiene para cada tipo gramatical su patrón de intercambio correspondiente
    CDict       *m_PreffixReduction;        // Diccionario que contiene los prefijos de reducción de palabras
    CDict       *m_SuffixReduction;         // Diccionario que contiene los sufijos de reducción de palabras
    CDict       *m_WordReduction;           // Diccionario para reducir las palabras irregulares del idioma
    CDict       *m_EspecialWordTranslation; // Diccionario que permite la traducción del ser o estar o del essere o avere
    CDict       *m_DataDictionary;          // Diccionario que contiene cadenas con datos especificas de este idioma
    CDict       *m_ProperNounDic;           // Diccionario con los nombres propios que no se traducen del idioma del idioma
    CSGrepArray *m_Contractions;           // RE para la eliminación de las contracciones en el idioma de partida
    CSGrepArray *m_Details;                // RE para pulir detalles en el idioma de llegada
    CSGrepArray *m_ProperNounGender;       // RE para obtener el género de los Nombres propios que no se encuentran en el diccionario
    CSGrepArray *m_SplitInWords;           // RE para la eliminación de las contracciones en el idioma de partida

  public:
    HREDW        m_hRedW;                   // Manipulador para la reducción de palabras
    HCONJ        m_hConj;                   // Manipulador para la conjugación de palabras
		CMap<HREDDATA> m_RedCache;              // Cache de la reduccion

    //////////////////////
    CStringA m_VerbalType;
    CStringA m_AdjectiveType;
    CStringA m_AdverbialType;
    CStringA m_ArticleType;
    CStringA m_TypeWithSpaceBeforeWord;
    CStringA m_TypeWithSpaceAfterWord;
    CStringA m_GerundType;
    CStringA m_TypeWithFirstLetterInUpperCase;
    CStringA m_ParticipleType;
    CStringA m_PastType;
    CStringA m_PastOrParticipleType;
    CStringA m_PosesiveType;
    CStringA m_PrepositionType;
    CStringA m_ProNounType;
    CStringA m_NounType;
    CStringA m_GradeType;
    CStringA m_FemaleType;
    CStringA m_PluralType;
    CStringA m_TypeThatNotNeedReduction;
    CStringA m_TypeNounDeclination;
    CStringA m_TypeAdjDeclination;
    CStringA m_FemaleTypeSiAsterisco;
    CStringA m_PluralTypeSiAsterisco;
    CStringA m_TiposPermitidosEnComodin;
    CStringA m_SemanticaPermitidaEnComodin;

    //////////////////////

    inline CStringA GetStringInDataDic(LPCSTR ptrTag)
      {
      CStringA sData;
			//LPCSTR sData;

      CDict *Dic = GetPtrDataDic();
      if( Dic != NULL && Dic->IsOpen() )
        Dic->GetKey(ptrTag, sData);

      return sData;
      }
  public:
    DECLARE_SERIAL(CLanguageData);

    CLanguageData()
      {
      m_Contractions     = NULL;
      m_Details          = NULL;
      m_ProperNounGender = NULL;
      m_SplitInWords     = NULL;

      m_hRedW            = NULL;
      m_hConj            = NULL;
      
      m_TypeByDictLanguage 			= new CDict("LangData_TypeByDictLanguage");
      m_TypeAndPattern 					= new CDict("LangData_TypeAndPattern");
      m_PreffixReduction 				= new CDict("LangData_PreffixReduction");
      m_SuffixReduction 				= new CDict("LangData_SuffixReduction");
      m_WordReduction 					= new CDict("LangData_WordReduction");
      m_EspecialWordTranslation = new CDict("LangData_EspecialWordTranslation");
      m_DataDictionary 					= new CDict("LangData_DataDictionary");
      m_ProperNounDic 					= new CDict("LangData_ProperNounDic");
      }

    virtual ~CLanguageData()
      {
      if( m_Contractions     != NULL) delete m_Contractions;
      if( m_Details          != NULL) delete m_Details;
      if( m_ProperNounGender != NULL) delete m_ProperNounGender;
      if( m_SplitInWords     != NULL) delete m_SplitInWords;

      if( m_hRedW != NULL ) RWFree( m_hRedW );
      if( m_hConj != NULL ) CVFree( m_hConj );
			m_RedCache.DeleteAllT<RWFreehData>();
      
      delete m_TypeByDictLanguage;
      delete m_TypeAndPattern;    
      delete m_PreffixReduction;
      delete m_SuffixReduction;
      delete m_WordReduction;
      delete m_EspecialWordTranslation;
      delete m_DataDictionary;
      delete m_ProperNounDic; 
      }
    
    // se llama cuando se modifica el dicc principal
    void onModify()
      {
      // limpiar la cache
			m_RedCache.DeleteAllT<RWFreehData>();
      }
      
    bool OpenConjAndRed()
      {
      int       iLang = GetIntLanguageName();
      CStringF DicsPath = GetRootDir();

      m_hConj = CVNew();
      CVSetDataPath( m_hConj, DicsPath );

      if( !CVOpen(m_hConj, iLang) )
        {
        CVFree(m_hConj);
        m_hConj = NULL;
        }

      m_hRedW = RWNew();
      RWSetDataPath( m_hRedW, DicsPath );
      RWSetConjHandle( m_hRedW, m_hConj, false );
      
      if( !RWOpen(m_hRedW, iLang) )
        {
        RWFree(m_hRedW);
        m_hRedW = NULL;
        }

      return true;
      }
      
	void SetTarget(bool b = true) {m_Target = b;}
	bool GetTarget()	{return m_Target;}

    inline CDccComodin *GetCmdFindGrFunctionDic()
      {return &m_CmdFindGrFunction;}
    inline CDccComodin *GetCmdFindQGrFunctionDic()
      {return &m_CmdFindQGrFunction;}
    inline CDccComodin *GetCmdFindIGrFunctionDic()
      {return &m_CmdFindIGrFunction;}
    inline CDccComodin *GetCmdFindTGrFunctionDic()
        {return &m_CmdFindTGrFunction;}
    inline CDccComodin *GetCmdFindProperNounDic()
      {return &m_CmdFindProperNoun;}
    inline CDict *GetPtrTypeByDictLanguageDic()
      {return m_TypeByDictLanguage;}
    inline CDict *GetPtrTypeAndPatternDic()
      {return m_TypeAndPattern;}
    inline CDict *GetPtrPreffixReductionDic()
      {return m_PreffixReduction;}
    inline CDict *GetPtrSuffixReductionDic()
      {return m_SuffixReduction;}
    inline CDict *GetPtrWordReductionDic()
      {return m_WordReduction;}
    inline CDict *GetPtrEspecialWordTranslationDic()
      {return m_EspecialWordTranslation;}
    inline CDict *GetPtrDataDic()
      {return m_DataDictionary;}
    inline CDict *GetPtrProperNounDic()
      {return m_ProperNounDic;}
    inline CSGrepArray *GetPtrContractions()
      {return m_Contractions;}
    inline CSGrepArray *GetPtrDetails()
      {return m_Details;}
    inline CSGrepArray *GetPtrProperNounGender()
      {return m_ProperNounGender;}
    inline CSGrepArray *GetPtrSplitInWords()
      {return m_SplitInWords;}
    inline CStringA GetVerbalType()
      {return m_VerbalType;}
    inline CStringA GetAdjectiveType()
      {return m_AdjectiveType;}
    inline CStringA GetAdverbialType()
      {return m_AdverbialType;}
    inline CStringA GetArticleType()
      {return m_ArticleType;}
    inline CStringA GetTypeWithSpaceBeforeWord()
      {return m_TypeWithSpaceBeforeWord;}
    inline CStringA GetTypeWithSpaceAfterWord()
      {return m_TypeWithSpaceAfterWord;}
    inline CStringA GetGerundType()
      {return m_GerundType;}
    inline CStringA GetTypeWithFirstLetterInUpperCase()
      {return m_TypeWithFirstLetterInUpperCase;}
    inline CStringA GetParticipleType()
      {return m_ParticipleType;}
    inline CStringA GetPastType()
      {return m_PastType;}
    inline CStringA GetPastOrParticipleType()
      {return m_PastOrParticipleType;}
    inline CStringA GetPosesiveType()
      {return m_PosesiveType;}
    inline CStringA GetPrepositionType()
      {return m_PrepositionType;}
    inline CStringA GetProNounType()
      {return m_ProNounType;}
    inline CStringA GetNounType()
      {return m_NounType;}
    inline CStringA GetGradeType()
      {return m_GradeType;}
    inline CStringA GetFemaleType()
      {return m_FemaleType;}
    inline CStringA GetPluralType()
      {return m_PluralType;}
    inline CStringA GetTypeThatNotNeedReduction()
      {return m_TypeThatNotNeedReduction;}
    inline CStringA GetTypeNounDeclination()
      {return m_TypeNounDeclination;}
    inline CStringA GetTypeAdjDeclination()
      {return m_TypeAdjDeclination;}
    inline CStringA GetFemaleTypeSiAsterisco()
      {return m_FemaleTypeSiAsterisco;}
    inline CStringA GetPluralTypeSiAsterisco()
      {return m_PluralTypeSiAsterisco;}
    inline CStringA GetTiposPermitidosEnComodin()
      {return m_TiposPermitidosEnComodin;}
    inline CStringA GetSemanticaPermitidaEnComodin()
      {return m_SemanticaPermitidaEnComodin;}
			
    inline const CStringA& ConjugateVerb(CStringA &str, const CStringA& verbo, Modo modo,Tiempo tiempo, Persona persona,Numero numero, BOOL negado,BOOL compuesto)
      {
      if( m_hConj )
        str = CVConjugate( m_hConj, verbo, modo, tiempo, persona, numero, negado, compuesto );
      return str;
      }
			
    inline const CStringA& PutWordInGender(CStringA &ptrWord, Genero gender)
      {
      if( m_hConj )
        ptrWord = CVConcordWords( m_hConj, ptrWord, MASCULINO, SINGULAR, gender, SINGULAR, POSITIVO );
      return ptrWord;
      }
			
    inline const CStringA& PutWordInNumber(CStringA &ptrWord,Numero number)
      {
      if( m_hConj )
        ptrWord = CVConcordWords( m_hConj, ptrWord, MASCULINO, SINGULAR, MASCULINO, number, POSITIVO );
      return ptrWord;
      }
			
    inline const CStringA& PutAdjectiveInGrade(CStringA &ptrWord, Grado eGrade)
      {
      if (m_hConj )
        ptrWord = CVConcordWords( m_hConj, ptrWord, MASCULINO, SINGULAR, MASCULINO, SINGULAR, eGrade );
      return ptrWord;
      }
			
    inline const CStringA& PutAdjectiveInDeclination(CStringA &ptrWord, Genero gender, Numero number,Declination declination, ArticleType article)
      {
      if( m_hConj )
        ptrWord = CVDeclineAdjective( m_hConj, ptrWord, gender, number, declination, article );
      return ptrWord;
      }
			
    inline const CStringA& PutNounInDeclination(CStringA &ptrWord, Genero gender, Numero number,Declination declination )
      {
      if( m_hConj )
        ptrWord = CVDeclineNoun( m_hConj, ptrWord, gender, number, declination );
      return ptrWord;
      }
			
    inline int GetIntLanguageName()
      {
      return CLangIso::Id(GetIsoName());
      }
			
    bool Load(bool target = false);
#ifdef ACTIVE_SAVE
		bool Save(bool target = false);
#endif

    virtual void Serialize(CArchive &ar);
  };

#endif // !defined(AFX_LANGUAGEDATA_H__659264E2_FF5D_11D2_8926_0060972DBBB5__INCLUDED_)
