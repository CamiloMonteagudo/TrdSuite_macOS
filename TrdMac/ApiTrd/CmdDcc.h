/////////////////////////////////////////////////////////////////////
// Header para declaración de las clases para el 
// trabajo con diccionarios de comodines y files .cmd
/////////////////////////////////////////////////////////////////////
#if !defined _DCC_COMODIN_HEADER_
#define _DCC_COMODIN_HEADER_

#include "UtilFun.h"
#include "Cora.h"
//#include "CmdDcc.h"
//#include "Cmd.h"
#include "CompiledCmdArray.h"

typedef struct  _yystype2
    {
    public:
    int     integer;
    CStringA string;
    WORD wItem1;
    WORD wItem2;
    WORD wItem3;
    WORD wIndexParametro1;
    WORD wIndexParametro2;
    WORD wIndexParametro3;
    WORD cant;
    } YYSTYPE2;

#define YYREDMAX  3000
#define YYMAXDEPTH  500
#define DIM(a)  ((int)(sizeof(a)/sizeof(a[0])))
#define YYERRCODE   256

/////////////////////////////////////////////////////////////////////
// Clase para cargar desde un file .cmd a un diccionario de comodines
// este diccionario tiene la forma 
// llave
// dato->Lista de comodines
/////////////////////////////////////////////////////////////////////

class CDccComodin: public CMFCObject
  {
  private:

  CStringA  m_DicName;
  CCompiledRuleTable m_RuleTable; // Es la que se retorna y el resultado de la compilacion
  CStringA		  m_StrFileName;		// Nombre del file de comodines

  protected:
  //	CCompiledCmdArray *m_CompiledCmdArray;
  // funciones protegidas

  public:

    DECLARE_SERIAL(CDccComodin);

  CDccComodin()
    {
	  Inicialize();
    }

  void Inicialize()
		{
		}

	~CDccComodin()
		{
		Inicialize();
		}

  /*virtual*/ CKeyWordEntry *Find(const CStringA& ptrStr)
		{
#if _WORKING_WITH_DCC_
		return m_RuleTable.FindKeyWordEntryInDcc(ptrStr);
#else
		int index = m_RuleTable.FindInKeyWordEntry(ptrStr);
		if(index != -1 && index != 65535)
			return m_RuleTable.GetAt(index);
		return NULL;
#endif
		}

  virtual void Serialize(CArchive &ar);

protected:
  /*virtual*/ const CStringA& GetDicName()
    {return m_DicName;}
  /*virtual*/ void SetDicName(const CStringA& ptrDicName)
    {m_DicName = ptrDicName;}

public:
  /*virtual*/ CCompiledRuleTable *GetRuleTable()
    {
    return &m_RuleTable;
    }

  /*virtual*/ CStringA &GetFileName()
    {return m_StrFileName;}

  /*virtual*/ void SetFileName(const CStringA& ptrString)
    {m_StrFileName = ptrString;}
  };


/////////////////////////////////////////////////////////////////////
//	Class que almacena dcc con los comodines para la conjugación de
//  verbos y coordinación de adjetivos y sustantivos
/////////////////////////////////////////////////////////////////////

class CDccCmdFrase : public CDccComodin
  {
  private:

  bool m_bSalirAlPrimero;

  public:

    DECLARE_SERIAL(CDccCmdFrase);

  CDccCmdFrase(bool bSalirAlPrimero = true)
    {
    m_bSalirAlPrimero = bSalirAlPrimero;
    }

  virtual void Serialize(CArchive &ar);
  };

/////////////////////////////////////////////////////////////////////
#endif
