/*
 ============================================================================
 Name		: DictData.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CDictData implementation
 ============================================================================
 */

#include "DictData.h"


#define MEAN_SINTAXIS_ERROR   -1
#define MEAN_END              -2
#define MEAN_NEXT              1

TInt CDictData::ParseTxt(const TDictData& aData)
	{
  Clear();                                  // Limpia todos los datos

	int numTypes = 0;

  int iFind = aData.Find("#E");
  if (iFind >= 0)
		{
		numTypes = SetDat( aData.Mid(iFind+2), aData.Mid(2,iFind-2) );

		// detectar si es un keyword o no
		if (numTypes > 0)
			{
			// cualquier tipo que no sea de estos califica como keyword
			LPCSTR sTypesOk = "SS,AA,NP,DD,VT,VI,VR,VA,PP,PT,PI,GT,GI,CC,JJ,AI";
			CStringA sType;
			for(int i=0; i<GetTypesCount(); ++i)
				{
				get_TypeAt(i, sType);
				if (strstr(sTypesOk, sType.c_str()) == NULL)
					{
					iIsKeyword = true;
					break;
					}
				}
			}
		}
  
  if( numTypes == 0 && !aData.IsEmpty() )
    iIsKeyword = true;

//	if (numTypes == 0 && !aData.IsEmpty())
//		{
//		// si no hay tipos gramaticales y el texto que define la palabra no esta vacio, entonces 
//		// es una palabra clave, los significados se buscan de los textos que esten entre comillas
//		Clear();
//
//		iIsKeyword = true;
//		AddType("KK");
//
//		CStringA csData(aData);
//
//    // eliminar lo que esta dentro de parentesis
//    RemoveParenthesis(csData);
//    
//		CStringA sAddedMeans;
//		CStringA sPending = csData;
//    TInt pos = sPending.Find('{');
//    if (pos == -1) // no hay llaves
//			ParseQuotes(sPending, sAddedMeans);
//    else
//			{ // si hay llaves, procesar solo el texto que este entre llaves
//	    while(true)
//	    	{
//		    pos = sPending.Find('{');
//		    if (pos == -1)
//		    	break;
//		    
//	    	sPending = sPending.Mid(pos + 1);
//	      pos = sPending.Find('}');
//	      if (pos == -1)
//	      	break;
//	
//	     	CStringA sText = sPending.Left(pos);
//	     	sPending = sPending.Mid(pos + 1);
//	      
//				ParseQuotes(sText, sAddedMeans);
//	    	}
//			}
//
//		numTypes = 1;
//		}

  return numTypes;
	}

BOOL CDictData::GetDataText(TDictData& aData)
	{
	aData.Empty();
  TDictData Data;
  TGramLongType cType;

  if( !GetDat( Data, cType ) ) return FALSE;
    
  aData.Append(_L8("#T"));
  aData.Append( cType);
  aData.Append( _L8("#E"));
  aData.Append( Data);
  
  return FALSE;
	}

void CDictData::ParseQuotes(CStringA aDataText, CStringA& aAddedMeans)
	{
  int pos = -1;
	int nQuotes = 0;

	// calcular el numero de comillas que sera el numero de significados
  while(true)
  	{
		pos = aDataText.Find('\"', pos + 1);
		if (pos == -1)
			break;

		pos = aDataText.Find('\"', pos + 1);
		if (pos == -1)
			break;

		nQuotes++;
		}

	if (nQuotes <= 0)
		return;

	int idxMean = 0;

  while(true)
  	{
		pos = aDataText.Find('\"');
		if (pos == -1)
			break;
		
		aDataText = aDataText.Mid(pos + 1);
		pos = aDataText.Find('\"');
		if (pos == -1)
			break;
	
		idxMean++;

		CStringA sMean = aDataText.Left(pos);
		aDataText = aDataText.Mid(pos + 1);

    sMean.Trim();
    if (!sMean.IsEmpty())
      { // no adicionar tipos vacios
      CStringA sDecoratedMean = "[&" + sMean + "&]";
      if (aAddedMeans.Find(sDecoratedMean) == -1)
        {
        aAddedMeans += sDecoratedMean;
			
        AddMean(0, sMean.c_str());
        }
      }
  	}
	}

