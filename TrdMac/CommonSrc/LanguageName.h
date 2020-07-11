// LanguageName.h: interface for the CLanguageName class.
// Clase para el trabajo con nombre iso de dos letras
//////////////////////////////////////////////////////////////////////

#ifndef LANGUAGENAME_H
#define LANGUAGENAME_H

static LPCFSTR IsoNames[] = {LF"Es", LF"En", LF"It", LF"Fr"};

#define NLANGS  (int)(sizeof(IsoNames)/sizeof(IsoNames[0]))

static LPCSTR FullNames[NLANGS][NLANGS]=	{
                                        {"Español" , "Inglés" , "Italiano", "Francés"  },
                                        {"Spanish" , "English", "Italian" , "French"   },
                                        {"Spagnolo", "Inglese", "Italiano", "Francese" },
                                        {"Espagnol", "Anglais", "Italien" , "Français" },
                                        };

class CLangIso
{
public:
  static int Count() {return NLANGS;}

	static CStringF Name( int Idx )
	  {return CStringF((Idx>=0 && Idx<NLANGS)? IsoNames[Idx] : LF"Xx");}

	static LPCSTR FullName( int Idx, int iUser )
	  {return (Idx>=0 && Idx<NLANGS && iUser>=0 && iUser<NLANGS)? FullNames[iUser][Idx]: "Unknown";}

 static bool IsValid(const CStringF& ptrName)
	 {return (Id(ptrName) != -1);}

 static int Id(const CStringF& ptrName)
	 {
   if( ptrName.Length()!=2 ) return -1;
   
   char c1 = (ptrName[0] & 0xDF);     // Lleva a mayuscula
   char c2 = (ptrName[1] | 0x20);     // Lleva a minuscula

	 for( int i=0; i<NLANGS; i++)
     if( c1==IsoNames[i][0] && c2==IsoNames[i][1] )
       return i;

	 return -1;
	 }

	/*static CStringW MakeName( LPCSTR sSrc, LPCSTR sDes, LPCSTR Sep = "2")
		{
		CStringA Name;
		Name =  sSrc;
		Name += Sep;
		Name += sDes;

		return Name.ToUnicode();
		}*/

	static CStringF MakeName(const CStringA& sSrc, const CStringA& sDes, LPCSTR Sep = "2")
		{
		CStringA Name;
		Name = sSrc;
		Name += Sep;
		Name += sDes;

		return Name;
		}

	static void MakeName(CStringF& aName, const CStringF& sSrc, const CStringF& sDes, LPCFSTR Sep = LF"2")
		{
		aName =  sSrc;
		aName += Sep;
		aName += sDes;
		}

	static void MakeName(CStringF& aName, int iSrc, int iDes, LPCFSTR Sep = LF"2")
		{
		CLangIso::MakeName(aName, CLangIso::Name(iSrc), CLangIso::Name(iDes), Sep );
		}
};

//class CTranslationIsoName  
//{
//public:
//	LPCSTR MakeIsoName(CStringA &sIsoName, LPCSTR sSrc, LPCSTR sDes, LPCSTR Sep = "2")
//		{
//    ASSERT(CLangIso::IsValid(sSrc) && CLangIso::IsValid(sDes));
//
//		sIsoName =  sSrc;
//		sIsoName += Sep;
//		sIsoName += sDes;
//
//		return sIsoName;
//		}
//
//	LPCSTR MakeIsoName(CStringA &Name, int iSrc, int iDes, LPCSTR Sep = "2")
//		{
//		return MakeIsoName( Name, CLangIso::Name(iSrc), CLangIso::Name(iDes), Sep );
//		}
//
//};

#endif
