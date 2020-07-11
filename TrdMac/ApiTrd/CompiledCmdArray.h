#if !defined(_COMPILED_CMD_ARRAY__INCLUDED_)
#define _COMPILED_CMD_ARRAY__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ApiConj.h"
#include "GramType.h"
#include "DccDict.h"
#include "UtilFun.h"

#define _WORKING_WITH_DCC_ 1

#define NOT_FOUND_IN_ARRAY ((WORD)-1)


#ifdef LOAD_FROM_DCX
	#define USE_DCX TRUE
	#define INVALID_COMAND_TYPE 15
	#define INVALID_MAIN_POS    15
#else
	#define USE_DCX FALSE
	#define INVALID_COMAND_TYPE NOT_FOUND_IN_ARRAY
	#define INVALID_MAIN_POS    NOT_FOUND_IN_ARRAY
#endif




//////////////////////////////////////////////////////////////////////
// Nueva compilacion de Reglas
//////////////////////////////////////////////////////////////////////
// new
enum WordProperty
	{
	WP_Genero,					// genero de la palabra
	WP_NGenero,					// genero de la palabra
	WP_Numero,					// numero
	WP_NNumero,					// numero
	WP_Persona,					// persona
	WP_NPersona,					// persona
	WP_Grado,					// Grado del adjetivo
	WP_NGrado,					// Grado del adjetivo
	WP_Tiempo,					// Tiempo del verbo
	WP_NTiempo,					// Tiempo del verbo
	WP_Modo,						// Modo del verbo
	WP_NModo,					// Modo del verbo
	WP_AModo,					// Modo alternativo para ? del verbo
	WP_Caso,						// Caso de la palabra Mayusculas, minusculas, primera mayusculas, etc
	WP_NCaso,					// Caso de la palabra Mayusculas, minusculas, primera mayusculas, etc
	WP_Declination,				// Declinacion de sust y adj
	WP_NDeclination,				// Declinacion de sust y adj
	WP_ArticleType,				// Type de articulo o palabra a insertar // no se usa mucho
	WP_NArticleType,				// Type de articulo o palabra a insertar // no se usa mucho
	WP_ToPhrase,					// si forma parte de la phrase o no
	WP_Negative,					// si esta en negativo
	WP_NNegative,				// si esta en negativo
	WP_Delete,					// si se borra
	WP_Translate,				// si se traduce o se deja como esta el original
	WP_Reflexive,				// si esta en reflexivo
	WP_NReflexive,				// si esta en reflexivo
	WP_Complex,					// si es un tiempo complejo
	WP_FindInDic,				// si se busca en el diccionario
	WP_InsertNum,				// Palabra a insertar cuando es numero
	WP_InsertArrayIndex,			// Palabra a insertar cuando es string
	WP_GrammarTypeArrayIndex,	// indice en el array de GrammarType
	WP_TranslationArrayIndex,	// indice en el array de Traducciones
	WP_CategoryArrayIndex,		// indice en el array de Categorias
	WP_AddKeyArrayIndex,			// indice en el array de Palabras a Insertar en la phrase
	WP_AddDataArrayIndex,		// indice en el array de AddData
	WP_NewKeyArrayIndex,			// indice en el array de nueva Key para la palabra
	WP_NewPos,					// nueva posicion de la palabra en la frase
	WP_AddBack,					// indice en el array de nueva Key adicionar detras de la palabra
	WP_AddBackArrayIndex,		// indice en el array de AddData
	};

//////////////////////////////////////////////////////////////////////
// Datos de cada palabra por los que se puede preguntar en el match
//////////////////////////////////////////////////////////////////////
enum MatchElement
  {
  eKwrd,
  eType,
  eWord,
  eFUpper,
  eAUpper,
  eFind,
  eSuffix,
  ePreffix,
  eCmdType,
  eWildCard,
  eField,
  eNull
  };

template<class ARRAY>
BOOL ArrayEquals(const ARRAY& array1, const ARRAY& array2)
	{
	if (array1.GetCount() != array2.GetCount())
		return FALSE;

	for(int i=0; i<array1.GetCount(); i++)
		if (!array1[i].IsEqual(array2[i]))
			return FALSE;

	return TRUE;
	}


//////////////////////////////////////////////////////////////////////
// Structura para almacenar cada elemento del match
//////////////////////////////////////////////////////////////////////
// significado de los bits de cada WORD de los MATCH 
//	0...10   : indice   (11 bits, 2048 valores)
//	11..14   : operador (4  bits, 16 valores)
//	15       : and      (1  bit,  2 valores)
#define MATCH_INDEX_BITS 11
#define MATCH_OPE_BITS   4
#define MATCH_AND_BITS   1

#define MATCH_OPE_SHIFT  MATCH_INDEX_BITS
#define MATCH_AND_SHIFT  (MATCH_INDEX_BITS + MATCH_OPE_BITS)

// maximo valor que puede tomar el indice
#define MAX_MATCH_INDEX  ((1 << MATCH_INDEX_BITS) - 1)
// maximo valor que puede tomar el operador
#define MAX_MATCH_OPE    ((1 << MATCH_OPE_BITS) - 1)

