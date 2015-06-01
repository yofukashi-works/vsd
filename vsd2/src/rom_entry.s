;*****************************************************************************
;	
;	VSD2 - vehicle data logger system2
;	Copyright(C) by DDS
;	
;	rom_entry.s -- rom entry point table
;	
;*****************************************************************************

	RSEG ROM_CODE:CODE(2)
	DS	0x1

	PUBWEAK	_Printf
_Printf:	// 0x080030ed
	DS	0x6AC

	PUBWEAK	_LitobDefault
_LitobDefault:	// 0x08003799
	DS	0x100

	PUBWEAK	_LdtobDefault
_LdtobDefault:	// 0x08003899
	DS	0x46C

	PUBWEAK	_GenldDefault
_GenldDefault:	// 0x08003d05
	DS	0x278

	PUBWEAK	_PutcharsDefault
_PutcharsDefault:	// 0x08003f7d
	DS	0x3C

	PUBWEAK	Standard_GetConfiguration
Standard_GetConfiguration:	// 0x08003fb9
	DS	0x18

	PUBWEAK	Standard_SetConfiguration
Standard_SetConfiguration:	// 0x08003fd1
	DS	0x30

	PUBWEAK	Standard_GetInterface
Standard_GetInterface:	// 0x08004001
	DS	0x24

	PUBWEAK	Standard_SetInterface
Standard_SetInterface:	// 0x08004025
	DS	0x44

	PUBWEAK	Standard_GetStatus
Standard_GetStatus:	// 0x08004069
	DS	0x8C

	PUBWEAK	Standard_ClearFeature
Standard_ClearFeature:	// 0x080040f5
	DS	0xBC

	PUBWEAK	Standard_SetEndPointFeature
Standard_SetEndPointFeature:	// 0x080041b1
	DS	0x70

	PUBWEAK	Standard_SetDeviceFeature
Standard_SetDeviceFeature:	// 0x08004221
	DS	0x20

	PUBWEAK	Standard_GetDescriptorData
Standard_GetDescriptorData:	// 0x08004241
	DS	0x1C

	PUBWEAK	DataStageOut
DataStageOut:	// 0x0800425d
	DS	0x94

	PUBWEAK	DataStageIn
DataStageIn:	// 0x080042f1
	DS	0x80

	PUBWEAK	NoData_Setup0
NoData_Setup0:	// 0x08004371
	DS	0xCC

	PUBWEAK	Data_Setup0
Data_Setup0:	// 0x0800443d
	DS	0x16C

	PUBWEAK	Setup0_Process
Setup0_Process:	// 0x080045a9
	DS	0x7C

	PUBWEAK	In0_Process
In0_Process:	// 0x08004625
	DS	0x58

	PUBWEAK	Out0_Process
Out0_Process:	// 0x0800467d
	DS	0x30

	PUBWEAK	Post0_Process
Post0_Process:	// 0x080046ad
	DS	0x48

	PUBWEAK	SetDeviceAddress
SetDeviceAddress:	// 0x080046f5
	DS	0x40

	PUBWEAK	NOP_Process
NOP_Process:	// 0x08004735
	DS	0x4

	PUBWEAK	__aeabi_ddiv
__aeabi_ddiv:	// 0x08004739
	DS	0x4FC

	PUBWEAK	RCC_DeInit
RCC_DeInit:	// 0x08004c35
	DS	0x4C

	PUBWEAK	RCC_HSEConfig
RCC_HSEConfig:	// 0x08004c81
	DS	0x3C

	PUBWEAK	RCC_WaitForHSEStartUp
RCC_WaitForHSEStartUp:	// 0x08004cbd
	DS	0x2C

	PUBWEAK	RCC_PLLConfig
RCC_PLLConfig:	// 0x08004ce9
	DS	0x18

	PUBWEAK	RCC_PLLCmd
RCC_PLLCmd:	// 0x08004d01
	DS	0xC

	PUBWEAK	RCC_SYSCLKConfig
RCC_SYSCLKConfig:	// 0x08004d0d
	DS	0x8

	PUBWEAK	RCC_GetSYSCLKSource
RCC_GetSYSCLKSource:	// 0x08004d15
	DS	0xC

	PUBWEAK	RCC_HCLKConfig
