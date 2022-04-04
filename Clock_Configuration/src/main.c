/*This project is an example of how to configure RCC clock
 * to reach max speed (168 MHz)
 */
#include "stm32f4xx.h"

#define CPU_FREQ 168000000 //AHB bus speed

//configure RCC clock to core works at 168 MHz
void RCC_ClockConfig(void);

//configure GPIO pins
void GPIO_Config(void);

//configure SysTick timer to overflows each 1 ms
void SysTick_Initialize(void);

//function to make some delays
void Delay_ms(uint16_t time);




int main(void)
{
	RCC_ClockConfig();
	GPIO_Config();
	SysTick_Initialize();


	while(1)
	{
		GPIOD -> BSRRL |= (uint32_t)(1 << 13);
		Delay_ms(100);
		GPIOD -> BSRRH |= (uint32_t)( 1 << 13);
		Delay_ms(100);


	}


	return 0;
}


void RCC_ClockConfig(void)
{
	//disable PLL, HSE clocks, reset PLLCFGR and CFGR registers
	RCC -> CFGR = (uint32_t)(0x0); //switch to HSI clock as main clock
	RCC -> CR &= ~(uint32_t)(1 << 24);
	RCC -> CR &= ~(uint32_t)(1 << 16);
	RCC -> PLLCFGR = (uint32_t)(0x0);

	//enable HSE clock and wait for it to stabilize
	RCC -> CR |= (uint32_t)(1 << 16); //turn on HSE clock
	while(!(RCC -> CR & (uint32_t)(1 << 17))); //wait until HSE clock is ready

	//enable prefetch, cache memory, management memory and configure wait states to 5
	FLASH -> ACR |= (uint32_t)(0x0705);

	//enable PWR clock
	RCC -> APB1ENR |= (uint32_t)(1 << 28);

	//configure PLLCFGR register to reach 168 MHz to AHB bus
	RCC -> PLLCFGR |= (uint32_t)(0x4); //set to 4 PLLM prescaler
	RCC -> PLLCFGR |= (uint32_t)(168 << 6); //set to 168 PLLN multiply
	RCC -> PLLCFGR |= (uint32_t)(0x2 << 16); // set PLLP prescaler to 2 to get 168 MHz
	RCC -> PLLCFGR |= (uint32_t)(0x7 << 24); // set PLLQ prescaler to 7 to get 48 MHz for USB
	RCC -> PLLCFGR |= (uint32_t)(1 << 22); //select PLL source from HSE clock

	//configure CFGR register, to get a frequency of 42 MHz for APB1 and 84 MHZ for APB2
	RCC -> CFGR |= (uint32_t)(0x5 << 10) | (uint32_t)(0x4 << 13);

	//finally turn on PLL and switch to PLL as main clock for AHB bus
	RCC -> CR |= (uint32_t)(1 << 24);
	while(!(RCC -> CR & (uint32_t)(1 << 25)));

	RCC -> CFGR |= (uint32_t)(0x2); // switch to PLL clock
	while(!(RCC -> CFGR & (uint32_t)(0x2 << 2))); // wait until PLL is main clock

}


void GPIO_Config(void)
{
	RCC -> AHB1ENR |= (uint32_t)(1 << 3); //enable PORTD clock

	GPIOD -> MODER |= (uint32_t)(1 << 26); //pin PD13 as output (orange led)
	GPIOD -> OTYPER &= ~(uint32_t)(1 << 13); //pin PD13 as Push-pull
	GPIOD -> OSPEEDR |= (uint32_t)(0x3); //pin PD13 with low speed

}


void SysTick_Initialize(void)
{
	//select AHB bus clock (168 MHz)
	SysTick -> CTRL = (uint32_t)(0x4);
	SysTick -> VAL = (uint32_t)(0x0);
	SysTick -> LOAD = (uint32_t)(CPU_FREQ/1000);
	SysTick -> CTRL &= ~(uint32_t)(1 << 16); //clear flag

	//turn on SysTick
	SysTick -> CTRL |= (uint32_t)(0x1);

}


void Delay_ms(uint16_t time)
{
	SysTick -> VAL = (uint32_t)(0x0); //reset current value
	SysTick -> CTRL &= ~(uint32_t)(1 << 16); //clear flag

	for(uint16_t i = 0; i < time; i++)
	{
		while(!(SysTick -> CTRL & (uint32_t)(1 << 16)));

		SysTick -> CTRL &= ~(uint32_t)(1 << 16); //clear flag

	}

}
