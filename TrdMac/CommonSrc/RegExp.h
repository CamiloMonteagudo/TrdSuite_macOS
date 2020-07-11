/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */
#if !defined(__REGEXP__)
#define __REGEXP__

#include <stdlib.h>

#define	MAGIC	0234

#define IsAlpha(c)  (isalpha(c)||(((c) >='À' /*&& (c) <='ÿ'*/) && (c) !='÷' && (c) !='ø'&& (c) !='Ø'))                    
// el (c) <='ÿ' da un warning de "comparison is always true". Sailer 06/02/2010
//#define IsSpace(c)  (c ==' ' || c =='\t')                    
#define IsAlphaNum(c)   isalnum(c)                    

#define NSUBEXP  10 

typedef struct regexp {
    char *startp[NSUBEXP];
    char *endp[NSUBEXP];
    char regstart;      /* Internal use only. */
    char reganch;       /* Internal use only. */
    char *regmust;      /* Internal use only. */
    int regmlen;        /* Internal use only. */
    char program[1];    /* Unwarranted chumminess with compiler. */
} regexp;

class CSGrep : public CMFCObject
    { 
public: 
    DECLARE_SERIAL(CSGrep);
    regexp *m_ptrData;                 // pointer to regexp
    int m_Index;                       // index to return
    CStringA m_sReplace;                // string to Replace                     
    CStringA m_sFindRE;
    
private:    
    /*
     * Forward declarations for regcomp()'s friends.
     */
    char *reg(int paren, int *flagp);
    char *regbranch(int *flagp);
    char *regpiece(int *flagp);
    char *regatom(int *flagp);
    inline char *regnode(char op);
    inline char *regnext(register char *p);
    void regc(char b);
    void reginsert(char op, char *opnd);
    void regtail(char *p, char *val);
    void regoptail(char *p, char *val);
//  int regtry(regexp *prog, char *string);
    int regtry(const char *string);
    int regmatch(char *prog);
    int regrepeat(char *p);
    void regerror(const char *s);
    regexp *regcomp(const char *exp);  //  puse const 
//  BOOL regexec(regexp *prog, char *string);
    #ifdef STRCSPN
    int strcspn(char *s1, char *s2);
    #endif
    #ifdef DEBUG
    char *regprop(char *op);
    void regdump(regexp *r);
    #endif

public:
    BOOL regexec(const char* string);
    void regsub(const char* source,const char* sOrig,CStringA &dest);
    
    CSGrep()              // compile a regexp
        {
        m_Index = -1;
        }                         

    CSGrep(const CStringA& s)              // compile a regexp
        {
        //ASSERT( s != NULL);
        m_sFindRE = s;
        m_ptrData = regcomp(CS2SZ(m_sFindRE));
        //ASSERT(m_ptrData != NULL);
        m_Index = -1;
        }                    
				
    CSGrep(const CStringA& s,const CStringA& r,int index = -1)      // compile a regexp to sgrep
        {
        //ASSERT( s != NULL && r != NULL);
        m_sFindRE = s;
        m_ptrData = regcomp(CS2SZ(m_sFindRE));
        m_sReplace = r;
        //ASSERT(m_ptrData != NULL);
        m_Index = index;
        }                    
    ~CSGrep()    
        {                              // free the regexp
        if(m_ptrData != NULL)
            free(m_ptrData);        
        }    
        
    BOOL Match(const CStringA& s)     // Match a CStringA and regexp
        {
        return regexec(s.c_str());
        }
        
    BOOL Replace(CStringA &s,BOOL romper = FALSE)      // Match a CString and regexp
        {
				//COLLECT_INTERVAL(m_sReplace.c_str());
        //return regexec(m_ptrData,s);
        //ASSERT(m_sReplace.IsEmpty()!= TRUE);
        BOOL ret = FALSE, r = FALSE;
        if(m_sReplace == s)
            return ret;
        do  {
            ret = regexec(s.c_str());
            if(ret)                          // codigo de sustituir
                {              
                CStringA tmp(s);
                regsub(m_sReplace.c_str(), s.c_str(), tmp);
                if(tmp == s)
                    break;
                s = tmp;
                r = TRUE;
                if( r == TRUE && romper == TRUE)
                    break;
                }    
            } while(ret == TRUE);   
        return r;
        }
        
    int LenghtMatch()    // Lenght of the last Match
        {
        ASSERT(m_ptrData != NULL);
        if(m_ptrData != NULL && m_ptrData->startp[0] != NULL && m_ptrData->endp[0] != NULL)
            {
            return (int)(m_ptrData->endp[0]-m_ptrData->startp[0]);
            }
        else    
            return 0;
        }   

    LPCSTR GetStartPos()
      {
      ASSERT(m_ptrData != NULL);
      return m_ptrData->startp[0];
      }

    LPCSTR GetEndPos()
      {
      ASSERT(m_ptrData != NULL);
      return m_ptrData->endp[0];
      }

    int GetIndex()
        {
        return m_Index;
        }   
    void SetIndex(int i)
        {
        m_Index = i;
        }   
    BOOL IsError()
        {
        return (m_ptrData == NULL)?TRUE:FALSE;
        }   
    /*const char *GetReplaceStr()
        {
        return m_sReplace;
        }*/   
    
    private:
    /*
     * Global work variables for regcomp().
     */
    const char *m_RegParse;     /* Input-scan pointer. */
    int m_RegNPar;      /* () count. */
    char m_RegDummy;
    char *m_RegCode;        /* Code-emit pointer; &m_RegDummy = don't. */
    long m_RegSize;     /* Code size. */
    const char *m_RegInput;     /* String-input pointer. */
    const char *m_RegBol;       /* Beginning of input, for ^ check. */
    char **m_RegStartP; /* Pointer to startp array. */
    char **m_RegEndP;       /* Ditto for endp. */
    
    public:
    
#ifdef ACTIVE_SAVE
    void Save(CArchive &ar) const
			{
			ar << m_sFindRE;
			ar << m_sReplace;
			ar << (WORD)m_Index;
			}
#endif

    void Load(CArchive &ar)
			{
      WORD tmp;
      ar >> m_sFindRE;
      ar >> m_sReplace;
      ar >> tmp;
      m_Index = (int)tmp;
      m_ptrData = regcomp(CS2SZ(m_sFindRE));
			}

    virtual void Serialize(CArchive &ar)
      {  
			ASSERT(ar.IsLoading());
      if(ar.IsLoading())  
				Load(ar);
      }
    };

