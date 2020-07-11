// Translation.cpp: implementation of the CTranslation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UtilFun.h"
#include "ApiConj.h"
#include "COra.h"
#include "LengDic.h"
#include "PeekMsg.h"
#include "ViewFlag.h"
#include "Translation.h"
#include "FindWordSentenceInDict.h"
#include "ExeWordAndTypeCmd.h"
#include "GetWordType.h"
#include "FindPhrasesInSentence.h"
#include "FindCmdPhrasesInSentence.h"
#include "ChangeWordType.h"
#include "ConvertVerbInNoun.h"
#include "FindPhraseInProperNounDictionary.h"
#include "RestoreWordType.h"
#include "FindWordTypeByLanguage.h"
#include "FillInterchagePattern.h"
#include "FillInsertedWord.h"
#include "InsertOrAddWord.h"
#include "GenderAndNumberInsertedWord.h"
#include "TranslatePreffix.h"
#include "ConjugateVerbsInSentence.h"
#include "CoordinateWords.h"
#include "TranslateTwoTranslationVerb.h"
#include "ChangeWordTypeForMood.h"
#include "IsAllWordFirstInUpperCase.h"
#include "ScanWordsInDictionary.h"
#include "PutDetailsInSentence.h"
#include "IsAllWordInUpperCase.h"
#include "IsFirstLetterInFirstWordUpperCase.h"
#include "MakeTranslatedSentence.h"
#include "FindWordSentenceInDict.h"
#include "ConjugateByPattern.h"
#include "ReflexiveVerbInSentence.h"
#include "KillContractionsInSentence.h"
#include "EmptyWordsInSentence.h"
#include "ParseWordsInSentence.h"
#include "IsInInterrogativeSentence.h"
#include "PutMoodAndTimeInGramType.h"
#include "FillWordAndTypeArray.h"
#include "SplitSentenceInPhrases.h"

#if !defined(MERCADO)
  #include "..\\Src2\\viewword.h"
//  #include "viewword.h"
#endif

#ifdef TRACE_DEFINED
/*const char *StringArrayStepInTranslation[]=
	{
	"stCaseSentenceProperty",
	"stTypeWordByWildCard",
	"stGetTypeOfWord",
	"stFindWords",
	"stFindPhrases",
	"stFindProperNoun",
	"stTranslationWildCard",
	"stFindWordType",
	"stTranslateWords",
	"stTranslateBe",
	"stCmdCoorCnj",
	"stFindMood",
	"stFindInterchangePattern",
	"stInsertArticle",
	"stFillInsertWords",
	"stGenderAndNumber",
	"stTranslatePreffix",
	"stConjugate",
	"stEnding",
	"stTranslateReflexiveVerbs",
	"stMakeTranslatedSentence",
	"stDetails",
	NULL*/

	/*"Detecta propiedades de mayúsculas y minúsculas en la oración",
	"Comodines de palabras y/o tipos gramaticales",
	"Ejecuta lenguaje de dicc si lo tienen en Tipo gramatical",
	"Busca palabras en el diccionario",
	"Busca frases idiomáticas en el diccionario",
	"Busca posibles nombres propios en forma de frases",
	"Busca palabras que sean nombres propios en el dcic de nombres propios",
	"Busca cadenas con primera en mayúsculas seguidas",
	"Busca frases de comodin al estilo P0 XX SS DD",
	"Comodines para la traducción de frases hechas",
	"Función gramatical mediante división en frases",
	"Función gramatical mediante Lenguaje de dic",
	"Traduce palabras y frases",
	"Traduce verbo be en ser o estar",
	"Comodines de Conjugación y coordinación",
	"Busca el Modo de los verbos por lenguaje de Dic",
	"Patrones de intercambio de palabras en la oración",
	"Inserta palabras a insertar o adiccionar",
	"Llena palabras insertadas",
	"Genero y número de las palabras insertadas",
	"Traduce palabras con prefijos",
	"Conjuga verbos",
	"Coordina adjetivos y sustantivos",
	"Traduce verbos reflexivos",
	"Borra palabras marcadas para borrar",
	"Construye la oración traducida",
	"Detalles del lenguaje de llegada en la oración",
	"Crea Detalles Evalua creando variantes de !!....!! y &&....&&",
	"Evalúa la oración traducida por NGram",
	"LLena un array con las diferentes variantes en tipos de la oraciones y lo traduce",
//	"Pone los tipos de una CString a cada una de las palabras de uns oración",
	"Busca palabras de la oración en dic general y llena un dic",
	"Busca palabras en el dic y llena dic con no encontradas",
	"Llena diccionario de posibles nombres propios en la oración",
	"Llena diccionario de palabras con valor semantico",
	"Llena diccionario con los tipos gramaticales de la oración",
	"Llena diccionario con las posibles frases sustantivas de la oración",
	"Busca en Dcc principal de no traduccion palabras para no traducir",
	"Busca en Dcc principal de no traduccion frases para no traducir",
	"Busca en arreglo de dcc de no traduccion palabras para no traducir",
	"Evalua la oracion de entrada",
	"Llena un dcc con el ngram encontrados en la oracion",
	"Busca en arreglo de dcc de no traduccion frases para no traducir",
	"Busca las posibles frases sustantivas en la oración a traves de reglas gramaticales",
	"Llena una cadena con el comodin y las palabras claves de la forma for [AA] [SS] in the [SS]",
	NULL*/
