/*****************************************************************************
	
	VSD -- vehicle data logger system  Copyright(C) by DDS
	
	CVsdFont.h - CVsdFont class header
	
*****************************************************************************/

#ifndef _CVsdFont_h_
#define _CVsdFont_h_

/*** new type ***************************************************************/

class CFontGlyph {
  public:
	BYTE	*pBuf;
	BYTE	*pBufOutline;
	int		iW, iH;
	int		iOrgY;
	int		iCellIncX;
	
	CFontGlyph(){
		pBuf = pBufOutline = NULL;
	}
	
	~CFontGlyph(){
		if( pBuf        ) delete [] pBuf;
		if( pBufOutline ) delete [] pBufOutline;
	}
};

class CVsdFont {
  public:
	CVsdFont( const char *szFontName, int iSize, UINT uAttr = 0 );
	~CVsdFont(){}
	
	void CreateFont( LOGFONT &logfont );
	
	static BOOL ExistFont( UCHAR c ){ return FONT_CHAR_FIRST <= c && c <= FONT_CHAR_LAST; }
	BOOL IsOutline( void ){ return m_uAttr & ATTR_OUTLINE; }
	BOOL IsFixed( void ){ return m_uAttr & ATTR_FIXED; }
	
	CFontGlyph& FontGlyph( UCHAR c ){
		return m_FontGlyph[ c - FONT_CHAR_FIRST ];
	}
	
	static const UINT ATTR_BOLD		= 1 << 0;
	static const UINT ATTR_ITALIC	= 1 << 1;
	static const UINT ATTR_OUTLINE	= 1 << 2;
	static const UINT ATTR_FIXED	= 1 << 3;
	
	int GetW( void ){ return m_iFontW; }
	int GetH( void ){ return m_iFontH; }	// !js_var:Height
	
	int GetW_Space( void ){ return m_iFontW_Space; }
	
	int GetTextWidth( char *szMsg ){	// !js_func
		
		if( m_uAttr & ATTR_FIXED ){
			return strlen( szMsg ) * GetW();
		}
		
		int iWidth = 0;
		for( int i = 0; szMsg[ i ]; ++i ){
			iWidth += FontGlyph( szMsg[ i ] ).iW;
		}
		return iWidth;
	}
	
  private:
	static const int FONT_CHAR_FIRST = '!';
	static const int FONT_CHAR_LAST	 = '~';
	
	CFontGlyph m_FontGlyph[ FONT_CHAR_LAST - FONT_CHAR_FIRST + 1 ];
	
	int	m_iFontW, m_iFontH, m_iFontW_Space;
	
	UINT	m_uAttr;
};
#endif
