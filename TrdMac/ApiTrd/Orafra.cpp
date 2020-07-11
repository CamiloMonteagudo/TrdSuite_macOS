////////////////////////////////////////////////////////////////////////////
//   Class COracion                                                       //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se implementa class para el trabajo con la oración                    // 
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <ctype.h>
#include "UtilFun.h"
#include "COra.h"
#include "LengDic.h"
#include "PeekMsg.h"
                        
/////////////////////////////////////////////////////////////////////////////

void COracion::Serialize(CArchive &ar)
    {         
    CMFCObject::Serialize(ar);

    if (ar.IsStoring())
        {
        /*ar << m_Orig;
        ar << m_Dest;
        ar << m_iCantWord;
        ar << m_iCantUppWord;
        ar << m_iCantLwrWord;
        ar << (WORD)m_Modo.Get();
        ar << (WORD)m_Case.Get();
        ar << (WORD)m_Genero.Get();
        ar << (WORD)m_Numero.Get();
        ar << (WORD)m_Persona.Get();
        ar << (WORD)m_Tiempo.Get();
        ar << (WORD)m_FirstWordUpper.Get();
        ar << (WORD)m_Voz.Get();
        ar << (WORD)m_PrimeraPasada.Get();
        ar << (WORD)m_InsSigno;*/
        //m_Ora.Serialize(ar);
        }
    else
        {
        WORD tmp;
        
        ar >> m_Orig;
        ar >> m_Dest;
        ar >> m_iCantWord;
        ar >> m_iCantUppWord;
        ar >> m_iCantLwrWord;
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
        m_FirstWordUpper = (BOOL)tmp;
        ar >> tmp;
        m_Voz = (BOOL)tmp;
        ar >> tmp;
        m_PrimeraPasada = (BOOL)tmp;
        ar >> tmp;
        m_InsSigno = (InsertarSigno)tmp;
        //m_Ora.Serialize(ar);
        }
    }


/////////////////////////////////////////////////////////////////////////////
// intercambia los patrones                                                //
/////////////////////////////////////////////////////////////////////////////
#define LA 50

void COracion::IntercambiaPatron(   
    POSITION nodoini,     // nodo de inicio del patron 
    int l,                // longitud del patron 
    LPCSTR patnewpos)// cadena la nueva posición
    {
    CWord *w[LA];         // arreglo de puntero para cambio de posicion 
    POSITION nodo = NULL; // nodo en que se trabaja 
    int   newpos[LA];     // arreglo de las nuevas posiciones 
    int  i = 0;           // contador de lazo 

    ASSERT(nodoini != NULL );
    ASSERT(l < LA );
    //ASSERT(patnewpos != NULL);
    
    for (i = 0,  nodo = nodoini;  i < l;  i++, m_Ora.GetNext(nodo))
        {
        ASSERT(nodo != NULL);
        newpos[i] = patnewpos[i] - '0' - 1;
        if(newpos[i] > -1 && newpos[i] < LA )
            {
            w[i] = (CWord *)m_Ora.GetAt(nodo);
            ASSERT(w[i] != NULL);
            }
        ASSERT(newpos[i] > -1 && newpos[i] < LA );
        }
    for (i = 0,  nodo = nodoini;  i < l;  i++, m_Ora.GetNext(nodo))
        {
        m_Ora.SetAt(nodo,w[newpos[i]]);
        }
    }

/////////////////////////////////////////////////////////////////////////////
// Busca patrones dentro de una oracion 
/////////////////////////////////////////////////////////////////////////////

void COracion::FindPatrones(CDictLoad *ptrDic)      /*Translate->m_Idioma->DPatrones*/
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT(ptrDic != NULL);
    
    char patron = ' ';  // patron de la palabra 
    int i = -1;         // indice para patron 
    char spatron[250];  // cadena para guardar patrones 
    bool bFin = FALSE;          // si se llegó a fin de oración

    
    PoneXX();

	//TRACE("Buscando Patrones ");
	POSITION nodo    = NULL;  // nodo indice  
    POSITION nodoini = NULL;  // nodo inicial 
    CWord *w = NULL;
    
    for (nodoini = nodo = m_Ora.GetHeadPosition(); nodoini != m_Ora.GetTailPosition() && nodoini != NULL;)
        {
        //ASSERT(nodo != NULL);
        ASSERT(nodoini != NULL);
        
        if(nodo != NULL && bFin == FALSE)
            {
            w  = (CWord *)m_Ora.GetAt(nodo);
            ASSERT(w != NULL);
            patron = w->m_Patron;
            }
        else
            patron = ' ';

        if (i == -1 && patron != ' ')
            {
            i = 0;
            nodoini = nodo;
            spatron[i] = patron;
            }
        else if (i != -1)
            {
            if (i != -1 && patron != ' ')
              spatron[++i] = patron;
            else
              {
              BOOL f = FALSE;     // si encontró patron o no
              spatron[++i] = '\0';
              if (i > 1)
                {
                do  
									{
                  LPCSTR szIntercambio;
                        
                  f = ptrDic->GetKey(spatron, szIntercambio);
                  if (f == TRUE )
                    {
										POSITION nodoini2 = nodoini;
										//if(i > 1 && nodo != nodoini)
										//		m_Ora.GetPrev(nodo);
                    for( UINT j = 0; j < strlen(spatron); j++)
                      m_Ora.GetNext(nodoini2);
                    IntercambiaPatron(nodoini, i, szIntercambio);
#ifdef _DEBUG
                            CWord *w = (CWord *)m_Ora.GetAt(nodoini);
                            ASSERT(w != NULL);
                            w->m_sPatron = spatron;
#endif
										if(nodoini2 != NULL)
                      m_Ora.GetPrev(nodoini2);
										nodo = nodoini = nodoini2;
#ifdef _DEBUG
                            w = (CWord *)m_Ora.GetAt(nodoini);
                            ASSERT(w != NULL);
#endif
                    }
                  else
                    {   
                    if(nodo != NULL)
                      m_Ora.GetPrev(nodo);
                    spatron[--i] = '\0';
                    }
                  }
                while (f == FALSE && i > 1);
								}

              if (nodo != NULL)  
                {  /// ojo quité comentario
	              if(nodo != nodoini && nodo != m_Ora.GetHeadPosition() && i > 0 && f == FALSE && /*nodo != m_Ora.GetHeadPosition() &&*/ nodo != nodoini)
									m_Ora.GetPrev(nodo);
								if(f == FALSE)
									nodoini = nodo;
                }
              else
                break;

              i = -1;
              }
            }
        else
            {
            i = -1;
            nodoini = nodo;
            }

        if (nodo != NULL && nodo != m_Ora.GetTailPosition())
            {
            m_Ora.GetNext(nodo);
            bFin = FALSE;
            }
        else
            bFin = TRUE;    
        PEEKMESSAGE();
        }
    QuitaXX();
    }

/////////////////////////////////////////////////////////////////////////////
