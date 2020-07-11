////////////////////////////////////////////////////////////////////////////
//   Class CWordEsp                                                       //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//  Se implementa class para el trabajo con la Especialidades             // 
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UtilFun.h"
#include "COra.h"


////////////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CWordEsp,CObject,0);
////////////////////////////////////////////////////////////////////////////

void CWordEsp::Serialize(CArchive &ar)
    {
    CMFCObject::Serialize(ar);
    
    if (ar.IsStoring())
        {
        /*ar << m_Esp;
        ar << m_Word;        
        ar << (WORD)m_Genero.Get();
        ar << (WORD)m_Numero.Get();
        ar << (WORD)m_Morf;*/
        }
    else
        {
        WORD tmp;

        ar >> m_Esp;
        ar >> m_Word;        
        ar >> tmp;
        m_Genero.Set((Genero)tmp);
        ar >> tmp;
        m_Numero.Set((Numero)tmp);
        ar >> tmp;
        m_Morf = (short int)tmp;
        
        }
    }          
    
