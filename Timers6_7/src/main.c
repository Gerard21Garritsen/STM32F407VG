/*This project is an example of how to configure
 * Timers 6 and 7 to use it as simple timer which
 * overflows every 180 ms and generate an ISR which
 * blinking Blue Led
 */
#include "stm32f4xx.h"

//configure RCC clock to work with HSE (8 MHz) directly
void RCC_ClockConfig(void);

//configure pin PTD15 as Output Push-pull
void GPIO_Config(void);

//configure timer 6 to overflows each 180 ms and generate an ISR
void Timer6_Initialize(void);

//timer 6 ISR routine
void TIM6_DAC_IRQHandler(void);



int main(void)
{
	RCC_ClockConfig();
	GPIO_Config();
	Timer6_Initialize();


	while(1)
	{



	}

	return 0;
}


void RCC_ClockConfig(void)
{
	//reset CFGR, disable PLL and reset PLLCFGR
	RCC -> CFGR = (uint32_t)(0x0);
	RCC -> CR &= ~(uint32_t)(1 << 24);
	RCC -> PLLCFGR = (uint32_t)(0x0);

	//switch wait states to zero due to speed AHB bus
	FLASH -> ACR &= ~(uint32_t)(0xF);

	//switch to HSE clock as main clock
	RCC -> CFGR |= (uint32_t)(0x1);
	while(!(RCC -> CFGR & (uint32_t)(0x4)));

}


void GPIO_Config(void)
{
	//enable PORTD clock
	RCC -> AHB1ENR |= (uint32_t)(0x8);

	//configure PD15 as Output Push-pull
	GPIOD -> MODER |= (uint32_t)(1 << 30);
	GPIOD -> OTYPER &= ~(uint32_t)(1 << 15);
	GPIOD -> OSPEEDR &= ~(uint32_t)(0x3 << 30);

}


void Timer6_Initialize(void)
{
	//enable Timer 6 clock
	RCC -> APB1ENR |= (uint32_t)(1 << 4);

	//configure timer to overflows each 180 ms
	TIM6 -> CR1 = (uint16_t)(0x80);
	TIM6 -> CR2 = (uint16_t)(0x00);
	TIM6 -> PSC = 127; //set up prescaler (prescaler calculated - 1)
	TIM6 -> ARR = 11249; //load ARR register
	TIM6 -> EGR = (uint16_t)(0x1); //enable UG bit

	TIM6 -> DIER = (uint16_t)(0x1); //enable Interrupt

	//configure priority interrupt and enable it
	NVIC_ClearPendingIRQ(TIM6_DAC_IRQn);
	NVIC_SetPriorityGrouping(0x7); //only one group
	NVIC_SetPriority(TIM6_DAC_IRQn, 0x0); //the highest
	NVIC_EnableIRQ(TIM6_DAC_IRQn); //enable TIM6 interrupt on NVIC

	//turn on Timer 6
	TIM6 -> CR1 |= (uint16_t)(0x1);
}


void TIM6_DAC_IRQHandler(void)
{
	if(TIM6 -> SR & (uint16_t)(0x1))
	{
		TIM6 -> SR &= ~(uint16_t)(0x1); //clear flag

		GPIOD -> ODR ^= (uint32_t)(1 << 15); //switch PD15 logic state

	}

}