// mascara para extraer el indice
#define MATCH_INDEX_MASK MAX_MATCH_INDEX
// mascara para extraer el operador
#define MATCH_OPE_MASK   (MAX_MATCH_OPE << MATCH_OPE_SHIFT)
// mascara para extraer el and
#define MATCH_AND_MASK   (1 << MATCH_AND_SHIFT)


class CItemKeyWord
  {
  private:
  
#ifdef LOAD_FROM_DCX
	  WORD m_OpAndIndex/*[2]*/;
#else
  	WORD m_Dummy;								// para poder hacer el serialize en una sola lectura

		// Dato primario a preguntar solo puede ser miembro del enum MatchElement
    // es o eKwrd, eType, etc
	  BYTE m_Match[2];
	// Indice del parametro string en el CStrArray del Match segun su tipo
	  WORD m_IndexArrayMatch[2];
#endif


  public:

  CItemKeyWord()
    {
    }

#ifdef LOAD_FROM_DCX
	inline BYTE getOp(int /*i*/) const
		{
	  return (m_OpAndIndex & MATCH_OPE_MASK) >> MATCH_OPE_SHIFT;
		}
#else
	inline BYTE getOp(int i) const
		{
		return m_Match[i];
		}
#endif

#ifdef LOAD_FROM_DCX
	inline WORD getIndex(int /*i*/) const
		{
	  return m_OpAndIndex & MATCH_INDEX_MASK;
		}
#else
	inline WORD getIndex(int i) const
		{
		return m_IndexArrayMatch[i];
		}
#endif

	// retorna TRUE si el proximo match corresponde a esta misma palabra o FALSE si corresponde
	// a la proxima palabra
	inline BOOL isAnd() const
		{
#ifdef LOAD_FROM_DCX
		return m_OpAndIndex >> MATCH_AND_SHIFT;
#else
		return m_Match[1] != 0xcd && m_Match[1] != eNull;
#endif
		}

#ifdef ACTIVE_SAVE
	void Dump()
		{
		TRACE("    match: %d,%d    indexs: %d,%d",m_Match[0],m_Match[1],m_IndexArrayMatch[0],m_IndexArrayMatch[1]);
		}

	BOOL IsEqual(const CItemKeyWord& aItemKeyWord) const
		{
		return	(m_Match[0] == aItemKeyWord.m_Match[0]) &&
						(m_Match[1] == aItemKeyWord.m_Match[1]) &&
						(m_IndexArrayMatch[0] == aItemKeyWord.m_IndexArrayMatch[0]) &&
						(m_IndexArrayMatch[1] == aItemKeyWord.m_IndexArrayMatch[1]);
		}

	void Save(CArchive &ar) const
		{
		//COUNTER("NumMatchItem"); // 77167 veces = 463002 bytes

		if (m_IndexArrayMatch[0] != 0xcdcd && m_IndexArrayMatch[0] > MAX_MATCH_INDEX)
			CRASH;
		if (m_IndexArrayMatch[1] != 0xcdcd && m_IndexArrayMatch[1] > MAX_MATCH_INDEX)
			CRASH;

		if (m_IndexArrayMatch[0] != 0xcdcd)
			COLLECT("m_IndexArrayMatch", m_IndexArrayMatch[0]);
		if (m_IndexArrayMatch[1] != 0xcdcd)
			COLLECT("m_IndexArrayMatch", m_IndexArrayMatch[1]);

		WORD w = m_Match[0] << MATCH_OPE_SHIFT;
		if (m_IndexArrayMatch[0] != 0xcdcd)
			w |= m_IndexArrayMatch[0];
		if (isAnd())
			w |= MATCH_AND_MASK;

		ar << w;

		if (isAnd())
			{ // hay un AND, escribir la otra condicion
			WORD w = m_Match[1] << MATCH_OPE_SHIFT;
			if (m_IndexArrayMatch[1] != 0xcdcd)
				w |= m_IndexArrayMatch[1];
			ar << w;
			}

		/*ar << m_Match[0];
		ar << m_Match[1];
		ar << m_IndexArrayMatch[0];
		ar << m_IndexArrayMatch[1];*/
		}

#endif
  };

//////////////////////////////////////////////////////////////////////
// Structura para almacenar cada elemento del action o del phrase
//////////////////////////////////////////////////////////////////////
//#define PACK_ACTION

