////////////////////////////////////////////////////////////////////////////
//   Class CArticleType                                                    //
//   enum  ArticleType                                                     //
//                                                                        //
//   -------------------------------------------------------------------  //
//   Programado por:    F. Suárez         12/12/99                        //
//   -------------------------------------------------------------------  //
//                                                                        //
//                                                                        //
//	Se define e implementa class y enum para el trabajo con el tipo de    //
//  articulo para realizar la declinación de adjetivos del alemán         //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#if !defined(__ARTICLE_TYPE__)
#define __ARTICLE_TYPE__

//enum ArticleType{aNULL,aDEFINITE,aINDEFINITE};

class CArticleType
	{   
	protected:
	
	ArticleType m_ArticleType;
	
	public:
	
	CArticleType()                  // Inicia ArticleType con aNULL como implícito
		{
		m_ArticleType = aNULL;
		}
		
	CArticleType(ArticleType c)        
		{
		m_ArticleType = c;
		}
		
	void Set(ArticleType g)         // Pone Tipo de articulo usado con el adjetivo
	 	{
	 	ASSERT( g >= aNULL && g <= aINDEFINITE);
	 	m_ArticleType = g;
	 	}
	 	
	void Set(CArticleType g)         // Pone Declinación
	 	{
	 	m_ArticleType = g.m_ArticleType;
	 	}
	 	
	CArticleType& operator=(ArticleType g)	
		{            
		Set(g);
		return *this;
		}
		
	CArticleType& operator=(CArticleType g)	
		{            
		Set(g);
		return *this;
		}

	ArticleType Get()               // Retorna el ArticleType
		{
	 	ASSERT( m_ArticleType >= aNULL && m_ArticleType <= aINDEFINITE);
		return m_ArticleType;
		}

    bool operator==(ArticleType g)  
        {            
	 	ASSERT( m_ArticleType >= aNULL && m_ArticleType <= aINDEFINITE);
        return (m_ArticleType == g);
        }
        
    bool operator!=(ArticleType g)  
        {            
	 	ASSERT( m_ArticleType >= aNULL && m_ArticleType <= aINDEFINITE);
        return (m_ArticleType != g);
        }

    bool operator==(CArticleType g) 
        {            
        return (m_ArticleType == g.Get());
        }
        
    bool operator!=(CArticleType g) 
        {            
        return (m_ArticleType != g.Get());
        }
	};
#endif 
