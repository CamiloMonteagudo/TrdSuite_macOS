// RWDefn.h: interface for the CRWDicRef class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWDEFN_H__659264E2_FF5D_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_RWDEFN_H__659264E2_FF5D_11D2_8926_0060972DBBB5__INCLUDED_


#include "UtilFun.h"
#include "RootLanguageData.h"
#include "LanguageName.h"
#include "Dict.h"
#include "DictLoad.h"
#include "DictMain.h"

class CRWDicRef : public CRootLanguageData  
{
	CDictMain* m_GenDic;		//Dicc. General de palabras	
	CDictLoad* m_SAIDic;		//Dicc. de reducción de sust. y adj. irregulares
	CDictLoad* m_VIDic;		  //Dicc. de reducción de verbos irregulares
	CDictLoad* m_SufVDic;		//Dicc. de reducción de sufijos verbales
	CDictLoad* m_SufSADic;	//Dicc. de reducción de sufijos de sust. y adj.
	CDictLoad* m_PrefDic;		//Dicc. de prefijos con su traducción

public:
  int      m_Lang;      // Idioma al que pertenecen los diccionarios

	//DECLARE_SERIAL(CRWDicRef);

//	CRWDicRef(const CStringF& ptrLanguageName,const CStringF& ptrRootDir=CFNull)
//		{
//		InitDicts();
//		
//    SetRootDir(ptrRootDir);
//
//    m_Lang = CLangIso::Id( ptrLanguageName );
//
//		SetIsoName(ptrLanguageName);
//		}
//
	CRWDicRef(int iLanguageName, const CStringF& ptrRootDir)
		{
		InitDicts();
		
    SetRootDir(ptrRootDir);

    m_Lang = iLanguageName;
    SetLang( iLanguageName );
		}


	~CRWDicRef()
		{
		CloseDicts();
		}
	
  void InitDicts()
    {
  	m_GenDic    = NULL;//new CDictSimple("RW_GenDic");
  	m_SAIDic    = new CDictLoad("RW_SAIDic");
  	m_VIDic     = new CDictLoad("RW_VIDic");
  	m_SufVDic   = new CDictLoad("RW_SufVDic");
  	m_SufSADic  = new CDictLoad("RW_SufSADic");
  	m_PrefDic   = new CDictLoad("RW_PrefDic");
    }
  
  void CloseDicts()
  	{
  	if (m_GenDic)
  		{
  		m_GenDic = NULL;
  		}
  	
  	if (m_SAIDic)
  		{
  		m_SAIDic->Close();
  		delete m_SAIDic;
  		m_SAIDic = NULL;
  		}
  	
  	if (m_VIDic)
  		{
  		m_VIDic->Close();
  		delete m_VIDic;
  		m_VIDic = NULL;
  		}
  	
  	if (m_SufVDic)
  		{
  		m_SufVDic->Close();
  		delete m_SufVDic;
  		m_SufVDic = NULL;
  		}
  	
  	if (m_SufSADic)
  		{
  		m_SufSADic->Close();
  		delete m_SufSADic;
  		m_SufSADic = NULL;
  		}
  	
  	if (m_PrefDic)
  		{
  		m_PrefDic->Close();
  		delete m_PrefDic;
  		m_PrefDic = NULL;
  		}
  	}
	
	CDictLoad *GetPtrIrregNounDict()  {return m_SAIDic;  }   // Get puntero a dicc 
	CDictLoad *GetPtrIrregVerbDict()  {return m_VIDic;   }   // Get puntero a dicc 
	CDictLoad *GetPtrNounSuffixDict() {return m_SufSADic;}   // Get puntero a dicc 
	CDictLoad *GetPtrPreffixDict()    {return m_PrefDic; }   // Get puntero a dicc 
	CDictLoad *GetPtrVerbSuffixDict() {return m_SufVDic; }   // Get puntero a dicc 
  int       GetIntLanguageName()    {return m_Lang;     }   // Idioma

	virtual bool Load();
	virtual void Serialize(CArchive &ar);
};

#endif // !defined(AFX_RWDEFN_H__659264E2_FF5D_11D2_8926_0060972DBBB5__INCLUDED_)
