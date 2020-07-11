////////////////////////////////////////////////////////////////////////////
//   Class COracion                                                       //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se implementa class para el trabajo con las funciones de oración      // 
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <ctype.h>
#include "UtilFun.h"
#include "COra.h"
#include "LengDic.h"
#include "PeekMsg.h"
#include "ViewFlag.h"
//#include "viewword.h"
                        
////////////////////////////////////////////////////////////////////////////
// Variables externas utilizadas                                          //
////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Retorna 1ra palabra de la oración                                       //
/////////////////////////////////////////////////////////////////////////////

CWord *COracion::GetFirstWord()
    {                          
    ASSERT(m_Ora.IsEmpty() == FALSE);
    CWord *w  = (CWord *)m_Ora.GetHead();
    ASSERT( w != NULL);
	if(w->m_sTipo == "XX")
		{
		POSITION n = m_Ora.GetHeadPosition();
		m_Ora.GetNext(n);
		if(n != NULL)
			{
		    w  = (CWord *)m_Ora.GetAt(n);
		    ASSERT( w != NULL);
			}
		}
    return (w);
    }

/////////////////////////////////////////////////////////////////////////////
// Retorna Ultima palabra de la oración                                    //
/////////////////////////////////////////////////////////////////////////////

CWord *COracion::GetLastWord()
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);
    CWord *w  = (CWord *)m_Ora.GetTail();
    ASSERT( w != NULL);
	if(w->m_sTipo == "XX")
		{
		POSITION n = m_Ora.GetTailPosition();
		m_Ora.GetPrev(n);
		if(n != NULL)
			{
		    w  = (CWord *)m_Ora.GetAt(n);
		    ASSERT( w != NULL);
			}
		}
    return (w);
    }

/////////////////////////////////////////////////////////////////////////////
// Retorna Próxima parte de la oración ( Sujeto, CD, CI, CCL, CCT )        //
/////////////////////////////////////////////////////////////////////////////

OParte COracion::GetNextParte(POSITION n)   // nodo de Palabra en que nos encontramos 
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT(n != NULL);
    
    CWord *w  = (CWord *)m_Ora.GetAt(n);
    ASSERT(w != NULL);

    COParte p(w->m_OParte);  // parte en que nos encontramos 
    COParte pp(w->m_OParte); // proxima parte de la Oración  
         
    POSITION nodo = n;    
    for ( m_Ora.GetNext(nodo); nodo != NULL; m_Ora.GetNext(nodo))
        {
        ASSERT(nodo != NULL);                  
        
        CWord *w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);

        pp = w->m_OParte;
        if (pp.Get() != p.Get())
            break;
        }
    return (pp.Get());
    }

/////////////////////////////////////////////////////////////////////////////
// Retorna Last parte de la oración ( Sujeto, CD, CI, CCL, CCT )           //
/////////////////////////////////////////////////////////////////////////////

OParte COracion::GetLastParte(
    POSITION n)              // nodo de Palabra en que nos encontramos 
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT(n != NULL);
    CWord *w  = (CWord *)m_Ora.GetAt(n);
    ASSERT(w != NULL);

    COParte p(w->m_OParte);  // parte en que nos encontramos 
    COParte pp(w->m_OParte); //  parte anterior de la Oración  
         
    POSITION nodo = n;    
    for ( m_Ora.GetPrev(nodo); nodo != NULL; m_Ora.GetPrev(nodo))
        {
        ASSERT(nodo != NULL);                  
        
        CWord *w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);

        pp = w->m_OParte;
        if (pp.Get() != p.Get())
            break;
        }
    return (pp.Get());
    }

/////////////////////////////////////////////////////////////////////////////
// Set parte de la oración ( Sujeto, CD, CI, CCL, CCT )                    //
/////////////////////////////////////////////////////////////////////////////

void COracion::SetParte(       
    POSITION ini,               // nodo inicial  
    POSITION _fin,              // nodo final 
    OParte p)                   // Parte a setear 
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT(ini != NULL);
    ASSERT(_fin != NULL);                     
    
    POSITION fin = _fin;
    m_Ora.GetNext(fin);
    
    for (POSITION nodo=ini; nodo!=NULL && nodo!=fin; m_Ora.GetNext(nodo))
        {
        ASSERT(nodo != NULL);
        CWord *w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);

        w->m_OParte = p;
        }
    }

/////////////////////////////////////////////////////////////////////////////
// Set Gen a varios nodos                                                  //
/////////////////////////////////////////////////////////////////////////////

void COracion::SetLGen(         
    POSITION ini,               // nodo inicial  
    POSITION _fin,              // nodo final 
    Genero p)                   // Genero a setear 
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT(ini != NULL);
    ASSERT(_fin != NULL);                     
    
    POSITION fin = _fin;
    m_Ora.GetNext(fin);
    
    for (POSITION nodo=ini; nodo!=NULL && nodo!=fin; m_Ora.GetNext(nodo))
        {
        ASSERT(nodo != NULL);
        CWord *w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);

        w->m_Genero = p;
        }
    }