//	};
#endif


//////////////////////////////////////////////////////////////////////
// Ejecuta todos los pasos de traducción en el orden del array
//////////////////////////////////////////////////////////////////////

void CTranslation::ProcessStepArray()
	{
	if(GetPtrSentence()->m_Ora.IsEmpty() == TRUE)
			return;

	bool bIsAllGramaticalTypeSimple = false;
	bool bEntreUnaVez               = false;

	sAllTypeInSentenceTranslation.Empty();
	sAllTypeInSentenceTranslationView.Empty();

	for(int i = 0; i < m_Steps.GetSize() && GetPtrSentence()->m_Traducida == false; i++)
		{
		int iOption = m_Steps.GetAt(i);

		View(iOption);
		//COLLECT_INTERVAL(StringArrayStepInTranslation[iOption]);

		switch(iOption)
			{
			case stCaseSentenceProperty:          // obtiene propiedades de la oración tales como si esta toda en mayúsculas, si es primera letra en mayúsculas, etc.
				FunctionCaseSentenceProperty();
				break;
			case stFindWords:                     // busca la palabra dentro el diccionario y llena sus datos
				FunctionFindWords();
				break;
			case stTypeWordByWildCard:            // busca el tipo gramatical de la palabra mediante comodines
				FunctionTypeWordByWildCard(bEntreUnaVez,bIsAllGramaticalTypeSimple);
				break;
			case stGetTypeOfWord:                 // Obtiene un tipo gramatical de dos letras si existe lenguaje de diccionario en el tipo gramatical de la palabra que viene del diccionario
				FunctionGetTypeOfWord();
				break;
			case stFindPhrases:                   // busca frases idiomáticas dentro de la oración
				FunctionFindPhrases();
				break;
			case stFindProperNoun:                // busca posibles nombres propios mediante comodines busca nuevas frases idiomáticas dentro del dcc de nombres propios
				FunctionFindProperNoun();
				break;
			case stTranslationWildCard:           // traduce mediante el uso de comodines
				FunctionTranslationWildCard();
				break;
			case stFindWordType:    // busca el tipo gramatical utilizando el lenguaje de diccionario
				FunctionFindWordType();
				break;
			case stTranslateWords:          // traduce todas las palabras de la oración
				FunctionTranslateWords();
				break;
			case stTranslateBe:         // traduce los verbos que tiene dos formas tales como ser o estar, essere o avere, etc
				FunctionTranslateBe();
				break;
			case stCmdCoorCnj:  // utiliza los comodines para lograr la conjugación y coordinación (es un algoritmo viejo no debe utilizarse más)
				FunctionCmdCoorCnj();
				break;
			case stFindMood:        // busca el modo de los verbo según el lenguaje de diccionario (es un algoritmo viejo)
				FunctionFindMood();
				break;
			case stFindInterchangePattern:  // intercambia de posición las palabras dentro de la oración (es un algoritmo viejo)
				FunctionFindInterchangePattern();
				break;
			case stInsertArticle:     // inserta palabras dentro de la oración
				FunctionInsertArticle();
				break;
			case stFillInsertWords: // busca las palabras insertadas en el diccionario y llena sus datos
				FunctionFillInsertWords();
				break;
			case stGenderAndNumber:     // concuerda en genero y número las palabras insertadas en la oración
				FunctionGenderAndNumber();
				break;
			case stTranslatePreffix:   // Traducción de prefijos
				FunctionTranslatePreffix();
				break;
			case stConjugate:   // conjuga los verbos que no pudieron ser conjugados mediante comodines
				FunctionConjugate();
				break;
			case stEnding:      // concuerda en genero y número las palabras que no pudieron ser traducidas mediante comodines
				FunctionEnding();
				break;
			case stTranslateReflexiveVerbs:     // pone la terminación a los verbos refléxivos
				FunctionTranslateReflexiveVerbs();
				break;
			case stMakeTranslatedSentence:  // construye la oración traducida
				FunctionMakeTranslatedSentence();
				break;
			case stDetails:     // pone detalles del idioma tales como contracciones, etc. en la oración traducida
				FunctionDetails();
				break;
			}
		}

	View(99999);
	}

