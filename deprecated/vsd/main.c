/*****************************************************************************
	
	$Id$
	
	VSD - poor VSD
	Copyright(C) by DDS
	
	main.c -- main routine
	
*****************************************************************************/

#include <machine.h>
#include "dds.h"
#include "3664s.h"
#include "sci.h"
#include "main.h"
#include "led_charcode.h"

#ifdef MONITOR_ROM
	#include "main2.c"
#else
	#include "rom_entry.h"
	//#define MINIMIZE	// �Ǿ� FIRMWARE
#endif

#ifndef MINIMIZE
/*** macros *****************************************************************/
/*** const ******************************************************************/
/*** new type ***************************************************************/
/*** prototype **************************************************************/
/*** extern *****************************************************************/
/*** gloval vars ************************************************************/

UINT	g_uThrottle;

#define rx_buffer_end	RX_BUFFER_LENGTH	// ��ROM ���ǤϾä�
#define tx_buffer_end	TX_BUFFER_LENGTH

#undef sci_write
size_t sci_write(UB* data, size_t n)
{
	UB *data_begin, *data_end;

	if(!data){
		int szbuf = ( UINT )tx_idx_tail - tx_idx_head;
		if(szbuf < 0)szbuf += RX_BUFFER_LENGTH;
		return (size_t)szbuf;
	}

	data_begin = data;
	data_end = data + n;
	while(data != data_end){
		tx_buffer[ tx_idx_tail++ ] = *data++;
		if(tx_idx_tail == tx_buffer_end)
			tx_idx_tail = 0;
	}
	/* �����쥸���������ξ����������� */
	SCI3.SCR3.BIT.TIE = 1;
	return data - data_begin;
}

// ��ROM ���κݡ��ؿ�̾����
#pragma interrupt( sci_int_handler_new )
void sci_int_handler_new( void ){
	/* �����ǡ�������ץƥ� */
	if(SCI3.SSR.BIT.TDRE){
		/* �Хåե������ξ���������λ */
		if(tx_idx_head == tx_idx_tail){
			/* TEND���� */
			SCI3.SCR3.BIT.TIE = 0;
		}else{
			SCI3.TDR = tx_buffer[ tx_idx_head++ ];	/* �񤭹��ळ�Ȥǥե饰�򥯥ꥢ���� */
			if(tx_idx_head == tx_buffer_end)
				tx_idx_head = 0;
		}
	}
	/* �����ǡ����ե� */
	if(SCI3.SSR.BIT.RDRF){
		rx_buffer[ rx_idx_tail++ ] = SCI3.RDR;	/* �ɤळ�Ȥǥե饰�򥯥ꥢ���� */
		if(rx_idx_tail == rx_buffer_end)
			rx_idx_tail = 0;
	}
	/* �����Х�� �ե졼�� �ѥ�ƥ� ���顼 */
	if(SCI3.SSR.BYTE & 0x38 ){
//		if(error_handler)error_handler();	// ��RAM ���κݤλ��ꡤ��Ǥ�ɤ�
		SCI3.SSR.BYTE &= ~0x38;
	}
}

/*** �����ߥ٥����ơ��֥� *************************************************/

//#undef VectorTable
void * const VectorTable2[] = {
	( void *)0x100,	//  0
	( void *)0x100,	//  1
	( void *)0x100,	//  2
	( void *)0x100,	//  3
	( void *)0x100,	//  4
	( void *)0x100,	//  5
	( void *)0x100,	//  6
	( void *)0x100,	//  7
	( void *)0x100,	//  8
	( void *)0x100,	//  9
	( void *)0x100,	// 10
	( void *)0x100,	// 11
	( void *)0x100,	// 12
	( void *)0x100,	// 13
	int_irq0,		// 14
	int_irq1,		// 15
	int_irq2,		// 16
	int_irq3,		// 17
	int_wkp,		// 18
	int_timer_a,	// 19
	( void *)0x100,	// 20
	int_timer_w,	// 21
	int_timer_v_IR,	// 22
	sci_int_handler_new,// 23
	( void *)0x100,	// 24
	( void *)0x100,	// 25
	( void *)0x100,	// 26
	( void *)0x100,	// 27
	( void *)0x100,	// 28
	( void *)0x100,	// 29
	( void *)0x100,	// 30
	( void *)0x100,	// 31
};

/*** itoa *******************************************************************/

#undef SerialPack
void SerialPack( UINT uVal ){
	
	UCHAR	szBuf[ 4 ];
	UCHAR	*p = szBuf;
	
	if(( uVal & 0xFF ) >= 0xFE ){ *p++ = 0xFE; uVal -= 0xFE; }
	*p++ = uVal & 0xFF;
	uVal >>= 8;
	
	if( uVal >= 0xFE ){ *p++ = 0xFE; uVal -= 0xFE; }
	*p++ = uVal;
	
	sci_write( szBuf, p - szBuf );
}

