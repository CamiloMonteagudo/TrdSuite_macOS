// ParseWordsInSentence.h: interface for the CParseWordsInSentence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSEWORDSINSENTENCE_H__AF8D0AC2_0848_11D3_8926_0060972DBBB5__INCLUDED_)
#define AFX_PARSEWORDSINSENTENCE_H__AF8D0AC2_0848_11D3_8926_0060972DBBB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StepInSentence.h"
#include "UtilFun.h"
#include "COra.h"

class CParseWordsInSentence : public CStepInSentence  
{
private:
	int m_Format;
	CStringA m_WordSeparator;
	CStringA m_MarcaInicial;
	CStringA m_MarcaFinal;
public:
	CParseWordsInSentence(CTranslationUser *ptrTranslationUser = NULL,LPCORACION ptrSentence = NULL, int iFormat=FMT_RTF):CStepInSentence(ptrTranslationUser,ptrSentence)
		{
		m_Format = iFormat;
		m_WordSeparator = " \t;()\"?¿!¡|,«»×¶[]<>";
		m_MarcaInicial = "[BNTMARKIDIOMAX]";		
		m_MarcaFinal = "[ENTMARKIDIOMAX]";		
		}

	virtual void Process()
		{
		//int		     iMatchedLength  = 0;	// longitud de lo casado
		//int		     iMatchedRE		= -1;	  // RE que casó
		CStringA   sRtfCommand;			    // Cadena con comandos RTF de la palabra
		CStringA   sBlankBeforeWord;		// Cadena con los blancos que anteceden a la palabra
		LPCWORD    ptrLastWord = NULL;	// ultima palabra entrada
		CStrArray  sMeaningWord;		    // Arreglo con las palabras que cambian el significado
		CWordArray iMeaningWord;		    // Arreglo con la posición del significado que se debe tomar en la traducción
		CWordArray pMeaningWord;		    // Arreglo con la posición de la palabra dentro del arreglo de palabras de la oración por si hay dos palabras iguales una con meaning y otra no
		CWordArray pNotTranslationWord; // arreglo con la posicion de las palabras a no traducir
		
		if(GetFormat() == FMT_RTF)
			GetPtrSentence()->m_Orig.Replace("\\tab ","\t");

		CStringA sSentence(GetPtrSentence()->m_Orig);
		// Cambio de las marcas para no traducir
		sSentence.Replace(GetTranslationUserPtr()->GetNotTranslationInitialMark(),m_MarcaInicial);
		CStringA IniM(GetTranslationUserPtr()->GetNotTranslationInitialMark()),FinM(GetTranslationUserPtr()->GetNotTranslationFinalMark());

		if(IniM == FinM)
			{
			bool final = false;
			int i = 0;
			for(;i != -1;)
				{
				i = sSentence.Find(m_MarcaInicial,i);

				if(i != -1)
					{
					if(final == true)
						{
						CStringA ini,fin;
						ini = sSentence.Left(i);
						fin = sSentence.Right(sSentence.GetLength()-(i+m_MarcaInicial.GetLength()));
						sSentence = ini + m_MarcaFinal + fin;
						i += m_MarcaFinal.GetLength();
						final = false;
						}
					else
						{
						i += m_MarcaInicial.GetLength();
						final = true;
						}
					}
				else
					break;
				}
			}
		else
			{
			int i = 0;
			for(;i != -1;)
				{
				i = sSentence.Find(m_MarcaInicial,i);
				if(i != -1)
					{
					i += m_MarcaInicial.GetLength();
					i = sSentence.Find(FinM,i);
					}
				if(i != -1)
					{
					CStringA ini,fin;
					ini = sSentence.Left(i);
					fin = sSentence.Right(sSentence.GetLength()-(i+FinM.GetLength()));
					sSentence = ini + m_MarcaFinal + fin;
					i += m_MarcaFinal.GetLength();
					}
				else
					break;
				}
			}
		// Fin de cambio de marcas para no traducir

		CStrArray WordArray;
		CStringA replacemark = m_MarcaInicial + m_MarcaFinal;

		sSentence.Replace(replacemark,"");
		StringToArrayWithSeparator(sSentence,&WordArray,m_WordSeparator.c_str());

		int kk = 0;
		for(kk = 0; kk < WordArray.GetSize(); kk++)
			{
			CStringA sWord(WordArray[kk]);
/***********			
			if(sWord.FindOneOf(";()\"?¿!¡|,«»¶[]") != -1)
				{
				CStrArray tmp;
				StringToArrayWithSeparator(sWord,&tmp," \t");
				WordArray.InsertAt(kk,&tmp);
				WordArray.RemoveAt(kk + tmp.GetSize());
				kk += tmp.GetSize();
				}
*********/
			if(sWord.Find(':') != -1 && sWord.Right(1) == ":")
				{
				CStrArray tmp;
				StringToArrayWithSeparator(sWord,&tmp,":");
				WordArray.InsertAt(kk,&tmp);
				WordArray.RemoveAt(kk + tmp.GetSize());
				kk += tmp.GetSize();
				}
			int i = -1;
			if((i = sWord.Find('\\')) != -1 /*&& m_Format == FMT_RTF*/&& (kk >0 && WordArray[kk-1] != "["))
				{
				CStringA cmdrtf("{\\}");

				if(i+1 < sWord.GetLength() && cmdrtf.Find(sWord[i+1]) == -1)
					{
					CStringA str(sWord,i);
					
					WordArray.InsertAt(kk,str);
					str = sWord.Right(sWord.GetLength()-i);
					WordArray[kk+1] = str;
					if(kk+2 < WordArray.GetSize())
						{
						sWord = WordArray[kk+2];
						if(sWord.Left(1) == " ")
							{
							str += " ";
							WordArray[kk+1] = str;
							sWord = sWord.Right(sWord.GetLength()-1);
							WordArray[kk+2] = sWord;
							}
						}
					kk++;
					}
				}
	#ifdef _DEBUG
			if(kk < WordArray.GetSize())
				sWord = WordArray[kk];
	#endif
			}
			
		for(kk = 0; kk < WordArray.GetSize();)
			{
			CStringA sWord;
			sWord = WordArray[kk];
			if(sWord == "(")
				{
				int pos = kk;
				kk++;
				if(kk < WordArray.GetSize())
					{
					sWord = WordArray[kk];
					while(sWord.Find(')') != -1 && kk < WordArray.GetSize())
						{
						sWord = WordArray[kk];
						WordArray[pos] += sWord;
						WordArray.RemoveAt(kk);
						}
					}
				}
			// hacer una funcion dode se pregunte esto
			if(FindAltMeaning(kk,WordArray,&sMeaningWord,&iMeaningWord,&pMeaningWord,sWord,"<idx nMean=\"0\">kkkk< / idx>",14)== false)
				{
				FindAltMeaning(kk,WordArray,&sMeaningWord,&iMeaningWord,&pMeaningWord,sWord,"<idx nMean=\"0\">kkkk</idx>",11);
				}
				
			// hacer una funcion dode se pregunte esto para que pueda ser mas de un caracter que sea de fin de palabra
			FindNotTranslationMark(kk,WordArray,&pNotTranslationWord,sWord);   // Cambie « por [

			if(sWord.IsEmpty() == TRUE || sWord.GetLength() == 0 /*|| sWord==" "*/ )
				WordArray.RemoveAt(kk);
			else
				kk++;
	#ifdef _DEBUG
			if(kk < WordArray.GetSize())
				sWord = WordArray[kk];
	#endif
			}

		for(kk = 0; kk < WordArray.GetSize();kk++)
			{
			PEEKMESSAGE();
			CStringA sWord(WordArray[kk]);
			int iMatchedRE = -1/*, iMatchedLength = 0*/;

			if(m_Format == FMT_RTF)
				{
				iMatchedRE = GetPtrSourceLanguageData()->GetPtrSplitInWords()->Match(sWord/*,&iMatchedLength*/);

				switch(iMatchedRE)
					{
					case DW_ENTREPARENTESIS:
						{
						sWord = sWord.Left(sWord.GetLength()-1);
						sWord = sWord.Right(sWord.GetLength()-1);
						if(ptrLastWord == NULL)
							{
							GetPtrSentence()->m_EntreParentesis = sWord;
							GetPtrSentence()->m_EntreQue = 1;
							}
						else
							{
							ptrLastWord->m_EntreParentesis = sWord;
							ptrLastWord->m_EntreQue = 1;
							}
						break;
						}
					case DW_ENTRECOMILLAS:
						{
						sWord = sWord.Left(sWord.GetLength()-1);
						sWord = sWord.Right(sWord.GetLength()-1);
						if(ptrLastWord == NULL)
							{
							GetPtrSentence()->m_EntreParentesis = sWord;
							GetPtrSentence()->m_EntreQue = 2;
							}
						else
							{
							ptrLastWord->m_EntreParentesis = sWord;
							ptrLastWord->m_EntreQue = 2;
							}
						break;
						}

					case DW_ENTRECOMILLASS:
						{
						sWord = sWord.Left(sWord.GetLength()-1);
						sWord = sWord.Right(sWord.GetLength()-1);
						if(ptrLastWord == NULL)
							{
							GetPtrSentence()->m_EntreParentesis = sWord;
							GetPtrSentence()->m_EntreQue = 3;
							}
						else
							{
							ptrLastWord->m_EntreParentesis = sWord;
							ptrLastWord->m_EntreQue = 3;
							}
						break;
						}
/***********
					case DW_ENTRECORCHETES:
						{
						sWord = sWord.Left(sWord.GetLength()-1);
						sWord = sWord.Right(sWord.GetLength()-1);
						if(ptrLastWord == NULL)
							{
							GetPtrSentence()->m_EntreParentesis = sWord;
							GetPtrSentence()->m_EntreQue = 4;
							}
						else
							{
							ptrLastWord->m_EntreParentesis = sWord;
							ptrLastWord->m_EntreQue = 4;
							}
						break;
						}
**********/
					case DW_FRASE_GUION:
						{
						CStrArray tmp;
						StringToArrayWithSeparator(sWord,&tmp,"-");
						for(int i = 0; i < tmp.GetSize(); i++)
							{
							CStringA tmp2(tmp[i]);
							iMatchedRE = GetPtrSourceLanguageData()->GetPtrSplitInWords()->Match(tmp2/*,&iMatchedLength*/);
							LPCWORD ptrWord = (LPCWORD)new CWord(tmp2);
            
							ptrLastWord = ptrWord;               
							ASSERT( ptrWord != NULL);
							iMatchedRE = (iMatchedRE == -1)?DW_NOMBRE_PROPIO:iMatchedRE;
							FillWordData((DiWord)iMatchedRE,ptrWord);
							if(sRtfCommand.IsEmpty() == FALSE)
								{
								ptrWord->m_RtfFijoIni = sRtfCommand;
								sRtfCommand.Empty();
								}
							if(sBlankBeforeWord.IsEmpty() == FALSE)
								{
								ptrWord->m_StrIniBlank = sBlankBeforeWord;
								sBlankBeforeWord.Empty();
								}
							GetPtrSentence()->m_Ora.AddTail(ptrWord);
							}
						break;
						}
					case DW_RTF_COMMAND:
						sRtfCommand = sWord;
						break;
					case DW_BLANCOS:
						sBlankBeforeWord = sWord;
						break;
					default:
						{
						LPCWORD ptrWord = (LPCWORD)new CWord(sWord);
            
						ptrLastWord = ptrWord;               
						ASSERT( ptrWord != NULL);   
						iMatchedRE = (iMatchedRE == -1)?DW_NOMBRE_PROPIO:iMatchedRE;
						FillWordData((DiWord)iMatchedRE,ptrWord);
						if(sRtfCommand.IsEmpty() == FALSE)
							{
							ptrWord->m_RtfFijoIni = sRtfCommand;
							sRtfCommand.Empty();
							}
						if(sBlankBeforeWord.IsEmpty() == FALSE)
							{
							ptrWord->m_StrIniBlank = sBlankBeforeWord;
							sBlankBeforeWord.Empty();
							}
						for(int ii = 0; ii < pMeaningWord.GetSize(); ii++)
							{
							WORD pos = pMeaningWord.GetAt(ii);
							if(pos == kk)
								{
								CStringA s = sMeaningWord[ii];

								if(ptrWord->m_Orig == s)
									{
									ptrWord->m_AltMeaning = (int)iMeaningWord[ii];
									}
								}
							}
						for(int ii = 0; ii < pNotTranslationWord.GetSize(); ii++)
							{
							WORD pos = pNotTranslationWord.GetAt(ii);
							if(pos == kk)
								{
								ptrWord->m_Trad = ptrWord->m_Orig;
								ptrWord->m_Key = ptrWord->m_Origlwr = ptrWord->m_Orig = "";
								ptrWord->m_Traducida = true;
								ptrWord->m_NoBusca = true;
								ptrWord->m_Buscada = true;
								ptrWord->m_sTipo = "NP";
								}
							}
						if(ptrWord->m_Orig == "--")
							{
							ptrWord->m_sTipo = "GZ";
							}
						GetPtrSentence()->m_Ora.AddTail(ptrWord);
						break;
						}
					}
				}
			else
				{
				iMatchedRE = GetPtrSourceLanguageData()->GetPtrSplitInWords()->Match(sWord/*,&iMatchedLength*/);
				if( iMatchedRE > DW_BLANCOS)
					{
					LPCWORD ptrWord = (LPCWORD)new CWord(sWord);
					ASSERT( ptrWord != NULL);                   
					FillWordData((DiWord)iMatchedRE,ptrWord);
					GetWordList().AddTail(ptrWord);
					ptrLastWord = ptrWord;
					}
				}
	#ifdef _DEBUG
			sWord = WordArray[kk];
			//iMatchedLength = iMatchedRE;
	#endif
			}

		if(m_Format == FMT_RTF && ptrLastWord != NULL && sRtfCommand.IsEmpty() == FALSE)
			ptrLastWord->m_RtfFijoFin = sRtfCommand;
		}

protected:
	virtual bool FindAltMeaning(int kk,CStrArray &WordArray,CStrArray *sMeaningWord,CWordArray *iMeaningWord, CWordArray *pMeaningWord,CStringA &sWord,LPCSTR ptrCmp,int posfin)
		{
		bool ret = true;
		int posWord = 8;
		int posInt  = 5;
		CStrArray CmpArray;
		CStringA sCmp(ptrCmp);

		StringToArrayWithSeparator(sCmp,&CmpArray,m_WordSeparator.c_str());

		for(int i = 0; i < CmpArray.GetSize(); i++)
			{
			if(i == posWord || i == posInt)
				continue;
			else
				{
				if(kk + i >= WordArray.GetSize())
					{
					ret = false;
					break;
					}
				CStringA s1 = CmpArray[i];
				CStringA s2(WordArray[kk+i]);
				s1.MakeLower();

				s2.MakeLower();

				if(s1 != s2)
					{
					ret = false;
					break;
					}
				}
			}
		if(ret == true)
			{
			sWord = WordArray[kk+posWord];
			sMeaningWord->Add(sWord);
			int im2 = atoics(WordArray[kk+posInt]);
			iMeaningWord->Add(im2);
			pMeaningWord->Add(kk);
			int iniApos = kk;
			int iniCpos = kk+7;
			int finApos = kk+9;
			int finCpos = kk+posfin; // la posicion de fin cambia en dependencia de la cadena de entrada
			// borrar segun iniApos y iniCpos
			for(int i = finApos; i <= finCpos; i++)
				{
				#ifdef _DEBUG
				CStringA ss = WordArray[finApos];
				#endif
				WordArray.RemoveAt(finApos);
				}
			for(int i = iniApos; i <= iniCpos; i++)
				{
				#ifdef _DEBUG
				CStringA ss = WordArray[iniApos];
				#endif
				WordArray.RemoveAt(iniApos);
				}
			}
		return ret;
		}
	virtual bool FindNotTranslationMark(int kk,CStrArray &WordArray,CWordArray *posNotTranslationWord,CStringA &/*sWord*/)
		{
		bool ret = true;
//		CStringA sIniMark = GetTranslationUserPtr()->GetNotTranslationInitialMark(); 
//		CStringA sFinMark = GetTranslationUserPtr()->GetNotTranslationFinalMark(); 
		CStringA sIniMark = m_MarcaInicial;
		CStringA sFinMark = m_MarcaFinal;

		//int iniApos = kk;
		//int iniCpos = kk;
		int posAFin = kk;
		int posCFin = kk;
		
		CStrArray CmpIniArray;
		CStrArray CmpFinArray;

		StringToArrayWithSeparator(sIniMark,&CmpIniArray,m_WordSeparator.c_str());
		StringToArrayWithSeparator(sFinMark,&CmpFinArray,m_WordSeparator.c_str());

		for(int i = 0; i < CmpIniArray.GetSize(); i++)
			{
			if(kk + i >= WordArray.GetSize())
				{
				ret = false;
				break;
				}
			CStringA s1 = CmpIniArray[i];
			CStringA s2(WordArray[kk+i]);
			s1.MakeLower();
			//_strlwr( (LPSTR)(LPCSTR)s1 );

			s2.MakeLower();
			//_strlwr( (LPSTR)(LPCSTR)s2 );
			if(s1 != s2)
				{
				ret = false;
				break;
				}
			}
			
		if(ret == true)
			{
			bool bHalleFin = false;
			for(int i = kk + CmpIniArray.GetSize(); i < WordArray.GetSize(); i++)
				{
				CStringA s1 = CmpFinArray[0];
				CStringA s2 = WordArray[i];
				s1.MakeLower();
				//_strlwr( (LPSTR)(LPCSTR)s1 );

				s2.MakeLower();
				//_strlwr( (LPSTR)(LPCSTR)s2 );

				if(s1 == s2)
					{
					bHalleFin = true;
					for(int j = 0; j < CmpFinArray.GetSize(); j++)
						{
						CStringA s1 = CmpFinArray[j];
						CStringA s2 = WordArray[i+j];
						s1.MakeLower();
						//_strlwr( (LPSTR)(LPCSTR)s1 );

						s2.MakeLower();
						//_strlwr( (LPSTR)(LPCSTR)s2 );

						if(s1 != s2)
							{
							bHalleFin = false;
							break;
							}
						}
					if(bHalleFin == true)
						{
						posAFin = i;
						posCFin = i += CmpFinArray.GetSize();
						break;
						}
					}
				}

			if(bHalleFin == true)
				{						// si hay marca de fin
				//int pos = kk;
				// booro el marcador de final
				for( int i = posAFin;  i < posCFin; i++)
					{
					CStringA gg = WordArray.GetAt(posAFin);
					WordArray.RemoveAt(posAFin);
					}
				}
			// borro siempre la marca de inicio
			for( int i = kk;  i < kk + CmpIniArray.GetSize(); i++)
				{
				CStringA gg = WordArray.GetAt(kk);
				WordArray.RemoveAt(kk);
				}
				
			// creo una palabra con el texto parea no traducir
			CStringA skk = WordArray.GetAt(kk);
			CStringA NotTranslationText(WordArray[kk]);
			posAFin = (posAFin == kk)?WordArray.GetSize()-1:posAFin;
			
			for( int i = 0;  i < posAFin -(kk + CmpIniArray.GetSize())-1; i++)
				{
				if(kk+1 < WordArray.GetSize())
					{
					CStringA gg = WordArray.GetAt(kk+1);
					NotTranslationText += WordArray[kk+1];
					WordArray.RemoveAt(kk+1);
					}
				}
				
			WordArray.SetAt(kk,NotTranslationText);
			posNotTranslationWord->Add(kk);
			}

/*****************
			if(sWord == )   // Cambie « por [
			{
			int pos = kk;
			for( kk++; kk < WordArray.GetSize();)
				{
				sWord = WordArray[kk];
				WordArray[pos] += sWord;
				WordArray.RemoveAt(kk);
				CStringA tmp = WordArray[pos];
				if(tmp.Find(GetTranslationUserPtr()->GetNotTranslationFinalMark()) > 0)
					break;
				}
			}
**********/
		return ret;
		}
		