//////////////////////////////////////////////////////////////////////
// Muestra la oracion en el paso dado
//////////////////////////////////////////////////////////////////////

void CTranslation::View(int iStep)
	{
	GetPtrSentence()->View(iStep);
	}

//////////////////////////////////////////////////////////////////////

void CTranslation::KillContractionsInSentence()
	{
	ASSERT(GetTranslationUserPtr() != NULL);
	ASSERT(GetPtrSentence() != NULL);
	CKillContractionsInSentence objKillContractionsInSentence(GetTranslationUserPtr(),GetPtrSentence());
	objKillContractionsInSentence.Process();
	PEEKMESSAGE();
	}

/////////////////////////////////////////////////////////////////////////////

void CTranslation::ParseWordsInSentence(/*int iFormat = FMT_RTF*/)
	{
	ASSERT(GetTranslationUserPtr() != NULL);
	ASSERT(GetPtrSentence() != NULL);
	CParseWordsInSentence objParseWordsInSentence(GetTranslationUserPtr(),GetPtrSentence());//,iFormat);
	objParseWordsInSentence.Process();
	PEEKMESSAGE();
	}

/////////////////////////////////////////////////////////////////////////////

void CTranslation::EmptyWordsInSentence()
	{
	CEmptyWordsInSentence objEmptyWordsInSentence(GetPtrSentence());

	objEmptyWordsInSentence.Process();
	PEEKMESSAGE();
	}

/////////////////////////////////////////////////////////////////////////////
// true     : si no existen diferencias entre los tipos anteriores y los nuevos
// false    : los tipos han sido cambiados
/////////////////////////////////////////////////////////////////////////////

bool CTranslation::TestIsAnyDifferent(CStringA &sStr)
	{
	PEEKMESSAGE();
    if(sStr == "FO ")
        return true;
    CGetAllTypeInSentence GetAllTypeInSentence(GetTranslationUserPtr(),GetPtrSentence());
    CStringA sTempAllType = GetAllTypeInSentence.GetTypeList();
    if(sStr.GetLength() > 0 && sStr == sTempAllType)
        return true;
    else
        sStr = sTempAllType;
    return false;
    }

/////////////////////////////////////////////////////////////////////////////
// Detiene la traducción si la oración está formada sólo por XX FO XX
/////////////////////////////////////////////////////////////////////////////

bool CTranslation::StopTranslation()
	{
	PEEKMESSAGE();
    CStopTranslationInTranslateSentence StopTranslationInTranslateSentence(GetTranslationUserPtr(),GetPtrSentence());
    if(StopTranslationInTranslateSentence.GetTranslatedFlag() == true)
        return true;
    return false;
    }

/////////////////////////////////////////////////////////////////////////////
// LLena la propiedad que dice si la oracion esta en mayusculas, etc
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionCaseSentenceProperty()
	{
	CIsInInterrogativeSentence objIsInInterrogativeSentence(GetPtrSentence());

	GetPtrSentence()->m_bInterrogative = objIsInInterrogativeSentence.Test();

	CIsAllWordInUpperCase objIsAllWordInUpperCase(GetPtrSentence());

	GetPtrSentence()->m_AllUpper = (objIsAllWordInUpperCase.Test())?TRUE:FALSE;

	if(GetPtrSentence()->m_AllUpper == FALSE)
		{
		CIsFirstLetterInFirstWordUpperCase objIsFirstLetterInFirstWordUpperCase(GetPtrSentence());

		// pregunta si la primera palabra es mayúsculas
		GetPtrSentence()->m_FirstWordUpper = (objIsFirstLetterInFirstWordUpperCase.Test())?TRUE:FALSE;
		}
	}
