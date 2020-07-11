/////////////////////////////////////////////////////////////////////
// Clase para trabajar con un comodín en conjunto 
//	"llave","dato función","dato traducción";
/////////////////////////////////////////////////////////////////////
#if !defined(_CCOMODIN_)
#define _CCOMODIN_
#include "StepInSentence.h"
#include "CompiledCmdArray.h"
#include "CmdItKey.h"

class CComodin : public CStepInSentence
	{
	private:
		POSITION			m_ptrNodo;
		LPCWORD				m_ptrWord;
		LPCWORD				m_ptrWordComodin;
		POSITION			m_InitialPos;		// Posición donde comienza la frase dentro de la oración
		POSITION			m_FinalPos;			// Posición donde termina la frase dentro de la oración
		int					m_Size;				// cantidad de nodos que casa el comodín de llave
		bool				m_bError;
		CMatchItem		   *m_ptrMatchItem;
		CStringA				m_sCmdType;			// tipo del comodín dato que viene en #T
		CCompiledRuleTable *m_ptrCompiledRuleTable;
#if defined(_DEBUG)
		CStringA				m_StrKeyWord;		// KeyWord de entrada
#endif
		CStringA				m_StrFileName;		// Nombre del file de comodines
		CComodinItemKey m_cmdKey;

		// funciones privadas
		void ReplaceDollar(CStringA &sTranslation);
		void ReplaceDollarKey(CStringA &sKey);
		void ReplaceParticle(CStringA &sTranslation);
		POSITION FindInitialTranslationNode(int &iIniSize, int &iPos);
		POSITION FindFinalTranslationNode(int iIniSize, int iPos);
		void Concordance(bool bMakeCorcondance = true);
		virtual void OrdenaFrase(CStringA &sTranslation);

public:
#if !defined(MERCADO)
		CStringA				m_strCmdKey;		// DEBUG para el comodin casado
#endif					

		//#if defined(_DEBUG)
		CComodin(CTranslationUser *ptrTranslationUser = NULL,	// puntero a los datos para la traducción
			LPCORACION ptrSentence  = NULL, 
			POSITION ptrNodo		= NULL,
			CCompiledRuleTable *ptrCompiledRuleTable = NULL,
			const CStringA& ptrStrKey = CANull):CStepInSentence(ptrTranslationUser,ptrSentence)
					,m_cmdKey(ptrSentence, ptrNodo, ptrCompiledRuleTable)
			/*#else
			CComodin(CTranslationUser *ptrTranslationUser = NULL,	// puntero a los datos para la traducción
			LPCORACION ptrSentence  = NULL,
			POSITION ptrNodo		= NULL,
			CCompiledRuleTable *ptrCompiledRuleTable = NULL
			):CStepInSentence(ptrTranslationUser,ptrSentence)
			#endif*/
			{
			m_ptrNodo     = NULL;
			m_ptrWord     = NULL;
			m_InitialPos  = NULL;
			m_FinalPos    = NULL;
			m_Size		  = 0;

			if(ptrNodo != NULL)
				{
				m_ptrNodo = ptrNodo;
				m_ptrWord = (LPCWORD)GetWordAt(m_ptrNodo);
				}

			m_ptrWordComodin = (LPCWORD)new CWord;
			m_sCmdType       = "DD";			

			m_ptrWordComodin->m_sTipo = m_sCmdType;
			m_bError = false;

			ASSERT(m_ptrNodo        != NULL);
			ASSERT(m_ptrWord        != NULL);
			ASSERT(m_ptrWordComodin != NULL);
			ASSERT(ptrCompiledRuleTable != NULL);
			m_ptrCompiledRuleTable = ptrCompiledRuleTable;

			//if(ptrStrKey != NULL)
#if defined(_DEBUG)
			m_StrKeyWord = ptrStrKey;
#endif
			}

		~CComodin()
			{
			if(m_ptrWordComodin != NULL)
				delete m_ptrWordComodin;
			}

		bool GetError()
			{return m_bError;}
		void SetError(bool bError)
			{m_bError = bError;}

		bool Math(CMatchItem *ptrMatchItem, POSITION nodeStart, POSITION nodeEnd);

		void Remove();
		virtual void ProccessDataFunction();
		virtual bool ProccessDataTranslation();

		POSITION &GetPtrInitialPos()
			{return m_InitialPos;}
		POSITION &GetPtrFinalPos()
			{return m_FinalPos;}
		POSITION GetFinalPos();

	protected:
		// les quite el virtual pues estos metodos no necesitan ser virtuales
		/*virtual*/ void SetPtrInitialPos(POSITION ptrInitialPos)
			{m_InitialPos = ptrInitialPos;}
		/*virtual*/ void SetPtrFinalPos(POSITION ptrFinalPos)
			{m_FinalPos = ptrFinalPos;}
		/*virtual*/  LPCWORD GetPtrWordComodin()
			{return m_ptrWordComodin;}
		//	virtual LPCSTR GetPtrCmdCData()
		//		{return m_strCmdCData;}
		//	virtual void SetPtrCmdCData(LPCSTR ptrString)
		//		{
		//		ASSERT(ptrString != NULL);
		//		m_strCmdCData = ptrString;
		//		}
		/*virtual CStringA &GetCmsType()
			{return m_sCmdType;}*/
		/*virtual void SetCmsType(LPCSTR ptrString)
			{m_sCmdType = ptrString;}*/
		virtual void SetSize(int iSize)
			{m_Size = iSize;}
		virtual int GetSizeByPosition(POSITION ptrInitialPos,POSITION ptrFinalPos);

		CCompiledRuleTable *GetRuleTable()
			{
			ASSERT(m_ptrCompiledRuleTable != NULL);
			return m_ptrCompiledRuleTable;
			}
	public:
		virtual CStringA &GetFileName()
			{return m_StrFileName;}
		virtual void SetFileName(const CStringA& ptrString)
			{m_StrFileName = ptrString;}
	};