RCC_HCLKConfig:	// 0x08004d21
	DS	0x10

	PUBWEAK	RCC_PCLK1Config
RCC_PCLK1Config:	// 0x08004d31
	DS	0x8

	PUBWEAK	RCC_PCLK2Config
RCC_PCLK2Config:	// 0x08004d39
	DS	0x14

	PUBWEAK	RCC_USBCLKConfig
RCC_USBCLKConfig:	// 0x08004d4d
	DS	0xC

	PUBWEAK	RCC_ADCCLKConfig
RCC_ADCCLKConfig:	// 0x08004d59
	DS	0x14

	PUBWEAK	RCC_GetClocksFreq
RCC_GetClocksFreq:	// 0x08004d6d
	DS	0x88

	PUBWEAK	RCC_APB2PeriphClockCmd
RCC_APB2PeriphClockCmd:	// 0x08004df5
	DS	0x18

	PUBWEAK	RCC_APB1PeriphClockCmd
RCC_APB1PeriphClockCmd:	// 0x08004e0d
	DS	0x18

	PUBWEAK	RCC_GetFlagStatus
RCC_GetFlagStatus:	// 0x08004e25
	DS	0x34

	PUBWEAK	Virtual_Com_Port_init
Virtual_Com_Port_init:	// 0x08004e59
	DS	0x50

	PUBWEAK	Virtual_Com_Port_Reset
Virtual_Com_Port_Reset:	// 0x08004ea9
	DS	0xC8

	PUBWEAK	Virtual_Com_Port_Status_In
Virtual_Com_Port_Status_In:	// 0x08004f71
	DS	0x14

	PUBWEAK	Virtual_Com_Port_Status_Out
Virtual_Com_Port_Status_Out:	// 0x08004f85
	DS	0x4

	PUBWEAK	Virtual_Com_Port_Data_Setup
Virtual_Com_Port_Data_Setup:	// 0x08004f89
	DS	0x50

	PUBWEAK	Virtual_Com_Port_NoData_Setup
Virtual_Com_Port_NoData_Setup:	// 0x08004fd9
	DS	0x20

	PUBWEAK	Virtual_Com_Port_GetDeviceDescriptor
Virtual_Com_Port_GetDeviceDescriptor:	// 0x08004ff9
	DS	0x10

	PUBWEAK	Virtual_Com_Port_GetConfigDescriptor
Virtual_Com_Port_GetConfigDescriptor:	// 0x08005009
	DS	0x10

	PUBWEAK	Virtual_Com_Port_GetStringDescriptor
Virtual_Com_Port_GetStringDescriptor:	// 0x08005019
	DS	0x18

	PUBWEAK	Virtual_Com_Port_Get_Interface_Setting
Virtual_Com_Port_Get_Interface_Setting:	// 0x08005031
	DS	0x10

	PUBWEAK	Virtual_Com_Port_GetLineCoding
Virtual_Com_Port_GetLineCoding:	// 0x08005041
	DS	0x8

	PUBWEAK	Virtual_Com_Port_SetLineCoding
Virtual_Com_Port_SetLineCoding:	// 0x08005049
	DS	0x1C

	PUBWEAK	SetBTABLE
SetBTABLE:	// 0x08005065
	DS	0x10

	PUBWEAK	SetEPType
SetEPType:	// 0x08005075
	DS	0x10

	PUBWEAK	SetEPTxStatus
SetEPTxStatus:	// 0x08005085
	DS	0x24

	PUBWEAK	SetEPRxStatus
SetEPRxStatus:	// 0x080050a9
	DS	0x24

	PUBWEAK	SetEPTxValid
SetEPTxValid:	// 0x080050cd
	DS	0x18

	PUBWEAK	SetEPRxValid
SetEPRxValid:	// 0x080050e5
	DS	0x18

	PUBWEAK	Clear_Status_Out
Clear_Status_Out:	// 0x080050fd
	DS	0x14

	PUBWEAK	ClearDTOG_RX
ClearDTOG_RX:	// 0x08005111
	DS	0x1C

	PUBWEAK	ClearDTOG_TX
ClearDTOG_TX:	// 0x0800512d
	DS	0x30

	PUBWEAK	SetEPTxAddr