/*** ���ꥢ����� ***********************************************************/

#undef OutputSerial
INLINE void OutputSerial( void ){
	UCHAR c = 0xFF;
	
	SerialPack( g_Tacho.uVal );
	SerialPack( g_Speed.uVal );
	
	SerialPack( g_uMileage );
	SerialPack(( TA.TCA << 8 ) | g_IR.uVal & 0xFF );
	SerialPack( g_DispVal.uGy );
	SerialPack( g_DispVal.uGx );
	SerialPack( g_uThrottle );
	
	/*** ��åץ�����ɽ�� ***/
	if( g_Flags.bNewLap ){
		g_Flags.bNewLap = FALSE;
		SerialPack( g_IR.Time.w.l );
		SerialPack( g_IR.Time.w.h );
	}
	
	sci_write( &c, 1 );
}

/*** �桼�� IO ���� *********************************************************/

#undef ProcessUIO
void ProcessUIO( void ){
	UCHAR c;
	if( g_Flags.bOutputSerial )	OutputSerial();		// serial ����
	while( sci_read( &c, 1 )) DoInputSerial( c );	// serial ����
	ProcessPushSW();								// sw ����
}

/*** ���ơ����Ѳ��Ԥ� & LED ɽ�� ********************************************/

#undef WaitStateChange
INLINE void WaitStateChange( void ){
	ULONG	uGx 		= 0;
	ULONG	uGy 		= 0;
	ULONG	uThrottle	= 0;
	UINT	uCnt		= 0;
	
	UCHAR	cTimerA;
	
	/*** ���ơ����Ѳ��Ԥ� ***/
	
	/*** WDT ***/
	WDT.TCSRWD.BYTE = ( 1 << 6 );	// TCWE
	WDT.TCWD		= 0;
	
	cTimerA = TA.TCA & ~( CALC_DIVCNT - 1 );
	
	while( cTimerA == ( TA.TCA & ~( CALC_DIVCNT - 1 ))){
		uGx			+= G_SENSOR_Z;	// ���� G �θ��м��ѹ�
		uGy			+= G_SENSOR_Y;
		uThrottle	+= ADC_THROTTLE;
		++uCnt;
		
		// ���Ȥꤢ������®���Τ��᥹���åס�ROM ���κݤ����褵����
		//if( !( uCnt & ( 128 - 1 ))) LED_Driver();
	}
	
	// G �η׻�
	g_DispVal.uGx = ((( UINT )( uGx / uCnt )) >> 1 ) + ( g_DispVal.uGx >> 1 );
	g_DispVal.uGy = ((( UINT )( uGy / uCnt )) >> 1 ) + ( g_DispVal.uGy >> 1 );
	
	// ����åȥ�
	g_uThrottle	= uThrottle	/ uCnt;
}

/*** main *******************************************************************/

#ifdef MONITOR_ROM
	#pragma entry( main )
#else
	__entry( vect = 0 )
#endif
int main( void ){
	UCHAR	bProcessUIOFlag = 0;
	
	#ifdef MONITOR_ROM
		if( !IO.PDR5.BIT.B4 ) IR_Flasher();
	#else
		// RAM �¹ԤΥ�������
		InitSector( __sectop( "B" ), __secend( "B" ));
	#endif
	
	InitMain();
	
	// ��
	g_uSpeedCalcConst = ( UINT )( 3600.0 * 100.0 / PULSE_PER_1KM * ( 1 << 11 ));
	VectorTblPtr = ( void *)VectorTable2;
	
	// �� SCI �ν���ͤ��ѹ�����
	IO.PMR1.BIT.TXD = 1;
	SCI3.SCR3.BIT.TE = 1;
	
//	Print( g_szMsgOpening );
	
	for(;;){
		WaitStateChange();
		CheckStartByGSensor();	// G���󥵡��ˤ�륹�����ȸ���
		
		/*
		//�ǥХå��ѥ�����
		g_Tacho.Time.dw = g_Speed.Time.dw = GetTimerW32();
		g_Tacho.uPulseCnt = (( GetRTC() >> 7 ) & 0x7 ) + 1;
		g_Speed.uPulseCnt = (( GetRTC() >> 6 ) & 0xF ) + 1;
		if( IO.PDR5.BYTE & ( 1 << 6 )){
			g_Tacho.uPulseCnt = 8;
		}
		*/
		
		ComputeMeter();			// speed, tacho, gear �׻�
		DispLED_Carib();		// LED ɽ���ǡ�������
		
		if( bProcessUIOFlag = ~bProcessUIOFlag ){
			ProcessUIO();		// SIO, sw ���� UserIO ����
		}
		
		ProcessAutoMode();		// �����ȥ⡼��
	}
}

#else
__entry( vect = 0 ) int main( void ){
	SoftReset(); // RAM �� ROM �������Ѥ��ʤ���Ф���
}
#endif
