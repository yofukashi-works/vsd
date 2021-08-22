// vsdwinDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "vsdwin.h"
#include "vsdwinDlg.h"
#include "erslib.h"
#include "mmsystem.h"

#include <math.h>
#include <conio.h>
#include "dds.h"
#include "../vsd/main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*** const ******************************************************************/

#define LAPTIME_INVALID		0xFFFFFFFF

#define WM_DOCOMM			( WM_USER + 0x100 )

enum {
	MODE_LAPTIME,
	MODE_GYMKHANA,
	MODE_ZERO_FOUR,
	MODE_ZERO_ONE,
};

enum {
	LEDMODE_SPEED,
	LEDMODE_TACO,
	LEDMODE_W_TEMP,
};

#define WINDOW_TITLE "VSDwin"
#define COMM_BUF_SIZE	2048
#define BUF_SIZE		1024

#define EOL		"\r"

enum {
	#define LED_PAT( pat, ch )	FONT_ ## ch = pat,
	#include "..\vsd\led_font.h"
};

const UCHAR CVsdwinDlg::m_cLEDFont[] = {
	#define LED_PAT( pat, ch )	pat,
	#define LED_PATN( pat, ch )	pat,
	#define LED_PATS( pat )		pat,
	#include "..\vsd\led_font.h"
};

#define LOG_OPENED	( m_File.m_hFile != CFile::hFileNull )
#define Log_fprintf	if( LOG_OPENED ) fprintf

#define BEEP_OFFS	22
#define BEEP_0	0
#define BEEP_C	1097337155
#define BEEP_CC	1162588218
#define BEEP_D	1231719311
#define BEEP_DC	1304961152
#define BEEP_E	1382558180
#define BEEP_F	1464769368
#define BEEP_FC	1551869087
#define BEEP_G	1644148025
#define BEEP_GC	1741914154
#define BEEP_A	1845493760
#define BEEP_AC	1955232530
#define BEEP_B	2071496706
#define BEEP( n, o, d )	{ ( BEEP_##n >> ( 22 - ( o ))), d }

/*** gloval var *************************************************************/

const NOTE CVsdwinDlg::m_BeepNoteBestLap[] = {
	BEEP( C, 2, 150 ),
	BEEP( G, 1, 150 ),
	BEEP( C, 2, 150 ),
	BEEP( G, 1, 150 ),
	BEEP( C, 2, 150 ),
	BEEP( G, 1, 150 ),
	BEEP( C, 1, 0 ),
};

const NOTE CVsdwinDlg::m_BeepNoteNewLap[] = {
	BEEP( C, 2, 100 ),	BEEP( 0, 0, 10 ),
	BEEP( C, 2, 100 ),	BEEP( 0, 0, 10 ),
	BEEP( C, 1, 0 ),
};

/////////////////////////////////////////////////////////////////////////////
// �A�v���P�[�V�����̃o�[�W�������Ŏg���Ă��� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂�
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �̃T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD){
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX){
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// ���b�Z�[�W �n���h��������܂���B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVsdwinDlg �_�C�A���O

CVsdwinDlg::CVsdwinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVsdwinDlg::IDD, pParent){
	//{{AFX_DATA_INIT(CVsdwinDlg)
	//}}AFX_DATA_INIT
	// ����: LoadIcon �� Win32 �� DestroyIcon �̃T�u�V�[�P���X��v�����܂���B
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVsdwinDlg::DoDataExchange(CDataExchange* pDX){
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVsdwinDlg)
	DDX_Control(pDX, IDC_PROGRESS_FUEL, m_ProgressFuel);
	DDX_Control(pDX, IDC_STATIC_FUEL, m_StaticFuel);
	DDX_Control(pDX, IDC_SLIDER_PLAY_POS, m_SliderPlayPos);
	DDX_Control(pDX, IDC_STATIC_GEAR, m_StaticGear);
	DDX_Control(pDX, IDC_STATIC_WTEMP, m_StaticWTemp);
	DDX_Control(pDX, IDC_PROGRESS_WTEMP, m_ProgressWTemp);
	DDX_Control(pDX, IDC_PROGRESS_SPEED, m_ProgressSpeed);
	DDX_Control(pDX, IDC_STATIC_SPEED, m_StaticSpeed);
	DDX_Control(pDX, IDC_STATIC_TACO, m_StaticTaco);
	DDX_Control(pDX, IDC_PROGRESS_TACO, m_ProgressTaco);
	DDX_Control(pDX, IDC_EDIT_DIFF, m_EditDiff);
	DDX_Control(pDX, IDC_EDIT_CLOCK, m_EditClock);
	DDX_Control(pDX, IDC_EDIT_BEST, m_EditBest);
	DDX_Control(pDX, IDC_LIST_LAP, m_ListLapTime);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVsdwinDlg, CDialog)
	//{{AFX_MSG_MAP(CVsdwinDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_MENU_EXIT, OnMenuitemExit)
	ON_COMMAND(ID_MENU_LAPTIME, OnMenuLaptime)
	ON_WM_TIMER()
	ON_COMMAND(ID_MENU_SETTING, OnMenuSetting)
	ON_COMMAND(ID_MENU_GYMKHANA, OnMenuGymkhana)
	ON_COMMAND(ID_MENU_ZERO_FOUR, OnMenuZeroFour)
	ON_COMMAND(ID_MENU_ZERO_ONE, OnMenuZeroOne)
	ON_WM_CLOSE()
	ON_COMMAND(ID_MENU_SPEED, OnMenuSpeed)
	ON_COMMAND(ID_MENU_TACO, OnMenuTaco)
	ON_COMMAND(ID_MENU_LED_REVERSE, OnMenuLedReverse)
	ON_WM_CTLCOLOR()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_MENU_SHIFT_WARN, OnMenuGearMode)
	ON_COMMAND(ID_MENU_BEEP, OnMenuBeep)
	ON_COMMAND(ID_MENU_W_TEMP, OnMenuWTemp)
	ON_COMMAND(ID_MENU_REPLAY, OnMenuReplay)
	ON_WM_HSCROLL()
	ON_COMMAND(ID_MENU_OPEN_FIRM, OnMenuOpenFirm)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_DOCOMM, OnDoComm )