	virtual int GetFormat()
		{return m_Format;}
	virtual void SetFormat(int iFormat)
		{m_Format = iFormat;}
	virtual int MatchCaseInString(const CStringA& ptrString)
		{
		CStringA sTempString(ptrString);

		sTempString.MakeLower();
		//_strlwr( (LPSTR)(LPCSTR)sTempString );

		if(sTempString == ptrString)
			return C_LOWER;
		sTempString.MakeUpper();
		//_strupr( (LPSTR)(LPCSTR)sTempString );

		if(sTempString == ptrString)
			return C_UPPER;

		sTempString.MakeLower();
		//_strlwr( (LPSTR)(LPCSTR)sTempString );
		
		CStringA sString(sTempString,1);
		sString.MakeUpper();
		//_strupr( (LPSTR)(LPCSTR)sString );

		sTempString = sString + sTempString.Right(sTempString.GetLength()-1);
		if(sTempString == ptrString)
			return C_MIXTO;

		/// Falta por preguntar si es plural de sigla CD-ROMs
		return C_OTHER;
		}

	// Llena los campos de palabra casada en la división de la oración en palabras
	virtual void FillWordData(CDiWord mat, LPCWORD ptrWord)
		{
		ptrWord->m_DiWord = mat;
		CStringA tmp;

		switch (mat.Get())
			{
		case DW_DOSPUNTOS:
			ptrWord->m_sTipo = "GG";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Orig;
			break;
		case DW_COMA:
			ptrWord->m_sTipo = "GZ";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key   = ptrWord->m_Orig;
			ptrWord->m_Case = C_LOWER;
			break;
		case DW_FININTERROGA:
			ptrWord->m_sTipo = "GS";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key   = ptrWord->m_Orig;
			ptrWord->m_Case = C_LOWER;
			break;
		case DW_FINADMIRACION:
			ptrWord->m_sTipo = "GA";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key   = ptrWord->m_Orig;
			break;
		case DW_ORDINAL:
			{
							   //  Carga y compila RE para identificar ordinales
	//        CSGrepArray *RE_Ordinal = new CSGrepArray(RE_ORDINAL,TRUE,FALSE);  
	//        ASSERT(RE_Ordinal != NULL); 
			//m_DicType = dtORDINALN;
			ptrWord->m_sTipo = "NU";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key = ptrWord->m_Orig;
	//        RE_Ordinal->Replace(m_Trad,FALSE);
			ptrWord->m_Case = C_LOWER;
			break;
			}
		case DW_DECADA_CS:                      
		case DW_DECADA_S:                       
			{
			ptrWord->m_sTipo = "GY";
			ptrWord->m_NoBusca  = TRUE;
        
			int ls = ptrWord->m_Orig.GetLength() - ((mat.Get() == DW_DECADA_CS) ? 2 : 1);
			int i = ptrWord->m_Orig.Find('\'');
			if(i != -1)
				ls = i;                              
			else
				ls = ptrWord->m_Orig.GetLength()-1;
			ptrWord->m_Origlwr  = ptrWord->m_Orig.Left(ls);      // se corta por la comilla 
			ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Key = ptrWord->m_Origlwr;
			ptrWord->m_Case = C_LOWER;
			ptrWord->m_Traducida = TRUE;
			break;
			}
		case DW_NUMERAL:
			ptrWord->m_sTipo = "NN";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key = ptrWord->m_Orig;
			ptrWord->m_Case = C_LOWER;
			break;
		case DW_DINERO:
			ptrWord->m_sTipo = "GN";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key = ptrWord->m_Orig;
			break;
		case DW_WORDENTRECOMILLAS:
			{
			ptrWord->m_Orig  = ptrWord->m_Orig.Left(ptrWord->m_Orig.GetLength()-1);     // se corta por la comilla 
			ptrWord->m_Orig  = ptrWord->m_Orig.Right(ptrWord->m_Orig.GetLength()-1); // se corta por la comilla 
			ptrWord->m_Origlwr = ptrWord->m_Orig;
			ptrWord->m_Origlwr.MakeLower();
			//_strlwr( (LPSTR)(LPCSTR)ptrWord->m_Origlwr );

			ptrWord->m_Key	= ptrWord->m_Origlwr;
			ptrWord->m_Romano = (MatchRomanNumber(ptrWord->m_Origlwr))?TRUE:FALSE;
			ptrWord->m_Case.Set((Caso)MatchCaseInString(ptrWord->m_Orig));
			ptrWord->m_EntreComillas = TRUE;
			break;
			}
		case DW_POSESIVO_CS:
		case DW_POSESIVO_SC:
			{
			ptrWord->m_sTipo = "GX";
			int ls     = ptrWord->m_Orig.GetLength() - ((mat.Get() == DW_POSESIVO_CS) ? 2 : 1);
			ptrWord->m_Orig     = ptrWord->m_Orig.Left(ls);      // se corta por la comilla 
			ptrWord->m_Origlwr  = ptrWord->m_Orig;
			ptrWord->m_Origlwr.MakeLower();   
			//_strlwr( (LPSTR)(LPCSTR)ptrWord->m_Origlwr );

			ptrWord->m_Key = ptrWord->m_Origlwr;
			ptrWord->m_Posesivo = TRUE;
			ptrWord->m_Case.Set((Caso)MatchCaseInString(ptrWord->m_Orig));
			break;
			}
		case DW_WORD:
			ptrWord->m_Origlwr  = ptrWord->m_Orig;
			ptrWord->m_Origlwr.MakeLower();
			//_strlwr( (LPSTR)(LPCSTR)ptrWord->m_Origlwr );

			ptrWord->m_Key      = ptrWord->m_Origlwr;
			ptrWord->m_Romano = (MatchRomanNumber(ptrWord->m_Origlwr))?TRUE:FALSE;
			ptrWord->m_Case.Set((Caso)MatchCaseInString(ptrWord->m_Orig));
			break;
		case DW_MENOS:
			ptrWord->m_sTipo = "GG";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key   = ptrWord->m_Orig;
			ptrWord->m_Case = C_LOWER;
			break;
		case DW_RAYA:
			ptrWord->m_sTipo = "GR";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key   = ptrWord->m_Orig;
			break;
		case DW_MENORQ:
		case DW_MAYORQ:
		case DW_LLAVEA:
		case DW_LLAVEC:
		case DW_CORCHETEA:
		case DW_CORCHETEC:
		case DW_PARENTA:
		case DW_PARENTC:
			ptrWord->m_sTipo = "GP";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key   = ptrWord->m_Orig;
			ptrWord->m_Case = C_LOWER;
			break;
		case DW_COMILLAD:
			ptrWord->m_sTipo = "GD";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key   = ptrWord->m_Orig;
			ptrWord->m_Case = C_LOWER;
			break;
		case DW_COMILLAS:
			ptrWord->m_sTipo = "GC";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key   = ptrWord->m_Orig;
			ptrWord->m_Case = C_LOWER;
			break;
		case DW_NOTRADUCIR:
			{
			ptrWord->m_NoBusca = TRUE;       // la marca no debe pasar a la traducción
			ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Orig;
			ptrWord->m_sTipo =(ptrWord->m_Orig == "[Æ]")?"XX":"NP"; // Cambie « por [
			ptrWord->m_Buscada = TRUE;
			ptrWord->m_Traducida = TRUE;
			}
			break;
		case DW_PORCIENTO:
			ptrWord->m_sTipo = "GO";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key   = ptrWord->m_Orig;
			break;
		case DW_NUMERO:
			ptrWord->m_sTipo = "NN";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key   = ptrWord->m_Orig;
			ptrWord->m_Case = C_LOWER;
			break;
		case DW_CUALQUIERA:
			ptrWord->m_sTipo = "GN";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Origlwr = ptrWord->m_Trad  = ptrWord->m_Key   = ptrWord->m_Orig;
			break;
		case DW_ABREVIATURA3:
		case DW_ABREVIATURA2:
			ptrWord->m_Abreviatura = TRUE;
		case DW_ABREVIATURA:
			tmp = ptrWord->m_Orig;
			DelPbrk(ptrWord->m_Orig,".");
			ptrWord->m_sTipo = "NP";
			ptrWord->m_Origlwr  = ptrWord->m_Orig;
			ptrWord->m_Origlwr.MakeLower();
			//_strlwr( (LPSTR)(LPCSTR)ptrWord->m_Origlwr );

			ptrWord->m_Key = ptrWord->m_Origlwr;
			ptrWord->m_Orig = tmp;
			ptrWord->m_Case.Set((Caso)MatchCaseInString(ptrWord->m_Orig));
			break;
		case DW_NOMBRE_PROPIO:
			ptrWord->m_Origlwr  = ptrWord->m_Orig;
			ptrWord->m_sTipo = "NP";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Trad     = ptrWord->m_Orig;
			break;
		case DW_FILENAME:          // stdio.h <stdio.h> "stdio.h"
			ptrWord->m_Origlwr  = ptrWord->m_Orig;
			ptrWord->m_sTipo = "FN";
			ptrWord->m_NoBusca  = TRUE;
			ptrWord->m_Traducida = TRUE;
			ptrWord->m_StrReserva1 = ptrWord->m_Trad = ptrWord->m_Orig;
			ptrWord->m_Case = C_LOWER;
			break;
    default:
       break; // para eliminar warnings del compilador
			}
		}

	// Casa si puede ser un número romano
	virtual bool MatchRomanNumber(const CStringA& ptrString)
		{
		bool  mat = true;
		CStringA sRomanNumber("ivxlcdm");
		CStringA sString(ptrString);
		sString.MakeLower();
		//_strlwr( (LPSTR)(LPCSTR)sString );
         
		for(int i = 0; i < sString.GetLength() && mat == true; i++)
			{
			if(sRomanNumber.Find(sString[i]) == -1)
				mat = false;
			}
		return mat;
		}

};

#endif // !defined(AFX_PARSEWORDSINSENTENCE_H__AF8D0AC2_0848_11D3_8926_0060972DBBB5__INCLUDED_)