/////////////////////////////////////////////////////////////////////////////
// busca la palabra dentro el diccionario y llena sus datos
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionFindWords()
	{
  CStringF sSourceLanguage(GetPtrTranslationData()->GetSourceLanguage().GetIsoName());

	View(99);
	{
	//CTranslationUser *ptrtu = GetTranslationUserPtr();
	CFindWordSentenceInDict Find(GetTranslationUserPtr(),GetPtrSentence());

	Find.Process();

	View(100);
	if(Find.GetWordCount()> 0)
		{
		WORD porciento = (Find.GetNotFoundCount()*100)/Find.GetWordCount();
		if((porciento > 50 && Find.GetWordCount() > 6) || (porciento >= 60 && Find.GetWordCount() >= 5) || (porciento >= 65 && Find.GetWordCount() < 6 && Find.GetWordCount() >= 3) || porciento == 100)
			{
			// La oracion esta en un idioma que no es el original
			GetPtrSentence()->m_Dest = GetPtrSentence()->m_Orig;
			GetPtrSentence()->m_Traducida = TRUE;
			GetTranslationUserPtr()->SetTranslationType(TT_BadSourceLang);
			return;
			}
		}
	}
	
	View(101);
	if(GetPtrSentence()->m_FirstWordUpper == TRUE) // pregunta si todas las palabras de la oración comienzan en mayusculas
		{
		CIsAllWordFirstInUpperCase IsAllWordFirstInUpperCase(GetPtrSentence());

		GetPtrSentence()->m_bAllFirstUpper = IsAllWordFirstInUpperCase.Test();
		}
		
	View(102);
	if(sSourceLanguage.Find("De") != -1)
		{
		CFindVerboSeparableDe FindVerboSeparableDe(GetTranslationUserPtr(),GetPtrSentence());

		FindVerboSeparableDe.Process();
		}
		
	View(103);
	{
	CPutMoodAndTimeInGramType objPutMoodAndTimeInGramType(GetTranslationUserPtr(),GetPtrSentence());

	objPutMoodAndTimeInGramType.Process();
	}

	View(104);
/*********	OJO 
	if(GetTranslationUserPtr()->GetInitialTypes().IsEmpty() == TRUE)
		{
		CGetAllTypeInSentence GetAllTypeInSentence(GetTranslationUserPtr(),GetPtrSentence());
		GetTranslationUserPtr()->SetInitialTypes(GetAllTypeInSentence.GetTypeList());
		}
**************/		
	}

/////////////////////////////////////////////////////////////////////////////
// busca el tipo gramatical de la palabra mediante comodines
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionTypeWordByWildCard(bool &bEntreUnaVez,bool &bIsAllGramaticalTypeSimple)
	{
	if(sAllTypeInSentenceTranslation == "FO ")
		return;

	if(bEntreUnaVez == true)
		{
		if(bIsAllGramaticalTypeSimple == false)
			{
			CIsAllGramaticalTypeInSentenceSimple IsAllGramaticalTypeInSentenceSimple(GetTranslationUserPtr(),GetPtrSentence());
			bIsAllGramaticalTypeSimple = IsAllGramaticalTypeInSentenceSimple.GetIsAllGramaticalTypeSimpleFlag();
			if( bIsAllGramaticalTypeSimple == true)
				return;
			}
		else
			return;
		}
	bEntreUnaVez = true;

	//                if(StopTranslation())
	//                    return;
	{
	CPutMoodAndTimeInGramType objPutMoodAndTimeInGramType(GetTranslationUserPtr(),GetPtrSentence());

	objPutMoodAndTimeInGramType.Process();
	}

	// si la oración es interrogativa 
	// si es interrrogativa ejecutar primero los comodines 
	// para interrogativas
	if(GetPtrSentence()->m_bInterrogative == true)
		{
		for(int i = 0; i < 3; i++)
			{
			CExeWordAndTypeQCmd ExecWordAndTypeQCmd(GetTranslationUserPtr(),GetPtrSentence());

			ExecWordAndTypeQCmd.Process();
		//    if(ExecWordAndTypeQCmd.IsMatch() == false)
		//        break;
				if(TestIsAnyDifferent(sAllTypeInSentenceTranslation))
					break;
			}
		}
	for(int i = 0; i < 3; i++)
		{
			{
			CExeWordAndTypeCmd ExecWordCmd(GetTranslationUserPtr(),GetPtrSentence(),true,false);

			ExecWordCmd.Process();
			}
			{
			CExeWordAndTypeCmd ExecTypeCmd(GetTranslationUserPtr(),GetPtrSentence(),false,true);

			ExecTypeCmd.Process();
			}
	  //  if(ExecWordAndTypeCmd.IsMatch() == false)
	  //      break;
		if(TestIsAnyDifferent(sAllTypeInSentenceTranslation))
			break;
		}

	if(GetPtrSentence()->m_WordsInSentence.GetSize() == 0)
		{
		CFillWordAndTypeArray FillWordAndTypeArray(GetTranslationUserPtr(),GetPtrSentence());
		FillWordAndTypeArray.Process();   // llena el arreglo de todas las palabras que conforman la oracion
		}
	// if(TestIsAnyDifferent(sAllTypeInSentenceTranslation))
	//    return;
	}
	
