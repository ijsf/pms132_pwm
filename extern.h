
#if	_SYS(AT_CHIP)			//	for real IC code
//	BIT	PWM_Pin	:	PA.4;
#else						//	for ICE emulation
//	.ICE_CMD		Change_PA4_to_PA1
//	BIT	PWM_Pin	:	PA.1;
#endif