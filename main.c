
#include "chuankou.h"
#include "main.h"

#include "math.h"
#include "duoji.h"
#include "yaogan.h"
#include "rom.h"

//步兵第一次补担，官方到英雄。英雄阵亡或不需要时，步兵改变请求，官方到步兵。
int prepareflag[4],preparecnt[4],begincnt[4],beginflag[4],pinstate[4]={0,0,0,0},timcnt,miao,fen,dutytime,bujinumx,herorequest,requestack;

uint32_t g_ui32SysClock,pwmcalc[4]={600,600,2500,2500},pwmdir[4];


#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

uint32_t yaogandata[2];

void duojimove(int dnum)
{
	
	if (pwmcalc[dnum]>=1532)
	{
			pwmdir[dnum]=0;//minus
	}
	else if(pwmcalc[dnum]<=550)
	{
			pwmdir[dnum]=1;
	}
	
	if(pwmdir[dnum])
	{
		pwmcalc[dnum]+=2;
	}
	else{
	  pwmcalc[dnum]-=2;
	}
	VAL_LIMIT(pwmcalc[dnum], 500,2500);
	switch (dnum)
	{
			case 0:PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, pwmcalc[dnum]);break;
			case 1:PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, pwmcalc[dnum]);break;
			case 2:PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 3000-pwmcalc[dnum]);break;

	}
	 
}

int
main(void)
{
    //
    // Set the clocking to run directly from the crystal at 120MHz.
    //

	g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 16000000);

	 pwminit();
	 ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
	 ROM_GPIOPinTypeGPIOInput(GPIO_PORTM_BASE,GPIO_PIN_0);
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTM_BASE,GPIO_PIN_1);
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTM_BASE,GPIO_PIN_2);
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTM_BASE,GPIO_PIN_3);
  GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_0,
                         GPIO_STRENGTH_2MA,     
                         GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_1,
                         GPIO_STRENGTH_2MA,     
                         GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_2,
                         GPIO_STRENGTH_2MA,     
                         GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_3,
                         GPIO_STRENGTH_2MA,     
                         GPIO_PIN_TYPE_STD_WPU);		

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	 ROM_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_3);
   
	
	SysTickPeriodSet(g_ui32SysClock / 1000);
    SysTickIntEnable();
    SysTickEnable();
	
	IntEnable(INT_PWM0_0|INT_PWM0_1|INT_UART3);
		IntMasterEnable();
		
		pwmcalc[3]=600;
	
    while(1)
    {
			pinstate[0]=GPIOPinRead(GPIO_PORTM_BASE,GPIO_PIN_0);
			if(ROM_GPIOPinRead(GPIO_PORTM_BASE,GPIO_PIN_0)==0)
			{
					prepareflag[0]=1;
				dutytime++;
				herorequest=0;
			}
			
			pinstate[1]=GPIOPinRead(GPIO_PORTM_BASE,GPIO_PIN_1);
			if(ROM_GPIOPinRead(GPIO_PORTM_BASE,GPIO_PIN_1)==0)
			{
					prepareflag[1]=1;
				dutytime++;
				herorequest=0;
			}
			
			pinstate[2]=GPIOPinRead(GPIO_PORTM_BASE,GPIO_PIN_2);
			if(ROM_GPIOPinRead(GPIO_PORTM_BASE,GPIO_PIN_2)==0)
			{
					prepareflag[2]=1;
				dutytime++;
				herorequest=1;
			}
			
			pinstate[3]=GPIOPinRead(GPIO_PORTM_BASE,GPIO_PIN_3);
			if(ROM_GPIOPinRead(GPIO_PORTM_BASE,GPIO_PIN_3)==0)
			{
					prepareflag[3]=1;
				dutytime++;
			}
    }
}

void waitingforlove(int who)
{
			if(prepareflag[who]==1)
		{
				preparecnt[who]++;
				bujinumx++
			;
		}
		
		
		if(preparecnt[who]>1)
		{
				beginflag[who]=1;
				preparecnt[who]=0;
		}
		
		if(beginflag[who]==1)
		{
				begincnt[who]++;
				duojimove(who);
			//led[who]=1;
		}
		
		if(begincnt[who]>1000)
		{
			
			beginflag[who]=0;
			prepareflag[who]=0;
			begincnt[who]=0;
			preparecnt[who]=0;
			switch (who)
			{
					case 0:PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0,600);break;
					case 1:PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 600);break;
					case 2:PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 2500);break;

			}
		}

}

int pause;
void turnhead()  //2500___bubing PE1/2     500----hero PE 3
{
		
			pwmcalc[3]=3000-pwmcalc[3];
			PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, pwmcalc[3]);
	if(pwmcalc[3]<1500)
	{
			requestack=1;
					ROM_GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_1|GPIO_PIN_3,
                             GPIO_PIN_3);

		//heroledon
		//infantryledoff
	}else{
		requestack=0;
			ROM_GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_1|GPIO_PIN_3,
                             GPIO_PIN_2|GPIO_PIN_1);

		//heroledoff
		//infantryledon
	}
			

}

void ifunmatch()
{
	if(herorequest!=requestack)   
	{
			turnhead();
		

	}
	

}	
void
SysTickIntHandler(void)
{
	waitingforlove(0);
	waitingforlove(1);
	waitingforlove(2);
	

	if(bujinumx==1)
	{
		herorequest=1;
	}
	ifunmatch();
}
