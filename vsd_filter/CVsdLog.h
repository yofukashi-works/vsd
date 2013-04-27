/*****************************************************************************
	
	VSD -- vehicle data logger system  Copyright(C) by DDS
	
	CVsdLog.h - CVsdLog class header
	
*****************************************************************************/

#pragma once

/*** macros *****************************************************************/

#define TIME_NONE	(( int )0x80000000 )
#define WATCHDOG_TIME	( 1E+12 )

#define WATCHDOG_REC_NUM	2	// ログ先頭の番犬分のレコード数
#define TIME_STOP			3	// 停車とみなす Log 時間間隔
#define TIME_STOP_MARGIN	0.03	// 停車とみなす Log に付加する Log の時間差分

/*** Lap Time ***************************************************************/

enum {
	LAPMODE_HAND,		// 手動計測モード
	LAPMODE_AUTO,		// GPS 自動計測モード
	LAPMODE_MAGNET,		// 磁気センサー自動計測モード
};
enum {
	LAPSRC_VIDEO,		// 時計は Video フレーム
	LAPSRC_VSD,			// 時計は VSD ログ時計
	LAPSRC_GPS,			// 時計は GPS ログ時計
};

typedef struct {
	UINT	uLap;
	float	fLogNum;
	int		iTime;
} LAP_t;

class CVsdLog;
class CLapLog {
  public:
	CLapLog(){
		// 初期化
		m_iLapNum		= 0;
		m_iBestTime		= TIME_NONE;
		m_iBestLap		= 0;
		m_iLapMode		= LAPMODE_HAND;
		m_iLapSrc		= LAPSRC_VIDEO;
		m_iLapIdx		= -1;
		m_iBestLogNumRunning	= 0;
	}
	
	std::vector<LAP_t>	m_Lap;
	int	m_iLapMode;
	int	m_iLapSrc;
	int	m_iLapNum;
	int	m_iBestTime;
	int	m_iBestLap;
	int	m_iLapIdx;
	int	m_iCurTime;		// 現在ラップ経過時間
	int	m_iDiffTime;
	int m_iBestLogNumRunning;
};

/*** ログ 1項目 *************************************************************/

class CLog {
	
  public:
	// 値取得
	virtual double GetRaw( int    iIndex ) = 0;
	virtual double Get( double dIndex ) = 0;
	
	virtual double GetMin() = 0;
	virtual double GetMax() = 0;
	virtual void SetMaxMin( double dMaxVal, double dMinVal ) = 0;
	
	// 値設定
	virtual void Set( int iIndex, double dVal ) = 0;
	virtual void SetRaw( int iIndex, double dVal ) = 0;
	virtual void InitMinMax( void ) = 0;
	virtual int GetCnt( void ) = 0;
	virtual void Resize( int iCnt, double dVal ) = 0;
};

template <class T, int dScale>
class CLogVariant : public CLog {
	
  public:
	CLogVariant(){
		SetMax( m_Min );
		SetMin( m_Max );
	}
	
	// 値取得
	double GetRaw( int    iIndex ){ return UnScaled( m_Log[ iIndex ] ); }
	double Get( double dIndex ){
		double alfa = dIndex - ( UINT )dIndex;
		return
			GetRaw(( int )dIndex ) +
			( GetRaw(( int )dIndex + 1 ) - GetRaw(( int )dIndex )) * alfa;
	}
	
	double GetMin(){ return UnScaled( m_Min ); }
	double GetMax(){ return UnScaled( m_Max ); }
	virtual void SetMinRaw( double d ){ m_Min = Scaled( d ); }
	virtual void SetMaxRaw( double d ){ m_Max = Scaled( d ); }
	
	void SetMaxMin( double dMaxVal, double dMinVal ){
		if( GetMin() > dMinVal ) SetMinRaw( dMinVal );
		if( GetMax() < dMaxVal ) SetMaxRaw( dMaxVal );
	}
	
	// 値設定
	void Set( int iIndex, double dVal ){
		SetMaxMin( dVal, dVal );
		SetRaw( iIndex, dVal );
	}
	
	void SetRaw( int iIndex, double dVal ){
		// ★無い値を線形補間する必要あり
		if( GetCnt() > iIndex ) m_Log[ iIndex ] = Scaled( dVal );
		else while( GetCnt() <= iIndex ) m_Log.push_back( Scaled( dVal ));
	}
	
