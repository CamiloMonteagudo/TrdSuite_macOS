// ReflexiveVerbInSentence.cpp: implementation of the CReflexiveVerbInSentence class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReflexiveVerbInSentence.h"

/////////////////////////////////////////////////////////////////////////////
//  Pone reflexivo a las palabras marcadas                                 // 
/////////////////////////////////////////////////////////////////////////////
const char *RefArrayEs[] =
    {
    "me",
    "nos",
    "se",
    "te",
    "vos"
    };                  
    
const char *RefArrayIt[] =
    {
    "mi",
    "ci",
    "si",
    "ti",
    "vi"
    };                  
    
const char *RefArrayFr[] =
    {
    "me",
    "nous",
    "(se)",
    "te",
    "vous"
    };                  
    
const char *RefArrayDe[] =
    {
    "mich",
    "uns",
    "sich",
    "sich",
    "uns"
    };                  
    
const char *RefGrepArray[] =
    {
    "endose$",
    "éndose",
    "andose$",
    "ándose",
    "endole$",
    "éndole",
    "andole$",
    "ándole",
    "endola$",
    "éndola",
    "andola$",
    "ándola",
    "endolas$",
    "éndolas",
    "andolas$",
    "ándolas",
    "endoles$",
    "éndoles",
    "andoles$",
    "ándoles",
    "endonos$",
    "éndonos",
    "andonos$",
    "ándonos",
    "endolo$",
    "éndolo",
    "andolo$",
    "ándolo",
    "endolos$",
    "éndolos",
    "andolos$",
    "ándolos",
    "endose ",
    "éndose ",
    "andose ",
    "ándose ",
    NULL
    };                  

const char *RefInfGrepArrayEs[] =
    {
    "(ar|er|ir) ",
	"?1# ",
    "(ar|er|ir)$",
	"?1#",
    "(ando|endo) ",
	"?1# ",
    "(ando|endo)$",
	"?1#",
    NULL
    };                  
                
const char *RefInfGrepArrayIt[] =
    {
    "(ar|er|ir)e ",
	"?1# ",
    "(ar|er|ir)e$",
	"?1#",
    "(ando|endo) ",
	"?1# ",
    "(ando|endo)$",
	"?1#",
    NULL
    };                  
                
const char *RefInfGrepArrayFr[] =
    {
    "([^ ]+)(ar|er|ir) ",
	"# ?1?2~",
    "([^ ]+)(ar|er|ir)$",
	"# ?1?2~",
    "([^ ]+)(ando|endo) ",
	"# ?1?2~",
    "([^ ]+)(ando|endo)$",
	"# ?1?2~",
    NULL
    };                  
                
POSITION CReflexiveVerbInSentence::Execute(POSITION ptrNode, LPCWORD ptrWord)
    {
    PutReflexivePronoun(ptrWord);
	return ptrNode;
    }