////////////////////////////////////////////////////////////////////////
// Clase para manipular frases de comodines de conjugación y coordinación
////////////////////////////////////////////////////////////////////////

class CComodinPhrase: public CComodin
	{
	private:

		CStringA			m_sType;			// tipo del comodín dato que viene en #T
		//	CStringA			m_sData;			// dato como viene en el dcc #T...#E...
		CStringA			m_sKey;				// Llave encontrada en el diccionario
		//	CStringA			m_sFunction;		// dato que viene en #E

		/********
		void Split()
		{
		if(m_sData.IsEmpty() == FALSE)
		{
		GetSubCampo(m_sType,m_sData,"T","#");
		GetSubCampo(m_sFunction,m_sData,"E","#");
		}

		}
		********/
		virtual void OrdenaFrase();


	public:

		CComodinPhrase(	CTranslationUser *ptrTranslationUser = NULL,	// puntero a los datos para la traducción
			LPCORACION ptrSentence = NULL, 
			POSITION ptrInitialPos = NULL, 
			POSITION ptrFinalPos = NULL, 
			const CStringA& ptrKey = CANull,
#if defined(_DEBUG)
			CCompiledRuleTable *ptrCompiledRuleTable = NULL):CComodin(ptrTranslationUser,ptrSentence,ptrInitialPos,ptrCompiledRuleTable,ptrKey)
#else
			CCompiledRuleTable *ptrCompiledRuleTable = NULL):CComodin(ptrTranslationUser,ptrSentence,ptrInitialPos,ptrCompiledRuleTable)
#endif
			{
			SetPtrInitialPos(ptrInitialPos);
			SetPtrFinalPos(ptrFinalPos);
			//if(ptrKey != NULL)
			m_sKey = ptrKey;
			//		if(ptrData != NULL)
			//			m_sData = ptrData;

			ASSERT(GetPtrInitialPos() != NULL);
			ASSERT(GetPtrFinalPos() != NULL);
			//ASSERT(ptrKey != NULL);
			//		ASSERT(ptrData != NULL);
			ASSERT(ptrCompiledRuleTable != NULL);
			}

		virtual void ProccessDataFunction(CMatchItem *ptrFrase);
		virtual bool ProccessDataTranslation()
			{return false;}
	};
#endif
