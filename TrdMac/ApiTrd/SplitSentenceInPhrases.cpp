// SplitSentenceInPhrases.cpp: implementation of the CSplitSentenceInPhrases class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SplitSentenceInPhrases.h"
#include "FindWordSentenceInDict.h"

/////////////////////////////////////////////////////////////////////////////
// Señala en que tipo de frase se está
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
enum TFrases { FRASE_VERBAL,FRASE_SUST,FRASE_ADJ,FRASE_ADV,FRASE_FIN,FRASE_SER_ESTAR,FRASE_COMPLEX_TIME,INTRO_VERBAL,INTRO_SUST,INTRO_ADJ,INTRO_ADV,INTRO_SER_ESTAR,INTRO_COMPLEX_TIME,INTRO_VERBO_AUXILIAR};
/////////////////////////////////////////////////////////////////////////////

void CSplitSentenceInPhrases::Process()
    {
    ASSERT(GetWordList().IsEmpty() == FALSE);

//    GetPtrSentence()->PoneXX();

	//POSITION ini = NULL,fin = NULL;
	//CWord *wini = NULL;
    CStringA FraseSust;
    CStringA FraseVerbal;
    CStringA FraseAdj;
    CStringA FraseAdv;
    CStringA InicioFraseSustReal("RD;RI;AI;OA;AL;AA;NP;UA;NN;XJ;FN;AF;MV;MU;MG;AN;NN;OR;OS;OP;OI;OO;OC;OG;YU;OD;OE;PB;NO;GW;GV;AP;ME;AY;AE;FS");
    CStringA ProximoEsSustReal("RD;RI;AI;AP;AN;AY;FN;MV;NN;AE");
    CStringA InicioFraseSustQuePideVerbo(";RD;RI;AL;AP;MG;NO;AY;FN;NN;MV;AN;OC;ME;OA;AE;FS");
    CStringA PalabraSolaEsFraseSust("OP;YU;OC;ME;OG.FS");
    CStringA PalabraSolaQuePideVerboAntes(";OC;ME;OG;YU");
    CStringA InicioFraseSust("RD;RI;AA;SS;AI;OA;GG;YF;NP;UA;NN;XJ;FN;MG;AL;OF;UF;UL;AF;MV;MU;MG;AN;NN;OR;OS;OP;OI;OO;OC;OG;YU;OD;OE;PB;QZ;UI;UX;UE;UJ;UK;UD;UN;NO;GW;GV;SP;AP;XM;ZJ;VC;UZ;AY;ZB;FS");
    CStringA EnFraseSust("RD;RI;AA;SS;NP;UA;AI;YF;SV;XJ;FN;MG;AL;OF;XB;UL;UF;UG;AF;MV;MU;MG;AN;NN;OR;OS;OP;OI;OO;OC;OG;YU;OD;OE;QZ;UH;UB;UW;QB;QC;QD;UC;UM;UT;UI;UX;UE;UJ;UK;UD;UN;GW;GV;GG;XH;SP;XI;UR;XQ;XA;QN;QF;TA;ZB;UO;QO;QA;XG;ZF;ZJ;ZZ;XK;QE;XC;XE;XM;XY;XO;UZ;UY;QL;QM;VP;PT;PI;AE;AP;HE;MN;ME;HS;AY;QZ;MO;VC;FS");
    CStringA EnFraseSustSi2EnFraseSust("DD;");
    CStringA InicioFraseVerbal("TO;VA;VT;VI;VV;VG;GT;GI;HT;HI;UF;XB;YF;UL;QK;VD;JI;JT;VS;VR;DO;QJ;WI;SV;UI;UX;UE;UJ;UK;UD;UN;ZV;XI;UR;XQ;XA;QN;QF;TA;ZB;UO;QA;XG;ZF;ZJ;ZZ;QE;XC;ZI;XE;ZH;XP;XL;QG;XF;ZG;UY;QH;QM;XP;XL;VC;UZ");
    CStringA EnFraseVerbal("VA;VT;VI;VV;VG;BN;BG;VG;GT;GI;YF;HT;HI;DD;SV;XB;CC;QK;VD;VC;JI;JT;VS;VR;LK;DN;DO;UH;UB;UW;QB;QC;QD;UZ;UC;QJ;WI;ZV;UL;XI;UR;XQ;XA;QN;QF;TA;ZB;UO;QA;XG;ZF;ZJ;ZZ;QE;XC;ZI;XE;ZH;XP;XL;QG;XF;ZG;UY;QH;QM;XP;XL;");
    CStringA InicioSerEstar(";BE;");
    CStringA EnFraseSerEstar("BE;BN;BG;VG;GT;GI;VC;VP;PT;PI;PB;DD;DN;UH;UB;UW;QB;QC;QD;UZ;UC;UM;UT;UI;UX;UE;UJ;UK;UD;UN;");
    CStringA IntroduceFraseVerbal(";OP;OL;FS");
    CStringA InicioVerboAuxiliar(";VA;QK;QJ;WI;");
    CStringA EnFraseVerboAuxiliar("VV;VT;VI;DD;");
    CStringA InicioComplexTime(";HA;");
    CStringA EnFraseComplexTime("HA;BN;BG;VG;GT;GI;VC;VP;PT;PI;PB;DD;DN;UH;UB;UW;QB;QC;QD;UZ;UC;");
    CStringA EnParticipio("UH;UB;UW;QB;QC;QD;UZ;UC;");
    CStringA EnGerundio("UM;UT;UX;UI;UE;UJ;UK;UD;UN;");

    POSITION nodo = GetWordList().GetHeadPosition();
    
    for(;nodo != NULL;)    // hace el lazo mientras existan palabras en la oración
        {
        ASSERT(nodo != NULL);

        LPCWORD w = GetWordAt(nodo);   // Palabra que se analiza 
        TFrases TipoFrase;

		POSITION nodo2 = nodo;
		POSITION nodo3 = NULL;

		GetNextPos(nodo2);

        CWord *w2 = NULL;
        CWord *w3 = NULL;
		if(nodo2 != NULL)
            {
            w2 = GetWordAt(nodo2);   // Palabra n2 de la que se analiza 
		    nodo3 = nodo2;

		    GetNextPos(nodo3);
            w3 = NULL;
		    if(nodo3 != NULL)
                {
                w3 = GetWordAt(nodo3);   // Palabra n3 de la que se analiza 
                }
            }

        if( w2 != NULL && IntroduceFraseVerbal.Find(w->m_sTipo) != -1 && InicioFraseVerbal.Find(w2->m_sTipo) != -1)
            TipoFrase = INTRO_VERBAL;
        if( w2 != NULL && IntroduceFraseVerbal.Find(w->m_sTipo) != -1 && InicioSerEstar.Find(w2->m_sTipo) != -1)
            TipoFrase = INTRO_SER_ESTAR;
        if( w2 != NULL && IntroduceFraseVerbal.Find(w->m_sTipo) != -1 && InicioComplexTime.Find(w2->m_sTipo) != -1)
            TipoFrase = INTRO_COMPLEX_TIME;
        else if(InicioVerboAuxiliar.Find(w->m_sTipo) != -1)
            TipoFrase = INTRO_VERBO_AUXILIAR;
//        else if( w2 != NULL && InicioFraseVerbal.Find(w->m_sTipo) != -1 && PalabraSolaQuePideVerboAntes.Find(w2->m_sTipo) != -1)
//            TipoFrase = FRASE_VERBAL;
        else if( w2 != NULL && InicioFraseVerbal.Find(w->m_sTipo) != -1 && InicioFraseSustReal.Find(w2->m_sTipo) != -1)
            TipoFrase = FRASE_VERBAL;
        else if(InicioFraseSust.Find(w->m_sTipo) != -1)
            TipoFrase = FRASE_SUST;
        else if(InicioSerEstar.Find(w->m_sTipo) != -1)
            TipoFrase = FRASE_SER_ESTAR;
        else if(InicioComplexTime.Find(w->m_sTipo) != -1)
            TipoFrase = FRASE_COMPLEX_TIME;
        else if(InicioFraseVerbal.Find(w->m_sTipo) != -1)
            TipoFrase = FRASE_VERBAL;
        else
            TipoFrase = FRASE_FIN;

        switch(TipoFrase)
            {
            case FRASE_VERBAL:
                {
                POSITION ini=nodo,fin=nodo;
                FraseVerbal += ';';
                FraseVerbal += ' ';
                FraseVerbal += w->m_Orig;
                for(GetNextPos(nodo);nodo != NULL;)
                    {
                    w = GetWordAt(nodo);   // Palabra que se analiza 
                    if(EnFraseVerbal.Find(w->m_sTipo) != -1)
                        {
                        fin = nodo;
                        FraseVerbal += ' ';
                        FraseVerbal += w->m_Orig;
                        GetNextPos(nodo);
                        }
                    else
                        break;
                    }
				LPCWORD wtmp = (LPCWORD) new CWord;
				ASSERT(wtmp != NULL);
				wtmp->m_sTipo = "FV";
				nodo = MakeArrayFrase(wtmp,ini,fin);
                ///delete wtmp;
                GetNextPos(nodo);
                break;
                }
            case INTRO_VERBAL:
                {
                FraseSust += ';';
                FraseSust += ' ';
                FraseSust += w->m_Orig;

                FraseVerbal += ';';

                GetNextPos(nodo);
                POSITION ini=nodo,fin=nodo;
                for(;nodo != NULL;)
                    {
                    w = GetWordAt(nodo);   // Palabra que se analiza 
                    if(EnFraseVerbal.Find(w->m_sTipo) != -1||InicioFraseVerbal.Find(w->m_sTipo) != -1)
                        {
                        fin = nodo;
                        FraseVerbal += ' ';
                        FraseVerbal += w->m_Orig;
                        GetNextPos(nodo);
                        }
                    else
                        break;
                    }
				LPCWORD wtmp = (LPCWORD) new CWord;
				ASSERT(wtmp != NULL);
				wtmp->m_sTipo = "FV";
				nodo = MakeArrayFrase(wtmp,ini,fin);
                ////delete wtmp;
                GetNextPos(nodo);
                break;
                }
            case INTRO_SER_ESTAR:
                {
                FraseSust += ';';
                FraseSust += ' ';
                FraseSust += w->m_Orig;

                FraseVerbal += ';';

                GetNextPos(nodo);
                POSITION ini=nodo,fin=nodo;
                for(;nodo != NULL;)
                    {
                    w = GetWordAt(nodo);   // Palabra que se analiza 
                    if(EnFraseSerEstar.Find(w->m_sTipo) != -1 || InicioSerEstar.Find(w->m_sTipo) != -1)
                        {
                        fin = nodo;
                        FraseVerbal += ' ';
                        FraseVerbal += w->m_Orig;
                        GetNextPos(nodo);
                        }
                    else
                        break;
                    }
				CWord *wtmp = (CWord *) new CWord;
				ASSERT(wtmp != NULL);
				wtmp->m_sTipo = "FB";
				nodo = MakeArrayFrase(wtmp,ini,fin);
                //////delete wtmp;
                GetNextPos(nodo);
                break;
                }
            case INTRO_VERBO_AUXILIAR:
                {
                FraseVerbal += ';';
                FraseVerbal += ' ';
                FraseVerbal += w->m_Orig;

                FraseVerbal += ';';

                GetNextPos(nodo);
                POSITION ini=nodo,fin=nodo;
                for(;nodo != NULL;)
                    {
                    w = GetWordAt(nodo);   // Palabra que se analiza 
                    if(EnFraseSerEstar.Find(w->m_sTipo) != -1 || InicioSerEstar.Find(w->m_sTipo) != -1)
                        {
                        fin = nodo;
                        FraseVerbal += ' ';
                        FraseVerbal += w->m_Orig;
                        GetNextPos(nodo);
                        }
                    else
                        break;
                    }
				CWord *wtmp = (CWord *) new CWord;
				ASSERT(wtmp != NULL);
				wtmp->m_sTipo = "FB";
				nodo = MakeArrayFrase(wtmp,ini,fin);
                //////delete wtmp;
                GetNextPos(nodo);
                break;
                }
            case INTRO_COMPLEX_TIME:
                {
                FraseSust += ';';
                FraseSust += ' ';
                FraseSust += w->m_Orig;

                FraseVerbal += ';';

                GetNextPos(nodo);
                POSITION ini=nodo,fin=nodo;
                for(;nodo != NULL;)
                    {
                    w = GetWordAt(nodo);   // Palabra que se analiza 
                    if(EnFraseComplexTime.Find(w->m_sTipo) != -1 || InicioComplexTime.Find(w->m_sTipo) != -1)
                        {
                        fin = nodo;
                        FraseVerbal += ' ';
                        FraseVerbal += w->m_Orig;
                        GetNextPos(nodo);
                        }
                    else
                        break;
                    }
				CWord *wtmp = (CWord *) new CWord;
				ASSERT(wtmp != NULL);
				wtmp->m_sTipo = "FC";
				nodo = MakeArrayFrase(wtmp,ini,fin);
                ////////delete wtmp;
                GetNextPos(nodo);
                break;
                }
            case FRASE_SER_ESTAR:
                {
                POSITION ini=nodo,fin=nodo;
                FraseVerbal += ';';
                FraseVerbal += ' ';
                FraseVerbal += w->m_Orig;
                for(GetNextPos(nodo);nodo != NULL;)
                    {
                    w = GetWordAt(nodo);   // Palabra que se analiza 
                    if(EnFraseSerEstar.Find(w->m_sTipo) != -1)
                        {
                        fin = nodo;
                        FraseVerbal += ' ';
                        FraseVerbal += w->m_Orig;
                        GetNextPos(nodo);
                        }
                    else
                        break;
                    }
				CWord *wtmp = (CWord *) new CWord;
				ASSERT(wtmp != NULL);
				wtmp->m_sTipo = "FB";
				nodo = MakeArrayFrase(wtmp,ini,fin);
                /////delete wtmp;
                GetNextPos(nodo);
                break;
                }
            case FRASE_COMPLEX_TIME:
                {
                POSITION ini=nodo,fin=nodo;
                FraseVerbal += ';';
                FraseVerbal += ' ';
                FraseVerbal += w->m_Orig;
                for(GetNextPos(nodo);nodo != NULL;)
                    {
                    w = GetWordAt(nodo);   // Palabra que se analiza 
                    if(EnFraseComplexTime.Find(w->m_sTipo) != -1)
                        {
                        fin = nodo;
                        FraseVerbal += ' ';
                        FraseVerbal += w->m_Orig;
                        GetNextPos(nodo);
                        }
                    else
                        break;
                    }
				CWord *wtmp = (CWord *) new CWord;
				ASSERT(wtmp != NULL);
				wtmp->m_sTipo = "FC";
				//CWord *wtmp3  = (CWord *)m_Ora.GetAt(ini);
				//wtmp3  = (CWord *)m_Ora.GetAt(fin);
				nodo = MakeArrayFrase(wtmp,ini,fin);
                ////delete wtmp;
                GetNextPos(nodo);
                break;
                }
            case FRASE_SUST:
                {
                POSITION ini=nodo,fin=nodo;
                FraseSust += ';';
                FraseSust += ' ';
                FraseSust += w->m_Orig;

                if(PalabraSolaEsFraseSust.Find(w->m_sTipo) == -1)
                    {
                    BOOL sust = TRUE;
                    for(GetNextPos(nodo);nodo != NULL;)
                        {
                        if(sust == TRUE && ProximoEsSustReal.Find(w->m_sTipo) != -1)
                            sust = TRUE;
                        else
                            sust = FALSE;

                        w = GetWordAt(nodo);   // Palabra que se analiza 
		                POSITION nodo2 = nodo;
		                POSITION nodo3 = NULL;

                        if(sust == TRUE)
                            {
                            FraseSust += ' ';
                            FraseSust += w->m_Orig;
                            fin = nodo;
                            GetNextPos(nodo);
                            sust = FALSE;
                            continue;
                            }

		                GetNextPos(nodo2);

                        CWord *w2 = NULL;
                        CWord *w3 = NULL;
		                if(nodo2 != NULL)
                            {
                            w2 = GetWordAt(nodo2);   // Palabra n2 de la que se analiza 
		                    nodo3 = nodo2;

		                    GetNextPos(nodo3);
                            w3 = NULL;
		                    if(nodo3 != NULL)
                                {
                                w3 = GetWordAt(nodo3);   // Palabra n3 de la que se analiza 
                                }
                            }


                        if(EnFraseSust.Find(w->m_sTipo) != -1 || (w2 != NULL && EnFraseSustSi2EnFraseSust.Find(w->m_sTipo) != -1 && EnFraseSust.Find(w2->m_sTipo) != -1 ))
                            {
                            if(w2 != NULL)
                                if(InicioFraseVerbal.Find(w->m_sTipo) != -1 && InicioFraseSustQuePideVerbo.Find(w2->m_sTipo) != -1) // si la proxima palabra es articulo, etc es verbo y no es sustantivo Ej: The man plays the piano
                                    break;
                            fin = nodo;
                            FraseSust += ' ';
                            FraseSust += w->m_Orig;
                            GetNextPos(nodo);
                            }
                        else
                            {
                            break;
                            }
                        }
                    }
				if(ini == fin && w->m_sTipo == "XX")
					{
					continue;
					}
				if(w->m_sTipo == "XX")
					{
					continue;
					}
				CWord *wtmp = (CWord *) new CWord;
				ASSERT(wtmp != NULL);
				wtmp->m_sTipo = "FS";
				nodo = MakeArrayFrase(wtmp,ini,fin);
                //OJO//// delete wtmp;
                GetNextPos(nodo);
                break;
                }
            case FRASE_FIN:
            default:
                GetNextPos(nodo);
                break;
            }
        //PEEKMESSAGE();
        }
    //GetPtrSentence()->QuitaXX();

//	CFindWordSentenceInDict objFindWordSentenceInDict(GetPtrTranslationData(),GetPtrSentence());

//	objFindWordSentenceInDict.Process();
    }               
/////////////////////////////////////////////////////////////////////////////
