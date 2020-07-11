// CVDefn.h: interface for the CDicRef class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__CVDEFN_H__)
#define __CVDEFN_H__


#include "RootLanguageData.h"
#include "LanguageName.h"
#include "ctabla.h"
#include "reftabla.h"
#include "DictLoad.h"

class CDicRef : public CRootLanguageData  
{
	CDict*  m_PIDic;
	CDictLoad*  m_SufDic;
	CDict*  m_AuxVerbs;
	CTabla*    m_TablaV;
	CTabla*    m_TablaPatron;
	CRefTabla* m_TablaRef;
	CDict*  m_FIDic;
	CDictLoad*  m_SufFDic;
	CDictLoad*  m_PluralIDic;
	CDictLoad*  m_SufPluralDic;
	CDictLoad*  m_AdjSupIDic;
	CDictLoad*  m_AdjCompIDic;
	CDictLoad*  m_AdjSupSufDic;
	CDictLoad*  m_AdjCompSufDic;
	CDictLoad*  m_PrefVerbDic;
	CDict*  m_DecNounIDic;
	CDict*  m_DecNounSufDic;
	CDict*  m_DecAdjIDic;
	CDict*  m_DecAdjSufDic;

	bool LoadTabla(CTabla *ptrTabla, LPCFSTR ptrFileName);

public:

	//int m_OpenCount;

	//DECLARE_SERIAL(CDicRef);

//	CDicRef(const CStringF& ptrLanguageName,const CStringF& ptrRootDir=CFNull)
//		{
//    SetRootDir(ptrRootDir);
//
//		ASSERT(CLangIso::IsValid(ptrLanguageName) == true);
//
//    SetIsoName(ptrLanguageName);
//
//    InitDicts();
//		}
//
	CDicRef(int iLanguageName, const CStringF& ptrRootDir)
		{
    SetRootDir(ptrRootDir);

    SetLang( iLanguageName );

    InitDicts();
		}

  void InitDicts()
    {
		m_PIDic         = new CDict("CV_PIDic");
		m_SufDic        = new CDictLoad("CV_SufDic");
		m_AuxVerbs      = new CDict("CV_AuxVerbs");
		m_TablaV        = (CTabla *)    new CTabla();
		m_TablaPatron   = (CTabla *)    new CTabla();
		m_TablaRef      = (CRefTabla *) new CRefTabla();
		m_FIDic         = new CDict("CV_FIDic");
		m_SufFDic       = new CDictLoad("CV_SufFDic");
		m_PluralIDic    = new CDictLoad("CV_PluralIDic");
		m_SufPluralDic  = new CDictLoad("CV_SufPluralDic");
		m_AdjSupIDic    = new CDictLoad("CV_AdjSupIDic");
		m_AdjCompIDic   = new CDictLoad("CV_AdjCompIDic");
		m_AdjSupSufDic  = new CDictLoad("CV_AdjSupSufDic");
		m_AdjCompSufDic = new CDictLoad("CV_AdjCompSufDic");
		m_PrefVerbDic   = new CDictLoad("CV_PrefVerbDic");
		m_DecNounIDic   = new CDict("CV_DecNounIDic");
		m_DecNounSufDic = new CDict("CV_DecNounSufDic");
		m_DecAdjIDic    = new CDict("CV_DecAdjIDic");
		m_DecAdjSufDic  = new CDict("CV_DecAdjSufDic");
    }

	virtual ~CDicRef()
		{
		if(m_PIDic         != NULL) delete m_PIDic;
		if(m_SufDic        != NULL) delete m_SufDic;
		if(m_AuxVerbs      != NULL) delete m_AuxVerbs;
		if(m_TablaV        != NULL) delete m_TablaV;
		if(m_TablaPatron   != NULL) delete m_TablaPatron;
		if(m_TablaRef      != NULL) delete m_TablaRef;
		if(m_FIDic         != NULL) delete m_FIDic;
		if(m_SufFDic       != NULL) delete m_SufFDic;
		if(m_PluralIDic    != NULL) delete m_PluralIDic;
		if(m_SufPluralDic  != NULL) delete m_SufPluralDic;
		if(m_AdjSupIDic    != NULL) delete m_AdjSupIDic;
		if(m_AdjCompIDic   != NULL) delete m_AdjCompIDic;
		if(m_AdjSupSufDic  != NULL) delete m_AdjSupSufDic;
		if(m_AdjCompSufDic != NULL) delete m_AdjCompSufDic;
		if(m_PrefVerbDic   != NULL) delete m_PrefVerbDic;
		if(m_DecNounIDic   != NULL) delete m_DecNounIDic;
		if(m_DecNounSufDic != NULL) delete m_DecNounSufDic;
		if(m_DecAdjIDic    != NULL) delete m_DecAdjIDic;
		if(m_DecAdjSufDic  != NULL)	delete m_DecAdjSufDic;
		}

	CDict  *GetPtrIrregWordDict()		 {return m_PIDic;}          // Get puntero a dicc 
	CDictLoad  *GetPtrSuffixDict()   		 {return m_SufDic;}         // Get puntero a dicc 
	CDict  *GetPtrAuxVerbDict()  		 {return m_AuxVerbs;}       // Get puntero a dicc 
	CTabla    *GetPtrVerbTable()      	 {return m_TablaV;}         // Get puntero a dicc 
	CTabla    *GetPtrPatternTable()   	 {return m_TablaPatron;}    // Get puntero a dicc 
	CRefTabla *GetPtrReferTable()  		   {return m_TablaRef;}       // Get puntero a dicc 
	CDict  *GetPtrIrregFemDict() 		 {return m_FIDic;}          // Get puntero a dicc 
	CDictLoad  *GetPtrSuffixFemDict()		 {return m_SufFDic;}        // Get puntero a dicc 
	CDictLoad  *GetPtrIrregPluralDict() 	 {return m_PluralIDic;}     // Get puntero a dicc 
	CDictLoad  *GetPtrSuffixPluralDict()	 {return m_SufPluralDic;}   // Get puntero a dicc 
	CDictLoad  *GetPtrIrregSupDict()    	 {return m_AdjSupIDic;}     // Get puntero a dicc 
	CDictLoad  *GetPtrIrregCompDict()   	 {return m_AdjCompIDic;}    // Get puntero a dicc 
	CDictLoad  *GetPtrSuffixSupDict()   	 {return m_AdjSupSufDic;}   // Get puntero a dicc 
	CDictLoad  *GetPtrSuffixCompDict()  	 {return m_AdjCompSufDic;}  // Get puntero a dicc 
	CDictLoad  *GetPtrPreffixVerbDict() 	 {return m_PrefVerbDic;}    // Get puntero a dicc 
	CDict  *GetPtrIrregDecNounDict()	 {return m_DecNounIDic;}    // Get puntero a dicc 
	CDict  *GetPtrSuffixDecNounDict() {return m_DecNounSufDic;}  // Get puntero a dicc 
	CDict  *GetPtrIrregDecAdjDict()   {return m_DecAdjIDic;}     // Get puntero a dicc 
	CDict  *GetPtrSuffixDecAdjDict()  {return m_DecAdjSufDic;}   // Get puntero a dicc 

	int GetIntLanguageName(){	return CLangIso::Id(GetIsoName()); }

	virtual bool Load();
	virtual void Serialize(CArchive &ar);
};

#endif