SetEPTxAddr:	// 0x0800515d
	DS	0x18

	PUBWEAK	SetEPRxAddr
SetEPRxAddr:	// 0x08005175
	DS	0x18

	PUBWEAK	GetEPTxAddr
GetEPTxAddr:	// 0x0800518d
	DS	0x18

	PUBWEAK	GetEPRxAddr
GetEPRxAddr:	// 0x080051a5
	DS	0x1C

	PUBWEAK	SetEPTxCount
SetEPTxCount:	// 0x080051c1
	DS	0x1C

	PUBWEAK	SetEPRxCount
SetEPRxCount:	// 0x080051dd
	DS	0x40

	PUBWEAK	GetEPRxCount
GetEPRxCount:	// 0x0800521d
	DS	0x20

	PUBWEAK	ByteSwap
ByteSwap:	// 0x0800523d
	DS	0xC

	PUBWEAK	__aeabi_memcpy
__aeabi_memcpy:	// 0x08005249
	DS	0x1A

	PUBWEAK	__aeabi_memcpy4
__aeabi_memcpy4:	// 0x08005263
	DS	0x0

	PUBWEAK	__aeabi_memcpy8
__aeabi_memcpy8:	// 0x08005263
	DS	0x1A2

	PUBWEAK	__aeabi_dmul
__aeabi_dmul:	// 0x08005405
	DS	0x1A4

	PUBWEAK	__aeabi_dsub
__aeabi_dsub:	// 0x080055a9
	DS	0x12

	PUBWEAK	__iar_dsub
__iar_dsub:	// 0x080055bb
	DS	0x14

	PUBWEAK	__iar_dsubStart
__iar_dsubStart:	// 0x080055cf
	DS	0x15A

	PUBWEAK	CTR_LP
CTR_LP:	// 0x08005729
	DS	0x15C

	PUBWEAK	USART_Init
USART_Init:	// 0x08005885
	DS	0x98

	PUBWEAK	USART_Cmd
USART_Cmd:	// 0x0800591d
	DS	0x1C

	PUBWEAK	USART_ITConfig
USART_ITConfig:	// 0x08005939
	DS	0x34

	PUBWEAK	USART_ReceiveData
USART_ReceiveData:	// 0x0800596d
	DS	0x8

	PUBWEAK	USART_GetITStatus
USART_GetITStatus:	// 0x08005975
	DS	0x3C

	PUBWEAK	USART_ClearITPendingBit
USART_ClearITPendingBit:	// 0x080059b1
	DS	0x10

	PUBWEAK	__iar_Dscale
__iar_Dscale:	// 0x080059c1
	DS	0x114

	PUBWEAK	NMIException
NMIException:	// 0x08005ad5
	DS	0x4

	PUBWEAK	HardFaultException
HardFaultException:	// 0x08005ad9
	DS	0x4

	PUBWEAK	MemManageException
MemManageException:	// 0x08005add
	DS	0x4

	PUBWEAK	BusFaultException
BusFaultException:	// 0x08005ae1
	DS	0x4

	PUBWEAK	UsageFaultException
UsageFaultException:	// 0x08005ae5
	DS	0x4

	PUBWEAK	DebugMonitor
DebugMonitor:	// 0x08005ae9
	DS	0x4

	PUBWEAK	SVCHandler
SVCHandler:	// 0x08005aed
	DS	0x4

	PUBWEAK	PendSVC
PendSVC:	// 0x08005af1
	DS	0x4

	PUBWEAK	SysTickHandler
SysTickHandler:	// 0x08005af5
	DS	0x4

	PUBWEAK	WWDG_IRQHandler
WWDG_IRQHandler:	// 0x08005af9
	DS	0x4

	PUBWEAK	PVD_IRQHandler
PVD_IRQHandler:	// 0x08005afd
	DS	0x4

	PUBWEAK	TAMPER_IRQHandler
TAMPER_IRQHandler:	// 0x08005b01
	DS	0x4

	PUBWEAK	RTC_IRQHandler
RTC_IRQHandler:	// 0x08005b05
	DS	0x4

	PUBWEAK	FLASH_IRQHandler
FLASH_IRQHandler:	// 0x08005b09
	DS	0x4

	PUBWEAK	RCC_IRQHandler
