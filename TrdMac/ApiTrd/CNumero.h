////////////////////////////////////////////////////////////////////////////
//   Class CNumero                                                        //
//   enum  Numero                                                         //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Su�rez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el tratamiento del N�mero de // 
//  las palabras en la oraci�n                                            //
//                                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__NUMERO__)
#define __NUMERO__

//enum Numero{SINGULAR,PLURAL};

class CNumero
	{   
	protected:
	Numero m_Num;
	
	public:
	CNumero()                  // Inicia N�mero con SINGULAR como impl�cito
		{
		m_Num = SINGULAR;
		}
		
	CNumero(Numero n)   
		{
	 	ASSERT( n >= SINGULAR && n <= PLURAL);
		m_Num = n;
		}
		
	void Set(Numero g)         // Pone N�mero
	 	{
	 	ASSERT( g >= SINGULAR && g <= PLURAL);
	 	m_Num = g;
	 	}
	 	
	void Set(CNumero  g)         // Pone Numero
	 	{
	 	m_Num = g.m_Num;
	 	}
                    
	CNumero& operator=(Numero g)	
		{            
		Set(g);
		return *this;
		}
		
	CNumero& operator=(CNumero  g)	
		{            
		Set(g);
		return *this;
		}
	BOOL operator==(Numero g)	
		{            
	 	ASSERT( g >= SINGULAR && g <= PLURAL);
		return (m_Num == g);
		}
	BOOL operator==(CNumero g)	
		{            
		return (m_Num == g.m_Num);
		}
	BOOL operator!=(Numero g)	
		{            
	 	ASSERT( g >= SINGULAR && g <= PLURAL);
		return (m_Num != g);
		}
	BOOL operator!=(CNumero g)	
		{            
		return (m_Num != g.m_Num);
		}

	Numero Get()               // Retorna el N�mero
		{
	 	ASSERT( m_Num >= SINGULAR && m_Num <= PLURAL);
		return m_Num;
		}
	};
#endif 