void CReflexiveVerbInSentence::PutReflexivePronoun(LPCWORD ptrWord)
    {
  int iTargetLanguage = CLangIso::Id(GetPtrTargetLanguageData()->GetIsoName());
    int i = 2;
    if(ptrWord->m_Persona == PRIMERAP)            
        i = (ptrWord->m_Numero == SINGULAR)?0:1;
	if(ptrWord->m_Persona == SEGUNDAP && iTargetLanguage == 3)  // para el francés el reflexivo es diferente para cada persona no hay segunda de respeto
        i = (ptrWord->m_Numero == SINGULAR)?3:4;

	CStringA pronombre;

	switch(iTargetLanguage)
		{
		case 4:
			{
			if(ptrWord->m_Trad.Find("(se)") != -1)  // para el francés 
				{
				if(ptrWord->m_Numero == SINGULAR)
					{
					if(ptrWord->m_Persona == PRIMERAP)
						pronombre = "me";
					else if(ptrWord->m_Persona == SEGUNDAP)
						pronombre = "te";
					else
						pronombre = "se";
					}
				else
					{
					if(ptrWord->m_Persona == PRIMERAP)
						pronombre = "nous";
					else if(ptrWord->m_Persona == SEGUNDAP)
						pronombre = "vous";
					else
						pronombre = "se";
					}
				ptrWord->m_Trad.Replace("(se)",pronombre);
				}
			break;
			}
		case 0:
			{
			if(ptrWord->m_Trad.Find("(se)") != -1)  // para el francés 
				{
				if(ptrWord->m_Numero == SINGULAR)
					{
					if(ptrWord->m_Persona == PRIMERAP)
						pronombre = "me";
					else if(ptrWord->m_Persona == SEGUNDAP)
						pronombre = "te";
					else
						pronombre = "se";
					}
				else
					{
					if(ptrWord->m_Persona == PRIMERAP)
						pronombre = "nos";
					else if(ptrWord->m_Persona == SEGUNDAP)
						pronombre = "vos";
					else
						pronombre = "se";
					}
				ptrWord->m_Trad.Replace("(se)",pronombre);
				}
			break;
			}
		case 2:
			{
			if(ptrWord->m_Trad.Find("(si)") != -1)  // para el francés 
				{
				if(ptrWord->m_Numero == SINGULAR)
					{
					if(ptrWord->m_Persona == PRIMERAP)
						pronombre = "mi";
					else if(ptrWord->m_Persona == SEGUNDAP)
						pronombre = "ti";
					else
						pronombre = "si";
					}
				else
					{
					if(ptrWord->m_Persona == PRIMERAP)
						pronombre = "ci";
					else if(ptrWord->m_Persona == SEGUNDAP)
						pronombre = "vi";
					else
						pronombre = "si";
					}
				ptrWord->m_Trad.Replace("(si)",pronombre);
				}
			break;
			}
		}

    if (ptrWord->m_Reflexivo == TRUE && GetPtrTargetLanguageData()->GetGerundType().Find(ptrWord->m_sTipo) != -1 && ptrWord->m_Modo == mGERUNDIO && (iTargetLanguage == 0))
        {
        CStringA tmp;
        tmp += ptrWord->m_Trad;
        CSGrepArray GrepInRef(RefInfGrepArrayEs,TRUE,FALSE);
        GrepInRef.Replace(tmp,FALSE);
		tmp.Replace("#",RefArrayEs[i]);
        ptrWord->m_Trad = tmp;

        //ptrWord->m_Trad += RefArray[i];
        CSGrepArray GrepRef(RefGrepArray,TRUE,FALSE);
        GrepRef.Replace(ptrWord->m_Trad,FALSE);
        }    
    else if(   ptrWord->m_Reflexivo == TRUE  
            && ptrWord->m_Modo == mINFINITIVO 
            && GetPtrTargetLanguageData()->GetVerbalType().Find(ptrWord->m_sTipo) != -1)
        {   
        CStringA tmp;
        tmp += ptrWord->m_Trad;
        CSGrepArray GrepRef((iTargetLanguage == 2)?RefInfGrepArrayIt:(iTargetLanguage == 3)?RefInfGrepArrayFr:RefInfGrepArrayEs,TRUE,FALSE);
        GrepRef.Replace(tmp,FALSE);
		tmp.Replace("#",(iTargetLanguage == 2)?RefArrayIt[i]:(iTargetLanguage == 3)?RefArrayFr[i]:RefArrayEs[i]);
        ptrWord->m_Trad = tmp;
        }    
    else if(   ptrWord->m_Reflexivo == TRUE  
            && ptrWord->m_Modo == mIMPERATIVO 
            && GetPtrTargetLanguageData()->GetParticipleType().Find(ptrWord->m_sTipo) == -1
            && GetPtrTargetLanguageData()->GetVerbalType().Find(ptrWord->m_sTipo) != -1)
        {   
		if(ptrWord->m_Trad.Find(' ') == -1)   // solo lo puedo hacer si es palabra ya que hay que buscar el verbo en la frase y no lo hago todavía
			{							// debe hacerse una función que conjugue teniendo en cuenta reflexivo y enclítico
			CStringA tmp(ptrWord->m_Trad);
			tmp += " ";
			tmp += ((iTargetLanguage == 2)?RefArrayIt[i]:(iTargetLanguage == 3)?RefArrayFr[i]:RefArrayEs[i]);
	        ptrWord->m_Trad = tmp;
			}
        }    
    else if(   ptrWord->m_Reflexivo == TRUE  
            && ptrWord->m_Modo != mGERUNDIO 
            && ptrWord->m_Modo != mPARTICIPIO
            && GetPtrTargetLanguageData()->GetParticipleType().Find(ptrWord->m_sTipo) == -1
            && GetPtrTargetLanguageData()->GetVerbalType().Find(ptrWord->m_sTipo) != -1)
        {   
		/****
        CStringA tmp;
        tmp += ptrWord->m_Trad;
        CSGrepArray GrepRef(RefInfGrepArray);
        GrepRef.Replace(tmp,FALSE);
		FindReplace(tmp,"#",RefArray[i]);
        ptrWord->m_Trad = tmp;
		****/
        CStringA tmp((iTargetLanguage == 2)?RefArrayIt[i]:(iTargetLanguage == 3)?RefArrayFr[i]:RefArrayEs[i]);

        ptrWord->m_Trad = tmp + " " + ptrWord->m_Trad;
        }
    
    ptrWord->m_Trad.Replace("~","");
    }
    