RCC_IRQHandler:	// 0x08005b0d
	DS	0x4

	PUBWEAK	EXTI0_IRQHandler
EXTI0_IRQHandler:	// 0x08005b11
	DS	0x4

	PUBWEAK	EXTI1_IRQHandler
EXTI1_IRQHandler:	// 0x08005b15
	DS	0x4

	PUBWEAK	EXTI2_IRQHandler
EXTI2_IRQHandler:	// 0x08005b19
	DS	0x4

	PUBWEAK	EXTI3_IRQHandler
EXTI3_IRQHandler:	// 0x08005b1d
	DS	0x4

	PUBWEAK	EXTI4_IRQHandler
EXTI4_IRQHandler:	// 0x08005b21
	DS	0x4

	PUBWEAK	DMAChannel1_IRQHandler
DMAChannel1_IRQHandler:	// 0x08005b25
	DS	0x4

	PUBWEAK	DMAChannel2_IRQHandler
DMAChannel2_IRQHandler:	// 0x08005b29
	DS	0x4

	PUBWEAK	DMAChannel3_IRQHandler
DMAChannel3_IRQHandler:	// 0x08005b2d
	DS	0x4

	PUBWEAK	DMAChannel4_IRQHandler
DMAChannel4_IRQHandler:	// 0x08005b31
	DS	0x4

	PUBWEAK	DMAChannel5_IRQHandler
DMAChannel5_IRQHandler:	// 0x08005b35
	DS	0x4

	PUBWEAK	DMAChannel6_IRQHandler
DMAChannel6_IRQHandler:	// 0x08005b39
	DS	0x4

	PUBWEAK	DMAChannel7_IRQHandler
DMAChannel7_IRQHandler:	// 0x08005b3d
	DS	0x4

	PUBWEAK	ADC_IRQHandler
ADC_IRQHandler:	// 0x08005b41
	DS	0x4

	PUBWEAK	USB_HP_CAN_TX_IRQHandler
USB_HP_CAN_TX_IRQHandler:	// 0x08005b45
	DS	0x4

	PUBWEAK	USB_LP_CAN_RX0_IRQHandler
USB_LP_CAN_RX0_IRQHandler:	// 0x08005b49
	DS	0xC

	PUBWEAK	CAN_RX1_IRQHandler
CAN_RX1_IRQHandler:	// 0x08005b55
	DS	0x4

	PUBWEAK	CAN_SCE_IRQHandler
CAN_SCE_IRQHandler:	// 0x08005b59
	DS	0x4

	PUBWEAK	EXTI9_5_IRQHandler
EXTI9_5_IRQHandler:	// 0x08005b5d
	DS	0x4

	PUBWEAK	TIM1_BRK_IRQHandler
TIM1_BRK_IRQHandler:	// 0x08005b61
	DS	0x4

	PUBWEAK	TIM1_UP_IRQHandler
TIM1_UP_IRQHandler:	// 0x08005b65
	DS	0x4

	PUBWEAK	TIM1_TRG_COM_IRQHandler
TIM1_TRG_COM_IRQHandler:	// 0x08005b69
	DS	0x4

	PUBWEAK	TIM1_CC_IRQHandler
TIM1_CC_IRQHandler:	// 0x08005b6d
	DS	0x4

	PUBWEAK	TIM2_IRQHandler
TIM2_IRQHandler:	// 0x08005b71
	DS	0x4

	PUBWEAK	TIM3_IRQHandler
TIM3_IRQHandler:	// 0x08005b75
	DS	0x4

	PUBWEAK	TIM4_IRQHandler
TIM4_IRQHandler:	// 0x08005b79
	DS	0x4

	PUBWEAK	I2C1_EV_IRQHandler
I2C1_EV_IRQHandler:	// 0x08005b7d
	DS	0x4

	PUBWEAK	I2C1_ER_IRQHandler
I2C1_ER_IRQHandler:	// 0x08005b81
	DS	0x4

	PUBWEAK	I2C2_EV_IRQHandler
I2C2_EV_IRQHandler:	// 0x08005b85
	DS	0x4

	PUBWEAK	I2C2_ER_IRQHandler
I2C2_ER_IRQHandler:	// 0x08005b89
	DS	0x4

	PUBWEAK	SPI1_IRQHandler
