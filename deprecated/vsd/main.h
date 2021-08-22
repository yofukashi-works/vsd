/*****************************************************************************
	
	$Id$
	
	VSD - poor VSD
	Copyright(C) by DDS
	
	main.h -- main / main2 common header
	
*****************************************************************************/

/*** common macros **********************************************************/

#define H8HZ			16030000
#define SERIAL_DIVCNT	16		// �V���A���o�͂��s������
#define LOG_FREQ		16.0

// ���݃M�A�����߂�Ƃ��̃X���b�V�����h�ݒ�l
// 10%�}�[�W���͔p�~
#define GEAR_TH( g )	(( UINT )( GEAR_RATIO ## g * 256 + 0.5 ))

// �X�s�[�h * 100/Taco ��
// �M�A�� * �}�[�W������Ȃ��āCave( �M�An, �M�An+1 ) �ɕύX
#define KPH_GEAR1 0.981168441
#define KPH_GEAR2 1.637149627
#define KPH_GEAR3 2.187031944
#define KPH_GEAR4 2.7815649
#define KPH_GEAR5 3.370602172

#define GEAR_RATIO1 (( KPH_GEAR1 + KPH_GEAR2 ) / 2 )
#define GEAR_RATIO2 (( KPH_GEAR2 + KPH_GEAR3 ) / 2 )
#define GEAR_RATIO3 (( KPH_GEAR3 + KPH_GEAR4 ) / 2 )
#define GEAR_RATIO4 (( KPH_GEAR4 + KPH_GEAR5 ) / 2 )

// �V�t�g�_�E���x�����C���̃M�A�ł����̉�]�������������x��
#define SHIFTDOWN_WARN	6500

#define REV_LIMIT		6500
#define SH_DOWN_TH( g )	(( UINT )( SHIFTDOWN_WARN * KPH_GEAR ## g ))


// ���Ԃ�C�z�C�������30�p���X
// ������g���Ă���Ƃ����ϐ��ŉςɂ��C�V���A���o�R�Őݒ�ɂ���K�v������
#define PULSE_PER_1KM_NORMAL	(( double )14958.80127 )	// �m�[�}��
#define PULSE_PER_1KM			(( double )15473.76689 )	// CE28N

#define ITOA_RADIX_BIT	7
#define ITOA_DIGIT_NUM	(( 32 + ITOA_RADIX_BIT - 1 ) / ITOA_RADIX_BIT )

#define ACC_1G_X	6762.594337
#define ACC_1G_Y	6591.556755
#define ACC_1G_Z	6659.691379

/*** macros *****************************************************************/

#ifndef _WIN32
//#define RISE_EDGE_CAR_SIGNAL

//#define IR_FLASHER
//#define TEST1SEC

// ���j�^�[�N��
#define MONITOR_ENTRY	( *(( void (*)( void ))0x10E ))
//#define GoMonitor	INT000

// �\�t�g���Z�b�g
#define SoftReset	( *(( void (*)( void ))0x100 ))

// LED �A�j���[�V�����f�[�^��`�p
#define LED_ANIME_PARAM_NUM	6
#define LED_ANIME( a, b, c, d, bar, time ) \
	( time ) / ( CALC_DIVCNT ), FONT_ ## a, FONT_ ## b, FONT_ ## c, FONT_ ## d, bar