void CDictData::RemoveParenthesis(CStringA& aDataText)
	{
  int pClose = 0;
  int pOpen = 0;
  
  // buscar el primer parentesis que cierra
  while ((pClose = aDataText.Find(')')) > 0)
  	{
  	// ir hacia atras buscando el primero que abre
   	pOpen = pClose - 1;
   	while (pOpen >= 0 && aDataText[pOpen] != '(')
   		pOpen--;
   	
   	if (pOpen >= 0)
   		// borrar el texto y los parentesis
   		aDataText.Delete(pOpen, pClose - pOpen + 1);
   	else // no se encontro el parentesis que cierra, terminar
   		break;
   	}
	}

TInt CDictData::MeanCount(const CStringA& aType)
	{
  int idx = GetIdxType(aType);
  if( idx != -1 )
    return MeanCountAt(idx);
  
  return 0;
	}

TInt CDictData::MeanCountAt(TInt aIdxType)
	{
  if( aIdxType>=0 && aIdxType<iNTypes )
    return GetNMeans( aIdxType );
  else
    return 0;
	}

void CDictData::get_ActualCmds(CStringA& aCmds)
	{
  if( iIdxType>=0 && iIdxType<iNTypes )
  	aCmds = iTypes[iIdxType].iCmds;
  else
  	aCmds.Empty();
	}

void CDictData::get_ActualMean(CStringA& aMean)
	{
  if( iIdxType>=0 && iIdxType < iNTypes && 
      iIdxMean>=0 && iIdxMean < GetNMeans(iIdxType) )
  	aMean = iTypes[iIdxType].iMean[iIdxMean].isMean;
  else
  	aMean.Empty();
	}

void CDictData::get_ActualEsp(TGramEsp& aEsp)
	{
  if( iIdxType>=0 && iIdxType < iNTypes && 
      iIdxMean>=0 && iIdxMean < GetNMeans(iIdxType) )
  	aEsp = iTypes[iIdxType].iMean[iIdxMean].iEsp;
  else
  	aEsp.Empty();
	}

TText CDictData::get_ActualGen()
	{
  if( iIdxType<0 || iIdxType >= iNTypes || 
  		iIdxMean<0 || iIdxMean >= GetNMeans(iIdxType) )
    return 0;

  BYTE n = iTypes[iIdxType].iMean[iIdxMean].iGen;

  return (n==1)? 'f' : ((n==2)? 'n' : 'm' );
	}

TText CDictData::get_ActualNum()
	{
  if( iIdxType<0 || iIdxType >= iNTypes || 
      iIdxMean<0 || iIdxMean >= GetNMeans(iIdxType) )
    return 0;

  BYTE n = iTypes[iIdxType].iMean[iIdxMean].iNum;

  return (n==1)? 'p' : 's';
	}

TText CDictData::get_ActualRef()
	{
  if( iIdxType<0 || iIdxType >= iNTypes || 
      iIdxMean<0 || iIdxMean >= GetNMeans(iIdxType) )
    return 0;

  BYTE n = iTypes[iIdxType].iMean[iIdxMean].iRef;

  return (n==1)? 'y' : 'n';
	}

TText CDictData::get_ActualTerm()
	{
  if( iIdxType<0 || iIdxType >= iNTypes || 
      iIdxMean<0 || iIdxMean >= GetNMeans(iIdxType) )
    return 0;

  BYTE n = iTypes[iIdxType].iMean[iIdxMean].iTerm;

  return (n==1)? 'y' : 'n';
	}

/*------------------------------------------------------------------------------------*/
//<!----------------------------------------------------------------------------------->
TInt CDictData::get_ActualTypeIdx()
	{
  return iNTypes;
	}

