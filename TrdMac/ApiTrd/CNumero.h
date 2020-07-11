////////////////////////////////////////////////////////////////////////////
//   Class CNumero                                                        //
//   enum  Numero                                                         //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         7/10/93                         //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el tratamiento del Número de // 
//  las palabras en la oración                                            //
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
	CNumero()                  // Inicia Número con SINGULAR como implícito
		{
		m_Num = SINGULAR;
		}
		
	CNumero(Numero n)   
		{
	 	ASSERT( n >= SINGULAR && n <= PLURAL);
		m_Num = n;
		}
		
	void Set(Numero g)         // Pone Número
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

	Numero Get()               // Retorna el Número
		{
	 	ASSERT( m_Num >= SINGULAR && m_Num <= PLURAL);
		return m_Num;
		}
	};
#endif 