// VRAM �p�f�[�^�ꊇ����
#define MakeDisp( a, b, c, d ) \
	(( FONT_ ## a << 24 ) | ( FONT_ ## b << 16 ) | ( FONT_ ## c << 8 ) | FONT_ ## d )

// AD �ϊ��@���W�X�^
#define	ADC_THROTTLE	AD.ADDRD
#define	ADC_BRAKE		AD.ADDRC
//#define	G_SENSOR_X	AD.ADDRC
#define	G_SENSOR_Y	AD.ADDRB
#define	G_SENSOR_Z	AD.ADDRA

#define BEEP_OFF	0xFFFF

#define BZero( v )			bzero(( UCHAR *)( &v ), sizeof( v ))
#define VOLATILE( t, v )	( *( volatile t *)&v )

/*** const ******************************************************************/

#define BLINK_RATE	( 1 << 4 )	// �u�����NRate

#define CALC_DIVCNT		8		// Taco �v�Z�����s������
#define DISP_DIVCNT		1		// speed �\�����s������

#define SPEED_ADJ		(( ULONG )(( double )H8HZ * 3600 * 100 / PULSE_PER_1KM ))
#define TACO_ADJ		(( ULONG )(( double )H8HZ * 60 / 2 ))

// ���O��NewLap���炱�ꂾ���󂩂Ȃ���NewLap�Ƃ��ĔF�߂Ȃ�(3�b)
#define NEWLAP_MIN_INTERVAL	( 3 * 256 )

#define EOL		"\r\n"

// LED �\���֌W
enum {
	MODE_LAPTIME,
	MODE_ZERO_FOUR,
	MODE_ZERO_ONE,
	MODE_ZERO_ONE_WAIT,	// 0-100 �S�[���҂�
};

enum {	// g_Flags �� bit���ɒ���
	DISPMODE_OPENING,
	DISPMODE_MSG,
	DISPMODE_MSG_LOOP,
	DISPMODE_MSG_PC,
	DISPMODE_ANIME_PC,
	DISPMODE_SPEED,
	DISPMODE_TACHO,
	DISPMODE_MILEAGE,
	DISPMODE_IR,
};

// �M�A�E�^�R�\���֌W
enum {
	GM_TOWN,			// 0 town ���[�h�̃^�R�o�[
	GM_CIRCUIT,			// 1 �T�[�L�b�g���[�h�̃^�R�o�[
	GM_BL_MAIN,			// 2 +���u���~�b�g���� Main �u�����N
	GM_GEAR,			// 3 +�M�A�x��
	GM_DESIRED_GEAR,	// 4 +�œK�M�A�\��
};

// �^�b�`�p�b�h�̃N���b�N��
enum {
	SWCMD_TACHO_SPD		= 1,
	SWCMD_AUTOMODE,
	SWCMD_VCARIB,
};

enum {
	AM_DISP,	// TBAR + Tacho��Speed �̎����؊���
	AM_TBAR,	// �^�R�o�[�����؊���
	AM_NUM
};

#define DISP_FINISH		0xFFFF
#define DBL_CLICK_TIME	8

/*** new type ***************************************************************/

// ��l�߂Ȃ̂ŉ��ɒǋL����
typedef struct {
	// 16bit
	UCHAR	uDispModeNext	:4;
	UCHAR	uDispMode		:4;
	UCHAR	uGearMode		:3;
	UCHAR	uAutoMode		:1;
	UCHAR	uLapMode		:2;
	BOOL	bBlinkMain		:1;
	BOOL	bBlinkSub		:1;
	
	BOOL	bBeep			:1;
	BOOL	bNewLap			:1;
	BOOL	bOpenCmd		:1;
	BOOL	bOutputSerial	:1;
} Flags_t;

typedef struct {
	UCHAR	uPushElapsed;
	UCHAR	uPushCnt;
	UCHAR	uPrev		:4;
	UCHAR	uTrig		:4;
} PushSW_t;

typedef union {
	ULONG	dw;
	struct	{ UINT	h, l; } w;
} UNI_LONG;

typedef struct {
	UNI_LONG	Time;			// timer W �̃J�E���g
	UNI_LONG	PrevTime;		// �O��� timer W �̃J�E���g
	UINT		uPulseCnt;		// �p���X���͉�
	UINT		uVal;			// �X�s�[�h���̌v�Z�l
} PULSE;

typedef union {
	ULONG	lDisp;
	UCHAR	cDisp[ 4 ];
} VRAM;

typedef struct{
	UINT	uGx, uGy;
	UINT	uPrevGx;
} DispVal_t;

/*** extern *****************************************************************/
#endif
