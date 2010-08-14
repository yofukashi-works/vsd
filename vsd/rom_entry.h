#define _INITSCT	((void (*)( void ))0x00003d36)
#define VectorTblPtr	(*(extern void *(*))0x0000ff7e)
#define g_VRAM	(*(VRAM (*))0x0000ff76)
#define g_Tacho	(*(PULSE (*))0x0000ff2c)
#define g_Speed	(*(PULSE (*))0x0000ff38)
#define g_IR	(*(PULSE (*))0x0000ff44)
#define g_lParam	(*(ULONG (*))0x0000ff50)
#define g_uTimerWovf	(*(UINT (*))0x0000ff54)
#define g_uRemainedMillage	(*(UINT (*))0x0000ff56)
#define g_uStartGTh	(*(UINT (*))0x0000ff58)
#define g_uMileage	(*(UINT (*))0x0000ff5a)
#define g_uDispNext	(*(UINT (*))0x0000ff5c)
#define g_uRTC	(*(UINT (*))0x0000ff5e)
#define g_uPrevTW	(*(UINT (*))0x0000ff60)
#define g_uHz	(*(UINT (*))0x0000ff62)
#define g_szLEDMsg	(*(UCHAR *(*))0x0000ff64)
#define g_DispVal	(*(DispVal_t (*))0x0000ff66)
#define g_Flags	(*(Flags_t (*))0x0000ff6c)
#define g_PushSW	(*(PushSW_t (*))0x0000ff6f)
#define g_cLEDBar	(*(char (*))0x0000ff72)
#define g_cAutoModeTimer	(*(UCHAR (*))0x0000ff73)
#define g_uVideoCaribCnt	(*(UCHAR (*))0x0000ff74)
#define g_cFont	(*(const UCHAR (*)[])0x00004316)
#define g_cFontR	(*(const UCHAR (*)[])0x00004320)
#define g_szMsgReadyToGo	(*(const char (*)[])0x0000432a)
#define g_szMsgTacho	(*(const char (*)[])0x00004336)
#define g_szMsgSpeed	(*(const char (*)[])0x0000433b)
#define g_szMsgWaterTemp	(*(const char (*)[])0x0000433f)
#define g_szMsgIRCnt	(*(const char (*)[])0x00004345)
#define g_szMsgFuel	(*(const char (*)[])0x00004348)
#define g_szMsgMileage	(*(const char (*)[])0x0000434d)
#define g_szMsgCalibrate	(*(const char (*)[])0x00004352)
#define g_szMsgOpening	(*(const char (*)[])0x0000435c)
#define g_LEDAnimeOpening	(*(const UCHAR (*)[])0x00004386)
#define g_cLEDFont	(*(const UCHAR (*)[])0x000043c3)
#define g_uTachoBar	(*(const UINT (*)[])0x000042cc)
#define PrintLED4	((void (*)( UINT uNum ))0x00000420)
#define PrintLED	((void (*)( UINT uNum ))0x00000476)
#define GetTimerW32	((ULONG (*)( void ))0x000004c0)
#define int_timer_a	((void (*)( void ))0x000004dc)
#define GetRTC	((ULONG (*)( void ))0x00000536)
#define int_timer_w	((void (*)( void ))0x00000564)
#define int_irq0	((void (*)( void ))0x00000578)
#define int_wkp	((void (*)( void ))0x000005e6)
#define int_irq1	((void (*)( void ))0x00000664)
#define int_irq2	((void (*)( void ))0x00000734)
#define int_irq3	((void (*)( void ))0x00000770)
#define INT_SCI3	((void (*)( void ))0x00000828)
#define int_timer_v_IR	((void (*)( void ))0x00000840)
#define VectorTable	(*(void * const (*)[])0x000042d6)
#define GoMonitor	((void (*)( void ))0x00000862)
#define SetBeep	((void (*)( UINT uCnt ))0x00000874)
#define ComputeMeterTacho	((void (*)( void ))0x000008c6)
#define g_uSpeedCalcConst	(*(UINT (*))0x0000ff7c)
#define ComputeMeterSpeed	((void (*)( void ))0x00000968)
#define ComputeGear	((void (*)( UINT uTachoBar[] ))0x00000ab8)
#define ComputeGear2	((void (*)( void ))0x00000c24)
#define ComputeMeter	((void (*)( void ))0x00000c46)
#define DoAnimation	((UINT (*)( UCHAR *LEDAnime, UINT uDispPtr ))0x00000e52)
#define DispLEDMsg	((UINT (*)( UCHAR *szMsg, UINT uDispPtr ))0x00000ec4)
#define DispLEDMsgPC	((void (*)( UCHAR cPat ))0x00000f54)
#define DispLEDAnimePC	((void (*)( ULONG lPat ))0x00000fe0)
#define DispMsgStart	((void (*)( char *pMsg ))0x00001048)
#define DispAnimeStart	((void (*)( UCHAR *pMsg ))0x0000106a)
#define PrintLEDStr	((void (*)( UCHAR *szMsg ))0x0000108a)
#define DispLED	((void (*)( UCHAR cDispMode ))0x000010d8)
#define DispLED_Carib	((void (*)( void ))0x00001332)
#define LED_Driver	((void (*)( void ))0x00001610)
#define Print	((void (*)( char *szMsg ))0x000016f0)
#define Print_ud	((void (*)( UINT uNum, char c ))0x00001712)
#define Print_ItoA	((void (*)( ULONG u, char c ))0x0000177a)
#define SerialPack	((void (*)( UINT uVal ))0x000017da)
#define ItoA128	((void (*)( UINT uUpper, UINT uLower ))0x0000182e)
#define OutputSerial	((void (*)( void ))0x0000187e)
#define DoInputSerial	((void (*)( char c ))0x00001b00)
#define ScanPushSW	((UCHAR (*)( void ))0x00001d9e)
#define ProcessPushSW	((void (*)( void ))0x00001e18)
#define ProcessAutoMode	((UINT (*)( void ))0x000024f2)
#define CheckStartByGSensor	((void (*)( void ))0x00002564)
#define SetupHW	((void (*)( void ))0x00002616)
#define InitMain	((void (*)( void ))0x000026ba)
#define bzero	((void (*)( UCHAR *p, UINT uSize ))0x0000278a)
#define InitSector	((void (*)( UINT *uBStart, UINT *uBEnd ))0x000027a2)
#define ProcessUIO	((void (*)( void ))0x000027b2)
#define WaitStateChange	((void (*)( void ))0x00003366)
#define IR_Flasher	((void (*)( void ))0x000034f2)
#define sci_init	((void (*)(VP_INT exinf))0x00000272)
#define sci_int_handler	((void (*)(VP_INT exinf))0x000003a4)
#define sci_flush_rx	((void (*)(void))0x00000206)
#define sci_flush_tx	((void (*)(void))0x00000212)
#define sci_read	((size_t (*)(UB* data, size_t n))0x000002bc)
#define sci_write	((size_t (*)(UB* data, size_t n))0x00000322)
// warning: not found: static UB rx_buffer[32]
// warning: not found: static UB tx_buffer[48]
#define rx_idx_head#0000ff28#, #rx_idx_tail	(*(UCHAR (*))0x0000ff29)
#define tx_idx_head#0000ff2a#, #tx_idx_tail	(*(UCHAR (*))0x0000ff2b)
#define error_handler	((void (*(*))(void))0x0000fed6)
#define sci_wait_tx_empty	((void (*)(void))0x0000021e)
#define sci_wait_rx_full	((BOOL (*)(void))0x00000224)
#define msecwait	((void (*)(int msec))0x0000023c)