/*------------------------------------------------------------------------------------*/
// Establece el tipo gramatical 'Idx' como tipo actual, si 'Idx' no esta entre 0 y el
// número de tipos introducidos no hace nada
//<!----------------------------------------------------------------------------------->
void CDictData::put_ActualTypeIdx(TInt aIdx)
	{
  if( aIdx>=0 && aIdx<=iNTypes )
    {
    if( aIdx==iNTypes && iTypes[aIdx].iTipo.Length() == 0)
      return;

    iIdxType = aIdx;
    }
	}

/*------------------------------------------------------------------------------------*/
//<!----------------------------------------------------------------------------------->
TInt CDictData::get_ActualMeanIdx()
	{
  return iIdxMean;
	}

/*------------------------------------------------------------------------------------*/
// Dentro del tipo activo, establece el significado con indice 'IdxMean'como activo, 
// si 'IdxMean' esta fuera de rango o no hay tipo activo no hace nada
//<!----------------------------------------------------------------------------------->
void CDictData::put_ActualMeanIdx(TInt aIdxMean)
	{
  if( iIdxType>=0 && iIdxType<iNTypes )
    {
    if( aIdxMean>=0 && aIdxMean < iTypes[iIdxType].iMean.GetCount() )
      {
      iIdxMean = aIdxMean;
      }
    }
	}

/*------------------------------------------------------------------------------------*/
// Obtiene el código de dos letras que representa al tipo actual, si no se puede 
// establecer tipo actual, retorna cadena vacia.
//<!----------------------------------------------------------------------------------->
void CDictData::get_ActualType(CStringA& aType)
	{
	aType.Empty();
	
  if( iIdxType>=0 && iIdxType<iNTypes )
    aType = iTypes[iIdxType].iTipo;
	}

void CDictData::get_TypeAt(int aIdx, CStringA& aType)
	{
	put_ActualTypeIdx(aIdx);
	get_ActualType(aType);
	}

void CDictData::Clear()
	{
  for( int i=0; i<MAX_TYPES; ++i )
    {
    iTypes[i].iTipo.Empty();         // Borra el tipo
    iTypes[i].iCmds.Empty();         // Borra los comandos
    iTypes[i].iMean.RemoveAll();		// Borra todos los significados  
    }

	iIsKeyword = false;
  iNTypes = 0;                     // Pone el número de tipos a 0
	}

int CDictData::SetDat( const CStringA& aData, const CStringA& aType )
	{
  int Cond = 0;
  int len  = aData.Length();
  
  TGramType cType(aType.Left(2));

  Clear();                                  // Limpia todos los datos

  for( int i=0; i<len; )
    {                                     
    if( aData[i]=='"' )                      // Parse para "...",....,"..."
      {   
      if( iTypes[iNTypes].iTipo.Length() == 0)
        {
        if( !GetDeafultType( cType ) )      // Determina el tipo por defecto
          return 0;
        }

      if( !CopyData( aData, aType, i ) )     // Copia para un tipo
        return 0;
      
      if( Cond==0 && i>=aData.Length() )        // Final cuando no condicion
        return iNTypes;                    // Retorna una sola traduccion

      // Verifica final de segunda fase de la condicion                                              
      if( Cond==3 && aData[i]=='}' && i+1==aData.Length() ) 
        return iNTypes;                    // Retorna # de traduccioines
                                                    
      // Verifica final de primera fase de la condicion                                              
      if( Cond==1 && aData[i]=='}' && aData[i+1]==':' )  
        {     
        Cond = 2;
        i += 2;                             // Salta }:
        continue;                           // Continua analizando
        } 
        
      return 0;                             // Hubo un error
      }
    else
      { 
      if( (Cond==0 || Cond==2) && IsCondition( aData, i ) )    
        {
        Cond = 1;                       // Fase inicial de la condicion
        continue;                       // Lee significados
        }
        
      if( Cond==2 && aData[i]=='{' )     // Abre segunda fase de la condicion
        {
        Cond = 3;                       // Fase final de la condicion
        ++i;                            // Salta llave
        continue;                       // Lee significados
        } 
      
      return 0;                         // Hubo error
      }        
    }    
    
  return 0;                               // Hubo error, no termino de analizar
	}

