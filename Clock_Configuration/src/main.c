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
	GPIO_Config();
	GPIOD -> BSRRL = (uint16_t)(1 << 13);


	while(1)
	{



	}


	return 0;
}


void RCC_ClockConfig(void)
{


}


void GPIO_Config(void)
{
	GPIOD -> MODER |= (uint32_t)(1 << 26); //pin PD13 as output (orange led)
	GPIOD -> OTYPER |= (uint32_t)(1 << 13); //pin PD13 as Push-pull
	GPIOD -> OSPEEDR |= (uint32_t)(0x0); //pin PD13 with low speed

}


void SysTick_Initialize(void)
{
	//select AHB bus clock
	SysTick -> CTRL = (uint32_t)(0x4);
	SysTick -> VAL = (uint32_t)(0x0);
	SysTick -> LOAD = (uint32_t)(CPU_FREQ/1000);

	//turn on SysTick
	SysTick -> CTRL |= (uint32_t)(1 << 16);

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