// el private es para asegurar que no se accedan a los metodos de CFastWordArray
//class CActionPhrase: private CFastWordArray
class CActionPhrase: public CFastWordArray
  {
public:
	class Iterator
		{
		private:

		friend class CActionPhrase;

		WORD* m_Property;
		WORD* m_Last;

		Iterator(const CActionPhrase* aList)
			{
			m_Property = aList->GetBuffer() + 1;
			m_Last = aList->GetBuffer() + aList->GetCount();
			}

		public:
			inline BOOL isValid()
				{
				return m_Property < m_Last;
				}

			inline void next()
				{
				m_Property += 2;
				}

			inline WORD getProperty()
				{
#ifdef PACK_ACTION
				return (*m_Property) >> 10;
#else
				return *m_Property;
#endif
				}

			inline WORD getValue()
				{
#ifdef PACK_ACTION
				return (*m_Property) & 0x03FF;
#else
				return *(m_Property+1);
#endif
				}
		};

	CActionPhrase() { }

	inline Iterator begin() const
		{
		return Iterator(this);
		}

	inline WORD GetPos() const
		{
#ifdef PACK_ACTION
		return *pData & 0x00FF;
#else
		return (GetSize() > 0) ? GetAt(0) : NOT_FOUND_IN_ARRAY;
#endif
		}
	
	WORD GetProperty(WordProperty wproperty) const
		{
		for(Iterator iter = begin(); iter.isValid(); iter.next())
			if (iter.getProperty() == wproperty)
				return iter.getValue();

		/*for(int i = 1; i < GetSize(); i+=2)
			{
			if(wproperty == GetAt(i))
				return GetAt(i+1);
			}*/

		return NOT_FOUND_IN_ARRAY;
		}

#ifdef ACTIVE_SAVE
	inline void Save(CArchive &ar) const
		{
/*
Nombre                        Minimo     Maximo   Promedio      Total Cantidad
m_IndexArrayMatch               0.00    1200.00     105.05 8138153.00    77466
GetSize                         3.00      19.00       4.35  258267.00    59377
GetPos                          0.00      17.00       2.58  152903.00    59377
Comando                         0.00      36.00      19.89 1977916.00    99445
Valor                           0.00     961.00      19.77 1966091.00    99445
los comandos llegan hasta 38, es decir 6 bits (hasta 0x3F, 64 valores), en un WORD
quedarian 10 bits que da para otra variable de hasta 1024 valores (0x3FF), justo lo necesario
para Valor

tamaño de todos los arrays: 258267 WORDs = 516534 bytes
hay 59377 size de cada array, son 118754 bytes
se puede combinar el size y el GetPos de cada array en un WORD y asi mantener la alineacion
a word
*/

		/*COLLECT("GetSize", GetSize());
		COLLECT("GetPos", GetAt(0));
		for(int i=1; i<GetSize(); i++)
			{
			if (i & 1)
				COLLECT("Comando", GetAt(i));
			else
				COLLECT("Valor", GetAt(i));
			}*/

		//CFastWordArray::Save(ar);
		ar.WriteCount(GetCount());
		ar.WriteBuffer((LPVOID)GetBuffer(), GetCount()*sizeof(GetAt(0)));

		}
#endif

#ifdef _WINDOWS_
	void Dump() const
		{
		CStringA s;
		for(int i=0; i<GetCount(); i++)
			{
			CString t;
			t.Format(", %d", (*this)[i]);
			s += t;
			}
			
		TRACE("    %s", s.c_str());
		}

	inline BOOL operator==(const CActionPhrase& aAction) const
		{ return IsEqual(aAction); };

	BOOL IsEqual(const CActionPhrase& aActionPhrase) const
		{
		if (GetCount() != aActionPhrase.GetCount())
			return FALSE;

		for(int i=0; i<GetCount(); i++)
			if ((*this)[i] != aActionPhrase[i])
				return FALSE;

		return TRUE;
		}
#endif

	template<class ARCHIVE>	
	void Load(ARCHIVE &ar)
		{
		CFastWordArray::Load(ar);
    if (GetSize() == 0)
    	{
    	//Add(NOT_FOUND_IN_ARRAY);
    	SetSize(1);
    	SetAt(0, NOT_FOUND_IN_ARRAY);
    	}
		}

	inline void SerializeMem(CArchiveMem &ar)
		{
		Load(ar);
		}
  };

#ifdef LOAD_FROM_DCX
class CActionPhraseArray : public CFastWordArray //CFastArrayValue<WORD> 
	{
	};
#else
class CActionPhraseArray : public CFastArray<CActionPhrase>
	{
#ifdef ACTIVE_SAVE
	public:

	void Save(CArchive &ar, CArray<CActionPhrase>& arrWords) const
		{
		//CFastArray<CActionPhrase>::Save(ar);
		ar.WriteCount(GetCount());
		for(int i=0; i<GetCount(); i++)
			{
			int idx = arrWords.AddUnique(GetAt(i));
			ASSERT(idx >= 0 && idx < 65535);
			ar << (WORD)idx;
			}
		}
#endif
	};
#endif

//typedef CFastArray<CActionPhrase> CActionPhraseArray;

//////////////////////////////////////////////////////////////////////
// Structura para almacenar cada item de un Match
//////////////////////////////////////////////////////////////////////

