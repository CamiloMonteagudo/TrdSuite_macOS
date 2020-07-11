// FindWordSentenceInDict.h: interface for the CFindWordSentenceInDict class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDWORDSENTENCEINDICT_H__05DFAA42_FBF1_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_FINDWORDSENTENCEINDICT_H__05DFAA42_FBF1_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ScanWordsInDictionary.h"
#include "FindRootWordInDic.h"
#include "ApiRed.h"
#include "TranslateSentence.h"
//#include "TDataBase.h"

class CFindWordSentenceInDict : public CScanWordsInDictionary  
{
private:
	WORD m_WordCount;
	WORD m_NotFoundCount;

public:
	CFindWordSentenceInDict(CTranslationUser* ptrTranslationUser=NULL,
							LPCORACION ptrSentence=NULL):CScanWordsInDictionary(ptrTranslationUser,ptrSentence,ptrTranslationUser->GetPtrTranslationDic())
		{
		m_WordCount = 0;
		m_NotFoundCount = 0;
		}

	WORD GetWordCount()
		{return m_WordCount;}
	WORD GetNotFoundCount()
		{return m_NotFoundCount;}

protected:

    bool FindRedWord(POSITION ptrNode, LPCWORD ptrWord) // Llama Función de Patricia
      {
			//COLLECT_INTERVAL("FindRedWord"); // 2.31 9534
      POSITION ptrNewPos = ptrNode;   // posición donde insertar los pronombres enclíticos

      //CTranslationUser *ptrtu = GetTranslationUserPtr();
      CFindRootWordInDic find(GetTranslationUserPtr(),ptrWord);
      bool bFound = find.UpdateWordData();
      if(bFound == true)
      	{
        if(find.GetDirectPronoun().IsEmpty() == FALSE)
        	{
        	CStringA palabra = find.GetDirectPronoun();
        	LPCWORD ptrInsertedWord = (LPCWORD)new CWord(palabra);
                                   
        	ASSERT( ptrInsertedWord != NULL);                   
        	ptrInsertedWord->m_Case = ptrWord->m_Case;
        	ptrNewPos = GetPtrSentence()->m_Ora.InsertAfter(ptrNewPos,ptrInsertedWord);
          //CFindWordSentenceInDict FindWordSentenceInDict(GetPtrTranslationData(),GetPtrSentence());
          //FindWordSentenceInDict.FindInDic(ptrInsertedWord);
          }
        if(find.GetIndirectPronoun().IsEmpty() == FALSE)
          {
          CStringA palabra = find.GetIndirectPronoun();
          LPCWORD ptrInsertedWord = (LPCWORD)new CWord(palabra);
                                   
          ASSERT( ptrInsertedWord != NULL);                   
          ptrInsertedWord->m_Case = ptrWord->m_Case;
          ptrNewPos = GetPtrSentence()->m_Ora.InsertAfter(ptrNewPos,ptrInsertedWord);
          //CFindWordSentenceInDict FindWordSentenceInDict(GetPtrTranslationData(),GetPtrSentence());
          //FindWordSentenceInDict.FindInDic(ptrInsertedWord);
          }
      	}

			return bFound;
      }
      
	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
		//COLLECT_INTERVAL("CFindWordSentenceInDict"); // 3.25
    if (ptrWord->m_Buscada == FALSE && ptrWord->m_NoBusca == FALSE)
    	{
    	m_WordCount++;

    	BOOL found = FALSE;    // si se encontro la palabra en dicc
    	CStringA buffer = ptrWord->m_Origlwr;  // salva de m_Origlwr
    	GetTranslationUserPtr()->AddWordToList(ptrWord->m_Orig);
    	//int mat = -1;

      // si es aleman y la 1ra palabra de la oración busco primero si verbo
      if(GetPtrSourceLanguageData()->GetIsoName() == LF"De" && ptrWord == GetPtrSentence()->GetFirstWord())
        {
        ptrWord->m_Key = ptrWord->m_Origlwr;
        found = FindInDic(ptrWord);
        }
      else
        {
				// búsqueda de cualquier palabra
        ptrWord->m_Key = ptrWord->m_Orig;

	      found = FindInDic(ptrWord);
        }

			// palabras que son nombres propios pero que no llevan traducción
			// ya que es el mismo del original
			if(found == TRUE && ptrWord->m_sTipo == "TT")
				{
				ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Orig;
				//ptrWord->m_sTipo == "NP";
				ptrWord->m_sTipo = "NP";
				ptrWord->m_Traducida = TRUE;
				ptrWord->m_Buscada = TRUE;
				return ptrNode;
				}

			// Parte para los acronimos que dan tantos bateos
			if(GetPtrSentence()->m_AllUpper == FALSE)
				{
				CSGrep grep("^[A-Z][A-Z]+s$");
				if((ptrWord->m_Case == C_UPPER || grep.Match(ptrWord->m_Orig)== TRUE)&& ptrWord->m_Orig.GetLength() < 9 && ptrWord->m_Orig.GetLength() > 1)
					{
					if(ptrWord->m_Orig == ptrWord->m_Key)
						{
						if(grep.Match(ptrWord->m_Orig)== TRUE)
							{
							ptrWord->m_Orig = ptrWord->m_Orig.Left(ptrWord->m_Orig.GetLength()-1);
							ptrWord->m_Numero = PLURAL;
							ptrWord->m_Key = ptrWord->m_Orig;
							}
						found = FindInDic(ptrWord);
						if(found == FALSE)
							{
							ptrWord->m_Trad = ptrWord->m_Orig;
							ptrWord->m_Key = ptrWord->m_Orig;
							ptrWord->m_Trad = ptrWord->m_Orig;
							ptrWord->m_sTipo = "NP";
							ptrWord->m_Traducida = TRUE;
							found = true;
							ptrWord->m_Buscada = TRUE;
							return ptrNode;
							}
						}
					}
				}

      if(found == TRUE && ptrWord->m_Case != C_UPPER && (GetPtrSourceLanguageData()->GetIsoName() != LF"De" || ptrWord != GetPtrSentence()->GetFirstWord()))
				{
				if(ptrWord->m_DiWord.Get() == DW_ABREVIATURA)
					{
					ptrWord->m_Case = C_LOWER;		
					}
        if( found == TRUE 
        		&& (ptrWord->m_Key == ptrWord->m_Origlwr 
            && (GetPtrSourceLanguageData()->GetIsoName() == LF"De" && ptrWord->m_Case == C_LOWER)
            && GetPtrSourceLanguageData()->GetTypeThatNotNeedReduction().Find(ptrWord->m_sTipo) == -1))
          {
          found = FindRedWord(ptrNode, ptrWord);
          }

				ptrWord->m_Key = ptrWord->m_Orig;
				}
      else if( found == FALSE && ptrWord->m_Key.Find(' ') == -1 && GetPtrSourceLanguageData()->GetIsoName() == LF"De")
        {
        ptrWord->m_Key = ptrWord->m_Orig;

        found = FindRedWord(ptrNode, ptrWord);

        if(ptrWord->m_Key.Find('|') != -1)
        	{
          CStringA sTemp(ptrWord->m_Key);
          sTemp.Replace('|',' ');
          sTemp = "KKKK " + sTemp + " KKKK";

		      COracion TmpSentence(sTemp,FMT_RTF);
			    	{
			    	CTranslateSentence objTranslateSentence(GetTranslationUserPtr(),&TmpSentence);
			    	objTranslateSentence.Process();
			    	}

          ptrWord->m_Key = TmpSentence.m_Orig;
          ptrWord->m_Trad = TmpSentence.m_Dest;
          ptrWord->m_Traducida = TRUE;
          ptrWord->m_Genero = TmpSentence.m_Genero;
          ptrWord->m_Numero = TmpSentence.m_Numero;
          ptrWord->m_Persona = TmpSentence.m_Persona;
          }
        if( found == FALSE && ptrWord->m_Key.Find(' ') == -1 && GetPtrSourceLanguageData()->GetIsoName() == LF"De" && GetPtrSentence()->m_AllUpper == TRUE)
          {
          ptrWord->m_Key = ptrWord->m_Origlwr;
                
          FirstToUpper(ptrWord->m_Key);
          ptrWord->m_Origlwr.MakeLower();
          //_strlwr( (LPSTR)(LPCSTR)ptrWord );

          found = FindRedWord(ptrNode, ptrWord);
          if(ptrWord->m_Key.Find('|') != -1)
            {
            CStringA sTemp(ptrWord->m_Key);
            sTemp.Replace('|',' ');
            sTemp = "KKKK " + sTemp + " KKKK";

		        COracion TmpSentence(sTemp,FMT_RTF);
			      	{
			        CTranslateSentence objTranslateSentence(GetTranslationUserPtr(),&TmpSentence);
			        objTranslateSentence.Process();
			        }

            ptrWord->m_Key = TmpSentence.m_Orig;
            ptrWord->m_Trad = TmpSentence.m_Dest;
            ptrWord->m_Traducida = TRUE;
            ptrWord->m_Genero = TmpSentence.m_Genero;
            ptrWord->m_Numero = TmpSentence.m_Numero;
            ptrWord->m_Persona = TmpSentence.m_Persona;
            }
          }

        if(found == FALSE && GetPtrSourceLanguageData()->GetIsoName() == LF"De")
          {
			    ptrWord->m_Key = ptrWord->m_Origlwr;
          found = FindRedWord(ptrNode, ptrWord);
          if(ptrWord->m_Key.Find('|') != -1)
            {
            CStringA sTemp(ptrWord->m_Key);
            sTemp.Replace('|',' ');
            sTemp = "KKKK " + sTemp + " KKKK";

	          COracion TmpSentence(sTemp,FMT_RTF);
		        	{
		          CTranslateSentence objTranslateSentence(GetTranslationUserPtr(),&TmpSentence);
		          objTranslateSentence.Process();
		          }

            ptrWord->m_Key = TmpSentence.m_Orig;
            ptrWord->m_Trad = TmpSentence.m_Dest;
            ptrWord->m_Traducida = TRUE;
            ptrWord->m_Genero = TmpSentence.m_Genero;
            ptrWord->m_Numero = TmpSentence.m_Numero;
            ptrWord->m_Persona = TmpSentence.m_Persona;
            }
          }
        if (found == FALSE && GetPtrSourceLanguageData()->GetIsoName() != LF"De")
			    {
			    ptrWord->m_Key = ptrWord->m_Origlwr;
			    found = FindInDic(ptrWord);
			    }
        if(ptrWord->m_Key.Find('|') != -1)
          {
          CStringA sTemp(ptrWord->m_Key);
          sTemp.Replace('|',' ');
          sTemp = "KKKK " + sTemp + " KKKK";

	        COracion TmpSentence(sTemp,FMT_RTF);
		        {
						COLLECT_INTERVAL("CTranslateSentence");
		        CTranslateSentence objTranslateSentence(GetTranslationUserPtr(),&TmpSentence);
		        objTranslateSentence.Process();
		        }

          ptrWord->m_Key = TmpSentence.m_Orig;
          ptrWord->m_Trad = TmpSentence.m_Dest;
          ptrWord->m_Traducida = TRUE;
          ptrWord->m_Genero = TmpSentence.m_Genero;
          ptrWord->m_Numero = TmpSentence.m_Numero;
          ptrWord->m_Persona = TmpSentence.m_Persona;
          }
        }
			else if (found == FALSE  && GetPtrSourceLanguageData()->GetIsoName() != LF"De")
				{
				ptrWord->m_Key = ptrWord->m_Origlwr;
				found = FindInDic(ptrWord);
				}

      if(    found == TRUE 
        && (ptrWord->m_Case == C_UPPER 
        || ptrWord->m_sTipo.GetLength() != 2
        || GetPtrSourceLanguageData()->GetIsoName() == LF"De"
        || ptrWord->m_Key.Find('$') != -1))
        {}
      else if( ptrWord->m_Key.Find(' ') == -1 
        && (found == false 
        || (found == TRUE 
        && ptrWord->m_Key == ptrWord->m_Origlwr 
        && GetPtrSourceLanguageData()->GetTypeThatNotNeedReduction().Find(ptrWord->m_sTipo) == -1) 
        || (found == TRUE 
        && (ptrWord->m_Case != C_MIXTO 
        && ptrWord->m_Case != C_OTHER) 
        && GetPtrSourceLanguageData()->GetTypeThatNotNeedReduction().Find(ptrWord->m_sTipo) == -1)))
        {
        if(found == TRUE)
          {
          found = FindRedWord(ptrNode, ptrWord);
          if(found == FALSE)
       	    found = TRUE;
          }
        else
          found = FindRedWord(ptrNode, ptrWord);
        }

      if(found == FALSE 
      	&& (ptrWord->m_Case == C_OTHER 
      	|| ptrWord->m_sTipo == "NP" 
      	|| (ptrWord->m_Orig.Find(' ') != -1 
      	&& ptrWord->m_Case == C_MIXTO)))   // si el caso es mixto es Nombre Propio
      	{
      	ptrWord->m_sTipo = "NP";
      	ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Orig;     
      	ptrWord->m_Traducida = TRUE;
      	m_NotFoundCount++;
      	GetTranslationUserPtr()->AddNotFoundWordToList(ptrWord->m_Orig);
      	}
      else
      	{
      	// Parte para que las letras independientes se cojan dentro de los comodines de frases idiomaticas como <LL1> o <LU1> segun mayusculas o minusculas
      	if(ptrWord->m_Key.GetLength() == 1)
      		{
      		if(ptrWord->m_sSemantica.IsEmpty() == FALSE)
      			ptrWord->m_sSemantica += ";";

      		if(ptrWord->m_Orig == ptrWord->m_Origlwr)
      			{
      			ptrWord->m_sSemantica += "LL";
      			}
      		else
      			{
      			ptrWord->m_sSemantica += "LU";
      			}
      		if(found == FALSE)
      			{
      			ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Orig;
						ptrWord->m_sTipo = "SS";
      			//ptrWord->m_sTipo == "SS";
      			ptrWord->m_Traducida = TRUE;
      			ptrWord->m_Buscada = TRUE;
      			found = TRUE;
      			}
      		}

      	if(found == FALSE)     // prueba primero si es un verbo irregular
      		found = FindIrrgVerb(ptrWord);
    
      	if(found == FALSE)     
      		{
      		found = FindKeySufijo(ptrWord,ptrNode);
      		}    
      	if(found == FALSE)     
      		{
      		found = FindKeyPrefijo(ptrWord,ptrNode,FALSE);
      		}   
      	if(found == FALSE && ptrWord->m_Case == C_UPPER && ptrWord->m_Key.Find(' ') == -1 && ptrWord->m_Key.GetLength() < 6 && GetPtrSentence()->m_AllUpper == FALSE && ptrWord->m_Romano == FALSE)
      		{
      		ptrWord->m_sTipo = "NP";
      		ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Orig;     
      		ptrWord->m_Traducida = TRUE;
      		GetTranslationUserPtr()->AddNotFoundWordToList(ptrWord->m_Orig);
      		m_NotFoundCount++;
      		}
				/*********
				if(found == FALSE && ((ptrWord->m_Case != C_UPPER && ptrWord->m_Case != C_MIXTO) || m_Abreviatura == TRUE)&& reduce == TRUE)
					{               
					ptrWord->m_Trad = ptrWord->m_Key;
					found = FindTradSufijo(ptrOra,ptrNode);
					if(found == TRUE)
						{
						FindTradPrefijo(ptrOra,ptrNode);
						ASSERT(Translate->m_Idioma->RE_Traduce != NULL);
						Translate->m_Idioma->RE_Traduce->Replace(m_Trad,TRUE);
						ptrWord->m_StrReserva1 = ptrWord->m_Trad;
						}
					}
				**********/
				CStringA issimo("issimo");
				CStringA issima("issima");
				if(ptrWord->m_Origlwr.Right(issimo.GetLength())==issimo || (ptrWord->m_Origlwr.Right(issima.GetLength()) == issima && GetPtrSourceLanguageData()->GetIsoName() == LF"It"))
					{
					CStringA Raiz(ptrWord->m_Origlwr.Left(ptrWord->m_Origlwr.GetLength()-issimo.GetLength()));
					CStringA buscar;
					buscar = Raiz +	'o';
					ptrWord->m_Key = buscar;
					found = FindInDic(ptrWord);
					if(found == FALSE)
						{
						buscar = Raiz +	'a';
						ptrWord->m_Key = buscar;
						found = FindInDic(ptrWord);
						}
					if(found == FALSE)
						{
						buscar = Raiz +	'e';
						ptrWord->m_Key = buscar;
						found = FindInDic(ptrWord);
						}
					if(found == TRUE)
						{
						ptrWord->m_sTipo = "AA";
						ptrWord->m_sPrefijo = "\"very @@\"";
						ptrWord->m_Buscada = TRUE;
						ptrWord->m_Trad = "";
						}
					}
				if (found == FALSE && ptrWord->m_Traducida == FALSE)
					{
					if (ptrWord->m_Romano == TRUE)
						{
						found = TRUE;
						//m_WordType = dtROMANO;
						ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Key;
						ptrWord->m_sTipo = "NN";
						m_NotFoundCount++;
						GetTranslationUserPtr()->AddNotFoundWordToList(ptrWord->m_Orig);
						}                      
					else if(ptrWord->m_Case == C_MIXTO)
						{
						//m_WordType = dtNOMBREPROPIO;
						ptrWord->m_sTipo = "NP";
						ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Orig;     
						//FindNameGender();
						m_NotFoundCount++;
						GetTranslationUserPtr()->AddNotFoundWordToList(ptrWord->m_Orig);
						}
					else if(ptrWord->m_Case == C_UPPER)
						{    
						//ptrWord->m_WordType = dtSUSTANTIVO;     // Acrónimo
						//ptrWord->m_Trad =  Translate->m_MarcaIni;
						//ptrWord->m_Trad += ptrWord->m_Key;
						//ptrWord->m_Trad += Translate->m_MarcaFin;
						ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Orig;     // por ahora no voy a ponerle marca de no encontrada a acrónimos
						ptrWord->m_Traducida = TRUE;
						ptrWord->m_sTipo = "SS";
						m_NotFoundCount++;
						GetTranslationUserPtr()->AddNotFoundWordToList(ptrWord->m_Orig);
						}
					else
						{    
						
						//m_WordType = dtSUSTANTIVO;
						//if(FindTipoPorSufijo() == FALSE)
							ptrWord->m_sTipo = "SS";
						ptrWord->m_Trad =  GetTranslationUserPtr()->GetNotFoundInitialMark();
						ptrWord->m_Trad += ptrWord->m_Orig;
						ptrWord->m_Trad += GetTranslationUserPtr()->GetNotFoundFinalMark();
						ptrWord->m_StrReserva1 = ptrWord->m_Trad;
						m_NotFoundCount++;
						GetTranslationUserPtr()->AddNotFoundWordToList(ptrWord->m_Orig);
						}
            
					//ptrWord->m_Data      = ptrWord->m_Origlwr;
					//ptrWord->m_Trad      = ptrWord->m_Orig;
					ptrWord->m_Traducida = TRUE;
					}
				}
			if (ptrWord->m_Plural == TRUE)
				{
				ptrWord->m_Numero  = PLURAL;
	//			ptrWord->m_Persona = TERCERAP;
				}   
			if (ptrWord->m_Femenino == TRUE)
				{
				ptrWord->m_Genero = FEMENINO;
				}
			if(ptrWord->m_sTipo.IsEmpty())
				{    
				ptrWord->m_sTipo = "SS";
				}
			if(ptrWord->m_Posesivo == TRUE)
				{
				CStringA PuedeSerSust("XQ,UR,XA,QF,TA,XB,ZB,UO,XH,QO,QN,XJ,QA,SV,YF,XG,ZF,ZJ,ZZ,YG,XK,UA,QB,QC,QD,UZ,UC,UM,UT,UX,UI,UE,UJ,QJ,QK,QL,QM,SS");
				if(ptrWord->m_sTipo == "NP")
					ptrWord->m_sTipo    = "GV";
				else if(PuedeSerSust.Find(ptrWord->m_sTipo) != -1)
					ptrWord->m_sTipo = "GW";
				else if(ptrWord->m_Orig != ptrWord->m_Key && ptrWord->m_Case == C_MIXTO)
          {
          ptrWord->m_sTipo    = "GV";
          ptrWord->m_Trad = ptrWord->m_Orig;
          ptrWord->m_Traducida = TRUE;
          }
				else if(ptrWord->m_sTipo == "ST")
					ptrWord->m_sTipo    = "GB";
				else 
					ptrWord->m_sTipo    = "GW";
				}
      ptrWord->m_Buscada = TRUE;
    	}

