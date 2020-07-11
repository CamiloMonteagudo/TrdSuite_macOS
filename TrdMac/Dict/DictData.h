/*
 ============================================================================
 Name		: DictData.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CDictData declaration
 ============================================================================
 */

#ifndef DICTDATA_H
#define DICTDATA_H

// INCLUDES
#include "ComplexTypes.h"


#define MAX_TYPES 6

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class CMean  
  {
  public:
    CMean()
      { iGen=iNum=iRef=iTerm=0; }

  void Copy( CMean &M)
    {
    iGen   = M.iGen  ;
    iNum   = M.iNum  ;
    iRef   = M.iRef  ;
    iTerm  = M.iTerm ;
    iEsp   = M.iEsp  ;
    isMean = M.isMean;
    }

  public:
  BYTE    	iGen;
  BYTE    	iNum;
  BYTE    	iRef;
  BYTE    	iTerm;
  TGramEsp 	iEsp;
  TGramMean isMean;
  };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class CType  
  {
  public:

  TGramType  iTipo;
  CStringA   iCmds;
  CArray<CMean> iMean;
  
  CType() {}
  ~CType() { iMean.Close(); }
  };


// CLASS DECLARATION

/**
 *  CDictData
 * 
 */
class CDictData
	{
public:
	// Constructors and destructor

	CDictData()
  	{
    iNTypes  = 0;
    iIdxMean = -1;
	  iIdxType = -1;
		iIsKeyword = false;
  	}

	/**
	 * Destructor.
	 */
	~CDictData()
		{
		}

  TInt ParseTxt(const TDictData& aData);
  BOOL GetDataText(TDictData& aData);
  //void GetDataXml(TDictData& aData);
  TInt GetTypesCount()
    {
    return iNTypes;
    }
  
  inline int GetNMeans( int nType )
    {
    ASSERT( nType>=0 && nType<iNTypes );
  
    return (int)iTypes[nType].iMean.GetCount();
    }
  
  TInt MeanCount(const CStringA& aType);
  TInt MeanCountAt(TInt aIdxType);
  void get_ActualCmds(CStringA& aCmds);
  void get_ActualMean(CStringA& aMean);
  void get_ActualEsp(TGramEsp& aEsp);
  TText get_ActualGen();
  TText get_ActualNum();
  TText get_ActualRef();
  TText get_ActualTerm();
  //void GetComplexType(TDes8& aType);
  TInt get_ActualTypeIdx();
  void put_ActualTypeIdx(TInt aIdx);
  TInt get_ActualMeanIdx();
  void put_ActualMeanIdx(TInt aIdx);
  void get_ActualType(CStringA& aType);
  void get_TypeAt(int aIdx, CStringA& aType);
	
  void SetType( int nType, LPCSTR aType);
  void SetMean( int nType, int nMean, LPCSTR Mean );
  void SetEsp ( int nType, int nMean, LPCSTR Esp  );
  void SetGen ( int nType, int nMean, int    Gen  );
  void SetNum ( int nType, int nMean, BOOL   Num  );
  void SetRef ( int nType, int nMean, BOOL   Ref  );
  void SetTerm( int nType, int nMean, BOOL   Term );
	
  //void   SetTypeCmds  ( int nType, LPCSTR Cmds  );
  int AddType( LPCSTR sType );
  int AddMean( int nType, LPCSTR Mean  );
  
  void DelType( int nType );
  void DelMean( int nType, int nMean );

  int CanAddType(LPCSTR sType, int Idx);

  bool MoveMean(int idxType, int idxFrom, int idxTo);
	
	bool isKeyword() { return iIsKeyword; };

private:
  CType iTypes[ MAX_TYPES ];
  int   iNTypes;
  int		iIdxMean;
  int		iIdxType;
	bool  iIsKeyword;

  void  	Clear();

  int    	SetDat( const CStringA& aData, const CStringA& aType );
  BOOL   	GetDat( TDictData& aData, CStringA& aType );

	void		ParseQuotes(CStringA aDataText, CStringA& aAddedMeans);
	void		RemoveParenthesis(CStringA& aDataText);

  BOOL   	GetComplexType( CStringA& aType );
  BOOL   	GetFullType   ( CStringA& aType );
  int    	GetIdxType    ( const CStringA& aType );
  
  CStringA* GetSingleType( int aNType );
  CStringA* GetTypeCmds  ( int aNType );

  CStringA* GetMean( int aNType, int aNMean );
  CStringA* GetEsp ( int aNType, int aNMean );
  BOOL   	GetGen ( int aNType, int aNMean );
  BOOL   	GetNum ( int aNType, int aNMean );
  BOOL   	GetRef ( int aNType, int aNMean );
  BOOL   	GetTerm( int aNType, int aNMean );

  BOOL 		GetDeafultType( const TGramType& Type ); 
  BOOL 		IsCondition( const CStringA& s, int &j );
  BOOL 		CopyData( const CStringA& aData, const CStringA& aType, int &i );
  BOOL 		GetMeans( const CStringA& aData, const CStringA& aType, int ini, int fin );
  int 		AddMean( const CStringA& aData, const CStringA& aType, int &i, int fin );

	void CpyMean( int T1, int M1, CDictData *Dat, int T2, int M2 );

	void CpyType( int nType1, CDictData *Dat, int nType2 );
  
	};

#endif // DICTDATA_H