class CMatchItem
  {
  private:
	// puede sustituirse por un CWord Array cada 4
#ifdef LOAD_FROM_DCX
		BYTE    m_RealKeyWordCount; // Cantidad real de items en m_ItemKeyword (el GetCount puede dar mas debido a los And)
    BYTE	  m_PosMain_Cmd;			// Posición donde se encuentra la palabra central de la frase
#else
    WORD	  m_PosMainWord;			// Posición donde se encuentra la palabra central de la frase
    WORD	  m_CmdType;				  // type en los comodines tipo CMD
#endif

  public:
    CFastArrayValue<CItemKeyWord>  m_ItemKeyword;			// Pregunta Array de array de CItemKeyWord
    CActionPhraseArray m_Action;					// Accion de tipo   Array de CActionPhrase
    CActionPhraseArray m_Translation;			// Accion de translation  Array de CActionPhrase
    CActionPhraseArray m_Execute;				  // Accion de Execute  Array de CActionPhrase

  public:

    //DECLARE_SERIAL(CMatchItem);

    CMatchItem()
      {
      }
    
    /*~CMatchItem()
      {
			// Como todos estos son FastArray no hay que llamar a los destructores
      m_ItemKeyword.RemoveAll();
      m_Action.RemoveAll(FALSE);
      m_Translation.RemoveAll(FALSE);
      m_Execute.RemoveAll(FALSE);
      }*/

	BYTE GetKeyWordCount() const
		{
#ifdef LOAD_FROM_DCX
		return m_RealKeyWordCount;
#else
		return m_ItemKeyword.GetCount();
#endif
		}

#ifdef ACTIVE_SAVE

	void Dump() const
		{
		TRACE("MatchItem");			
		TRACE("  m_PosMainWord : %d, m_CmdType : %d", m_PosMainWord, m_CmdType);
		CStringA sMatch;
		for(int i=0; i<m_ItemKeyword.GetCount(); i++)
			{
			//m_ItemKeyword[i].Dump();
			CStringA s;
			s.Format("%d,%d, ", m_ItemKeyword[i].getOp(0), m_ItemKeyword[i].getIndex(0));
			sMatch += s;
			if (m_ItemKeyword[i].getOp(1) != 0xcd)
				{
				s.Format("%d,%d, ", m_ItemKeyword[i].getOp(1), m_ItemKeyword[i].getIndex(1));
				sMatch += s;
				}
			}
		TRACE("  match : %s", sMatch.c_str());

		TRACE("  m_Action");
		for(int i=0; i<m_Action.GetCount(); i++)
			m_Action[i].Dump();

		TRACE("  m_Translation");
		for(int i=0; i<m_Translation.GetCount(); i++)
			m_Translation[i].Dump();

		TRACE("  m_Execute");
		for(int i=0; i<m_Execute.GetCount(); i++)
			m_Execute[i].Dump();
		}

		BOOL IsEqual(const CMatchItem& aMatchItem) const
			{
			if ((m_PosMainWord != aMatchItem.m_PosMainWord) ||
				(m_CmdType != aMatchItem.m_CmdType))
				return FALSE;

			if (!ArrayEquals(m_ItemKeyword, aMatchItem.m_ItemKeyword))
				return FALSE;
			if (!ArrayEquals(m_Action, aMatchItem.m_Action))
				return FALSE;
			if (!ArrayEquals(m_Translation, aMatchItem.m_Translation))
				return FALSE;
			if (!ArrayEquals(m_Execute, aMatchItem.m_Execute))
				return FALSE;

			return TRUE;
			}
#endif

#ifdef LOAD_FROM_DCX
    void Load(CArchiveMem &ar)
      {
			ar >> m_RealKeyWordCount;
			BYTE nCount;
			ar >> nCount;
			m_ItemKeyword.SetSize(nCount, FALSE);
			ar.ReadBuffer(m_ItemKeyword.GetBuffer(), nCount*sizeof(m_ItemKeyword.GetAt(0)));

      //m_ItemKeyword.LoadBurst(ar);

      m_Action.Load(ar);
      m_Translation.Load(ar);
      m_Execute.Load(ar);

      ar >> m_PosMain_Cmd;
      //ar >> m_PosMainWord;
      //ar >> m_CmdType;
			}

    inline WORD GetMainWordPos() const
      {return m_PosMain_Cmd >> 4;}

		inline WORD GetCmdType() const
      {return m_PosMain_Cmd & 0x0F;}

#else
    inline WORD GetMainWordPos() const
      {return m_PosMainWord;}

		inline WORD GetCmdType() const
      {return m_CmdType;}
#endif

    void SerializeMem(CArchiveMem &ar)
      {
#ifdef LOAD_FROM_DCX
			Load(ar);
#else
      m_ItemKeyword.SerializeAsObArrayMem(ar);
      m_Action.SerializeAsObArrayMem(ar);
      m_Translation.SerializeAsObArrayMem(ar);
      m_Execute.SerializeAsObArrayMem(ar);

      ar >> m_PosMainWord;
      ar >> m_CmdType;
#endif
      }

#ifdef ACTIVE_SAVE
		void Save(CArchive &ar, CArray<CActionPhrase>& arrWords) const
			{
			// determinar la cantidad real de m_ItemKeyword que se salvaran, pues cada 
			// m_ItemKeyword puede producir hasta dos de ellos si tiene un and
			BYTE nCount = m_ItemKeyword.GetCount();
			for(int i=0; i<m_ItemKeyword.GetCount(); i++)
				{
				if (m_ItemKeyword.GetAt(i).isAnd())
					nCount++;
				}

			COLLECT("nCount",nCount); // de 0 a 17
			// primero se escribe la cantidad de item escritos y despues la cantidad real
			ar << (BYTE)m_ItemKeyword.GetCount();
			ar << nCount;
			for(int i=0; i<m_ItemKeyword.GetCount(); i++)
				m_ItemKeyword.GetAt(i).Save(ar);
			//m_ItemKeyword.Save(ar);


      m_Action.Save(ar, arrWords);
      m_Translation.Save(ar, arrWords);
      m_Execute.Save(ar, arrWords);

			//COLLECT("m_PosMainWord",m_PosMainWord); // de 0 a 8
			//COLLECT("m_CmdType",m_CmdType); // de 0 a 6
			if (m_PosMainWord != NOT_FOUND_IN_ARRAY && m_PosMainWord >= 15)
				CRASH;
			if (m_CmdType != NOT_FOUND_IN_ARRAY && m_CmdType >= 15)
				CRASH;

			// mezclar m_PosMainWord y m_CmdType en un solo byte
			BYTE b = ((BYTE)m_PosMainWord << 4) | ((BYTE)m_CmdType & 0x0F);
      ar << b;
			}
#endif
  };


