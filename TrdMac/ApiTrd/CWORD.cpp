////////////////////////////////////////////////////////////////////////////
//   Class CWord                                                          //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se implementa class para el trabajo con la Palabras                   // 
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UtilFun.h"
#include "COra.h"
#include "LengDic.h"
#include "idGrep.h"
#include "TranslationData.h"

IMPLEMENT_SERIAL(CWord,CObject,0);

////////////////////////////////////////////////////////////////////////////
//   Patrones para el intercambio de palabras
//
//   A   Adj Movible   
//   B   Adverbio     
//   C   Nombre Propio      
//   D   Adj Determinativo
//   E   Adj que precede al artículo (ALL) 
//   F   Adverbio de frecuencia 
//   G   gerundio
//   H   Auxiliar
//   I   Adj Indefinido
//   J   Posesivo
//   K   participio
//   L   Verbo Infinitivo
//   M   Numeral
//   N   Negación
//   O   Ordinal
//   P   Preposicion
//   Q   Pronombre
//   R   Artículo
//   S   Sustantivo
//   T   Adj Cuantitativo
//   U   Pronombre Reflexivo
//   V   Verbo
//   W   Adj Posesivo
//   X   Adj Inamovible
//   Y   Conjunción
//   Z   Adj Demostrativo
//   0   Pronombre Enfatico
//   1   OF
//   -   Guión
//   ,   Coma
//   /   Slash
//  ' '  (blanco)sin patrón
//   ?   interrogación
//   !   admiración
//   2   Verbo auxiliar
// 
   
/////////////////////////////////////////////////////////////////////////////
//  copy una palabra en otra                                               //
/////////////////////////////////////////////////////////////////////////////

CWord &CWord::CopyWord(CWord &w)
    {
	m_Orig      = w.m_Orig;
    m_Origlwr   = w.m_Origlwr;
    m_Dest      = w.m_Dest;
	m_Trad      = w.m_Trad;
    m_Key       = w.m_Key;
	m_Data      = w.m_Data;
    m_DataC     = w.m_DataC;
    m_MinMay    = w.m_MinMay;
    m_sTipo     = w.m_sTipo;
    m_sTipo2    = w.m_sTipo2;
    m_sPatron   = w.m_sPatron;
    m_sComodin  = w.m_sComodin;
    m_sPrefijo  = w.m_sPrefijo;
    m_sSufijo   = w.m_sSufijo;
    m_sSufijo   = w.m_sKeyPrefijo;
    m_sSufijo   = w.m_sKeySufijo;
    m_OfAs      = w.m_OfAs;
    m_Prep      = w.m_Prep;
    m_EntreParentesis = w.m_EntreParentesis;
	m_sInsert   = w.m_sInsert;
	m_sAdd      = w.m_sAdd;
    m_sParticle = w.m_sParticle;
    m_sSemantica =w.m_sSemantica;
    m_EntreQue  = w.m_EntreQue;
	m_AltMeaning	= w.m_AltMeaning;

	m_RtfFijoIni    = w.m_RtfFijoIni;
	m_RtfFijoFin    = w.m_RtfFijoFin;
    
    m_Modo        = w.m_Modo;
    m_Case        = w.m_Case;
    m_Genero      = w.m_Genero;
    m_Numero      = w.m_Numero;
    m_Persona     = w.m_Persona;
    m_Tiempo      = w.m_Tiempo;
    m_DiWord      = w.m_DiWord;
    m_DicType     = w.m_DicType;
    m_OParte      = w.m_OParte;
    m_Declination = w.m_Declination;
    m_ArticleType = w.m_ArticleType;
    m_Grade       = w.m_Grade;

	m_Articulo  = w.m_Articulo;

	m_Adiciona      = w.m_Adiciona;

//    m_ParseType = w.m_ParseType;
//    m_WordType  = w.m_WordType;

	m_NoBusca   = w.m_NoBusca;
    m_Buscada   = w.m_Buscada;
    m_BuscadaC  = w.m_BuscadaC;
    m_Traducida = w.m_Traducida;
    m_Delete    = w.m_Delete;
    m_Plural    = w.m_Plural;
    m_Romano    = w.m_Romano;
    m_EntreComillas = w.m_EntreComillas;
    m_Posesivo  = w.m_Posesivo;
    m_TradTipo  = w.m_TradTipo;
    m_Reflexivo = w.m_Reflexivo;
    m_LastPrepNoArt = w.m_LastPrepNoArt;        
    m_Femenino = w.m_Femenino;
    m_Abreviatura = w.m_Abreviatura;
    m_NotArtInOf = w.m_NotArtInOf;
    m_ArtAtFirst = w.m_ArtAtFirst;

    m_Negado    = w.m_Negado;
    m_Compuesto = w.m_Compuesto;

	m_NoEnComodin = w.m_NoEnComodin;
	m_Concordada  = w.m_Concordada;

	m_PosFraseComodin = w.m_PosFraseComodin;
    m_SustDerivado  = w.m_SustDerivado;
    m_Reserva2      = w.m_Reserva2;
    m_Reserva3      = w.m_Reserva3;
    m_Reserva4      = w.m_Reserva4;
    m_Reserva5      = w.m_Reserva5;
    m_StrReserva1   = w.m_StrReserva1;
    m_Frases        = w.m_Frases;
    m_FrasesTipo    = w.m_FrasesTipo;
    m_StrFormat     = w.m_StrFormat;
    m_StrReserva5   = w.m_StrReserva5;
    m_StrSaveTrad   = w.m_StrSaveTrad;
	m_StrIniBlank   = w.m_StrIniBlank;
    m_Frecuencia    = w.m_Frecuencia;
	m_TipoTrd       =  w.m_TipoTrd;
	m_EspTrd        = w.m_EspTrd;
	m_AddKey        = w.m_AddKey;
	m_AddData        = w.m_AddData;


	m_Mascara   = w.m_Mascara;
    m_MascaraC  = w.m_MascaraC;
    m_Morf      = w.m_Morf;
    m_Patron    = w.m_Patron;
    m_Prefijo   = w.m_Prefijo;

    
    for(int i = 0; i < w.m_AcepArray.GetSize(); i++)
        m_AcepArray.Add(new CWordEsp((CWordEsp *)w.m_AcepArray[i]));
        
    for(int i = 0; i < w.m_ComodinArray.GetSize(); i++)
        m_ComodinArray.Add(w.m_ComodinArray[i]);
	return *this;
    }    