BOOL CDictData::GetDat( TDictData& aData, CStringA& aType )
	{
	aData.Preallocate(1024);

  if( iNTypes<=0 )                        // Verifica qe existe información
    return FALSE; 
  
  if( !GetFullType( aType ) )
    return FALSE; 

  for( int i=0; i<iNTypes; ++i ) 
    {
    if( i>0 ) aData += ':'; 

    if( iNTypes>1 )
      {
      if( i<iNTypes-1 )
      	{
      	aData.Append(_L8("(W="));
      	aData.Append(iTypes[i].iTipo);
      	aData.Append(_L8(")?"));
      	}
//      else
//        Data += ':' ; 

      aData += '{'; 
      }

    int nMeans = (int)iTypes[i].iMean.GetCount();  // Obtiene el numero de significados

    for( int j=0; j<nMeans ; ++j )                  // Coje todos los significado
      {
      //TGramEsp Esp =  iTypes[i].iMean[j].iEsp;    // Coje especialidad
      CString Esp(iTypes[i].iMean[j].iEsp);         // Coje especialidad
      
      if( Esp.Length()!=2 ) Esp = _L8("GG");          // Si no hay, pone GG

      if( j>0 )																				// Si no es el primer significado
        aData += '@' + Esp ;                         // Adiciona la especilidad

      aData += '"';                                       // Abre comilla
      aData += iTypes[i].iMean[j].isMean;               // Pone significado
      aData += '"';                                       // Cierra comilla

      if( iTypes[i].iMean[j].iNum    ) aData += '$';    // Pone plurar
      if( iTypes[i].iMean[j].iGen==1 ) aData += '*';    // Pone femenino
      if( iTypes[i].iMean[j].iGen==2 ) aData += '-';    // Pone neutro
      if( iTypes[i].iMean[j].iRef    ) aData += '!';    // Pone reflexivo

      if( j<nMeans-1 ) aData += ',';                // Pone coma si no es el ultimo
      }         
      
    if( iTypes[i].iCmds.Length() > 0 )              // Si hay comandos
    	{
    	aData += ',';          							    // Los agrega al final
    	aData += iTypes[i].iCmds;            	  // Los agrega al final
    	}

    if( iNTypes>1 ) aData += '}';                  // Cierra llave, si mas de un tipo
    }                                               
  
  return TRUE;
  }

//---------------------------------------------------------------------------
// Esta funcion copia desde el indice 'i' de 'Data' a 'Str' todos los 
// caracteres desde el caracter inicial de 'Data' que debe ser '"' hasta un 
// caracter terminador que puede ser Null, o }, la cantidad de caracteres 
// maximos a copiar es 127, esta funcion tambien chequea que la cantidad de
// caracteres '"' esten macheados, si una de las dos condiciones anteriores
// no se cumple esta funcion returna -1, en otro caso retorna el indice al 
// ultimo caracter analizado ( caracter terminador ).
//---------------------------------------------------------------------------

BOOL CDictData::CopyData( const CStringA& aData, const CStringA& aType, int &i )
	{
  int UnMacth = 0;                        // 1- " Abierto 0- " Cerrado
 
  if( iNTypes<0 || iNTypes>=MAX_TYPES )  // Pueden ser hasta 4 traducciones 
    return FALSE;
       
  int j=i;
  for( ; i<aData.Length() && aData[i]!='}'; ++i )  // Busca hasta el final
    if( aData[i]=='"' ) UnMacth ^= 1;          // Registra el macheo de "
      
  if( UnMacth || j==i )                       // Chequea si hubo error
    return FALSE;
		
  if( !GetMeans( aData, aType, j, i-((i<aData.Length())? 1:0) ) )     // Coje todos los significados
    return FALSE;

  ++iNTypes;                                // Aumenta numero de tipos  
  return TRUE;
	}