SPI1_IRQHandler:	// 0x08005b8d
	DS	0x4

	PUBWEAK	SPI2_IRQHandler
SPI2_IRQHandler:	// 0x08005b91
	DS	0x4

	PUBWEAK	USART1_IRQHandler
USART1_IRQHandler:	// 0x08005b95
	DS	0x28

	PUBWEAK	USART2_IRQHandler
USART2_IRQHandler:	// 0x08005bbd
	DS	0x4

	PUBWEAK	USART3_IRQHandler
USART3_IRQHandler:	// 0x08005bc1
	DS	0x4

	PUBWEAK	EXTI15_10_IRQHandler
EXTI15_10_IRQHandler:	// 0x08005bc5
	DS	0x4

	PUBWEAK	RTCAlarm_IRQHandler
RTCAlarm_IRQHandler:	// 0x08005bc9
	DS	0x4

	PUBWEAK	USBWakeUp_IRQHandler
USBWakeUp_IRQHandler:	// 0x08005bcd
	DS	0xFC

	PUBWEAK	__aeabi_dadd
__aeabi_dadd:	// 0x08005cc9
	DS	0x12

	PUBWEAK	__iar_dadd
__iar_dadd:	// 0x08005cdb
	DS	0x10A

	PUBWEAK	__aeabi_ldivmod
__aeabi_ldivmod:	// 0x08005de5
	DS	0x14

	PUBWEAK	__aeabi_uldivmod
__aeabi_uldivmod:	// 0x08005df9
	DS	0x150

	PUBWEAK	GPIO_Init
GPIO_Init:	// 0x08005f49
	DS	0xA0

	PUBWEAK	GPIO_ReadInputDataBit
GPIO_ReadInputDataBit:	// 0x08005fe9
	DS	0x10

	PUBWEAK	GPIO_SetBits
GPIO_SetBits:	// 0x08005ff9
	DS	0x4

	PUBWEAK	GPIO_ResetBits
GPIO_ResetBits:	// 0x08005ffd
	DS	0x4

	PUBWEAK	NVIC_PriorityGroupConfig
NVIC_PriorityGroupConfig:	// 0x08006001
	DS	0x10

	PUBWEAK	NVIC_Init
NVIC_Init:	// 0x08006011
	DS	0x84

	PUBWEAK	NVIC_SetVectorTable
NVIC_SetVectorTable:	// 0x08006095
	DS	0x18

	PUBWEAK	__iar_Dnorm
__iar_Dnorm:	// 0x080060ad
	DS	0xAC

	PUBWEAK	__WFI
__WFI:	// 0x08006159
	DS	0x4

	PUBWEAK	__WFE
__WFE:	// 0x0800615d
	DS	0x4

	PUBWEAK	__SEV
__SEV:	// 0x08006161
	DS	0x4

	PUBWEAK	__ISB
__ISB:	// 0x08006165
	DS	0x6

	PUBWEAK	__DSB
__DSB:	// 0x0800616b
	DS	0x6

	PUBWEAK	__DMB
__DMB:	// 0x08006171
	DS	0x6

	PUBWEAK	__SVC
__SVC:	// 0x08006177
	DS	0x4

	PUBWEAK	__MRS_CONTROL
__MRS_CONTROL:	// 0x0800617b
	DS	0x6

	PUBWEAK	__MSR_CONTROL
__MSR_CONTROL:	// 0x08006181
	DS	0xA

	PUBWEAK	__MRS_PSP
__MRS_PSP:	// 0x0800618b
	DS	0x6

	PUBWEAK	__MSR_PSP
__MSR_PSP:	// 0x08006191
	DS	0x6

	PUBWEAK	__MRS_MSP
__MRS_MSP:	// 0x08006197
	DS	0x6

	PUBWEAK	__MSR_MSP
__MSR_MSP:	// 0x0800619d
	DS	0x6

	PUBWEAK	__SETPRIMASK
__SETPRIMASK:	// 0x080061a3
	DS	0x4

	PUBWEAK	__RESETPRIMASK
__RESETPRIMASK:	// 0x080061a7
	DS	0x4

	PUBWEAK	__SETFAULTMASK
