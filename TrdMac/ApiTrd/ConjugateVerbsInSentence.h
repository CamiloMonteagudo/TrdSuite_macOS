// ConjugateVerbsInSentence.h: interface for the CConjugateVerbsInSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONJUGATEVERBSINSENTENCE_H__F6797930_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
#define AFX_CONJUGATEVERBSINSENTENCE_H__F6797930_FD51_11D2_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"

class CConjugateVerbsInSentence : public CStepInSentence  
  {
  public:
    CConjugateVerbsInSentence(CTranslationUser *ptrTranslationUser=NULL,LPCORACION ptrSentence=NULL):CStepInSentence(ptrTranslationUser,ptrSentence)
      {}

  protected:

    virtual POSITION Execute(POSITION ptrNode, LPCWORD ptrWord)
      {
      if ((ptrWord->m_Modo == mINFINITIVO 
        && ptrWord->m_Trad.Find('<') != -1 
        && ptrWord->m_Trad.Find('>') != -1)
        || GetPtrSourceLanguageData()->GetVerbalType().Find(ptrWord->m_sTipo) != -1)
        ConjugateVerb(ptrWord);
      return ptrNode;
      }
  public:
    void ConjugateVerb(LPCWORD ptrWord)
      {
      for(int lazo = 0; lazo < 6; lazo ++)
        {
        CStringA word(ptrWord->m_Trad);
        //int morf = 0;
        CStringA verbo,verboini,verbofin;
        int ini = word.Find('<');
        int fin = 0;

        if(ini == -1 && word.Find(' ') != -1)
          return;
        if( ini != -1 && (fin = word.Find('>')!= -1))
          {                        
          int fin = word.Find('>');
          ASSERT( fin != -1);
          verbo = word.Mid(ini+1,fin-ini-1);
          if(ini > 0)
            verboini = word.Left(ini);
          if((word.GetLength()-(fin+1)) > 0)    
            verbofin = word.Right(word.GetLength()-(fin+1));
          }    
        else
          verbo = word; 

        word = verboini;
        Tiempo t = ptrWord->m_Tiempo.Get();
        CStringF sSourceLanguageName(GetPtrSourceLanguageData()->GetIsoName());
        CStringF sTargetLanguageName(GetPtrTargetLanguageData()->GetIsoName());

        if(sTargetLanguageName == LF"Fr" && t == PASADO && ptrWord->m_Modo.Get() == mINDICATIVO)
          {
          if(ptrWord->m_Key != "have" && ptrWord->m_Compuesto == FALSE)
            {
            ptrWord->m_Compuesto = TRUE;
            t = PRESENTE;
            }
          else
            t = (t == PASADO)?PASADOIMP:t;
          }
        else
          t = ( sSourceLanguageName == "En"&& (sTargetLanguageName == "It" || sTargetLanguageName == "Es") && t == PASADO && ptrWord->m_Modo.Get() == mINDICATIVO && (ptrWord->m_sTipo == "BE" || ptrWord->m_Reflexivo == TRUE))?PASADOIMP/*PASADO*/:t;

        verbo.Replace("·","");
        CStringA sCnjVerbo;
        word += (GetPtrTargetLanguageData()->ConjugateVerb(sCnjVerbo, verbo, ptrWord->m_Modo.Get(),t,ptrWord->m_Persona.Get(),ptrWord->m_Numero.Get(),ptrWord->m_Negado,ptrWord->m_Compuesto));
        if(word != "-" && !word.IsEmpty() && word.Find("xxxx")==-1)
          {
          if(word.Find('.') != -1)
            {
            int len = word.GetLength()-word.Find('.')-1;
            if(len > 0)
              ptrWord->m_sParticle = word.Right(len);
            word = word.Left(word.Find('.'));
            }
          }
        else
          word = verbo;

        if(sTargetLanguageName != "En" && ptrWord->m_Modo == mPARTICIPIO)
          word += "*";
        word += verbofin;
        if(verbofin.FindOneOf("<>") != -1)
          {
          ptrWord->m_Trad = word;
          continue;
          }
        else
          {
          DelPbrk(word,"<>");
          ptrWord->m_Trad = word;
          break;
          }
        }
      return;
      }
  };

#endif // !defined(AFX_CONJUGATEVERBSINSENTENCE_H__F6797930_FD51_11D2_8926_0060972DBBB5__INCLUDED_)