//----------------------------------------------------------------------------------------
// Esta funcion obtiene el tipo por defecto basandose en la lista de definicion de tipos 
// y los tipos determinados hasta ese momento.
//----------------------------------------------------------------------------------------
BOOL CDictData::GetDeafultType( const TGramType& Type ) 
  {                    
  CComplexTypes Tipos;                                // Objeto para manejar tipos

  for( int j=0; j<iNTypes; ++j )                      // Recorre todos los tipos 
    Tipos.AddSingleType( iTypes[j].iTipo );          	// Los adiciona al objeto

  TGramType sType;
  Tipos.GetDeafultType(sType, Type );        					// Busca tipos que falta
  if( sType.Length() == 0)                            // No lo pudo encontar
    return FALSE;

  iTypes[iNTypes].iTipo = sType;                    	// Asigna tipo por defecto

  return TRUE;                                        // Todo OK
  }

//---------------------------------------------------------------------------
// Esta funcion determina si la cadena 's' a partir del caracter 'i'
// tiene el formato (W=??)?{ en ese caso retorna TRUE, los caracteres ??
// son copiados a 'sType' y el indice 'i' es adelantado hasta el caracter
// siguiente al patron antes señalado.
//---------------------------------------------------------------------------

BOOL CDictData::IsCondition( const CStringA& s, int &j )
	{
  if( iNTypes<0 || iNTypes>=MAX_TYPES )  
    return FALSE;
    
  if( s.Length() < j+8 )       // No quedan las letras necesarias
    return FALSE;
               
  int k = 0;
               
  // Verifica el patron (W=??)?{                   
  if( s[j]=='(' || s[j+1]=='W' || s[j+2]=='=' || s[j+5]==')' )
		{
		k = 6;
	
		if( s[j+6]==' ' )
	      ++k;
	
		if( s[j+k]!='?' || s[j+k+1]!='{'  )
		  return FALSE;     
		}
  else
  	return FALSE;     
       
  TGramType Tipo(s.Mid(j+3, 2));
  
  //Tipo[0]= s[j+3]; Tipo[1]= s[j+4];// Tipo[2]= '\0';

  iTypes[iNTypes].iTipo = Tipo;
            
  j += k + 2;
                
  return TRUE;
	}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