/////////////////////////////////////////////////////////////////////////////
// Obtiene un tipo gramatical de dos letras si existe lenguaje de diccionario en el tipo gramatical de la palabra que viene del diccionario
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionGetTypeOfWord()
	{
	if(sAllTypeInSentenceTranslation == "FO ")
		return;
//  if(StopTranslation())
//      break;

	CGetWordType GetTypeOfWord(GetTranslationUserPtr(),GetPtrSentence());
	GetTypeOfWord.Process();
	}

/////////////////////////////////////////////////////////////////////////////
// busca frases idiomáticas dentro de la oración
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionFindPhrases()
	{
	/******** OJO poner de nuevo
	CFindPhrasesInSentence FindPhrases(GetTranslationUserPtr(),GetPtrSentence());

	FindPhrases.Process();

	***************/
	CFindBracketPhraseInSentence FindBracketPhrases(GetTranslationUserPtr(),GetPtrSentence());

	FindBracketPhrases.Process();

	// Cambia el tipo gramatical de VT,VI a BL o BZ en dependencia del Modo del verbo
		{
		CChangeWordType ChangeWordType(GetTranslationUserPtr(),GetPtrSentence(),"F");

		ChangeWordType.Process();
		}
// Nuevo para poner las frases y el tipo como un grupo dentro de las palabras de la oracion		
	if (GetTranslationUserPtr()->GetInitialTypes().IsEmpty() == TRUE)
		{
		CGetAllTypeInSentence GetAllTypeInSentence(GetTranslationUserPtr(),GetPtrSentence());
		GetTranslationUserPtr()->SetInitialTypes(GetAllTypeInSentence.GetTypeList());
		
		for(POSITION ptrNode = GetHeadPos();
			  ptrNode != NULL;
			  GetNextPos(ptrNode))
			{
			ASSERT(ptrNode != NULL);

			LPCWORD ptrWord = GetWordAt(ptrNode);   // Palabra que se analiza 
			ASSERT(ptrWord != NULL);

			if (ptrWord != NULL)
				{
				if(ptrWord->m_Origlwr.IsEmpty() == FALSE)
					{
					GetTranslationUserPtr()->AddWordToList(ptrWord->m_Origlwr);
					}
				else
					{
					GetTranslationUserPtr()->AddWordToList(ptrWord->m_Key);
					}
				}
			}
		}
	}

	
