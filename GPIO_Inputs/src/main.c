/*This project is an example of how to configure GPIO Inputs
 * using User button onboard, besides we configure RCC clock
 * to working at 8 MHz using external oscillator directly
 */
#include "stm32f4xx.h"

#define CPU_FREQ 8000000 //AHB bus speed is 8 MHz

//configure RCC clock to work directly with HSE clock
void RCC_ClockConfig(void);

//configure pin PD15 as Output Push-pull and PA0 as Input Pull-down
void GPIO_Config(void);

void SysTick_Initialize(void);

void Delay_ms(uint16_t time);


int main(void)
{
	RCC_ClockConfig();
	GPIO_Config();
	SysTick_Initialize();


	while(1)
	{
		if(GPIOA -> IDR & (uint32_t)(0x1))
			GPIOD -> BSRRL |= (uint32_t)(1 << 15);
		else
			GPIOD -> BSRRH |= (uint32_t)(1 << 15);


	}

	return 0;
}


void RCC_ClockConfig(void)
{
	//Reset CFGR, disable PLL and reset PLLCFGR
	RCC -> CFGR = (uint32_t)(0x0); //switch to HSI as main clock
	RCC -> CR &= ~(uint32_t)(1 << 24);
	RCC -> PLLCFGR = (uint32_t)(0x0);

	//switch to HSE as main clock
	RCC -> CFGR |= (uint32_t)(0x1);
	while(!(RCC -> CFGR & (uint32_t)(0x4)));

}


void GPIO_Config(void)
{
	//enable PORTA and PORTD clocks
	RCC -> AHB1ENR |= (uint32_t)(0x9);

	//configure pin PA0 as Input with Pull-down and PD13 as Output Push-pull
	GPIOA -> MODER &= ~(uint32_t)(0x3);
	GPIOA -> PUPDR |= (uint32_t)(0x2);

	GPIOD -> MODER |= (uint32_t)(1 << 30);
	GPIOD -> OTYPER &= ~(uint32_t)(1 << 15);
	GPIOD -> OSPEEDR &= ~(uint32_t)(0x3);

}


void SysTick_Initialize(void)
{
	SysTick -> CTRL |= (0x4);
	SysTick -> VAL = (uint32_t)(0x0);
	SysTick -> LOAD = (uint32_t)(CPU_FREQ/1000);

	SysTick -> CTRL |= (uint32_t)(0x1); //turn on SysTick

}

void Delay_ms(uint16_t time)
{
	SysTick -> VAL = (uint32_t)(0x0);
	SysTick -> CTRL &= ~(uint32_t)(1 << 16);

	for(uint32_t i = 0; i < time; i++)
	{
		while(!(SysTick -> CTRL & (uint32_t)(0x10000)));

		SysTick -> CTRL &= ~(uint32_t)(1 << 16); //clear flag

	}


}