BOOL CDictData::GetMeans( const CStringA& aData, const CStringA& aType, int ini, int fin )
  {

  for(;;)
    {
    int Ret = AddMean( aData, aType, ini, fin );

    if( Ret == MEAN_SINTAXIS_ERROR )
      return FALSE;

    if( Ret == MEAN_END )
      {
      if( ini<fin )  // El resto son los comandos
        iTypes[iNTypes].iCmds = aData.Mid(ini, fin-ini);// CString( aData+ini, fin-ini );
      break;
      }
    }

  return TRUE;
  }


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int CDictData::AddMean( const CStringA& aData, const CStringA& aType, int &i, int fin )
	{
  CMean Mean;

//  int Idx = (int)m_Types[m_nTypes].m_Mean.GetSize();  // Numero del significado     
  int lenData = aData.Length();

  if( i<lenData && aData[i] == '"' /*&& Idx==0*/ )                    // Si empieza con " 
    {
    ++i;                                                // Salta "
    }
  else if( i+4<fin && aData[i]=='@' && aData[i+3]=='"' )  // Verifica que empiece con @??"
    {
    Mean.iEsp = aData.Mid(i+1, 2);// CString( &aData[i+1], 2 );              // Copia la especialidad

    i +=4 ;                                             // Salta @??"

    // Determina un termino de esa especialidad
    //CStringA sTerm;
    //sTerm.Format(";%s:%s", Mean.iEsp., &iTypes[iNTypes].iTipo); // Forma cadena
    CString sTerm = ";" + Mean.iEsp + ":" + iTypes[iNTypes].iTipo ; // Forma cadena

    //Mean.iTerm = ( strstr( aType, sTerm ) != NULL );   // Esta en el tipo?... es termino
    Mean.iTerm = (aType.Find(sTerm) != -1);           // Esta en el tipo?... es termino
    }
  else if( i+2<fin && aData[i]=='@' && aData[i+1]=='[' )  // Verifica que empiece con @[
    {
    for( ; i<fin && i<lenData && aData[i]!=']'; ++i ) {};
	  if( i<lenData && aData[i]==']' && aData[i+1]=='"' )
	    i +=2;
    }
  else
    return MEAN_END;                                    // Puede ser un comando
  
  // Busca la palabra hasta que encuentre la " final.    
  int j=i;
  for(;j<fin && j<lenData && aData[j]!='"'; ++j ) {};

  if( j>=lenData || aData[j]!='"' )                     // Si termino en " y la salta
    return MEAN_SINTAXIS_ERROR;                         // Retorna error, no termino en "

  Mean.isMean = aData.Mid(i, j-i );              				// Copia el significado

  i = j+1;                                              // Salta la comilla final

  // Coje la marca de genero, numero y reflexivo    
  for( j=0; j<3 && i<lenData && (aData[i]=='$' || aData[i]=='*' || aData[i]=='-'  || aData[i]=='!'); ++j )
    { 
    if( aData[i]=='$' ) Mean.iNum=1;
    if( aData[i]=='*' ) Mean.iGen=1;
    if( aData[i]=='-' ) Mean.iGen=2;
    if( aData[i]=='!' ) Mean.iRef=1;

    ++i;    
    }

  if( i<lenData && aData[i]==',' ) ++i;                               // Si tremino en , la saltar.

  iTypes[iNTypes].iMean.Add( Mean );                 // Adiciona el significado     
                                      
  return MEAN_NEXT;                                       // Retorna buscar proxima acepcion
	}

BOOL CDictData::GetComplexType( CStringA& aType )
	{
  CComplexTypes Tipos;

  for( int j=0; j<iNTypes; ++j )   
    Tipos.AddSingleType( iTypes[j].iTipo );
  
  return Tipos.GetComplexType( aType );
	}

BOOL CDictData::GetFullType( CStringA& aType )
	{
	aType.Empty();

  CStringA cType;
  if (!GetComplexType( cType ))
    return FALSE; 

  aType = cType;

  for( int i=0; i<iNTypes; ++i ) 
    for( int j=0; j<iTypes[i].iMean.GetCount() ; ++j ) 
      {
      if( iTypes[i].iMean[j].iTerm )
        {
        CStringA* Esp = &iTypes[i].iMean[j].iEsp;

        if( Esp->Length()==2 )
        	{
          aType += _L8(';');
          aType += *Esp;
          aType += _L8(':');
          aType += iTypes[i].iTipo;
        	}
        }
      }

  return TRUE;
  }

int CDictData::GetIdxType( const CStringA& aType )
	{
  for( int i=0; i<iNTypes; ++i )
    if( iTypes[i].iTipo == aType)
      return i;

  return -1;
	}

CStringA* CDictData::GetSingleType( int aNType )
	{
  ASSERT( aNType>=0 && aNType<iNTypes );

  return &iTypes[aNType].iTipo;
	}

CStringA* CDictData::GetTypeCmds( int aNType )
	{
  ASSERT( aNType>=0 && aNType<iNTypes );

  return &iTypes[aNType].iCmds;
	}

CStringA* CDictData::GetMean( int aNType, int aNMean )
	{
  ASSERT( aNMean>=0 && aNMean<GetNMeans(aNType) );

  return &iTypes[aNType].iMean[aNMean].isMean;
	}

CStringA* CDictData::GetEsp( int aNType, int aNMean )
	{
  ASSERT( aNMean>=0 && aNMean<GetNMeans(aNType) );

  return &iTypes[aNType].iMean[aNMean].iEsp;
	}