/////////////////////////////////////////////////////////////////////////////
// busca posibles nombres propios mediante comodines busca nuevas frases idiomáticas dentro del dcc de nombres propios
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionFindProperNoun()
	{
  CStringF sSourceLanguage(GetPtrTranslationData()->GetSourceLanguage().GetIsoName());

	View(100);
	// Busca posibles frases en el diccionario de nombres propios 
		{
		CFindPhraseInProperNounDictionary FindPhraseInProperNounDictionary(GetTranslationUserPtr(),GetPtrSentence());

		FindPhraseInProperNounDictionary.Process();
		}
		
	View(101);
	// Cambia el tipo para palabras que son plurales y verbos o gerundios
		{
	//	CChangeWordType ChangeWordType(GetTranslationUserPtr(),GetPtrSentence(),"N");

	//	ChangeWordType.Process();
		}

	// Convierte los verbos que tengan delante un artículo en sustantivos
	if(sSourceLanguage.Find("En") != -1)
		{
		CConvertVerbInNoun ConvertVerbInNoun(GetTranslationUserPtr(),GetPtrSentence());

		ConvertVerbInNoun.Process();
		}

	View(102);
		{
		// Busca frases que sean nombres propios utilizando el diccionario de nombres propios
		CProperNounWildCard objProperNounWordWildCard(GetTranslationUserPtr(),GetPtrSentence(),true,false);

		objProperNounWordWildCard.Process();

		View(103);
		CProperNounWildCard objProperNounTypeWildCard(GetTranslationUserPtr(),GetPtrSentence(),false,true);

		objProperNounTypeWildCard.Process();
		}
	View(104);
	}

/////////////////////////////////////////////////////////////////////////////
// traduce mediante el uso de comodines
/////////////////////////////////////////////////////////////////////////////
void CTranslation::FunctionTranslationWildCard()
	{
  if(sAllTypeInSentenceTranslation == "FO ")
    return;
	View(100);

	// Cambia el tipo gramatical de VT,VI a BL o BZ en dependencia del Modo del verbo
		{
		CChangeWordType ChangeWordType(GetTranslationUserPtr(),GetPtrSentence(),"F");

		ChangeWordType.Process();
		}

	View(101);
	// Contiene las reglas que deben pasar primero con Sustantivo + numero y las frases con guion
	CTranslationIWildCard objTranslationIWildCard(GetTranslationUserPtr(),GetPtrSentence());

	objTranslationIWildCard.Process();

	View(102);
	for(int inew = 0 ; inew < 4; inew++)
		{
		GetTranslationUserPtr()->SetTranslationType(TT_RuleBase);
		
      {
		  CTranslationFWildCard objTranslationWordFWildCard(GetTranslationUserPtr(),GetPtrSentence(),true,false);

		  objTranslationWordFWildCard.Process();
      }
			
      {
		  CTranslationFWildCard objTranslationTypeFWildCard(GetTranslationUserPtr(),GetPtrSentence(),false,true);

		  objTranslationTypeFWildCard.Process();
      }
			
    if(TestIsAnyDifferent(sAllTypeInSentenceTranslation))
      break;
    }

  // Busqueda de frases verbales
	//for(iNewLength =0,iOldLength = GetPtrSentence()->GetLength(),inew = 0; iNewLength < iOldLength && inew < 5; inew++)	
  for(int inew = 0 ; inew < 3; inew++)
		{
      {
		  CTranslationVWildCard objTranslationWordVWildCard(GetTranslationUserPtr(),GetPtrSentence(),true,false);

		  objTranslationWordVWildCard.Process();
      }
				
      {
		  CTranslationVWildCard objTranslationTypeVWildCard(GetTranslationUserPtr(),GetPtrSentence(),false,true);

		  objTranslationTypeVWildCard.Process();
      }

    if(TestIsAnyDifferent(sAllTypeInSentenceTranslation))
      break;
    }

	//View(stTranslationWildCard);
/***************
    if(GetPtrSentence()->m_bImperative == true)
		{
		CTranslationIWildCard objTranslationIWildCard(GetTranslationUserPtr(),GetPtrSentence());

		objTranslationIWildCard.Process();
        }
*************/
  if(GetPtrSentence()->m_bInterrogative == true)
		{
//		for(iNewLength =0,iOldLength = GetPtrSentence()->GetLength(),inew = 0; iNewLength < iOldLength && inew < 5; inew++)	
    for(int inew = 0 ; inew < 2; inew++)
			{
			//iOldLength = GetPtrSentence()->GetLength();
				{
		    CTranslationQWildCard objTranslationQWildCard(GetTranslationUserPtr(),GetPtrSentence());

		    objTranslationQWildCard.Process();
        }
			//iNewLength = GetPtrSentence()->GetLength();
      if(TestIsAnyDifferent(sAllTypeInSentenceTranslation))
        break;
      }
    }

//                if(StopTranslation())
//                    break;

  for(int inew = 0 ; inew < 4; inew++)
	//for(iNewLength =0,iOldLength=GetPtrSentence()->GetLength(),inew = 0; iNewLength < iOldLength && inew < 5; inew++)	
		{
		//iOldLength = GetPtrSentence()->GetLength();

			{	// para que se eliminen las XX al inicio y al final
			CTranslationWildCard objTranslationWildCard(GetTranslationUserPtr(),GetPtrSentence());

			objTranslationWildCard.Process();
			}

		//View(stTranslationWildCard);
		//iNewLength = GetPtrSentence()->GetLength();
    if(TestIsAnyDifferent(sAllTypeInSentenceTranslation))
			break;
		}
  //if(TestIsAnyDifferent(sAllTypeInSentenceTranslation))
  //    break;
	//View(stTranslationWildCard);
	}
	