END_MESSAGE_MAP()

/*** prototype **************************************************************/

#ifdef USE_GIVEIO
extern "C" {
BOOL LoadBeepDriver( char *drivername );
BOOL UnloadDriver( char *drivername );
void SetBeepFreq(int hz);
}
#endif

/*** AtoI *******************************************************************/

UINT AtoI( char *p ){
	UINT	uRet = 0;
	
	while( *p ){
		uRet <<= ITOA_RADIX_BIT;
		uRet |= *( UCHAR *)p - '@';
		++p;
	}
	return( uRet );
}

/*** �V���A���p�X���b�h *****************************************************/

void CVsdwinDlg::CommThread(){
	int		iReadSize;
	UINT	uBufDataSize;
	char	*p;
	char	*pp;
	
	CStdioFile	fpLog;
	CStdioFile	fpFirm;
	
	static char szBuf[ COMM_BUF_SIZE + 1 ];
	
	/*** �V���A�� open�E������ **********************************************/
	
	m_bExit = FALSE;
	
	// LogFile �I�[�v��
	if( m_bCommLog ) fpLog.Open( "log.txt", CFile::modeCreate /*| CFile::modeNoTruncate | CFile::typeText*/ | CFile::modeWrite );
	
	// �Ȃ��� ephemeris �� com3 �� open ���Ȃ��� com1 ���J���Ȃ��̂�
	
	if( m_iCommDebug ){
		ERS_Open( m_iCommDebug, COMM_BUF_SIZE, COMM_BUF_SIZE );
		ERS_Close( m_iCommDebug );
	}
	
	// comm �|�[�g open
	if( ERS_Open( m_iComm, COMM_BUF_SIZE, COMM_BUF_SIZE )){
		MessageBox( "Can't open COM port.", WINDOW_TITLE, MB_OK | MB_ICONWARNING );
		return;
	}
	
	ERS_Config( m_iComm, ERS_38400 );
	
  ReloadFirm:
	m_bReloadFirmware = FALSE;
	
	// S �t�@�C�� open
	if( !fpFirm.Open( m_strFirmwareName, CFile::modeRead | CFile::typeText )){
		NoDebugCmd( MessageBox( "Can't open firmware.", WINDOW_TITLE, MB_OK | MB_ICONWARNING ); )
	}else{
		// ���Z�b�g & ���[�h�R�}���h
		ERS_Puts( m_iComm, "z" EOL ); Sleep( 100 );
		ERS_Puts( m_iComm, "l" EOL ); Sleep( 100 );
		
		// S �t�@�C�����[�h
		while( fpFirm.ReadString( szBuf, COMM_BUF_SIZE )){
			iReadSize = ::strlen( szBuf );
			if( szBuf[ iReadSize - 1 ] == '\n' ){
				szBuf[ iReadSize - 1 ] = '\r';
			}
			ERS_Send( m_iComm, szBuf, iReadSize );
		}
		Sleep( 100 );
		
		// go �R�}���h
		ERS_Puts( m_iComm, "g" EOL ); Sleep( 100 );
		
		fpFirm.Close();
	}
	
	/*** ini �ŃV���A���o�͂��K�v�ȏ��� *************************************/
	
	if( m_bLEDReverse )		PostMessage( WM_COMMAND, ID_MENU_LED_REVERSE,	0 );
	if( m_bShiftWarning )	PostMessage( WM_COMMAND, ID_MENU_SHIFT_WARN,	0 );
	if( !m_bBeep )			PostMessage( WM_COMMAND, ID_MENU_BEEP,			0 );
	PostMessage( WM_COMMAND, m_uLEDMode == LEDMODE_SPEED ? ID_MENU_SPEED : ID_MENU_TACO, 0 );
	
	/*** ���O�t�@�C���쐬 ***************************************************/
	
	Sleep( 1000 );
	ERS_Recv( m_iComm, szBuf, iReadSize = ERS_CheckRecv( m_iComm ));
	szBuf[ iReadSize ] = '\0';
	if( m_bCommLog ) fpLog.WriteString( szBuf );
	
	uBufDataSize = 0;
	
	// �Ō�� "DDS" ������
	p = NULL;
	pp = szBuf;
	while( pp = strstr( pp, "DDS" )){
		p = pp;
		pp += 3;
	}
	
	if( p ){
		p += 3;
	}else{
		// �Ō�� \n �܂Ńf�[�^���̂Ă�
		p = strrchr( szBuf, '\n' );
	}
	
	if( p ){
		strcpy( szBuf, p + 1 );
		uBufDataSize = strlen( szBuf );
	}
	
	/*** �V���A���������[�v *************************************************/
	
	while( !m_bExit ){
		Sleep( 100 );
		iReadSize = ERS_CheckRecv( m_iComm );
		if( iReadSize > 0 ){
			ERS_Recv( m_iComm, szBuf + uBufDataSize, iReadSize );
			szBuf[ uBufDataSize + iReadSize ] = '\0';
			if( m_bCommLog ) fpLog.WriteString( szBuf + uBufDataSize );
			
			// Token ��؂�o��
			uBufDataSize += iReadSize;
			
			p = pp = szBuf;
			for(; *pp; ++pp ){
				// space / ���s�Ȃ�C1�R�}���h���
				if( *pp == ' ' || *pp == '\n' ){
					*pp = 0;
					++pp;
					
					// ���Z�b�g���ꂽ��CFirmware �ă��[�h
					if( m_bReloadFirmware || strncmp( p, "VSD", 3 ) == 0 ){
						m_File.Close();
						goto ReloadFirm;
					}
					
					// ���s���O�t�@�C�����I�[�v������Ă��Ȃ���΁C�쐬
					if( !LOG_OPENED ){
						CTime CurTime = CTime::GetCurrentTime();
						
						CString strFileName = CurTime.Format( "vsd%Y%m%d_%H%M%S.log" );
						if( !m_File.Open( strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText | CFile::shareDenyWrite )){
							MessageBox( "Can't open log file.", WINDOW_TITLE, MB_OK | MB_ICONWARNING );
						}
						
						m_File.WriteString( CurTime.Format( "; VSD started at %Y/%m/%d %H:%M:%S" ));
					}
					
					// comm ���b�Z�[�W���M
					PostMessage( WM_DOCOMM, *p, AtoI( p + 1 ));
					// ���̃g�[�N���̐擪
					while( *pp == ' ' || *pp == '\n' || *pp == '\r' ) ++pp;
					p = pp;
					if( *pp == 0 ) break;
				}
			}
			// ���r���[�ȃR�}���h�� buf �擪�ɃR�s�[
			strcpy( szBuf, p );
			uBufDataSize = strlen( szBuf );
		}
	}
	
	ERS_Close( 1 );
	if( m_bCommLog ) fpLog.Close();
	
	m_bExit = FALSE;
}

