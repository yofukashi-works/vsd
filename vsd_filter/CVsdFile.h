/*****************************************************************************
	
	VSD -- vehicle data logger system  Copyright(C) by DDS
	
	CVsdFile.h - JavaScript File
	
*****************************************************************************/

#pragma once

#include "unzip.h"

class CVsdFile {
  public:
	CVsdFile(){
		m_gzfp	= NULL;
		m_fp	= NULL;
		m_unzfp	= NULL;
		
		m_bZipCurrentFileOpen = FALSE;
	}
	
	~CVsdFile(){
		Close();
	}
	
	int Open( LPCWSTR szFile, LPCWSTR szMode );	// !js_func
	void Close( void ); // !js_func
	char *ReadLine( void ); // !js_func
	int WriteLine( char *str ); // !js_func
	int Seek( int iOffs, int iOrg );	// !js_func
	int IsEOF( void ); // !js_func
	
	v8::Handle<v8::Value> ZipNextFile( void ); // !js_func
	
	// バイナリアクセス
	UCHAR *ReadBin( int iSize );
	int WriteBin( void *pBuf, int iSize );
	
	int ReadChar	( void ){ return *( char *)ReadBin( 1 ); }	// !js_func
	int ReadUChar	( void ){ return *( UCHAR *)ReadBin( 1 ); }	// !js_func
	int ReadShortL	( void ){ return *( short *)ReadBin( 2 ); }	// !js_func
	int ReadUShortL	( void ){ return *( USHORT *)ReadBin( 2 ); } // !js_func
	int ReadIntL	( void ){ return *( int *)ReadBin( 4 ); }	// !js_func
	double ReadUIntL( void ){ return *( UINT *)ReadBin( 4 ); }	// !js_func
	double ReadFloat( void ){ return *( float *)ReadBin( 4 ); }	// !js_func
	double ReadDouble( void ){ return *( double *)ReadBin( 8 ); }	// !js_func
	
	int ReadShortB	( void ){	// !js_func
		UCHAR *p = ReadBin( 2 );
		return ( short )(( p[ 0 ] << 8 ) + p[ 1 ] );
	}
	int ReadUShortB	( void ){	// !js_func
		UCHAR *p = ReadBin( 2 );
		return ( p[ 0 ] << 8 ) + p[ 1 ];
	}
	
	int ReadIntB( void ){	// !js_func
		UCHAR *p = ReadBin( 4 );
		return	( int )(
			( p[ 0 ] << 24 ) |
			( p[ 1 ] << 16 ) |
			( p[ 2 ] <<  8 ) |
			( p[ 3 ]       )
		);
	}
	double ReadUIntB( void ){	// !js_func
		UCHAR *p = ReadBin( 4 );
		return	( UINT )(
			( p[ 0 ] << 24 ) |
			( p[ 1 ] << 16 ) |
			( p[ 2 ] <<  8 ) |
			( p[ 3 ]       )
		);
	}
	
	int WriteChar	( int iVal ){ return WriteBin( &iVal, 1 ); }	// !js_func
	int WriteShortL	( int iVal ){ return WriteBin( &iVal, 2 ); }	// !js_func
	
	int WriteIntL( double dVal ){	// !js_func
		dVal += ( double )( 1 << 31 ) * 2;
		UINT uTmp = ( UINT )fmod( dVal, ( double )( 1 << 31 ) * 2 );
		return WriteBin( &uTmp, 4 );
	}
	
	int WriteFloat ( double dVal ){	// !js_func
		float fVal = ( float )dVal;
		return WriteBin( &fVal, 4 );
	}
	int WriteDouble( double dVal ){ return WriteBin( &dVal, 8 ); }	// !js_func
	
	int WriteShortB( int iVal ){	// !js_func
		int iTmp = (( UINT )iVal >> 8 ) | (( iVal & 0xFF ) << 8 );
		return WriteShortL( iTmp );
	}
	
	int WriteIntB( double dVal ){	// !js_func
		dVal += ( double )( 1 << 31 ) * 2;
		UINT uTmp = ( UINT )fmod( dVal, ( double )( 1 << 31 ) * 2 );
		UINT uTmp2 =
			(( uTmp              ) >> 24 ) |
			(( uTmp & 0x00FF0000 ) >>  8 ) |
			(( uTmp & 0x0000FF00 ) <<  8 ) |
			(( uTmp              ) << 24 );
		return WriteBin( &uTmp2, 4 );
	}
	
	static const int BUF_LEN = 10240;
	
  private:
	gzFile	m_gzfp;
	FILE	*m_fp;
	unzFile	m_unzfp;
	
	BOOL	m_bZipCurrentFileOpen;
	char	m_cBuf[ BUF_LEN ];
};