/////////////////////////////////////////////////////////////////////////////
// busca el tipo gramatical utilizando el lenguaje de diccionario
/////////////////////////////////////////////////////////////////////////////
void CTranslation::FunctionFindWordType()
	{

//  if(StopTranslation())
//     return;
	// llena la varable de tipos
	CGetAllTypeInSentence GetAllTypeInSentence(GetTranslationUserPtr(),GetPtrSentence());
	GetTranslationUserPtr()->SetMiddleTypes(GetAllTypeInSentence.GetTypeList());

	CFindWordTypeByLanguage FindWordTypeByLanguage(GetTranslationUserPtr(),GetPtrSentence());

	FindWordTypeByLanguage.Process();
	}
/////////////////////////////////////////////////////////////////////////////
// traduce todas las palabras de la oración
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionTranslateWords()
	{
//  if(StopTranslation())
//     return;
	CTranslateWordsInSentence TranslateWords(GetTranslationUserPtr(),GetPtrSentence());

	TranslateWords.Process();
	}
/////////////////////////////////////////////////////////////////////////////
// traduce los verbos que tiene dos formas tales como ser o estar, essere o avere, etc
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionTranslateBe()
	{
	// Traduce el verbo be a ser o estar
	// Debe agregarse en el italiano la traducción del verbo have a avere o essere

	CTranslateTwoTranslationVerb TwoTranslationVerb(GetTranslationUserPtr(),GetPtrSentence());

	TwoTranslationVerb.Process();
	}
/////////////////////////////////////////////////////////////////////////////
// utiliza los comodines para lograr la conjugación y coordinación (es un algoritmo viejo no debe utilizarse más)
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionCmdCoorCnj()
	{
    if(sAllTypeInSentenceTranslation == "FO ")
        return;
//                if(StopTranslation())
//                    break;

	// las llaves garantizan que se ejecute el destructor
	// comodines para la coordinación de frases sustantivas y verbales
		{
		CCoordinateAndConjugateSentence objCoordinateAndConjugateSentence(GetTranslationUserPtr(),GetPtrSentence());
		objCoordinateAndConjugateSentence.Process();
		}

	// Cambia el tipo para que los comodines no sean muchos por ejemplo une transitivo con intransitivo
		{
		CChangeWordType ChangeWordType(GetTranslationUserPtr(),GetPtrSentence(),"C");

		ChangeWordType.Process();
		}

		{
		CConjugateByPattern objConjugateByPattern(GetTranslationUserPtr(),GetPtrSentence());

		objConjugateByPattern.Process();
		}

		{
		CRestoreWordType RestoreWordType(GetPtrSentence());

		RestoreWordType.Process();
		}
	}

/////////////////////////////////////////////////////////////////////////////
// busca el modo de los verbo según el lenguaje de diccionario (es un algoritmo viejo)
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionFindMood()
	{
    if(sAllTypeInSentenceTranslation == "FO ")
        return;
//  if(StopTranslation())
//      break;
	CChangeWordTypeForMood ChangeWordTypeForMood(GetTranslationUserPtr(),GetPtrSentence(),"M");

	ChangeWordTypeForMood.Process();
//			DWORD iT = GetTickCount();
	}