////////////////////////////////////////////////////////////////////////////////////////
// sirve para almacenar las cadenas de Type, Word como etc con su indice,
// tambien puede almacenar para una palabra o un tipo todos los indices 
// de las cadenas donde se encuentra
////////////////////////////////////////////////////////////////////////////////////////

#ifndef LOAD_FROM_DCX
class CStringWordArrayEntry: public CMFCObject
  {
  public:
		//CStringA 	m_KeyWord;			// cadena para indice
   // CFastWordArray	m_WArray;			// arreglo con el o los indices que tiene la cadena
    //WORD	    m_Word;			    // indice que tiene la cadena

  public:

    DECLARE_SERIAL(CStringWordArrayEntry);

    CStringWordArrayEntry()
      {
      //m_Word = NOT_FOUND_IN_ARRAY;
      }
    
    virtual void Serialize(CArchive &ar)
      {
      if(ar.IsLoading() == TRUE)
        {
		    CStringA 	m_KeyWord;			// cadena para indice
        ar >> m_KeyWord;
				WORD m_Word;
        ar >> m_Word;
        }

	    CFastWordArray	m_WArray;			// arreglo con el o los indices que tiene la cadena
      m_WArray.Serialize(ar);
      }

    virtual void SerializeMem(CArchiveMem &ar)
      {
      if(ar.IsLoading() == TRUE)
        {
		    CStringA 	m_KeyWord;			// cadena para indice
        ar >> m_KeyWord;
				WORD m_Word;
        ar >> m_Word;
        }

	    CFastWordArray	m_WArray;			// arreglo con el o los indices que tiene la cadena
      m_WArray.SerializeMem(ar);
      }
  };
#endif