/////////////////////////////////////////////////////////////////////////////
// Set Num a varios nodos                                                  //
/////////////////////////////////////////////////////////////////////////////

void COracion::SetLNum(         
    POSITION ini,               // nodo inicial  
    POSITION _fin,              // nodo final 
    Numero p)                   // Numero a setear 
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT(ini != NULL);
    ASSERT(_fin != NULL);                     
    
    POSITION fin = _fin;
    m_Ora.GetNext(fin);
    
    for (POSITION nodo=ini; nodo!=NULL && nodo!=fin; m_Ora.GetNext(nodo))
        {
        ASSERT(nodo != NULL);
        CWord *w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);

        w->m_Numero = p;
        }
    }
    
/////////////////////////////////////////////////////////////////////////////
// Retorna verbo de la oración                                             //
/////////////////////////////////////////////////////////////////////////////

CWord *COracion::GetVerbo()
    {           
    CWord *w = NULL;
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT(m_VerIni != NULL && m_VerFin != NULL);
    
    POSITION fin = m_VerFin;
    m_Ora.GetNext(fin);
    
    ASSERT(fin != NULL);
    

    for (POSITION nodo = m_VerIni; 
         nodo != NULL && nodo != fin; 
         m_Ora.GetNext(nodo))
        {
        ASSERT(nodo != NULL);
        w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);
        //dt = W_GetParseType(w);

        //if (w->m_ParseType.Get()== dtVTRANSITIVO 
        // || w->m_ParseType.Get() == dtVINTRANSITIVO)
//        if(Translate->m_Idioma->TipoVerbo.Find(w->m_sTipo) != -1)
//            break;
//        else
//            w = NULL;
        }
    return (w);
    }

/////////////////////////////////////////////////////////////////////////////
// Retorna TRUE si la una palabra de la lista [s] existe a la derecha 
/////////////////////////////////////////////////////////////////////////////

BOOL COracion::ExistRight(POSITION actual,const CStringA& s,BOOL tipo)
    {           
    CWord *w = NULL;
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT(actual != NULL);
    BOOL ret = FALSE;
    POSITION nodo = actual;
    
    CStringA coma(",");
    CStringA wherefind = coma + s + coma;       // pone la cadena en que se busca entre comas
    
    for (m_Ora.GetNext(nodo);
         nodo != NULL; 
         m_Ora.GetNext(nodo))
        {
        ASSERT(nodo != NULL);
        w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);     
        CStringA tmp;
        if (tipo ==FALSE)
            tmp = w->m_Origlwr;
        else
            tmp = w->m_sTipo;
        CStringA wfind = coma + tmp + coma;   // pone la palabra entre comas
        if( wherefind.Find(wfind) != -1)
            {
            ret = TRUE;
            break;
            }
        }
    return (ret);
    }

/////////////////////////////////////////////////////////////////////////////
// Retorna TRUE si la una palabra de la lista [s] existe a la Izquierda
/////////////////////////////////////////////////////////////////////////////

BOOL COracion::ExistLeft(POSITION actual,const CStringA& s,BOOL tipo)
    {           
    CWord *w = NULL;
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT(actual != NULL);
    BOOL ret = FALSE;
    POSITION nodo = actual;    
    
    CStringA coma(",");
    CStringA wherefind = coma + s + coma;       // pone la cadena en que se busca entre comas
    
    for (m_Ora.GetPrev(nodo);
         nodo != NULL; 
         m_Ora.GetPrev(nodo))
        {
        ASSERT(nodo != NULL);
        w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);
        CStringA tmp;
        if (tipo ==FALSE)
            tmp = w->m_Origlwr;
        else
            tmp = w->m_sTipo;
        CStringA wfind = coma + tmp + coma;   // pone la palabra entre comas
        if( wherefind.Find(wfind) != -1)
            {
            ret = TRUE;
            break;
            }
        }
    return (ret);
    }

/////////////////////////////////////////////////////////////////////////////
// Pone valor de la palabra anterior que no sea adverbio y sea de [tipo]   //
// especial para poner valor al verbo be de ser o estar
/////////////////////////////////////////////////////////////////////////////

void COracion::IsLastNotDDSetByType(POSITION actual,const CStringA& tipo,const CStringA& s,int morf, const CStringA& ptrAdverbialType)
    {           
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT(actual != NULL);
    //ASSERT(tipo != NULL);
    //ASSERT(s != NULL);
    //ASSERT(ptrAdverbialType != NULL);
    
    //CStringA t(tipo);
    CWord *w = LastNotType(actual,ptrAdverbialType);                   

    if(w != NULL)
        {
        if( tipo.Find(w->m_sTipo) != -1)
            {
            w->m_StrReserva1 = w->m_Trad = s;
            w->m_Traducida = TRUE;
            if(morf > -1)
                w->m_Morf = morf;     
            }
        }
    }
