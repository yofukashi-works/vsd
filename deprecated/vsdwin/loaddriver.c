/*****************************************************************************
		Device driver easy load / unload
		Aug 20 1999 kashiwano masahiro
*****************************************************************************/

#include <windows.h>
#include <conio.h>
#include "dds_lib.h"

/*****************************************************************************

�f�o�C�X�h���C�o��o�^�E�J�n����
�߂�l
	TRUE	����I��
	FALSE	�h���C�o�[�o�^�C�J�n���s
			�f�o�C�X�h���C�o�𐧌�ł��錠�����Ȃ��Ǝ��s����

����
	szFileName		�h���C�o�[�̃t�@�C����
	szDriverName	�h���C�o�[�̖��O�D�h���C�o�[�����ł��閼�O
					UnloadDriver�̈����ɂ��g��

*****************************************************************************/

BOOL LoadDriver( char *szFileName, char *szDriverName ){
	SC_HANDLE		hSCManager;
	SC_HANDLE		hService;
	SERVICE_STATUS	serviceStatus;
	BOOL			bRet = FALSE;
	
	// �T�[�r�X�R���g���[���}�l�[�W�����J��
	hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !hSCManager ) return( FALSE );
	
	// ���Ƀh���C�o�[�����݂��邩�m�F���邽�߂Ƀh���C�o�[���J��
	hService = OpenService(
		hSCManager,
		szDriverName,
		SERVICE_ALL_ACCESS
	);
	
	if( hService ){
		// ���ɓ��삵�Ă���ꍇ�͒�~�����č폜����
		// �ʏ�̓h���C�o�[�����݂���Ƃ���LoadDriver���Ăяo���Ȃ��̂ŕ��i�͂��肦�Ȃ�
		// �h���C�o�ُ̈킪�l������
		bRet = ControlService( hService, SERVICE_CONTROL_STOP, &serviceStatus );
		bRet = DeleteService( hService );
		CloseServiceHandle( hService );
	}
	
	// �h���C�o�[��o�^����
	hService = CreateService(
		hSCManager,
		szDriverName,
		szDriverName,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,		// �J�[�l���h���C�o
		SERVICE_DEMAND_START,		// ���StartService()�ɂ���ĊJ�n����
		SERVICE_ERROR_NORMAL,
		szFileName,					// �h���C�o�[�t�@�C���̃p�X
		NULL, NULL, NULL, NULL, NULL
	);
	
	if( hService ) {
		
		// �h���C�o�[���J�n����
		bRet = StartService( hService, 0, NULL );
		
		// �n���h�������
		CloseServiceHandle( hService );
	}
	// �T�[�r�X�R���g���[���}�l�[�W�������
	CloseServiceHandle( hSCManager );
	
	return( bRet );
}

/*****************************************************************************

�h���C�o�[���~����
�߂�l
	TRUE	����I��
	FALSE	���s

����
	szDriverName	�h���C�o�[�̖��O

*****************************************************************************/

BOOL UnloadDriver( char *szDriverName ){
	SC_HANDLE		hSCManager;
	SC_HANDLE		hService;
	SERVICE_STATUS  serviceStatus;
	BOOL			bRet = FALSE;
	
	// �T�[�r�X�R���g���[���}�l�[�W�����J��
	hSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( !hSCManager ) return FALSE;
	
	// �h���C�o�[�̃T�[�r�X���J��
	hService = OpenService( hSCManager, szDriverName, SERVICE_ALL_ACCESS );
	
	if( hService ) {
		// �h���C�o�[���~������ 
		bRet = ControlService( hService, SERVICE_CONTROL_STOP, &serviceStatus );
		
		// �h���C�o�[�̓o�^������
		if( bRet == TRUE ) bRet = DeleteService( hService );
		
		// �n���h�������
		CloseServiceHandle( hService );
	}
	// �T�[�r�X�R���g���[���}�l�[�W�������
	CloseServiceHandle( hSCManager );
	
	return( bRet );
}

/*** �J�����g dir �� giveio.sys �����[�h���� ********************************/
/*
	���� : szDriverName �� LoadDriver �̂���Ɠ���
	�o�� : TRUE = ����
*/
BOOL LoadGiveIO( char *szDriverName ){
	
	HANDLE	hDriver;
	char	szModuleName[ MAX_PATH ];
	char	*p;
	
	GetModuleFileName( NULL, szModuleName, MAX_PATH );
	if( p = StrTokFile( NULL, szModuleName, STF_NODE )) strcpy( p, "GIVEIO.SYS" );
	
	if( !LoadDriver( szModuleName, szDriverName )) return( FALSE );
	
    hDriver = CreateFile(
    	"\\\\.\\giveio", GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	);
    
    if( hDriver == INVALID_HANDLE_VALUE ) return( FALSE );
	
	CloseHandle( hDriver );
	return( TRUE );
}

/*********************************************************************
  Set PC's speaker frequency in Hz.  The speaker is controlled by an
Intel 8253/8254 timer at I/O port addresses 0x40-0x43.
*********************************************************************/

void SetBeepFreq(int hz){
	hz = 1193180 / hz;						// clocked at 1.19MHz
	_outp(0x43, 0xb6);						// timer 2, square wave
	_outp(0x42, hz);
	_outp(0x42, hz >> 8);
}

/*********************************************************************
  Pass a note, in half steps relative to 400 Hz.  The 12 step scale
is an exponential thing.  The speaker control is at port 0x61.
Setting the lowest two bits enables timer 2 of the 8253/8254 timer
and turns on the speaker.
*********************************************************************/
/*
void PlayNote(NOTE note){
	_outp(0x61, _inp(0x61) | 0x03);			// start speaker going
	SetBeepFreq((int)(400 * pow(2, note.pitch / 12.0)));
	Sleep(note.duration);
	_outp(0x61, _inp(0x61) & ~0x03);		// stop that racket!
}
*/
/*********************************************************************
  Open and close the GIVEIO device.  This should give us direct I/O
access.  Then try it out by playin' our tune.
*********************************************************************/

BOOL LoadBeepDriver( char *drivername ){
	HANDLE h;
	
	char	szModuleName[ MAX_PATH ];
	char	*p;
	
	GetModuleFileName( NULL, szModuleName, MAX_PATH );
	if( p = StrTokFile( NULL, szModuleName, STF_NODE ))
		strcpy( p, "GIVEIO.SYS" );
	
	if( LoadDriver( szModuleName, drivername ) == FALSE ){
		return FALSE;
	}
	
    h = CreateFile("\\\\.\\giveio", GENERIC_READ, 0, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(h == INVALID_HANDLE_VALUE) {
        //printf("Couldn't access giveio device\n");
        return FALSE;
    }
	CloseHandle(h);
	return TRUE;
}