/////////////////////////////////////////////////////////////////////////////

void CWord::Dump()
	{
	TRACE("%s|%s|%s|%s|%s", m_Key.c_str(), m_Orig.c_str(), m_Trad.c_str(), m_sTipo.c_str(), m_Data.c_str());
	TRACE("%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", 
		m_Modo.Get(), m_Case.Get(), m_Genero.Get(), m_Numero.Get(), m_Persona.Get(), m_Tiempo.Get(), 
		m_DiWord.Get(), m_ArticleType.Get(), m_Grade.Get(), m_Articulo, m_Traducida, m_Plural);
	}

/////////////////////////////////////////////////////////////////////////////

void CWord::Serialize(CArchive &ar)
    {
    
    CMFCObject::Serialize(ar);
    
    if (ar.IsStoring())
        {
        /*CStringA tmp;
                  
        tmp = m_Orig;          
        ar << tmp;
        tmp = m_Origlwr;
        ar << tmp;
        tmp = m_Dest;
        ar << tmp;
        tmp = m_Trad;
        ar << tmp;
        ar << m_Key;
        ar << m_Data;
        ar << m_DataC;
        ar << m_MinMay;
        ar << m_sTipo;
        ar << m_sTipo2;
        ar << m_sPatron;
        ar << m_sComodin;
        ar << m_sPrefijo;
        ar << m_sSufijo;
        ar << m_OfAs;
        ar << m_Prep;
        
        ar << (WORD)m_Modo.Get();
        ar << (WORD)m_Case.Get();
        ar << (WORD)m_Genero.Get();
        ar << (WORD)m_Numero.Get();
        ar << (WORD)m_Persona.Get();
        ar << (WORD)m_Tiempo.Get();
        ar << (WORD)m_DiWord.Get();
        ar << (WORD)m_ArticleType.Get();
        ar << (WORD)m_Grade.Get();

        //ar << (WORD)m_DicType.Get();
        //ar << (WORD)m_OParte.Get();
        ar << (WORD)m_Articulo;
        ar << (WORD)m_NoBusca.Get();
        ar << (WORD)m_Buscada.Get();
        ar << (WORD)m_BuscadaC.Get();
        ar << (WORD)m_Traducida.Get();
        ar << (WORD)m_Delete.Get();
        ar << (WORD)m_Plural.Get();
        ar << (WORD)m_Romano.Get();
        ar << (WORD)m_EntreComillas.Get();
        ar << (WORD)m_Posesivo.Get();
        ar << (WORD)m_TradTipo.Get();
        ar << (WORD)m_Reflexivo.Get();
        ar << (WORD)m_LastPrepNoArt.Get();
        ar << (WORD)m_Femenino.Get();
        ar << (WORD)m_Abreviatura.Get();
        ar << (WORD)m_NotArtInOf.Get();
        ar << (WORD)m_ArtAtFirst.Get();
        ar <<       m_Mascara;
        ar <<       m_MascaraC;
        ar << (WORD)m_Morf;
        ar << (WORD)m_Patron;
        ar << (WORD)m_Prefijo;
        ar << m_SustDerivado;
        ar << m_Adiciona;
        ar << m_Reserva2;
        ar << m_Reserva3;
        ar << m_Reserva4;
        ar << m_Reserva5;
        ar << m_StrReserva1;
        ar << m_Frases;
        ar << m_FrasesTipo;
        ar << m_StrFormat;
        ar << m_StrReserva5;
        ar << m_Frecuencia;
		ar << m_StrIniBlank;
        m_AcepArray.Serialize(ar);*/
        }
    else
        {
        WORD tmp;
        CStringA stmp;
        
        ar >> stmp;
        m_Orig = stmp;
        ar >> stmp;
        m_Origlwr = stmp;
        ar >> stmp;
        m_Dest = stmp;
        ar >> stmp;
        m_Trad = stmp;
        ar >> m_Key;
        ar >> m_Data;
        ar >> m_DataC;
        ar >> m_MinMay;
        ar >> m_sTipo;
        ar >> m_sTipo2;
        ar >> m_sPatron;
        ar >> m_sComodin;
        ar >> m_sPrefijo;
        ar >> m_sSufijo;
        ar >> m_OfAs;
        ar >> m_Prep;
        
        ar >> tmp;
        m_Modo.Set((Modo)tmp);
        ar >> tmp;
        m_Case.Set((Caso)tmp);
        ar >> tmp;
        m_Genero.Set((Genero)tmp);
        ar >> tmp;
        m_Numero.Set((Numero)tmp);
        ar >> tmp;
        m_Persona.Set((Persona)tmp);
        ar >> tmp;
        m_Tiempo.Set((Tiempo)tmp);
        ar >> tmp;
        m_DiWord.Set((DiWord)tmp);
        //ar >> tmp;
        //m_DicType.Set((DicType)tmp);
        //ar >> tmp;
        //m_OParte.Set((OParte)tmp);
        ar >> tmp;
        m_Articulo = (short int)tmp;
        ar >> tmp;
        m_NoBusca = (BOOL)tmp;
        ar >> tmp;
        m_Buscada = (BOOL)tmp;
        ar >> tmp;
        m_BuscadaC = (BOOL)tmp;
        ar >> tmp;
        m_Traducida = (BOOL)tmp;
        ar >> tmp;
        m_Delete = (BOOL)tmp;
        ar >> tmp;
        m_Plural = (BOOL)tmp;
        ar >> tmp;
        m_Romano = (BOOL)tmp;
        ar >> tmp;
        m_EntreComillas = (BOOL)tmp;
        ar >> tmp;
        m_Posesivo = (BOOL)tmp;
        ar >> tmp;
        m_TradTipo = (BOOL)tmp;
        ar >> tmp;
        m_Reflexivo = (BOOL)tmp;
        ar >> tmp;
        m_LastPrepNoArt = (BOOL)tmp;
        ar >> tmp;
        m_Femenino = (BOOL)tmp;
        ar >> tmp;
        m_Abreviatura = (BOOL)tmp;
        ar >> tmp;
        m_NotArtInOf = (BOOL)tmp;
        ar >> tmp;
        m_ArtAtFirst = (BOOL)tmp;
        ar >> m_Mascara;
        ar >> m_MascaraC;
        ar >> tmp;
        m_Morf = (short int)tmp;
        ar >> tmp;
        m_Patron = (char)tmp;
        ar >>  tmp;
        m_Prefijo = (short int)tmp;
        ar >> m_SustDerivado;
        ar >> m_Adiciona;
        ar >> m_Reserva2;
        ar >> m_Reserva3;
        ar >> m_Reserva4;
        ar >> m_Reserva5;
        ar >> m_StrReserva1;
        ar >> m_Frases;
        ar >> m_FrasesTipo;
        ar >> m_StrFormat;
        ar >> m_StrReserva5;
        ar >> m_Frecuencia;
		ar >> m_StrIniBlank;
        m_AcepArray.Serialize(ar);
        }
        
    }