class CKeyWordEntry
  {
  public:
  // Esta funcion es la entrada como tal en el dcc
#ifdef ACTIVE_SAVE
    CArray<CMatchItem>	m_MatchArray;			// arreglo de CMatchItem MATCH que existen para ese KEYWORD
#else
    CFastArray<CMatchItem>	m_MatchArray;			// arreglo de CMatchItem MATCH que existen para ese KEYWORD

#endif

    CArray<CGramTypeSet>  m_MatchTypeTest;
		int m_TestMainWordPos;

    // el arreglo esta por orden de ejecución
  public:

    CKeyWordEntry()
      {
      }

    CKeyWordEntry(const CStringA& aData)
      {
			m_TestMainWordPos = -1;
			CArchiveMem arMem(aData);
			SerializeMem(arMem);
      }

    /* no hay que hacer nada en el destructor porque usa FastArray
		~CKeyWordEntry()
      {
      //m_MatchArray.RemoveAll(FALSE);
      }*/
    
#ifdef ACTIVE_SAVE

		void Dump()
			{
			TRACE("*********** KeyWordEntry ***********");
			for(int i=0; i<m_MatchArray.GetCount(); i++)
				m_MatchArray[i].Dump();
			TRACE("************************************");
			}

		BOOL IsEqual(const CKeyWordEntry& aKeyWordEntry) const
			{
			return ArrayEquals(m_MatchArray, aKeyWordEntry.m_MatchArray);
			}

#endif

  public:
		/*int GetTestMainWordPos()
			{
			return m_TestMainWordPos;
			}

		void CreateMatchTest(const CArrayValue<CGramTypeSet>& arrTypes)
			{
			//COLLECT_INTERVAL("CreateMatchTest");

			// detectar el intervalo a testear
			int MinMainWordPos = 10000;
			int MinLenAfterMainWord = 10000;
			for(int i=0; i<m_MatchArray.GetCount(); i++)
				{
				WORD pos = m_MatchArray[i].GetMainWordPos();
				MinMainWordPos = Min(MinMainWordPos, pos);

				WORD p = m_MatchArray[i].GetKeyWordCount() - pos;
				MinLenAfterMainWord = Min(MinLenAfterMainWord, p);
				}
			//ASSERT(MaxMainWordPos >= 0 && MaxLenAfterMainWord >= 0);

			m_TestMainWordPos = MinMainWordPos;

			if (MinMainWordPos + MinLenAfterMainWord == 1)
				{
				//COUNTER("cond1");
				m_TestMainWordPos = -2; // no chequear pues esta full
				return;
				}

			m_MatchTypeTest.SetSize(MinMainWordPos + MinLenAfterMainWord);
			//for(int i=0; i<m_MatchTypeTest.GetCount(); i++)
			//	m_MatchTypeTest[i].Clear();

			for(int i=0; i<m_MatchArray.GetCount(); i++)
				{
				const CFastArrayValue<CItemKeyWord>* keyWords = &m_MatchArray.GetAt(i).m_ItemKeyword;
				int idxWord = m_TestMainWordPos - m_MatchArray.GetAt(i).GetMainWordPos();
				for(int j=0; j<keyWords->GetCount(); j++)
					{
					if (idxWord >= 0 && idxWord < m_MatchTypeTest.GetCount())
						{
						if (keyWords->GetAt(j).getOp(0) == eType)
							m_MatchTypeTest[idxWord].Add(arrTypes[keyWords->GetAt(j).getIndex(0)]);
						else
							m_MatchTypeTest[idxWord].SetAll();
						}

					if (!keyWords->GetAt(j).isAnd())
						idxWord++;
					}
				
				}

			// si alguno quedo vacio es porque permite todos los tipos
			for(int i=0; i<m_MatchTypeTest.GetCount(); i++)
				if (m_MatchTypeTest[i].IsEmpty())
					m_MatchTypeTest[i].SetAll();

			bool bIsFull = true;
			for(int i=0; i<m_MatchTypeTest.GetCount(); i++)
				if (!m_MatchTypeTest[i].IsFull())
					{
					bIsFull = false;
					break;
					}

			if (bIsFull)
				m_TestMainWordPos = -2; // no chequear pues esta full
			}*/

    void SerializeMem(CArchiveMem &ar)
      {
#ifdef LOAD_FROM_DCX
			m_MatchArray.Load(ar);
#else
			// para cargar de los dcc
			CStringWordArrayEntry stringWordArray;
      stringWordArray.SerializeMem(ar);

			m_MatchArray.SerializeAsObArrayMem(ar);
#endif
      }

#if defined(ACTIVE_SAVE)
		// Elimina los MatchItem duplicados
		int RemoveDuplicates()
			{
			int n = 0;

			for(int i=0; i<m_MatchArray.GetCount(); i++)
				{
				for(int j=i+1; j<m_MatchArray.GetCount(); j++)
					if (m_MatchArray[i].IsEqual(m_MatchArray[j]))
						{
						m_MatchArray.RemoveAt(j);
						j--;
						n++;
						}

				}

			return n;
			}

    void Save(CArchive &ar, CArray<CActionPhrase>& arrWords) const
			{
			//m_arrItemKeyWord.Save(ar);

			m_MatchArray.Save(ar, arrWords);
			}
#endif

  };

/////////////////////////////////////////////////////////////////////////////////
// class que contiene un arreglo fijo y otro ordenado
/////////////////////////////////////////////////////////////////////////////////
class CFixedAndOrderStrArray//: public CMFCObject
  {
  public:
#ifdef LOAD_FROM_DCX
    CFastStrArray m_FixedArray;	// arreglo donde se almacenan los elementos sin ordenar
#else
    CStrArray  m_FixedArray;	// arreglo donde se almacenan los elementos sin ordenar
#endif
    //CObOrderArray m_OrderArray;	// arreglo de CStringWordArrayEntry donde se almacenan los elementos ordenados

    //DECLARE_SERIAL(CFixedAndOrderStrArray);

    /*void Inicialize()
      {
      //m_OrderArray.Inicialize();
      m_FixedArray.RemoveAll();
      }*/

    virtual void Serialize(CArchive &ar)
      {
#ifndef LOAD_FROM_DCX
			if (ar.IsLoading())
				{
				// solo para compatibilidad con los .dat viejos
				CArray<CStringWordArrayEntry> m_OrderArray;	// arreglo de CStringWordArrayEntry donde se almacenan los elementos ordenados
				m_OrderArray.SerializeAsObArray(ar);
				m_OrderArray.DeleteAll();
				}

			if (ar.IsLoading())
				m_FixedArray.Serialize(ar);
			else
				{
#ifdef ACTIVE_SAVE
				CSmartStrArray arrStrings;
				arrStrings.Copy(m_FixedArray);

				arrStrings.Save(ar);
#endif
				}
#else
			if (ar.IsLoading())
				{
				m_FixedArray.Load(ar);
				}
#endif
      }
  };


//////////////////////////////////////////////////////////////////////
// Clase principal de datos donde se almacenan la reglas de traduccion
// compiladas
//////////////////////////////////////////////////////////////////////