class CSGrepArray : public CMFCObject
  {      
  protected:
  //CObArray m_ptrArray;
  CArray<CSGrep> m_ptrArray;
	BOOL m_Error;
	BOOL m_Id;
	BOOL m_FindReplace;
  void CSGrepArrayL(const char **fn,BOOL findreplace,BOOL id);

  public: 
  DECLARE_SERIAL(CSGrepArray);
  CSGrepArray()
		{
		m_Error = FALSE;
		m_Id = FALSE;
		m_FindReplace = FALSE;
		}
  CSGrepArray(const char **fn,BOOL findreplace=FALSE,BOOL id=FALSE);
  CSGrepArray(int fn,BOOL findreplace=FALSE,BOOL id=FALSE);
  ~CSGrepArray();

  int Match(const CStringA& s/*, int *lenMatch = NULL*/);
  void Replace(CStringA &r,BOOL continuo = TRUE);

  virtual void Serialize(CArchive &ar)
    {          
    if(ar.IsStoring())  
      {                  
#ifdef ACTIVE_SAVE
      ar << m_Id;
      ar << m_FindReplace;
#endif
      }
    else
      {
      DWORD tmp;
      ar >> tmp;
      m_Id = (tmp != 0);
      ar >> tmp;
      m_FindReplace = (tmp != 0);
      }

#ifdef LOAD_FROM_DCX
		m_ptrArray.Load(ar);
#else
		if (ar.IsLoading())
			m_ptrArray.SerializeAsObArray(ar);
#ifdef ACTIVE_SAVE
		else
			m_ptrArray.Save(ar);
#endif		
#endif		

    //m_ptrArray.Serialize(ar);
    }
  
	BOOL IsError()
		{
		return m_Error;
		}
  };

#endif 