/////////////////////////////////////////////////////////////////////////////

BOOL CWord::IsInMode(Modo /*m*/)
	{
/********
	BOOL salir = FALSE;
	if(m_Origlwr != m_Key)
		{
		for(int per = PRIMERAP; per <= TERCERAP && salir == FALSE; per++)
			{              
			for(int tiempo = PRESENTE; tiempo <= FUTURO && salir == FALSE; tiempo++)
				{              
				for(int numero = SINGULAR; numero <= PLURAL && salir == FALSE; numero++)
					{              
					CWord w1;
					w1.m_Tiempo = (Tiempo)tiempo;
					w1.m_Modo = m; 
					w1.m_Persona = (Persona)per;
					w1.m_Trad = m_Key;
					w1. m_sTipo = "VV";
					w1.m_Numero = (Numero)numero;
					w1.ConjugaVerbo(w1.m_Trad,Translate->m_Idioma->m_Origen);  // la conjugo según el tiempo
					if(m_Origlwr == w1.m_Trad)
						{
						salir = TRUE;
						break;
						}
					//w1.m_Origlwr = w1.m_Key = w1.m_Orig = w1.m_Trad;    // preparo para buscar en dicc fantasma
					}
				}
			}
		}
	return salir;
************/
	return FALSE;
	}
/////////////////////////////////////////////////////////////////////////////