class CCompiledRuleTable : public CMFCObject
  {
  public:
    // Arreglos donde almacenar cada una de las cadenas de
#ifndef LOAD_FROM_DCX
    CFixedAndOrderStrArray m_aKwrds;			    // Todas las cadenas que van al lado de kwrd
    CFixedAndOrderStrArray m_aFUpper;			    // Todas las cadenas que van al lado de FUPR
    CFixedAndOrderStrArray m_aAUpper;			    // Todas las cadenas que van al lado de AUPR
    CFixedAndOrderStrArray m_aCmdTypes;			  // Todas las cadenas que van al lado de CMDTYPE
    CFixedAndOrderStrArray m_aWildCards;		  // Todas las cadenas que van al lado de WILDCARD
    CFixedAndOrderStrArray m_aFieldDefines;		// para guardar el #define asociado
    CFixedAndOrderStrArray m_aAddBack;			  // Todas las cadenas que van al lado de AddBack (adiciona detrás)
    CFixedAndOrderStrArray m_aDefine;			    // para guardar el #define asociado
#endif
    CFixedAndOrderStrArray m_aTypes;			    // Todas las cadenas que van al lado de Type
    CFixedAndOrderStrArray m_aWords;			    // Todas las cadenas que van al lado de Word
    CFixedAndOrderStrArray m_aSuffixes;			  // Todas las cadenas que van al lado de SUFIX
    CFixedAndOrderStrArray m_aPreffixes;		  // Todas las cadenas que van al lado de PREFIX
    CFixedAndOrderStrArray m_aFinds;			    // Todas las cadenas que van al lado de FIND
    CFixedAndOrderStrArray m_aFields;			    // Todas las cadenas que van al lado de FIELD
    CFixedAndOrderStrArray m_aTranslations;		// Todas las cadenas que van al lado de Translation
    CFixedAndOrderStrArray m_aGrammarType;		// Todas las cadenas que van al lado de GrammarType
    CFixedAndOrderStrArray m_aInsert;			    // Todas las cadenas que van al lado de Insert	(Inserta delante)
    CFixedAndOrderStrArray m_aCategory;			  // Todas las cadenas que van al lado de Category
    CFixedAndOrderStrArray m_aAddKey;			    // Todas las cadenas que van al lado de AddKey   (Inserta delante)
    CFixedAndOrderStrArray m_aAddData;			  // Todas las cadenas que van al lado de AddData
    CFixedAndOrderStrArray m_aCmdType;			  // Todas las cadenas que van al lado de CmdType
    CFixedAndOrderStrArray m_aKey;				    // Todas las cadenas que van al lado de Key (Cambia el Key)

		CArrayValue<CGramTypeSet> m_aTypesT;

#ifdef LOAD_FROM_DCX
		CArray<CActionPhrase>  m_Actions;
#endif

		CStringA				  m_FnameDccTable;

#if defined(LOAD_FROM_DCX)
		CDccDict m_DccTable;		// tabla con los datos guardados en un dcc mapped file
#else
		//CSimpleDict<CKeyWordEntry> m_DccTable;		// tabla con los datos guardados en un dcc mapped file
		CDict m_DccTable;		// tabla con los datos guardados en un dcc mapped file
#endif

  public:
    DECLARE_SERIAL(CCompiledRuleTable);

    CCompiledRuleTable():m_DccTable("DccDic")
      {
      //Inicialize();
      }

    ~CCompiledRuleTable()
      {
      m_DccTable.Close();
      }
      
    /*void Inicialize()
      {
      m_LastKeyNumber = NOT_FOUND_IN_ARRAY;
      m_LastWordNumber = NOT_FOUND_IN_ARRAY;
      m_LastTypeNumber = NOT_FOUND_IN_ARRAY;
      }*/

    virtual void Serialize(CArchive &ar)
      {
			COLLECT_INTERVAL("Load-CCompiledRuleTable");
      //if (ar.IsStoring() || USE_DCX)
			//	TRACE("-------- New Comodin -----------");
#ifndef LOAD_FROM_DCX
      if (ar.IsLoading())
				m_aKwrds.Serialize(ar);			
#endif

      m_aTypes.Serialize(ar);			
      m_aWords.Serialize(ar);			
#ifndef LOAD_FROM_DCX
      if (ar.IsLoading())
	      m_aFUpper.Serialize(ar);			
      if (ar.IsLoading())
	      m_aAUpper.Serialize(ar);			
#endif
      m_aSuffixes.Serialize(ar);		
      m_aPreffixes.Serialize(ar);		
      m_aFinds.Serialize(ar);			
#ifndef LOAD_FROM_DCX
      if (ar.IsLoading())
	      m_aCmdTypes.Serialize(ar);		
      if (ar.IsLoading())
	      m_aWildCards.Serialize(ar);		
#endif
      m_aFields.Serialize(ar);			
#ifndef LOAD_FROM_DCX
      if (ar.IsLoading())
	      m_aFieldDefines.Serialize(ar);	
#endif
      m_aTranslations.Serialize(ar);	
      m_aGrammarType.Serialize(ar);		
      m_aInsert.Serialize(ar);			
      m_aCategory.Serialize(ar);		
      m_aAddKey.Serialize(ar);
      m_aAddData.Serialize(ar);
      m_aCmdType.Serialize(ar);
      m_aKey.Serialize(ar);
#ifndef LOAD_FROM_DCX
      if (ar.IsLoading())
	      m_aAddBack.Serialize(ar);
      if (ar.IsLoading())
	      m_aDefine.Serialize(ar);
#endif

      if (ar.IsLoading())
        {
        ar >> m_FnameDccTable;
#ifndef LOAD_FROM_DCX
				TRACE("Loading %s", m_FnameDccTable.c_str());
#endif

#ifdef LOAD_FROM_DCX
				{
				COLLECT_INTERVAL("LoadActions"); // 0.10
				m_Actions.Load(ar);
				}
#endif

#if defined(ACTIVE_SAVE) && defined(REPORT_ERRORS)
				// chequear la existencia de palabras que podrian ser tipos
				for(int i=0; i<m_aWords.m_FixedArray.GetCount(); i++)
					{
					CStrArray arrWords;
					arrWords.Split(m_aWords.m_FixedArray[i], ',');
					for(int j=0; j<arrWords.GetCount(); j++)
						{
						if (arrWords[j].GetLength() == 2)
							{
							char c1 = arrWords[j][0];
							char c2 = arrWords[j][1];
							if (c1 >= 'A' && c1 <= 'Z' && c2 >= 'A' && c2 <= 'Z')
								{
								TRACE("possible bad words: %s", m_aWords.m_FixedArray[i].c_str());
								break;
								}
							}
						}
					}


				// ordenar las palabras para hacer la busqueda mas eficiente
				for(int i=0; i<m_aWords.m_FixedArray.GetCount(); i++)
					{
					CStrArray arrWords;
					arrWords.Split(m_aWords.m_FixedArray[i], ',');
					arrWords.Sort();
					m_aWords.m_FixedArray[i] = arrWords.Join(',');
					}
#endif

				m_aTypesT.SetSize(m_aTypes.m_FixedArray.GetCount(), FALSE);
				for(int i=0; i<m_aTypes.m_FixedArray.GetCount(); i++)
					{
#ifdef LOAD_FROM_DCX
					CGramTypeSet TypeSet(m_aTypes.m_FixedArray[i]);
#else
					CGramTypeSet TypeSet(m_aTypes.m_FixedArray[i].c_str());
#endif
					m_aTypesT[i] = TypeSet;
					}
				
				/*for(int i=0; i<m_aGrammarType.m_FixedArray.GetSize(); i++)
					{
					CGramTypeSet TypeSet(m_aGrammarType.m_FixedArray[i].c_str());
					}*/


				// 2009/10/23 Sailer: no cargar este diccionario que podria estar corrupto
				if (m_FnameDccTable != "FrIGrFun.dcc")
					{
					//COLLECT_INTERVAL("LoadOpt");
					if (!m_DccTable.Load(StrGetDir(ar.m_strFileName) + m_FnameDccTable))
						{
						ar.SetError();
						return;
						}

#ifndef LOAD_FROM_DCX
					m_DccTable.SetNoHide(TRUE);
#endif
					}

#ifndef LOAD_FROM_DCX
				WORD m_LastKeyNumber;						          // ultimo numero que se le dio a la key
				WORD m_LastWordNumber;						        // ultimo numero que se le dio a la word
				WORD m_LastTypeNumber;						        // ultimo numero que se le dio al type

        ar >> m_LastKeyNumber;
        ar >> m_LastWordNumber;
        ar >> m_LastTypeNumber;
#endif
        }
			else
				{
#ifdef ACTIVE_SAVE
				CArray<CActionPhrase>  arrActions;
				// 2009/10/23 Sailer: no salvar este diccionario que podria estar corrupto
				if (m_FnameDccTable != "FrIGrFun.dcc")
					m_DccTable.SaveDCX(arrActions);

			  ar << m_FnameDccTable;

				arrActions.Save(ar);

				/*WORD m_LastKeyNumber;						          // ultimo numero que se le dio a la key
				WORD m_LastWordNumber;						        // ultimo numero que se le dio a la word
				WORD m_LastTypeNumber;						        // ultimo numero que se le dio al type

        ar << m_LastKeyNumber;
        ar << m_LastWordNumber;
        ar << m_LastTypeNumber;*/

#endif
				}
      }

  CKeyWordEntry *FindKeyWordEntryInDcc(const CStringA& ptrStr)
		{
	  CKeyWordEntry *ptrKeyWordEntry = NULL;

#if defined(LOAD_FROM_DCX)
		//TRACE(m_DccTable.GetFileName().c_str());
		if (m_DccTable.GetKey(ptrStr, ptrKeyWordEntry))
			{
			//if (ptrKeyWordEntry->GetTestMainWordPos() == -1)
			//	ptrKeyWordEntry->CreateMatchTest(m_aTypesT);
			}

#else
		CStringA sData;
		if (m_DccTable.GetKey(ptrStr, sData))
			ptrKeyWordEntry = new CKeyWordEntry(sData);
#endif

		return ptrKeyWordEntry;
		}

  };
//////////////////////////////////////////////////////////////////////

#endif // !defined(_COMPILED_CMD_ARRAY__INCLUDED_)