__SETFAULTMASK:	// 0x080061ab
	DS	0x4

	PUBWEAK	__RESETFAULTMASK
__RESETFAULTMASK:	// 0x080061af
	DS	0x4

	PUBWEAK	__BASEPRICONFIG
__BASEPRICONFIG:	// 0x080061b3
	DS	0x6

	PUBWEAK	__GetBASEPRI
__GetBASEPRI:	// 0x080061b9
	DS	0x6

	PUBWEAK	__REV_HalfWord
__REV_HalfWord:	// 0x080061bf
	DS	0x4

	PUBWEAK	__REV_Word
__REV_Word:	// 0x080061c3
	DS	0x6

	PUBWEAK	USB_Istr
USB_Istr:	// 0x080061c9
	DS	0x5C

	PUBWEAK	__aeabi_d2iz
__aeabi_d2iz:	// 0x08006225
	DS	0x2C

	PUBWEAK	__aeabi_d2uiz
__aeabi_d2uiz:	// 0x08006251
	DS	0xA

	PUBWEAK	__iar_d2uiz
__iar_d2uiz:	// 0x0800625b
	DS	0x22

	PUBWEAK	UserToPMABufferCopy
UserToPMABufferCopy:	// 0x0800627d
	DS	0x28

	PUBWEAK	PMAToUserBufferCopy
PMAToUserBufferCopy:	// 0x080062a5
	DS	0x63

	PUBWEAK	Virtual_Com_Port_ConfigDescriptor
Virtual_Com_Port_ConfigDescriptor:	// 0x08006308
	DS	0x45

	PUBWEAK	__iar_SubNrmlX
__iar_SubNrmlX:	// 0x0800634d
	DS	0x1C

	PUBWEAK	__iar_SubNrmlY
__iar_SubNrmlY:	// 0x08006369
	DS	0x1B

	PUBWEAK	Virtual_Com_Port_StringProduct
Virtual_Com_Port_StringProduct:	// 0x08006384
	DS	0x35

	PUBWEAK	EP3_OUT_Callback
EP3_OUT_Callback:	// 0x080063b9
	DS	0x28

	PUBWEAK	EP1_IN_Callback
EP1_IN_Callback:	// 0x080063e1
	DS	0xC

	PUBWEAK	sprintf
sprintf:	// 0x080063ed
	DS	0x34

	PUBWEAK	__aeabi_cdcmple
__aeabi_cdcmple:	// 0x08006421
	DS	0x30

	PUBWEAK	__aeabi_cdrcmple
__aeabi_cdrcmple:	// 0x08006451
	DS	0x30

	PUBWEAK	__aeabi_i2d
__aeabi_i2d:	// 0x08006481
	DS	0x14

	PUBWEAK	__aeabi_ui2d
__aeabi_ui2d:	// 0x08006495
	DS	0x2

	PUBWEAK	__iar_ui2d
__iar_ui2d:	// 0x08006497
	DS	0x1A

	PUBWEAK	PowerOn
PowerOn:	// 0x080064b1
	DS	0x2B

	PUBWEAK	Virtual_Com_Port_StringVendor
Virtual_Com_Port_StringVendor:	// 0x080064dc
	DS	0x29

	PUBWEAK	FLASH_SetLatency
FLASH_SetLatency:	// 0x08006505
	DS	0xC

	PUBWEAK	FLASH_PrefetchBufferCmd
FLASH_PrefetchBufferCmd:	// 0x08006511
	DS	0x18

	PUBWEAK	USB_Init
USB_Init:	// 0x08006529
	DS	0x67

	PUBWEAK	Virtual_Com_Port_StringSerial
Virtual_Com_Port_StringSerial:	// 0x08006590
	DS	0x31

	PUBWEAK	__iar_Memchr
__iar_Memchr:	// 0x080065c1
	DS	0x0

	PUBWEAK	memchr
memchr:	// 0x080065c1
	DS	0x18

	PUBWEAK	div
div:	// 0x080065d9
	DS	0x17

	PUBWEAK	APBAHBPrescTable
APBAHBPrescTable:	// 0x080065f0
	DS	0x10

	PUBWEAK	ADCPrescTable
ADCPrescTable:	// 0x08006600
	DS	0x4

	PUBWEAK	Virtual_Com_Port_DeviceDescriptor