	int GetCnt( void ){ return m_Log.size(); }
	
	void InitMinMax( void ){
		SetMax( m_Min );
		SetMin( m_Max );
	}
	
	void Resize( int iCnt, double dVal ){
		m_Log.resize( iCnt, Scaled( dVal ));
	}
	
  protected:
	std::vector<T>	m_Log;
	T	m_Min;
	T	m_Max;
	
	// 最大・最小設定ヘルパ
	void SetMax( USHORT	&v ){ v = USHRT_MAX; }	void SetMin( USHORT	&v ){ v = 0; }
	void SetMax( short	&v ){ v = SHRT_MAX; }	void SetMin( short	&v ){ v = SHRT_MIN; }
	void SetMax( UINT	&v ){ v = UINT_MAX; }	void SetMin( UINT	&v ){ v = 0; }
	void SetMax( int	&v ){ v = INT_MAX; }	void SetMin( int	&v ){ v = INT_MIN; }
	void SetMax( float	&v ){ v = FLT_MAX; }	void SetMin( float	&v ){ v = -FLT_MAX; }
	
	// 内部形式変換
	T Scaled( double d ){ return ( T )( d * dScale ); }
	double UnScaled( T v ){ return ( double )v / dScale; }
};

typedef CLogVariant<float, 1>	CLogFloat;

class CLogFloatOffset : public CLogFloat {
  public:
	CLogFloatOffset(){
		m_dBaseVal	= 0;
	}
	
	// 値取得
	double GetRaw( int    iIndex ){ return CLogFloat::GetRaw( iIndex ) + m_dBaseVal; }
	
	double GetDiff( int    iIndex ){ return CLogFloat::GetRaw( iIndex ); }
	double GetDiff( double dIndex ){ return CLogFloat::Get( dIndex ); }
	
	double GetMin(){ return CLogFloat::GetMin() + m_dBaseVal; }
	double GetMax(){ return CLogFloat::GetMax() + m_dBaseVal; }
	void SetMinRaw( double d ){ CLogFloat::SetMinRaw( d - m_dBaseVal ); }
	void SetMaxRaw( double d ){ CLogFloat::SetMaxRaw( d - m_dBaseVal ); }
	
	void SetRaw( int iIndex, double dVal ){
		CLogFloat::SetRaw( iIndex, dVal - m_dBaseVal );
	}
	
	void SetBaseVal( double dVal ){ m_dBaseVal = dVal; }
	
  protected:
	double	m_dBaseVal;
};

typedef CLogVariant<short, 4096>	CLogShort4096;
typedef CLogVariant<USHORT, 128>	CLogUShort128;
typedef CLogVariant<USHORT, 1>		CLogUShort;
typedef CLogVariant<UINT, 1024>		CLogUInt1024;

class CLogDirection : public CLogUShort128 {
  public:
	double Get( double dIndex ){
		double alfa = dIndex - ( int )dIndex;
		double a = GetRaw(( int )dIndex );
		double b = GetRaw(( int )dIndex + 1 );
		
		if     ( a - b >= 180 ) a -= 360;
		else if( a - b < -180 ) b -= 360;
		
		double dResult = a + ( b - a )* alfa;
		if( dResult < 0 ) dResult += 360;
		
		return dResult;
	}
};

/*** 1個のログセット ********************************************************/

class CVsdFilter;
class CVsdLog {
	
  public:
	int GetCnt( void ){ return m_iCnt; }
	
	/////////////////////////
	
	int		m_iLogNum;
	double	m_dLogNum;
	
	double	m_dFreq;
	double	m_dLogStartTime;	// ログ開始時間
	
	// VSD ログ位置自動認識用
	double	m_dCalibStart;
	double	m_dCalibStop;
	
	// ログの map
	std::map<std::string, CLog *> m_Logs;
	
	// コンストラクタ・デストラクタ
	CVsdLog( CVsdFilter *pVsd );
	
	UINT GPSLogRescan();
	void RotateMap( double dAngle );
	double GetIndex( double dFromVal, int *piFrom, int *piLog, int iPrevIdx = -1 );
	double GetIndex( double dTime, int iPrevIdx = -1 );
	
	#ifdef DEBUG
		void Dump( char *szFileName );
	#endif
	
	BOOL IsDataExist( int iLogNum ){
		return 0 <= iLogNum && iLogNum < GetCnt();
	}
	