UINT CommThread( LPVOID pParam ){
	(( CVsdwinDlg *)pParam )->CommThread();
	return 0;
}

/*** �^�C�}�[�̏��� *********************************************************/

enum {
	TIMER_ID_CLK = 1,
	TIMER_ID_MSG,
	TIMER_ID_REPLAY,
	TIMER_ID_BEEP,
};

#define TIMER_CLK_KILL		KillTimer( TIMER_ID_CLK )
#define TIMER_CLK_RESTART	( m_uClock = 0, m_uNextClock = GetTickCount(), OnTimer( TIMER_ID_CLK ))
#define TIMER_CLK_STOP		( TIMER_CLK_KILL, m_EditClock.SetWindowText( "  -:--    " ))

#define TIMER_MSG_START( msg )	( \
	ERS_Puts( m_iComm, "N" ), \
	m_pLEDMsg = m_szLEDMsg,	\
	strcpy( m_szLEDMsg, msg ),	\
	strcat( m_szLEDMsg, "    " ), \
	SetTimer( TIMER_ID_MSG, /*250*/ 150, NULL )	\
)

#define TIMER_MSG_KILL			( KillTimer( TIMER_ID_MSG ), ERS_Puts( m_iComm, "N" ))
#define LOG_TIME_INTVL			( 1000.0 / LOG_FREQ )
#define StartReplayTimer()		SetTimer( TIMER_ID_REPLAY, ( UINT )( LOG_TIME_INTVL ), NULL )

#ifdef USE_GIVEIO
#define StartPlayNote( note )	do \
	if( !m_bIsOS_NT || m_bLoadGiveIO ){		\
		m_pPlayNote = ( NOTE *)( note );	\
		OnTimer( TIMER_ID_BEEP );			\
	} while( 0 )
#else
#define StartPlayNote( note )	( m_pPlayNote = ( NOTE *)( note ), OnTimer( TIMER_ID_BEEP ))
inline void SetBeepFreq(int hz){
	hz = 1193180 / hz;						// clocked at 1.19MHz
	_outp(0x43, 0xb6);						// timer 2, square wave
	_outp(0x42, hz);
	_outp(0x42, hz >> 8);
}
#endif