BOOL CDictData::GetGen( int aNType, int aNMean )
	{
  ASSERT( aNMean>=0 && aNMean<GetNMeans(aNType) );

  return iTypes[aNType].iMean[aNMean].iGen != 0;
	}

BOOL CDictData::GetNum( int aNType, int aNMean )
	{
  ASSERT( aNMean>=0 && aNMean<GetNMeans(aNType) );

  return iTypes[aNType].iMean[aNMean].iNum != 0;
	}

BOOL CDictData::GetRef( int aNType, int aNMean )
	{
  ASSERT( aNMean>=0 && aNMean<GetNMeans(aNType) );

  return iTypes[aNType].iMean[aNMean].iRef != 0;
	}

BOOL CDictData::GetTerm( int aNType, int aNMean )
	{
  ASSERT( aNMean>=0 && aNMean<GetNMeans(aNType) );

  return iTypes[aNType].iMean[aNMean].iTerm != 0;
	}

void CDictData::SetType( int nType, LPCSTR aType)
  {
  //ASSERT( nMean>=0 && nMean<GetNMeans(nType) );
  
  iTypes[nType].iTipo = aType;
  }

void CDictData::SetMean( int nType, int nMean, LPCSTR Mean )
  {
  ASSERT( nMean>=0 && nMean<GetNMeans(nType) );

  iTypes[nType].iMean[nMean].isMean = Mean;
  }

void CDictData::SetEsp ( int nType, int nMean, LPCSTR Esp  )
  {
  ASSERT( nMean>=0 && nMean<GetNMeans(nType) );

  iTypes[nType].iMean[nMean].iEsp = Esp;
  }

void CDictData::SetGen ( int nType, int nMean, int Gen )
  {
  ASSERT( nMean>=0 && nMean<GetNMeans(nType) );

  iTypes[nType].iMean[nMean].iGen = Gen;
  }

void CDictData::SetNum ( int nType, int nMean, BOOL Num )
  {
  ASSERT( nMean>=0 && nMean<GetNMeans(nType) );

  iTypes[nType].iMean[nMean].iNum = Num;
  }

void CDictData::SetRef ( int nType, int nMean, BOOL Ref )
  {
  ASSERT( nMean>=0 && nMean<GetNMeans(nType) );

  iTypes[nType].iMean[nMean].iRef = Ref;
  }

void CDictData::SetTerm( int nType, int nMean, BOOL Term )
  {
  ASSERT( nMean>=0 && nMean<GetNMeans(nType) );

  iTypes[nType].iMean[nMean].iTerm = Term;
  }
	