/////////////////////////////////////////////////////////////////////////////
// Pone valor de la palabra anterior que no sea adverbio y sea = word      //
/////////////////////////////////////////////////////////////////////////////

void COracion::IsLastNotDDSetByWord(POSITION actual,const CStringA& word,const CStringA& s,int morf, const CStringA& ptrAdverbialType)
    {           
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT(actual != NULL);
    //ASSERT(word != NULL);
    //ASSERT(s != NULL);
    CWord *w = LastNotType(actual,ptrAdverbialType);
            
    if(w != NULL)
        {             
        CStringA coma(",");
        CStringA wfind = coma + w->m_Origlwr + coma;   // pone la palabra entre comas
        CStringA wherefind = coma + word + coma;       // pone la cadena en que se busca entre comas
        
        if(wherefind.Find(wfind) != -1)
            {
            w->m_StrReserva1 = w->m_Trad = s;
            w->m_Traducida = TRUE;
            if(morf > -1)
                w->m_Morf = morf;     
            }
        }
    }

/////////////////////////////////////////////////////////////////////////////
// Retorna primera palabra hacia la izquierda que no es Tipo s             //
/////////////////////////////////////////////////////////////////////////////

CWord *COracion::LastNotType(POSITION nodo,const CStringA& tipo)
    {           
    CWord *w = NULL;
    ASSERT(m_Ora.IsEmpty() == FALSE);
    //ASSERT( s != NULL);
    
    if(nodo == NULL)    
        return NULL;
        
    //CStringA tipo(s);
    
    for (m_Ora.GetPrev(nodo);
         nodo != NULL; 
         m_Ora.GetPrev(nodo))
        {
        ASSERT(nodo != NULL);
        w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);
        if(w != NULL)
            {
            if(tipo.Find(w->m_sTipo) == -1)
                break;
            }    
        else
            break;    
        }

    if(nodo != NULL)
        {
        w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);
        }
    else 
        w = NULL;   
    return (w);
    }

/////////////////////////////////////////////////////////////////////////////
// Retorna primera palabra hacia la derecha que no es advervio           
/////////////////////////////////////////////////////////////////////////////

CWord *COracion::NextNotType(POSITION nodo,const CStringA& tipo)
    {           
    CWord *w = NULL;
    ASSERT(m_Ora.IsEmpty() == FALSE);
    //ASSERT( s != NULL);
    
    if(nodo == NULL)    
        return NULL;
    //CStringA tipo(s);

    for (m_Ora.GetNext(nodo);
         nodo != NULL; 
         m_Ora.GetNext(nodo))
        {
        ASSERT(nodo != NULL);
        w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);
        if(tipo.Find(w->m_sTipo) == -1)
            break;
        }

    if(nodo != NULL)
        {
        w  = (CWord *)m_Ora.GetAt(nodo);
        ASSERT(w != NULL);
        }
    else
        w = NULL;   
    return (w);
    }
/////////////////////////////////////////////////////////////////////////////
// Retorna palabra n de la oración hacia la derecha (next)(to = TRUE) o hacia la 
// Izquierda (prev) (to = FALSE)
/////////////////////////////////////////////////////////////////////////////

CWord *COracion::GetXNode(  
    POSITION n,              // nodo actual 
    WORD x,
    BOOL to)
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT( n != NULL);
    CWord *w  = NULL;
     
    for(int i=0;i < x; i++)       
        {
        if(n != NULL)
            {
            if(to == TRUE)
                m_Ora.GetNext(n);
            else
                m_Ora.GetPrev(n);   
            }
        }   
    if( n != NULL)
        w  = (CWord *)m_Ora.GetAt(n);
    return w;
    }

/////////////////////////////////////////////////////////////////////////////
// Retorna nodo x de la oración hacia la derecha (next)(to = TRUE) o hacia la 
// Izquierda (prev)(to = FALSE)
/////////////////////////////////////////////////////////////////////////////

POSITION COracion::GetXPosition(  
    POSITION n,              // nodo actual 
    WORD x,
    BOOL to)
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);
    ASSERT( n != NULL);
     
    for(int i=0;i < x; i++)       
        {
        if(n != NULL)
            {
            if(to == TRUE)
                m_Ora.GetNext(n);
            else
                m_Ora.GetPrev(n);   
            }
        }   

    return n;
    }

/////////////////////////////////////////////////////////////////////////////
// Set palabra w en el campo Trad y en campo morfología                    //
/////////////////////////////////////////////////////////////////////////////

void COracion::SetWordTradMorf(     
    CWord *w,                 // nodo actual    
    const CStringA& s,            // valor a setear 
    int morf)                // valor de morfologia 0 si no es necesario
    {
    ASSERT(m_Ora.IsEmpty() == FALSE);
    //ASSERT( s != NULL);     
    
    if (w != NULL)
        {
        w->m_StrReserva1 = w->m_Trad = s;
        w->m_Traducida = TRUE;
        if(morf > -1)
            w->m_Morf = morf;
        }
    }

/////////////////////////////////////////////////////////////////////////////
