/////////////////////////////////////////////////////////////////////
// Header para declaración de las clases para el 
// trabajo con datos de los comodines
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// Clase para manipular los comodines de los datos de la forma "CmdData"
/////////////////////////////////////////////////////////////////////
#if !defined(_CCOMODINTEMDATA_)
#define _CCOMODINTEMDATA_

#include "CompiledCmdArray.h"

class CComodinItemData: public CMFCObject
	{
	private:
	LPCWORD m_ptrWord;
	CCompiledRuleTable *m_ptrCompiledRuleTable;
	// Funciones privadas
	bool Proccess(const CActionPhrase *ptrFrase);
	LPSTR GetItemCommand(LPSTR ptrini,int &iParam,CStringA &sParam,int &iCommand);

	public:
		// funciones públicas
	CComodinItemData(LPCWORD ptrWord, CCompiledRuleTable *ptrCompiledRuleTable)
		{
	    SetPtrWord(ptrWord);
		ASSERT(GetPtrWord() != NULL);
		ASSERT(ptrCompiledRuleTable != NULL);
		m_ptrCompiledRuleTable = ptrCompiledRuleTable;
		}
	//bool Execute(CObArray *ptrAsignaValores);
	bool Execute(CActionPhraseArray *ptrAsignaValores);

protected:
	virtual LPCWORD GetPtrWord()
		{return m_ptrWord;}
	virtual void SetPtrWord(LPCWORD ptrWord)
		{m_ptrWord = ptrWord;}
	CCompiledRuleTable *GetRuleTable()
		{
		ASSERT(m_ptrCompiledRuleTable != NULL);
		return m_ptrCompiledRuleTable;
		}
	};


#endif