//---------------------------------------------------------------------------------------
// Adiciona el tipo 'sType' a los datos, si el tipo ya de encuentra retorna el indice
// al tipo que ya esta (no lo adiciona), si no es posible adicionar el tipo retorna -1
// y si se puede adicionar retorna el inidice donde se adiciono (ultimo elemento).
//---------------------------------------------------------------------------------------
int CDictData::AddType( LPCSTR sType )
  {
  CComplexTypes Tipos;

  int i;
  for( i=0; i<iNTypes; ++i )
    {
    if( iTypes[i].iTipo == sType )
      return i;

    Tipos.AddSingleType( iTypes[i].iTipo );
    }

  Tipos.AddSingleType( sType );

  if( !Tipos.IsGoodTypes() )
    return -1;

  iTypes[i].iTipo = sType;
  
  if (i == iNTypes)
    iNTypes++; 
  return i;
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int CDictData::AddMean( int nType, LPCSTR sMean )
  {
  ASSERT( nType>=0 && nType<=iNTypes );

  if( nType == iNTypes )
    {
    if( iTypes[nType].iTipo.IsEmpty() )
      return -1;

    ++iNTypes;
    }

  CMean Mean;

  Mean.isMean = sMean;

  return (int)iTypes[nType].iMean.Add( Mean );     // Adiciona el significado     
  }
  
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void CDictData::DelMean( int nType, int nMean )
  {
  ASSERT( nMean>=0 && nMean<GetNMeans(nType) );

  iTypes[nType].iMean.RemoveAt(nMean);   // Borra el significados  
  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void CDictData::DelType( int nType )
  {
  ASSERT( nType>=0 && nType<iNTypes );
  
  int i;
  for( i=nType; i<iNTypes-1; ++i )
    CpyType( i, this, i+1 );

  for( ; i < MAX_TYPES; ++i )
    {
    iTypes[i].iTipo = "";         
    iTypes[i].iCmds = "";         
    iTypes[i].iMean.RemoveAll();  
    }

  --iNTypes;                       
  }  

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void CDictData::CpyType( int nType1, CDictData *Dat, int nType2 )
  {
  CType &Type1 =      iTypes[nType1];
  CType &Type2 = Dat->iTypes[nType2];

  Type1.iTipo = Type2.iTipo;         
  Type1.iCmds = Type2.iCmds;         

  Type1.iMean.RemoveAll();

  for( int j=0; j<Type2.iMean.GetSize() ; ++j )         
    {
    Type1.iMean.Add( CMean() );

    CpyMean( nType1, j, Dat, nType2, j );
    }

  }

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void CDictData::CpyMean( int T1, int M1, CDictData *Dat, int T2, int M2 )
  {
  CMean &Mean1 =      iTypes[T1].iMean[M1];
  CMean &Mean2 = Dat->iTypes[T2].iMean[M2];

  Mean1.isMean = Mean2.isMean;
  Mean1.iEsp   = Mean2.iEsp;
  Mean1.iGen   = Mean2.iGen;
  Mean1.iNum   = Mean2.iNum;
  Mean1.iRef   = Mean2.iRef;
  Mean1.iTerm  = Mean2.iTerm;
  }

/*------------------------------------------------------------------------------------*/
// Determina si el tipo simple 'sType' puede ser adicionado como un tipo nuevo a la 
// lista de datos, esta funcion retorna el indice donde se agrearia el tipo, existen
// 3 situaciones distintas segun el valor de retorno.
//     Ret = -1        El tipo no puede ser adicionado
//     Ret < nTipos    El tipo ya se encuentra en los datos
//     Ret = nTipos    Es un tipo nuevo (Se adiciona al final de la lista)
//
// Si lo que se quiere es sustituir un tipo por otro se puede usar el arfumento 'Idx'
// para indicar que el tipo con ese indice no se tenga en cuenta, en otro caso ese 
// parametro debe ser igual a -1.
//<!----------------------------------------------------------------------------------->
int CDictData::CanAddType(LPCSTR sType, int Idx)
  {
  int ret = -1;
  CString newType( sType );
  CComplexTypes Tipos;
  
  int i;
  for( i=0; i<iNTypes; ++i )
    {
    if( iTypes[i].iTipo == newType )
      {
      return i;
      }
    
    if (i != Idx)
      Tipos.AddSingleType( iTypes[i].iTipo );
    }
  
  Tipos.AddSingleType( newType );
  
  if( Tipos.IsGoodTypes() )
    ret = i;
  
  return ret;
  }
  

bool CDictData::MoveMean(int idxType, int idxFrom, int idxTo)
  {
  if( idxType<0 || idxType>=iNTypes)
    return false;
    
  int nMeans = GetNMeans(idxType);
  
  if( idxTo <  0 ) idxTo = 0;
  if( idxTo >= nMeans ) idxTo = nMeans-1;
  
  if (idxFrom < 0 || idxFrom >= nMeans || idxFrom == idxTo)
    return false;
  
  CArray<CMean> &Means = iTypes[idxType].iMean;
  
  CMean Now;
  Now.Copy(Means[idxFrom]);
  
  Means.RemoveAt(idxFrom);
  Means.InsertAt(idxTo, Now);
  
  return true;
  }

