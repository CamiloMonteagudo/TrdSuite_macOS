//-------------------------------------------------------------------------
// RootLanguageData.h: interface for the CRootLanguageData class.
//-------------------------------------------------------------------------

#if !defined(AFX_ROOTLANGUAGEDATA_H__9B388481_02CD_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_ROOTLANGUAGEDATA_H__9B388481_02CD_11D3_8926_0060972DBBB5__INCLUDED_

#include "UtilFun.h"
#include "LanguageName.h"

//-------------------------------------------------------------------------

class CRootLanguageData: public CMFCObject
  {
  public:
    DECLARE_SERIAL(CRootLanguageData);

  private:
    CStringA m_sIsoName;							// Nombre iso de dos letras para este idioma
    CStringA m_sRootDir;              // Raiz de Directorio donde diccionarios
    CFile iCVDictFile;                // Fichero del diccionario
  
  public:
    int lngSrc;                       // Idioma fuente
    int lngDes;                       // Idioma destino

  	virtual ~CRootLanguageData()
  		{
  		Close();
  		}
  	
  	virtual void Close()
  		{
  		iCVDictFile.Close();
  		}
  	
    virtual CStringA GetIsoName()
      {
      ASSERT(m_sIsoName.Length() > 0);
      return m_sIsoName;
      }
    
    virtual void SetDir( int src, int des)
      {
      lngSrc = src;
      lngDes = des;
      
      m_sIsoName = CLangIso::MakeName(CLangIso::Name(src), CLangIso::Name(des));
      }
  
    virtual void SetLang( int lng)
      {
      lngSrc = lng;
      lngDes = -1;
  
      m_sIsoName = CLangIso::Name(lng);
      }
  
    virtual CStringA GetRootDir()
      {
      ASSERT(m_sRootDir.IsEmpty() == FALSE);
      return m_sRootDir;
      }

    virtual void SetRootDir(const CStringA& ptrRootDir)
      {
      m_sRootDir = ptrRootDir;
			if (!m_sRootDir.IsEmpty() && m_sRootDir.Right(1) != FOLDER_SEP)
				m_sRootDir += FOLDER_SEP;

      ASSERT(m_sRootDir.IsEmpty() == FALSE);
      }

    virtual CStringA MakeName(LPCSTR ptrName, bool bAddLanguageNameAtIni = false, bool bAddLanguageNameAtEnd = false, LPCSTR sExt=NULL)
      {
      //ASSERT(ptrExt != NULL);
      //ASSERT(ptrName != NULL);

      CStringA sName = GetRootDir();

      ASSERT(sName.IsEmpty() == FALSE);

#if !defined(MERCADO)
      sName += GetIsoName();
      sName += FOLDER_SEP;
#endif
      if( sName.Right(1) != FOLDER_SEP) sName += FOLDER_SEP;
      if( bAddLanguageNameAtIni  ) sName += GetIsoName();

      sName += ptrName;

      if( bAddLanguageNameAtEnd ) sName += GetIsoName();

      StrPutFileExt(sName, sExt);
      
      return sName;
      }

    //----------------------------------------------------------------------
    // Carga el diccionario si no lo encuentra carga el .txt correspondiente
    //----------------------------------------------------------------------
		template<class DICT>
    bool LoadDictionary(DICT *ptrDic, LPCSTR ptrFileName, bool bAddLanguageNameAtEnd = false)
      {
      CStringF sFileName;		// Nombre del diccionario a cargar

      sFileName = MakeName(ptrFileName,!bAddLanguageNameAtEnd,bAddLanguageNameAtEnd, ".dic");

      if(ptrDic->Load(sFileName) == FALSE)
        return false;

      return true;
      }

    //----------------------------------------------------------------------

    /*virtual CSGrepArray *LoadSGrepArray(LPCSTR ptrFileName,BOOL bSustituir,BOOL bId)
      {
      CStringA sFileName;		// Nombre del fichero de RE a cargar

      MakeName(sFileName,ptrFileName,true,false,".RE");

      return (CSGrepArray *)new CSGrepArray(sFileName,bSustituir,bId);
      }*/

    //----------------------------------------------------------------------
    // Carga todos los elementos de la clase
    // a través de la función Serialize
    //----------------------------------------------------------------------

    virtual bool LoadSerialize(const CStringA& aFileName)
      {
      if (!iCVDictFile.Open(aFileName, CFile::modeRead))
      	{
      	LOG_ERROR("LoadSerialize, file not found: %s", aFileName.c_str());
        return false;
      	}

      CArchive loadArchive(&iCVDictFile, CArchive::load);

      Serialize(loadArchive);     // load me
      loadArchive.Close();
      //file.Close();

      return !loadArchive.hasError();
      }

#ifdef ACTIVE_SAVE
    //----------------------------------------------------------------------
    // Carga todos los elementos de la clase
    // a través de la función Serialize
    //----------------------------------------------------------------------
    virtual bool SaveSerialize(const CStringA& ptrFileName)
      {
      CFile file;

      if (!file.Open(CA2CT(ptrFileName), CFile::modeWrite))
        {
        return false;
        }

      CArchive saveArchive(&file, CArchive::store);
      Serialize(saveArchive);     // save me
      saveArchive.Close();
      file.Close();

      return true;                // success
      }
#endif

    //----------------------------------------------------------------------
    // Salva diccionario en disco de ser necesario
    //----------------------------------------------------------------------
    /*virtual void SaveDictionary(CDict &Dic)
      {
      if(Dic.GetModifiedFlag() == TRUE)
        Dic.Save();
      }*/

    virtual void Serialize(CArchive &/*ar*/) {}
  };

//-------------------------------------------------------------------------

#endif // !defined(AFX_ROOTLANGUAGEDATA_H__9B388481_02CD_11D3_8926_0060972DBBB5__INCLUDED_)

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
