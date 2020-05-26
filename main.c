
#include	"extern.h"

/*
	PMS132 ICE notes -- p.95 of PMS132 datasheet

	PDK5S-I-S01 doesn’t support the instruction NADD/COMP of PMS132.
	PDK5S-I-S01 doesn’t support SYSCLK=ILRC/16 of PMS132.
	PDK5S-I-S01 doesn’t support the function TM2.GPCRS/TM3.GPCRS of PMS132.
	PDK5S-I-S01 doesn’t support the function PWMG2C.PA5.
	PDK5S-I-S01 doesn’t support the function ADCRGC.BG_2V/BG_3V_BG_4V, and fix BG_1V2 only.
	PDK5S-I-S01 doesn’t support the code options: GPC_PWM, PWM_Source, and TMx_bit.
	Fast Wakeup time is different from PDK5S-I-S01: 128 SysClk, PMS132: 45 ILRC
	Watch dog time out period is different from PDK5S-I-S01 ...
*/

/*
	PMS132 PWM -- p.45 of PMS132 datasheet

	Timer2 and Timer3 as source for PWM.

	Timer2: 8-bit incrementing counter, clear to zero automatically on counter upper bound.
		tm2c[7:4] clock source
		tm2s[6:5] pre scalar
		tm2s[4:0] scalar
		tm2b[7:0] counter upper bound

		Periodical
			f = (clock source) / (2 * (upper bound + 1) * (prescalar) * (scalar + 1))

		PWM
			f = (clock source) / (256 * (prescalar) * (scalar + 1))
			duty = (upper bound + 1) / 256
*/
	
void	FPPA0 (void)
{
	/*
		Internal high RC oscillator
		IHRC: 16 MHz typical
		Current consumption when SYSCLK=IHRC/2: 0.8-1.8 mA

		Internal low RC oscillator
		ILRC: 55-75 KHz typical, low precision
		Current consumption when SYSCLK=ILRC/1: 30-60 uA = 0.03-0.06 mA
	*/

	.ADJUST_IC	SYSCLK=IHRC/8, IHRC=16MHz, VDD=5V;
	//	WatchDog Disable, RAM 0 ~ 0xF temporary be used
	//	You can add the follow code :
	//		CLKMD.En_WatchDog	=	1;		// WatchDog Enable

	// Set TM2 clock source
	tm2c = 0b0010_1010; // IHRC, PA3, PWM, normal polarity

	// Set TM2 prescalar and scalar
	tm2s = 0b0010_0000; // 8-bit PWM, prescalar 4, scalar 0

	// Set TM2 upper bound
	tm2b = 127;	// 25%

	// f = 16 MHz / (256 * 4 * (0 + 1)) = 15 KHz

	BYTE duty = 0;
	BIT state = 0;

	while(1)
	{
		// Wait 100 us @ 8 MHz (0.125 us): 100/0.125 = 800 cycles
		.delay 800 * 10;

		// Change PWM duty
		if (state == 0)
		{
			duty += 1;
			if (duty == 0xFF) {
				state = 1;
			}
		}
		else
		{
			duty -= 1;
			if (duty == 0x00) {
				state = 0;
			}
		}
		tm2b = duty;

		nop;
	}
}