Virtual_Com_Port_DeviceDescriptor:	// 0x08006604
	DS	0x15

	PUBWEAK	__aeabi_uidiv
__aeabi_uidiv:	// 0x08006619
	DS	0x0

	PUBWEAK	__aeabi_uidivmod
__aeabi_uidivmod:	// 0x08006619
	DS	0x14

	PUBWEAK	__iar_Strchr
__iar_Strchr:	// 0x0800662d
	DS	0x0

	PUBWEAK	strchr
strchr:	// 0x0800662d
	DS	0x14

	PUBWEAK	__exit
__exit:	// 0x08006641
	DS	0x20

	PUBWEAK	strlen
strlen:	// 0x08006661
	DS	0x20

	PUBWEAK	_SProut
_SProut:	// 0x08006681
	DS	0x20

	PUBWEAK	exit
exit:	// 0x080066a1
	DS	0x7

	PUBWEAK	Virtual_Com_Port_StringLangID
Virtual_Com_Port_StringLangID:	// 0x080066a8
	DS	0x9

	PUBWEAK	__aeabi_idiv0
__aeabi_idiv0:	// 0x080066b1
	DS	0x4

	PUBWEAK	__aeabi_ldiv0
__aeabi_ldiv0:	// 0x080066b5

	RSEG FLASH_DATA:DATA(4)
	DS	0x0

	PUBWEAK	pEpInt_IN
pEpInt_IN:	// 0x20004e54
	DS	0x1C

	PUBWEAK	pEpInt_OUT
pEpInt_OUT:	// 0x20004e70
	DS	0x1C

	PUBWEAK	linecoding
linecoding:	// 0x20004e8c
	DS	0x8

	PUBWEAK	Device_Table
Device_Table:	// 0x20004e94
	DS	0x4

	PUBWEAK	Device_Property
Device_Property:	// 0x20004e98
	DS	0x30

	PUBWEAK	User_Standard_Requests
User_Standard_Requests:	// 0x20004ec8
	DS	0x24

	PUBWEAK	Device_Descriptor
Device_Descriptor:	// 0x20004eec
	DS	0x8

	PUBWEAK	Config_Descriptor
Config_Descriptor:	// 0x20004ef4
	DS	0x8

	PUBWEAK	String_Descriptor
String_Descriptor:	// 0x20004efc
	DS	0x78

	PUBWEAK	HSEStatus
HSEStatus:	// 0x20004f74
	DS	0x4

	PUBWEAK	StartUpCounter
StartUpCounter:	// 0x20004f78
	DS	0x4

	PUBWEAK	StatusInfo
StatusInfo:	// 0x20004f7c
	DS	0x4

	PUBWEAK	count_out
count_out:	// 0x20004f80
	DS	0x4

	PUBWEAK	buffer_out
buffer_out:	// 0x20004f84
	DS	0x40

	PUBWEAK	count_in
count_in:	// 0x20004fc4
	DS	0x4

	PUBWEAK	Device_Info
Device_Info:	// 0x20004fc8
	DS	0x1C

	PUBWEAK	pInformation
pInformation:	// 0x20004fe4
	DS	0x4

	PUBWEAK	pProperty
pProperty:	// 0x20004fe8
	DS	0x4

	PUBWEAK	pUser_Standard_Requests
pUser_Standard_Requests:	// 0x20004fec
	DS	0x4

	PUBWEAK	wInterrupt_Mask
wInterrupt_Mask:	// 0x20004ff0
	DS	0x2

	PUBWEAK	SaveRState
SaveRState:	// 0x20004ff2
	DS	0x2

	PUBWEAK	SaveTState
SaveTState:	// 0x20004ff4
	DS	0x2

	PUBWEAK	bIntPackSOF
bIntPackSOF:	// 0x20004ff6
	DS	0x2

	PUBWEAK	wIstr
wIstr:	// 0x20004ff8
	DS	0x3

	PUBWEAK	EPindex
EPindex:	// 0x20004ffb
	DS	0x1

	PUBWEAK	Request
Request:	// 0x20004ffc
	DS	0x1

	PUBWEAK	bDeviceState
bDeviceState:	// 0x20004ffd
	END
