////////////////////////////////////////////////////////////////////////////
//   Funciones utiles                                                     //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez        29/12/97                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__UTILFUN_H__)
#define __UTILFUN_H__

#include <ctype.h>
#include "WinUtil.h"

//-------------------------------------------------------------------------------------------
// Convierte la primera letra de la cadena 's' a mayusculas
//-------------------------------------------------------------------------------------------
inline void FirstToUpper( CStringA& s )
  {         
  if( !s.IsEmpty() )
    s.SetAt(0, CharAnsiToUpper(s[0]));
  }

//-------------------------------------------------------------------------------------------
// Convierte la primera letra de la cadena 's' a minusculas
//-------------------------------------------------------------------------------------------
inline void FirstToLower( CStringA& s )
  {         
  if( !s.IsEmpty() )
    s.SetAt(0, CharAnsiToLower(s[0]));
  }
/*
//-------------------------------------------------------------------------------------------
// Convierte la primera letra de la cadena 's' a minusculas
//-------------------------------------------------------------------------------------------
inline void FirstChgCase( CStringA& s )
  {         
  if( !s.IsEmpty() )
    {
    if( isupper(s[0]) )
      s.SetAt(0, CharAnsiToLower(s[0]));
    else
      s.SetAt(0, CharAnsiToUpper(s[0]));
    }
  }*/

//-------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
#define BeginWaitCursor()  AfxGetApp()->BeginWaitCursor()
#define EndWaitCursor()    AfxGetApp()->EndWaitCursor()
//////////////////////////////////////////////////////////////////////

#define ILLEGAL_NOFOUNDWORDS_MARKERS    "[]{}()<>\\\"\'/*#"
	
//////////////////////////////////////////////////////////////////////
// Funciones de uso general
//////////////////////////////////////////////////////////////////////////////

void   DelPbrk(CStringA &s,const CStringA& s2);
void	 PutSufijo(CStringA &left,CStringA &corchete,const CStringA &right);
/*int		 StrIndex(const char *s,const char c);*/
void	 DelAccent(CStringA &s);
void	 StrPutFileExt(CStringF &name,  const CStringF& ext);
CStringA StrGetDir(const CStringA& aPath);

/*CStringA &IntToString(CStringA &str,int i);
void	 StringOfIntToArray(CStringA s,CWordArray *array);*/
//BOOL	 GetSubCampo(CStringA &subcampo,const CStringA& str,const CStringA& name,LPCSTR divisor);
BOOL	 GetSubCampo(CStringA &subcampo,const CStringA& str, char name, char divisor);
BOOL	 SetSubCampo(CStringA &str,const CStringA& subcampo,LPCSTR name,LPCSTR divisor);
//void	 MsgError(int i,const char *key);
void	 StringToArray(const CStringA& s, CSmartStrArray *array, const char* separador = ",");
void	 StringToArray(LPCSTR s, CSmartStrArray *array, const char* separador = ",");
void	 StringToArrayWithSeparator(const CStringA& s, CStrArray *array, LPCSTR separador);
void	 ArrayToString(CStringA &s,CStrArray &array, LPCSTR separador = ",");
/*void	 DelQuotationMarks(LPSTR buffer);
void   DelArguments(LPSTR buffer);
bool	 FindStringBetweenMark(CStringA strMain, CStringA strFind,char cMark);

bool    ReadRegKey(HKEY Root, LPCSTR Path, LPCSTR Key, LPBYTE pValue, int len);
bool    SaveRegKey(HKEY Root, LPCSTR Path, LPCSTR Key, LPBYTE pValue, DWORD Type, int len);
void    StrPutFileExt(CStringA &name, const char *ext);*/
void SetFileExtension(CStringF &filename, CStringF ext);
/*CStringA GetProgName(LPCSTR dirname);
*/

LPSTR ReadLine(LPSTR lpsz, UINT nMax, CFile& f );

//////////////////////////////////////////////////////////////////////

#endif