/////////////////////////////////////////////////////////////////////////////
// intercambia de posición las palabras dentro de la oración (es un algoritmo viejo)
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionFindInterchangePattern()
	{
    if(sAllTypeInSentenceTranslation == "FO ")
        return;
//  if(StopTranslation())
//      break;
	CFillInterChangePattern FillInterChangePattern(GetTranslationUserPtr(),GetPtrSentence());

	FillInterChangePattern.Process();

	GetPtrSentence()->FindPatrones(GetPtrTranslationData()->GetPtrInterchangePatternDic());
	}

/////////////////////////////////////////////////////////////////////////////
// inserta palabras dentro de la oración
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionInsertArticle()
	{
	CInsertOrAddWord InsertOrAddWord(GetTranslationUserPtr(),GetPtrSentence());

	InsertOrAddWord.Process();
	}
/////////////////////////////////////////////////////////////////////////////
// busca las palabras insertadas en el diccionario y llena sus datos
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionFillInsertWords()
	{
	CFillInsertedWord FillInsertedWord(GetTranslationUserPtr(),GetPtrSentence());

	FillInsertedWord.Process();
	}

/////////////////////////////////////////////////////////////////////////////
// concuerda en genero y número las palabras insertadas en la oración
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionGenderAndNumber()
	{
	CGenderAndNumberInsertedWord GenderAndNumberInsertedWord(GetTranslationUserPtr(),GetPtrSentence());

	GenderAndNumberInsertedWord.Process();
	}

/////////////////////////////////////////////////////////////////////////////
// Traducción de prefijos
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionTranslatePreffix()
	{
	CTranslatePreffix TranslatePreffix(GetTranslationUserPtr(),GetPtrSentence());

	TranslatePreffix.Process();
	}
/////////////////////////////////////////////////////////////////////////////
// conjuga los verbos que no pudieron ser conjugados mediante comodines
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionConjugate()
	{
	CConjugateVerbsInSentence ConjugateVerbsInSentence(GetTranslationUserPtr(),GetPtrSentence());

	ConjugateVerbsInSentence.Process();
	}
/////////////////////////////////////////////////////////////////////////////
// concuerda en genero y número las palabras que no pudieron ser traducidas mediante comodines
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionEnding()
	{
	CCoordinateWords CoordinateWords(GetTranslationUserPtr(),GetPtrSentence());

	CoordinateWords.Process();
	}
/////////////////////////////////////////////////////////////////////////////
// pone la terminación a los verbos refléxivos
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionTranslateReflexiveVerbs()
	{
	CReflexiveVerbInSentence objReflexiveVerbInSentence(GetTranslationUserPtr(),GetPtrSentence());

	objReflexiveVerbInSentence.Process();
	}

/////////////////////////////////////////////////////////////////////////////
// construye la oración traducida
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionMakeTranslatedSentence()
	{
	View(100);
	CGetAllTypeInSentence GetAllTypeInSentence(GetTranslationUserPtr(),GetPtrSentence());
	GetTranslationUserPtr()->SetFinalTypes(GetAllTypeInSentence.GetTypeList());

	CMakeTranslatedSentence objMakeTranslatedSentence(GetTranslationUserPtr(),GetPtrSentence());
	objMakeTranslatedSentence.Process();
	View(101);
	}
/////////////////////////////////////////////////////////////////////////////
// pone detalles del idioma tales como contracciones, etc. en la oración traducida
/////////////////////////////////////////////////////////////////////////////

void CTranslation::FunctionDetails()
	{
	CPutDetailsInSentence objPutDetailsInSentence(GetTranslationUserPtr(),GetPtrSentence());
	objPutDetailsInSentence.Process();
	View(100);

	if(GetPtrSentence()->m_AllUpper == TRUE)
		{
		GetPtrSentence()->m_Dest.MakeUpper();
    //_strupr( (LPSTR)(LPCSTR)GetPtrSentence()->m_Dest );

		GetPtrSentence()->m_Dest.Replace("\\B0","\\b0");
		GetPtrSentence()->m_Dest.Replace("\\FS","\\fs");
		GetPtrSentence()->m_Dest.Replace("\\F0 ","\\f0");
		}

  CPutGramaticalAccidentInTranslateSentence PutGramaticalAccidentInTranslateSentence(GetTranslationUserPtr(),GetPtrSentence());
  PutGramaticalAccidentInTranslateSentence.Process();
	View(101);
	}