		//TRACE("%s,%s", ptrWord->m_Orig.c_str(), ptrWord->m_sTipo.c_str());
		return ptrNode;
		}

};

class CFindVerboSeparableDe : public CScanWordsInDictionary  
{
public:
	CFindVerboSeparableDe(CTranslationUser* ptrTranslationUser=NULL,
							LPCORACION ptrSentence=NULL):CScanWordsInDictionary(ptrTranslationUser,ptrSentence,ptrTranslationUser->GetPtrTranslationDic())
		{}

protected:

	virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
		{
    CStringA sParticulas(",ab,acht,an,aneinander,auf,aufeinander,aufrecht,aufwärts,aus,auseinander,bei,beieinander,beisammen,bekannt,bereit,besser,bestehen,bevor,blau,bleiben,bloß,bloss,brach,breit,da,dabei,dagegen,daher,dahin,dahinter,dar,daran,darunter,darüber,davon,dazu,dazwischen,dicht,dran,drauflos,drein,durch,durcheinander,ein,eis,empor,entgegen,entlang,fallen,fehl,fern,fertig,fest,flach,flott,flöten,fort,frei,fremd,gefangen,gegen,gegenüber,geheim,gehen,gerade,gering,glatt,gleich,groß,gut,haften,halt,hand,haus,heilig,heim,heimlich,heiß,hell,her,herab,heran,herauf,heraus,herbei,herein,herum,herunter,hervor,herüber,hier,hierher,hin,hinauf,hinaus,hindurch,hinein,hinter,hintereinander,hinterher,hinunter,hinweg,hinzu,hinüber,hoch,hof,hohn,hops,hängen,höher,ineinander,inne,irre,kahl,kalt,kaputt,kehrt,kennen,klar,kleben,klein,knapp,kopf,krank,krumm,kund,kurz,kürzer,lahm,lang,leicht,lieb,liegen,locker,los,mal,maschine,maß,mit,nach,nackt,nahe,nieder,not,näher,ob,offen,preis,rad,ran,raus,rein,richtig,ruhig,rund,rüber,rück,sauber,sausen,scharf,schief,schlapp,schlecht,schwarz,schwer,schön,seil,sein,selig,sicher,sitzen,spazieren,spitz,stand,statt,staub,stecken,stehen,steif,stil,still,stramm,teil,tief,tot,trocken,um,umher,umhin,unter,untereinander,urauf,verloren,verschütt,vol,voll,vor,voran,voraus,vorbei,vorher,vorweg,vorwärts,vorüber,wach,wahr,warm,weg,weh,weis,weiter,weiß,wett,wider,wieder,wohl,wund,zu,zufrieden,zurecht,zurück,zusammen,zuvor,zuwider,übel,über,überein,übereinander,überhand,übrig,");
    CStringA sFinalType(",XX,CC,GS,GZ,GA,");
    CStringA sKey(ptrWord->m_Key);
    sKey = "," + sKey;
    sKey += ",";

    POSITION ptrNextNode = ptrNode;
        
    /*LPCWORD ptrNextWord1 =*/ GetWordAt(ptrNextNode);
    GetNextPos(ptrNextNode);
    LPCWORD ptrNextWord = GetWordAt(ptrNextNode);

    if (    ptrNextNode != NULL 
      && ptrNextWord != NULL 
      && sParticulas.Find(sKey) != -1 
      && sFinalType.Find(ptrNextWord->m_sTipo) != -1 
      && ptrWord->m_Delete == FALSE)
      {
      CStringA sNoType(",SS,DD,RD,RI,AI,AA,FP,FT,FL,FI,FS,NP,XX,PP,XM,XJ,AO,OP,VP,PT,PI,AE,AF,AP,GP,GD,GG");

      for(POSITION ptrPosNode = ptrNode; ptrPosNode != NULL;)
        {
        GetPrevPos(ptrPosNode);

        if(ptrPosNode == NULL)
            break;

        LPCWORD ptrPosWord = GetWordAt(ptrPosNode);
                
        if(ptrPosWord == NULL)
            break;

        if(sNoType.Find(ptrPosWord->m_sTipo) != -1)
            continue;

        CStringA sTemp(ptrPosWord->m_Key);
        ptrPosWord->m_Key = ptrWord->m_Key + ptrPosWord->m_Key;

				BOOL bFound = FindInDic(ptrPosWord);
        if(bFound == FALSE)
					ptrPosWord->m_Key = sTemp;
        else
          {
          ptrWord->m_Delete = TRUE;
          ptrWord->m_sTipo = "XX";
          break;
          }
        }
      }
		return ptrNode;
		}

};


#endif // !defined(AFX_FINDWORDSENTENCEINDICT_H__05DFAA42_FBF1_11D2_8926_0060972DBBB5__INCLUDED_)

