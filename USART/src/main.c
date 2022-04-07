/*This is an example of how to configure USART1
 * with a baud rate of 115200 bps
 */
#include <stdio.h>
#include "stm32f4xx.h"

#define AHB_FREQ 168000000 //AHB bus clock frequency

//configure RCC clock to work using HSE clock directly (8 MHz)
void RCC_ClockConfigure(void);

//configure USART1 with a baud rate of 115200
void USART1_Initialize(void);

void USART1_WriteChar(char caracter);

void USART1_WriteString(char *cadena);

void SysTick_Initialize(void);

void Delay_ms(uint16_t time);




int main(void)
{
	char buffer[25];
	float dato = 3.141592;

	RCC_ClockConfigure();
	SysTick_Initialize();
	USART1_Initialize();

	sprintf(buffer, "Value is:%.3f\n\r", dato);


	while(1)
	{
		USART1_WriteString(buffer);
		Delay_ms(100);

	}

	return 0;
}


void RCC_ClockConfigure(void)
{
	//reset CFGR register, switch to HSI, disable PLL, HSE and clear PLLCFGR
	RCC -> CFGR = (uint32_t)(0x0);
	RCC -> CR &= ~(uint32_t)(1 << 24);
	RCC -> CR &= ~(uint32_t)(1 << 16);
	RCC -> PLLCFGR = (uint32_t)(0x0);

	//enable HSE clock
	RCC -> CR |= (uint32_t)(1 << 16);
	while(!(RCC -> CR & (uint32_t)(1 << 17)));

	//set up Flash wait states to 5, and enable cache, management memory and prefetch
	FLASH -> CR |= (uint32_t)(0x05);
	FLASH -> CR |= (uint32_t)(0x07 << 8);

	//set up PLLCFGR prescalers to get 168 MHz as PLL Out
	RCC -> PLLCFGR |= (uint32_t)(0x04); //set PLLM prescaler with 4
	RCC -> PLLCFGR |= (uint32_t)(168 << 6); //set PLLN x 168
	RCC -> PLLCFGR &= ~(uint32_t)(0x3 << 16); //set PLLP prescaler by 2 to get 168 MHz
	RCC -> PLLCFGR |= (uint32_t)(0x4 << 24); //set PLLQ prescaler by 4 to get 42 MHz
	RCC -> PLLCFGR |= (uint32_t)(1 << 22); //set PLL source comes from HSE

	//set up APB1 and APB22 prescalers
	RCC -> CFGR |= (uint32_t)(0x9400); //set prescalers to get 84 MHz for APB2 and 42 MHz for APB1

	//enable PLL
	RCC -> CR |= (uint32_t)(1 << 24);
	while(!(RCC -> CR & (uint32_t)(1 << 25)));

	//switch to PLL as main clock
	RCC -> CFGR |= (uint32_t)(0x02);
	while(!(RCC -> CFGR & (uint32_t)(0x8)));

}


void USART1_Initialize(void)
{
	RCC -> APB2ENR |= (uint32_t)(0x10); //enable USART1 clock
	RCC -> AHB1ENR |= (uint32_t)(0x2); //enable PORTB clock

	//configure Tx (PB6) and Rx (PB7) pins to use them with USART1
	GPIOB -> MODER |= (uint16_t)(0x2 << 12); //Alternate function PB6 and PB7 Input
	GPIOB -> OTYPER &= ~(uint16_t)(1 << 6); //pin PB6 as Push-pull
	GPIOB -> OSPEEDR |= (uint16_t)(0xA << 12); //very High Speed
	GPIOB -> PUPDR |= (uint16_t)(1 << 14); //enable Pull-up for PB7 (Rx)

	//adressing pins PA6 and PA7 to USART1 Alternate Function
	GPIOB -> AFR[0] |= (uint32_t)(0x77 << 24);

	//set up USART1 with 8 bits lenght, no parity and baud rate 115200
	USART1 -> CR2 &= ~(uint16_t)(0x3 << 12); //select 1 Stop bit
	USART1 -> BRR = (uint16_t)(45 << 4) | (uint16_t)(0x9); //load BRR to get 115200 bauds
	USART1 -> CR1 |= (uint16_t)(0x000C); //OVER8 with 0, 8 bits lenght, with none parity, and Enable Tx and Rx

	//Clear flags
	USART1 -> SR = (uint16_t)(0x0);

	//Turn on USART1
	USART1 -> CR1 |= (uint16_t)(1 << 13);

}


void USART1_WriteChar(char caracter)
{

	USART1 -> DR = caracter;
	while(!(USART1 -> SR & (uint16_t)(1 << 6))); //wait until Tx is empty

}


void USART1_WriteString(char *cadena)
{
	while(*cadena)
	{
		USART1_WriteChar(*cadena);
		cadena++;

	}

}


void SysTick_Initialize(void)
{
	SysTick -> CTRL = (uint32_t)(0x04); //select AHB bus clock as reference
	SysTick -> VAL = (uint32_t)(0x0);
	SysTick -> LOAD = (AHB_FREQ/1000);

	SysTick -> CTRL |= (uint32_t)(0x1); //turn on SysTick

}


void Delay_ms(uint16_t time)
{
	SysTick -> VAL = (uint32_t)(0x0);
	SysTick -> CTRL &= ~(uint32_t)(1 << 16); //clear flag

	for(uint16_t i = 0; i < time; i++)
	{
		while(!(SysTick -> CTRL & (uint32_t)(1 << 16)));

		SysTick -> CTRL &= ~(uint32_t)(1 << 16);

	}

}
