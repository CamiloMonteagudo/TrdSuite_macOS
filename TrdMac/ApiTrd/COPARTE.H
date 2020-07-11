////////////////////////////////////////////////////////////////////////////
//   Class COParte                                                        //
//   enum  OParte                                                         //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         15/10/93                        //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el tratamiento de las partes // 
//  las palabras en la oración                                            //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__OParte__)
#define __OParte__

enum OParte
	 {
	 opSUJETO,
     opVERBO,
     opCD,
     opCI,
     opCCT,
	 opCCM,
     opCCL
     };

class COParte
	{   
	protected:
	
	OParte m_OParte;
	
	public:
	
	COParte()                  // Inicia OParte con opSUJETO como implícito
		{
		m_OParte = opSUJETO;
		}
		
	COParte(OParte c)        
		{
		m_OParte = c;
		}
		
	void Set(OParte g)         // Pone OParte
	 	{
	 	ASSERT( g >= opSUJETO && g <= opCCL);
	 	m_OParte = g;
	 	}
	 	
	void Set(COParte g)         // Pone OParte
	 	{
	 	m_OParte = g.m_OParte;
	 	}
	 	
	COParte& operator=(OParte g)	
		{            
		Set(g);
		return *this;
		}
		
	COParte& operator=(COParte g)	
		{            
		Set(g);
		return *this;
		}

	OParte Get()               // Retorna el OParte
		{
	 	ASSERT( m_OParte >= opSUJETO && m_OParte <= opCCL);
		return m_OParte;
		}
		
	BOOL operator==(OParte g)	
		{            
	 	ASSERT( g >= opSUJETO && g <= opCCL);
		return (m_OParte == g);
		}
		
	BOOL operator!=(OParte g)	
		{            
	 	ASSERT( g >= opSUJETO && g <= opCCL);
		return (m_OParte != g);
		}                    
		
	BOOL operator==(COParte &g)	
		{            
		return (m_OParte == g.Get());
		}
		
	BOOL operator!=(COParte &g)	
		{            
		return (m_OParte != g.Get());
		}

	};
#else	
	#pragma message ("Fichero <<COParte.h>> incluido en más de una ocasión")
#endif 