void CVsdwinDlg::OnTimer(UINT nIDEvent){
	static char szMsg[] = "000:00    ";
	static char szLEDMsg[] = "00m";
	char szBuf[ BUF_SIZE ];
	
	UINT	uTaco	= 0;
	float	fSpeed	= 0;
	UINT	uWTemp	= 0;
	UINT	uMin, uSec, uMSec;
	UCHAR	cType;
	char	*p;
	
	static double	dNextTimeReplay	= 0;
	
	switch( nIDEvent ){
		
		/*** 1�b���v ********************************************************/
	  case TIMER_ID_CLK:
		KillTimer( nIDEvent );
		
		sprintf( szMsg, "%3d:%02d    ", m_uClock / 60, m_uClock % 60 );
		m_EditClock.SetWindowText( szMsg );
		++m_uClock;
		
		m_uNextClock += 1000;
		SetTimer( TIMER_ID_CLK, m_uNextClock - GetTickCount(), NULL );
		
		/*** ���v���C *******************************************************/
	  Case TIMER_ID_REPLAY:
		if( fgets( szBuf, BUF_SIZE, m_fpRecordLog.m_pStream ) != NULL && m_bReplay == TRUE ){
			if(
				// ���b�v�^�C���L�^��������
				( p = strstr( szBuf, "LAP" )) != NULL &&
				sscanf( p, "%*s %d:%d.%d", &uMin, &uSec, &uMSec ) == 3
			){
				AddLapTime( szBuf, ( uMin * 60 + uSec ) * 1000 + uMSec, FALSE );
			}
			
			// ���ʂ� log
			UINT	uCnt;
			if(( uCnt = sscanf( szBuf, "%u%g%*g%1s%u", &uTaco, &fSpeed, &cType, &uWTemp )) >= 2 ){
				OnDoComm( 'T', uTaco );
				OnDoComm( 'S', ( UINT )( fSpeed * 100 ));
				if( uCnt >= 3 ) OnDoComm( cType, uWTemp );
			}
			
			// �X���C�_�X�V
			m_SliderPlayPos.SetPos( m_fpRecordLog.GetPosition());
			
			// �^�C�}���x���P�p�Đݒ�
			
			dNextTimeReplay += LOG_TIME_INTVL;
			UINT uCurTime = GetTickCount();
			
			if(
				dNextTimeReplay <= uCurTime ||
				dNextTimeReplay >= uCurTime + LOG_TIME_INTVL * 2
			){
				dNextTimeReplay = uCurTime + LOG_TIME_INTVL;
			}else{
				KillTimer( nIDEvent );
				SetTimer( nIDEvent, ( UINT )( dNextTimeReplay - uCurTime ), NULL );
				if( dNextTimeReplay >= (( double )( UINT )0xFFFFFFFF + 1 )){
					dNextTimeReplay -= (( double )( UINT )0xFFFFFFFF + 1 );
				}
			}
			
		}else{
			// ���R�[�h���O�I��
			KillTimer( nIDEvent );
			if( !m_bReplay ) m_fpRecordLog.Close();
		}
		
		/*** LED ���b�Z�[�W *************************************************/
	  Case TIMER_ID_MSG:
		
		// msg �̍Ō�ɒB������C�O�ɖ߂�
		if( *m_pLEDMsg == '\0' ) m_pLEDMsg = m_szLEDMsg;
		
		// font ���b�Z�[�W���M
		sprintf( szLEDMsg, "%Xm", m_cLEDFont[ *m_pLEDMsg & 0x7F - ' ' ] );
		ERS_Puts( m_iComm, szLEDMsg );
		++m_pLEDMsg;
		
		/*** Beep �T�E���h **************************************************/
	  Case TIMER_ID_BEEP:
		
		_outp(0x61, _inp(0x61) & ~0x03);		// stop that racket!
		
		if( m_pPlayNote && m_pPlayNote->uDuration != 0 ){
			if( m_pPlayNote->uHz ){
				SetBeepFreq( m_pPlayNote->uHz );
				_outp(0x61, _inp(0x61) | 0x03);			// start speaker going
			}
			SetTimer( TIMER_ID_BEEP, m_pPlayNote->uDuration, NULL );
			++m_pPlayNote;
		}else{
			KillTimer( TIMER_ID_BEEP );
			m_pPlayNote = NULL;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVsdwinDlg ���b�Z�[�W �n���h��

BOOL CVsdwinDlg::OnInitDialog(){
	CDialog::OnInitDialog();
	
	// "�o�[�W�������..." ���j���[���ڂ��V�X�e�� ���j���[�֒ǉ����܂��B
	
	// IDM_ABOUTBOX �̓R�}���h ���j���[�͈̔͂łȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL){
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty()){
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// ���̃_�C�A���O�p�̃A�C�R����ݒ肵�܂��B�t���[�����[�N�̓A�v���P�[�V�����̃��C��
	// �E�B���h�E���_�C�A���O�łȂ����͎����I�ɐݒ肵�܂���B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R����ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R����ݒ�
	
	// TODO: ���ʂȏ��������s�����͂��̏ꏊ�ɒǉ����Ă��������B
	m_uClock		= 0;
	m_uLapCnt		= 0;
	m_iDiffSign		= 0;
	m_bReplay		= TRUE;
	m_uLapTimePrev	= LAPTIME_INVALID;
	m_uBestLap		= LAPTIME_INVALID;
	
	m_Menu.Attach( GetMenu()->m_hMenu );
	OnMenuLaptime();
	
	m_EditBest.SetWindowText( "  -:--.---" );
	m_EditDiff.SetWindowText( "+ -:--.---" );
	
	// ini �ǂݍ���
	CWinApp* pApp = AfxGetApp();
	
	m_iComm					= pApp->GetProfileInt   ( "Config", "Comm", 1 );
	m_iCommDebug			= pApp->GetProfileInt   ( "Config", "CommDebug", 0 );
	m_bLEDReverse			= pApp->GetProfileInt   ( "Config", "LEDReverse", 0 );
	m_bShiftWarning			= pApp->GetProfileInt   ( "Config", "ShiftWarning", 0 );
	m_uLEDMode				= pApp->GetProfileInt   ( "Config", "LEDMode", 0 );
	m_bBeep					= pApp->GetProfileInt   ( "Config", "Beep", 1 );
	m_bCommLog				= pApp->GetProfileInt   ( "Config", "CommLog", 0 );
	m_uGymkhanaStartMargin	= pApp->GetProfileInt   ( "Config", "GymkhanaStartMargin", 5 );
	m_uStartGThrethold		= pApp->GetProfileInt   ( "Config", "StartGThrethold", 500 );
	m_strNewLapSound		= pApp->GetProfileString( "Config", "NewLapSound", "" );
	m_strBestLapSound		= pApp->GetProfileString( "Config", "BestLapSound", "" );
	m_strFirmwareName		= pApp->GetProfileString( "Config", "Firmware", "vsd.mot" );
	
	// �V���A���p�X���b�h�N��
	AfxBeginThread( ::CommThread, this );
	
	logfont.lfHeight        = 36;   // ���������傫������B
    logfont.lfWidth         = 0;
    logfont.lfEscapement    = 0;
    logfont.lfOrientation   = 0;
    logfont.lfWeight        = FW_NORMAL;
    logfont.lfItalic        = FALSE;
    logfont.lfUnderline     = FALSE;
    logfont.lfStrikeOut     = FALSE;
    logfont.lfCharSet       = SHIFTJIS_CHARSET;
    logfont.lfOutPrecision  = OUT_DEFAULT_PRECIS;
    logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logfont.lfQuality       = DEFAULT_QUALITY;
    logfont.lfPitchAndFamily= FF_DONTCARE|DEFAULT_PITCH;
	
    strcpy(logfont.lfFaceName, "�l�r �S�V�b�N");
	
    font.CreateFontIndirect( &logfont );
	
	m_EditDiff.SetFont(&font);
	m_EditClock.SetFont(&font);
	m_EditBest.SetFont(&font);
	m_ListLapTime.SetFont(&font);
	
	// �J�����ǉ�
	m_ListLapTime.InsertColumn( 0, "Lap",  LVCFMT_RIGHT, 87 );
	m_ListLapTime.InsertColumn( 1, "Time", LVCFMT_LEFT, 204 );
	
	// ProgressBar �͈͐ݒ�
	m_ProgressTaco	.SetRange32( 0, 8000 );
	m_ProgressSpeed	.SetRange32( 0, 180 );
	m_ProgressWTemp	.SetRange32( 50, 150 );
	m_ProgressFuel	.SetRange32( 0, 100 );
	
	DWORD dwExStyle = ListView_GetExtendedListViewStyle( m_ListLapTime.m_hWnd );
	dwExStyle |= LVS_EX_FULLROWSELECT /*| LVS_EX_GRIDLINES*/;
	ListView_SetExtendedListViewStyle( m_ListLapTime.m_hWnd , dwExStyle );
	
	DragAcceptFiles(TRUE);	//�h���b�O���󂯕t����
	
	#ifdef USE_GIVEIO
		// OS version
		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
		GetVersionEx( &osvi );
		m_bIsOS_NT = osvi.dwMajorVersion >= 5;
		
		// beep �h���C�o���[�h
		m_bLoadGiveIO = FALSE;
		if( m_bIsOS_NT ) m_bLoadGiveIO = LoadBeepDriver( WINDOW_TITLE );
	#endif
	
	return TRUE;  // TRUE ��Ԃ��ƃR���g���[���ɐݒ肵���t�H�[�J�X�͎����܂���B
}

void CVsdwinDlg::OnClose(){
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	
	#ifdef USE_GIVEIO
		// beep �h���C�o�A�����[�h
		StartPlayNote( NULL );
		if( m_bLoadGiveIO ) UnloadDriver( WINDOW_TITLE );
	#endif
	
	TIMER_MSG_KILL;
	
	m_bExit = TRUE;
	while( m_bExit ) Sleep( 100 );
	
	if( LOG_OPENED ){
		// LAP log �o��
		fprintf( m_File.m_pStream, "\n=LAP======TIME====\n" );
		for( UINT u = 0; u < m_uLapCnt; ++u ){
			fprintf(
				m_File.m_pStream, "%u\t%s\n", u + 1,
				m_ListLapTime.GetItemText( m_uLapCnt - u - 1, 1 )
			);
		}
		fprintf( m_File.m_pStream, "==================\n" );
		m_File.Close();
	}
	
	CDialog::OnClose();
}

void CVsdwinDlg::OnSysCommand(UINT nID, LPARAM lParam){
	if ((nID & 0xFFF0) == IDM_ABOUTBOX){
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}else{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����_�C�A���O�{�b�N�X�ɍŏ����{�^����ǉ�����Ȃ�΁A�A�C�R����`�悷��
// �R�[�h���ȉ��ɋL�q����K�v������܂��BMFC �A�v���P�[�V������ document/view
// ���f�����g���Ă���̂ŁA���̏����̓t���[�����[�N�ɂ�莩���I�ɏ�������܂��B

void CVsdwinDlg::OnPaint(){
	if (IsIconic()){
		CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// �N���C�A���g�̋�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// �A�C�R����`�悵�܂��B
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// �V�X�e���́A���[�U�[���ŏ����E�B���h�E���h���b�O���Ă���ԁA
// �J�[�\����\�����邽�߂ɂ������Ăяo���܂��B
HCURSOR CVsdwinDlg::OnQueryDragIcon(){
	return (HCURSOR) m_hIcon;
}

/*** �V���A�������ݏ��� *****************************************************/

BOOL CVsdwinDlg::AddLapTime( char *szBuf, UINT uLapTimeDiff, BOOL bLog ){
	BOOL bBestLap = FALSE;
	
	// �^�C���o�^
	++m_uLapCnt;
	sprintf( szBuf, "%3d", m_uLapCnt );
	m_ListLapTime.InsertItem( 0, szBuf, 0 );
	
	sprintf(
		szBuf, "%3d:%02d.%03d",
		uLapTimeDiff / ( 60000 ),
		uLapTimeDiff / 1000 % 60,
		uLapTimeDiff % 1000
	);
	m_ListLapTime.SetItemText( 0, 1, szBuf );
	
	if( bLog ) Log_fprintf( m_File.m_pStream, "\tLAP%d\t%s", m_uLapCnt, szBuf );
	
	// �x�X�g�X�V - �\��
	if( uLapTimeDiff < m_uBestLap ){
		m_EditBest.SetWindowText( szBuf );
	}
	
	// �x�X�g�Ƃ̍����\��
	if( m_uBestLap != LAPTIME_INVALID ){
		UCHAR c;
		UINT  uLapDiff;
		
		if( uLapTimeDiff > m_uBestLap ){
			c = '+';
			m_iDiffSign = 1;
			uLapDiff = uLapTimeDiff - m_uBestLap;
		}else{
			c = '-';
			m_iDiffSign = -1;
			uLapDiff = m_uBestLap - uLapTimeDiff;
			bBestLap = TRUE;
		}
		sprintf(
			szBuf, "%c%2d:%02d.%03d", c,
			uLapDiff / ( 60000 ),
			uLapDiff / 1000 % 60,
			uLapDiff % 1000
		);
		m_EditDiff.SetWindowText( szBuf );
	}
	
	// �x�X�g�X�V - �ϐ�
	if( uLapTimeDiff < m_uBestLap ){
		m_uBestLap = uLapTimeDiff;
	}
	
	return( bBestLap );
}

LONG CVsdwinDlg::OnDoComm( WPARAM wParam, LPARAM lParam ){
	UINT uType = wParam;
	UINT uNum  = lParam;
	BOOL bBestLap = FALSE;
	
	static UINT		uPrevMillage	= 0;
	static UINT		uMillageOvf		= 0;
	static UINT		uTaco			= 0;
//	static float	fWTemp			= 0;
//	float			fCurWTemp;
//	static float	fFuel			= 0;
//	float			fCurFuel;
	
	UINT uGearRatio;
	
	static char szBuf[] = "000:00:000";
	
	switch( uType & 0xFF ){
	  Case 'L':
		TIMER_MSG_KILL;
		// �`�F�b�N�|�C���g��ʉߍς݂Ȃ�΁C����^�C�������߂�
		if( m_uLapTimePrev != LAPTIME_INVALID ){
			UINT uLapTimeDiff = ( UINT )(( uNum - m_uLapTimePrev ) / ( H8HZ / 1000.0 / 65536 ));
			bBestLap = AddLapTime( szBuf, uLapTimeDiff, TRUE );
		}
		
		// LapTimeMode �܂��́C�v���X�^�[�g�Ȃ�Cclk �����Z�b�g����
		if( m_uMode == MODE_LAPTIME || m_uLapTimePrev == LAPTIME_INVALID ){
			m_uLapTimePrev = uNum;
			TIMER_CLK_RESTART;
			if( m_uMode != MODE_LAPTIME || m_uLapCnt == 0 )
				Log_fprintf( m_File.m_pStream, "\tLAP%d start", m_uLapCnt + 1 );
		}else{
			m_uLapTimePrev = LAPTIME_INVALID;
			TIMER_CLK_STOP;
		}
		
		if( bBestLap ){
			if((( LPCSTR )m_strBestLapSound )[ 0 ] ){
				sndPlaySound( m_strBestLapSound, SND_ASYNC );
			}else{
				StartPlayNote( m_BeepNoteBestLap );
			}
		}else{
			if((( LPCSTR )m_strNewLapSound )[ 0 ] ){
				sndPlaySound( m_strNewLapSound, SND_ASYNC );
			}else{
				StartPlayNote( m_BeepNoteNewLap );
			}
		}
		
	  Case 'T':
		Log_fprintf( m_File.m_pStream, "\n%u", uNum );
		// �_�C�A���O�X�V
		sprintf( szBuf, "%drpm", uNum );
		m_StaticTaco.SetWindowText( szBuf );
		m_ProgressTaco.SetPos( uNum );
		
		uTaco = uNum;
		
	  Case 'S':
		Log_fprintf( m_File.m_pStream, "\t%u.%02u", uNum / 100, uNum % 100 );
		// �_�C�A���O�X�V
		sprintf( szBuf, "%dkm/h", uNum / 100 );
		m_StaticSpeed.SetWindowText( szBuf );
		m_ProgressSpeed.SetPos( uNum / 100 );
		
		// �M�A�_�C�A���O�X�V ////////////////////////////////////////////////
		
		if( uTaco ){
			uGearRatio = ( uNum << 8 ) / uTaco;
			
			if( uGearRatio < GEAR_TH( 2 )){
				if( uGearRatio < GEAR_TH( 1 ))		uNum = 1;
				else								uNum = 2;
			}else if( uGearRatio < GEAR_TH( 3 ))	uNum = 3;
			else  if( uGearRatio < GEAR_TH( 4 ))	uNum = 4;
			else									uNum = 5;
			
			sprintf( szBuf, "%d", uNum );
			m_StaticGear.SetWindowText( szBuf );
		}
		
		//////////////////////////////////////////////////////////////////////
		
	  Case 'g':
		OnDoComm( 'x',	 uNum		  & 0xFFFF );
		OnDoComm( 'y', ( uNum >> 16 ) & 0xFFFF );
		
	  Case 'x':
	  case 'y':
	  case 'z':
	  case 'I':
		Log_fprintf( m_File.m_pStream, "\t%u", uNum );
		
		#if 0
		/*
	  Case 'a':	// AD#0
	  case 'b':	// AD#1
		Log_fprintf( m_File.m_pStream, "\t%c0", uType & 0xFF );
		
	  Case 'c':	// AD#2
		#define WTEMP_Re	0.319314945
		#define WTEMP_R0	2.4
		#define WTEMP_T0	293.0
		#define WTEMP_B		3528.052344
		#define WTEMP_Vm	63678.70445
		
		// �_�C�A���O�X�V
		Log_fprintf( m_File.m_pStream, "\tc%u", uNum );
		
		fCurWTemp = ( float )( 1 / ( log( uNum * WTEMP_Re / ( WTEMP_Vm - uNum ) / WTEMP_R0 ) / WTEMP_B + 1 / WTEMP_T0 )) - 273;
		if( fCurWTemp - 10 <= fWTemp && fWTemp <= fCurWTemp + 10 ){
			fWTemp = ( fWTemp * 4 + fCurWTemp ) / 5;
		}else{
			fWTemp = fCurWTemp;
		}
		
		uNum = ( UINT )( fWTemp + 0.5 );
		
		if( uNum < 300 ){
			sprintf( szBuf, "%u��", uNum );
			m_StaticWTemp.SetWindowText( szBuf );
			m_ProgressWTemp.SetPos( uNum );
		}
		
	  Case 'd':	// AD#3
		#define FUEL_FILTER	100
		#define FUEL_FULL	48		// 100% �� ADC �l >> 6
		#define FUEL_EMPTY	1023	//   0% �� ADC �l >> 6
		
		Log_fprintf( m_File.m_pStream, "\td%u", uNum );
		
		if( uNum ){
			fCurFuel = 100 - (( float )( uNum >> 6 ) - FUEL_FULL ) * 100 / ( FUEL_EMPTY - FUEL_FULL );
			
			//if( fCurFuel - 10 <= fFuel && fFuel <= fCurFuel + 10 ){
			if( fFuel != 0 ){
				fFuel = ( fFuel * ( FUEL_FILTER - 1 ) + fCurFuel ) / FUEL_FILTER;
			}else{
				fFuel = fCurFuel;
			}
			
			uNum = ( UINT )( fFuel + 0.5 );
			sprintf( szBuf, "%u%%", uNum );
			m_StaticFuel.SetWindowText( szBuf );
			m_ProgressFuel.SetPos( uNum );
		}
		*/
		#endif
		
	  Case 'M':
		if( uNum < uPrevMillage ) ++uMillageOvf;
		uPrevMillage = uNum;
		Log_fprintf( m_File.m_pStream, "\t%.2f", (( double )uMillageOvf * 0x10000 + uNum ) / ( PULSE_PAR_1KM / 1000 ));
	}
	return 0;
}

HBRUSH CVsdwinDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor){
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: ���̈ʒu�� DC �̃A�g���r���[�g��ύX���Ă�������
	if( nCtlColor == CTLCOLOR_STATIC ){
		if(
			IDC_EDIT_CLOCK == pWnd->GetDlgCtrlID() ||
			IDC_EDIT_BEST  == pWnd->GetDlgCtrlID()
		){
			// edit box �̐F
			pDC->SetBkMode( OPAQUE );
			pDC->SetBkColor( RGB( 255, 255, 255 ));
		}else if( IDC_EDIT_DIFF == pWnd->GetDlgCtrlID()){
			pDC->SetBkMode( OPAQUE );
			if( m_iDiffSign < 0 ){
				// diff �̐�
				pDC->SetTextColor( RGB( 255, 255, 255 ));
				pDC->SetBkColor( RGB( 0, 0, 255 ));
			}else if( m_iDiffSign > 0 ){
				// diff �̐�
				pDC->SetTextColor( RGB( 255, 255, 255 ));
				pDC->SetBkColor( RGB( 255, 0, 0 ));
			}else{
				// diff �̔�
				pDC->SetBkColor( RGB( 255, 255, 255 ));
			}
		}
	}
	// TODO: �f�t�H���g�̃u���V���]�݂̂��̂łȂ��ꍇ�ɂ́A�Ⴄ�u���V��Ԃ��Ă�������
	return hbr;
}

/*** ���j���[�̏��� *********************************************************/

void CVsdwinDlg::OnMenuitemExit(){
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	PostQuitMessage( 0 );
}

void CVsdwinDlg::OnMenuLaptime(){
	m_uMode = MODE_LAPTIME;
	TIMER_CLK_STOP;
	m_Menu.CheckMenuRadioItem( ID_MENU_LAPTIME, ID_MENU_ZERO_ONE, ID_MENU_LAPTIME, MF_BYCOMMAND );
	SetWindowText( WINDOW_TITLE " - Lap timer" );

	TIMER_MSG_KILL;
}


void CVsdwinDlg::OnMenuGymkhana(){
	char szTmp[ 8 ];

	m_uMode = MODE_GYMKHANA;
	TIMER_CLK_STOP;
	m_Menu.CheckMenuRadioItem( ID_MENU_LAPTIME, ID_MENU_ZERO_ONE, ID_MENU_GYMKHANA, MF_BYCOMMAND );
	SetWindowText( WINDOW_TITLE " - Gymkhana" );
	
	m_EditClock.SetWindowText( "Ready     " );
	m_uLapTimePrev = LAPTIME_INVALID;
	
	// �R�}���h���M
	sprintf( szTmp, "%Xg", m_uGymkhanaStartMargin );
	ERS_Puts( m_iComm, szTmp );
	
	//TIMER_MSG_START( "GYMKHAnA" );
	TIMER_MSG_START( "GYMKA" );
}

void CVsdwinDlg::OnMenuZeroFour(){
	char szTmp[ 8 ];
	
	m_uMode = MODE_ZERO_FOUR;
	TIMER_CLK_STOP;
	m_Menu.CheckMenuRadioItem( ID_MENU_LAPTIME, ID_MENU_ZERO_ONE, ID_MENU_ZERO_FOUR, MF_BYCOMMAND );
	SetWindowText( WINDOW_TITLE " - 0-400m" );
	
	m_EditClock.SetWindowText( "Ready     " );
	m_uLapTimePrev = LAPTIME_INVALID;
	
	// �R�}���h���M
	sprintf( szTmp, "%Xf", m_uStartGThrethold );
	ERS_Puts( m_iComm, szTmp );
	
	TIMER_MSG_START( "0-400" );
}

void CVsdwinDlg::OnMenuZeroOne(){
	char szTmp[ 8 ];
	
	m_uMode = MODE_ZERO_ONE;
	TIMER_CLK_STOP;
	m_Menu.CheckMenuRadioItem( ID_MENU_LAPTIME, ID_MENU_ZERO_ONE, ID_MENU_ZERO_ONE, MF_BYCOMMAND );
	SetWindowText( WINDOW_TITLE " - 0-100km/h" );
	
	m_EditClock.SetWindowText( "Ready     " );
	m_uLapTimePrev = LAPTIME_INVALID;
	
	// �R�}���h���M
	sprintf( szTmp, "%Xo", m_uStartGThrethold );
	ERS_Puts( m_iComm, szTmp );
	
	TIMER_MSG_START( "0-100" );
}

void CVsdwinDlg::OnMenuSpeed(){
	m_uLEDMode = LEDMODE_SPEED;
	m_Menu.CheckMenuRadioItem( ID_MENU_SPEED, ID_MENU_W_TEMP, ID_MENU_SPEED, MF_BYCOMMAND );
	ERS_Puts( m_iComm, "s" );
}

void CVsdwinDlg::OnMenuTaco(){
	m_uLEDMode = LEDMODE_TACO;
	m_Menu.CheckMenuRadioItem( ID_MENU_SPEED, ID_MENU_W_TEMP, ID_MENU_TACO, MF_BYCOMMAND );
	ERS_Puts( m_iComm, "t" );
}

void CVsdwinDlg::OnMenuWTemp() 
{
	m_uLEDMode = LEDMODE_W_TEMP;
	m_Menu.CheckMenuRadioItem( ID_MENU_SPEED, ID_MENU_W_TEMP, ID_MENU_W_TEMP, MF_BYCOMMAND );
	ERS_Puts( m_iComm, "w" );
}

void CVsdwinDlg::OnMenuLedReverse(){
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	UINT	uMenuState = m_Menu.GetMenuState( ID_MENU_LED_REVERSE, MF_BYCOMMAND ) & MF_CHECKED;
	
	m_Menu.CheckMenuItem(
		ID_MENU_LED_REVERSE,
		MF_BYCOMMAND | ( uMenuState ? MF_UNCHECKED : MF_CHECKED )
	);
	ERS_Puts( m_iComm, uMenuState ? "0r" : "1r" );
}

void CVsdwinDlg::OnMenuSetting(){
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	#ifdef DEBUG
		UINT uLapTime = ( UINT )( GetTickCount() * ( H8HZ / 1000.0 / 65536 ));
		PostMessage( WM_DOCOMM, 'L', uLapTime );
	#endif
}

void CVsdwinDlg::OnMenuGearMode(){
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	UINT	uMenuState = m_Menu.GetMenuState( ID_MENU_SHIFT_WARN, MF_BYCOMMAND ) & MF_CHECKED;
	
	m_Menu.CheckMenuItem(
		ID_MENU_SHIFT_WARN,
		MF_BYCOMMAND | ( uMenuState ? MF_UNCHECKED : MF_CHECKED )
	);
	ERS_Puts( m_iComm, uMenuState ? "0G" : "3G" );
}

void CVsdwinDlg::OnMenuBeep(){
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	UINT	uMenuState = m_Menu.GetMenuState( ID_MENU_BEEP, MF_BYCOMMAND ) & MF_CHECKED;
	
	m_Menu.CheckMenuItem(
		ID_MENU_BEEP,
		MF_BYCOMMAND | ( uMenuState ? MF_UNCHECKED : MF_CHECKED )
	);
	ERS_Puts( m_iComm, uMenuState ? "0b" : "1b" );
}

void CVsdwinDlg::OnMenuReplay(){
	UINT	uMenuState = m_Menu.GetMenuState( ID_MENU_REPLAY, MF_BYCOMMAND ) & MF_CHECKED;
	
	m_Menu.CheckMenuItem(
		ID_MENU_REPLAY,
		MF_BYCOMMAND | ( uMenuState ? MF_UNCHECKED : MF_CHECKED )
	);
	
	m_bReplay = !uMenuState;
}

void CVsdwinDlg::OnMenuOpenFirm(){
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	
	CFileDialog dlg(
		TRUE, NULL, m_strFirmwareName, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		"Firmware (*.mot)|*.mot||", this
	);
	if( dlg.DoModal() == IDOK ){
		m_strFirmwareName = dlg.GetPathName();
		m_bReloadFirmware = TRUE;
	}
}

/*** �h���b�v�n���h�� *******************************************************/

void CVsdwinDlg::OnDropFiles(HDROP hDropInfo){
	
	char szFileName[ MAX_PATH ];
	char szBuf[ BUF_SIZE ];
	UINT uFileNum = ::DragQueryFile( hDropInfo, 0xFFFFFFFF, szFileName, MAX_PATH );
	
	UINT	uMin, uSec, uMSec;
	char	*p;
	
	// ���łɊJ���Ă�����C����
	if( m_fpRecordLog.m_hFile != CFile::hFileNull ){
		m_fpRecordLog.Close();
	}
	
	for( UINT u = 0; u < uFileNum; ++u ){
		::DragQueryFile( hDropInfo, u, szFileName, MAX_PATH );
		
		// �t�@�C�����J��
		if( !m_fpRecordLog.Open( szFileName, CFile::modeRead /*| CFile::typeText*/ )){
			// �J���Ȃ������̂ŁC����
			continue;
		}
		
		// �t�@�C���T�C�Y�擾
		m_uSizeRecordLog = m_fpRecordLog.GetLength();
		
		// �X���C�_�R���g���[���ݒ�
		m_SliderPlayPos.SetRangeMin( 0 );
		m_SliderPlayPos.SetRangeMax( m_uSizeRecordLog );
		m_SliderPlayPos.SetPos( 0 );
		
		if( m_bReplay ){
			// ���v���C���[�h�Ȃ�C1���� FileOpen ���āCReplay �^�C�}�[�N��
			StartReplayTimer();
			break;
		}
		while( fgets( szBuf, BUF_SIZE, m_fpRecordLog.m_pStream ) != NULL ){
			if(
				// ���b�v�^�C���L�^��������
				( p = strstr( szBuf, "LAP" )) != NULL &&
				sscanf( p, "%*s %d:%d.%d", &uMin, &uSec, &uMSec ) == 3
			){
				AddLapTime( szBuf, ( uMin * 60 + uSec ) * 1000 + uMSec, FALSE );
			}
		}
		
		m_fpRecordLog.Close();
	}
}

/*** �X���C�_�R���g���[�� ***************************************************/

void CVsdwinDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar){
	
	char szBuf[ BUF_SIZE ];
	
	if( m_fpRecordLog.m_hFile != CFile::hFileNull ){
		CSliderCtrl* pTrackBar = ( CSliderCtrl* )pScrollBar;
		
		// �^�C�}�[����
		if( feof( m_fpRecordLog.m_pStream )) StartReplayTimer();
		
		// seek
		m_fpRecordLog.Seek( pTrackBar->GetPos(), CFile::begin );
		
		// ���̉��s�܂ŃX�L�b�v
		fgets( szBuf, BUF_SIZE, m_fpRecordLog.m_pStream );
		
		DebugMsgD( "scroll:%d %d\n",  pTrackBar->GetPos(), feof( m_fpRecordLog.m_pStream ));
	}
}