	int ReadLog( const char *szFileName, const char *szReaderFunc, CLapLog *&pLapLog );
	
	double GPSLogGetLength(
		double dLong0, double dLati0,
		double dLong1, double dLati1
	);
	
	// 番犬追加
	void AddWatchDog( void );
	void AddStopRecord( int iIndex, double dTime ){
		if( m_pLogSpeed ) SetSpeed( iIndex, 0 );
		if( m_pLogGx    ) SetGx(    iIndex, 0 );
		if( m_pLogGy    ) SetGy(    iIndex, 0 );
		SetTime( iIndex, dTime );
	}
	
	// key の存在確認
	
	CLog *GetElement( const char *szKey, BOOL bCreate = FALSE );
	
	// レコードコピー
	void CopyRecord( int iTo, int iFrom );
	
	// CLog 取得
	CLog *GetLog( const char *szKey ){
		std::string strKey( szKey );
		if( m_Logs.find( strKey ) == m_Logs.end()){
			return NULL;
		}
		return m_Logs[ strKey ];
	}
	
	// set / get 関数
	double Get( const char *szKey, double dIndex ){
		std::string strKey( szKey );
		if( m_Logs.find( strKey ) == m_Logs.end()){
			return NaN;	// 要素なし
		}
		return m_Logs[ strKey ]->Get( dIndex );
	}
	
	void Set( const char *szKey, int iIndex, double dVal );
	
	double GetMin( const char *szKey ){
		CLog	*pLog = GetElement( szKey );
		return pLog ? pLog->GetMin() : NaN;
	}
	
	double GetMax( const char *szKey ){
		CLog	*pLog = GetElement( szKey );
		return pLog ? pLog->GetMax() : NaN;
	}
	
	#define DEF_LOG( name )	DEF_LOG_T( name, CLogFloat )
	#define DEF_LOG_T( name, type )	type	*m_pLog##name;
	#include "def_log.h"
	
	#define DEF_LOG( name ) double name( void          ){ return m_pLog##name->Get( m_dLogNum ); }
	#include "def_log.h"
	#define DEF_LOG( name ) double name( int    iIndex ){ return m_pLog##name->GetRaw( iIndex ); }
	#include "def_log.h"
	#define DEF_LOG( name ) double name( double dIndex ){ return m_pLog##name->Get( dIndex ); }
	#include "def_log.h"
	
	#define DEF_LOG( name )	void Set##name( int iIndex, double dVal ){ \
		if( !m_pLog##name ) GetElement( #name, TRUE ); \
		m_pLog##name->Set( iIndex, dVal ); \
		if( m_iCnt <= iIndex ) m_iCnt = iIndex + 1; \
	}
	#include "def_log.h"
	
	#define DEF_LOG( name )	void SetRaw##name( int iIndex, double dVal ){ \
		if( !m_pLog##name ) GetElement( #name, TRUE ); \
		m_pLog##name->SetRaw( iIndex, dVal ); \
		if( m_iCnt <= iIndex ) m_iCnt = iIndex + 1; \
	}
	#include "def_log.h"
	
	#define DEF_LOG( name ) double Max##name( void ){ return m_pLog##name->GetMax(); }
	#include "def_log.h"
	#define DEF_LOG( name ) double Min##name( void ){ return m_pLog##name->GetMin(); }
	#include "def_log.h"
	#define DEF_LOG( name ) void SetMaxMin##name( double dMax, double dMin ){ m_pLog##name->SetMaxMin( dMax, dMin ); }
	#include "def_log.h"
	
	double DateTime( void ){
		return ( m_pLogTime->Get( m_dLogNum ) + m_dLogStartTime ) * 1000;
	}
	
	double X0( int    iIndex ){ return m_pLogLongitude->GetDiff( iIndex ) * m_dLong2Meter; }
	double X0( double dIndex ){ return m_pLogLongitude->GetDiff( dIndex ) * m_dLong2Meter; }
	double Y0( int    iIndex ){ return m_pLogLatitude->GetDiff( iIndex ) * m_dLati2Meter; }
	double Y0( double dIndex ){ return m_pLogLatitude->GetDiff( dIndex ) * m_dLati2Meter; }
	
  private:
	double m_dLong2Meter;
	double m_dLati2Meter;
	
	int	m_iCnt;
	CVsdFilter	*m_pVsd;
};
