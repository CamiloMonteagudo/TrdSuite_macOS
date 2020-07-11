////////////////////////////////////////////////////////////////////////////
//   Class CGrado                                                    //
//   enum  Grado                                                     //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         12/12/99                        //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el tratamiento del grado     // 
//  de los adjetivos.                                                     //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__GRADO__)
#define __GRADO__

#include "ApiConj.h"

//enum Grado{POSITIVO,COMPARATIVO,SUPERLATIVO};

class CGrado
	{   
	protected:
	
	Grado m_Grado;
	
	public:
	
	CGrado()                  // Inicia Grado con POSITIVO como implícito
		{
		m_Grado = POSITIVO;
		}
		
	CGrado(Grado c)        
		{
		m_Grado = c;
		}
		
	void Set(Grado g)         // Pone Declinación
	 	{
	 	ASSERT( g >= POSITIVO && g <= SUPERLATIVO);
	 	m_Grado = g;
	 	}
	 	
	void Set(CGrado g)         // Pone Grado al adjetivo
	 	{
	 	m_Grado = g.m_Grado;
	 	}
	 	
	CGrado& operator=(Grado g)	
		{            
		Set(g);
		return *this;
		}
		
	CGrado& operator=(CGrado g)	
		{            
		Set(g);
		return *this;
		}

	Grado Get()               // Retorna el Grado del adjetivo
		{
	 	ASSERT( m_Grado >= POSITIVO && m_Grado <= SUPERLATIVO);
		return m_Grado;
		}

    bool operator==(Grado g)  
        {            
	 	ASSERT( m_Grado >= POSITIVO && m_Grado <= SUPERLATIVO);
        return (m_Grado == g);
        }
        
    bool operator!=(Grado g)  
        {            
	 	ASSERT( m_Grado >= POSITIVO && m_Grado <= SUPERLATIVO);
        return (m_Grado != g);
        }

    bool operator==(CGrado g) 
        {            
        return (m_Grado == g.Get());
        }
        
    bool operator!=(CGrado g) 
        {            
        return (m_Grado != g.Get());
        }

	};
#endif 
