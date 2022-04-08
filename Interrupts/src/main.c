//This project is an example of how to configure External Interrupts
#include "stm32f4xx.h"

#define AHB_FREQ 8000000 //AHB bus clock 8 MHz

//configure RCC clock to work with HSE clock directly (8 MHz)
void RCC_ClockConfig(void);

/*configure pins PA0 as Input with Pull-down
 * PD12 and PD13 as Output Push-pull
 */
void GPIO_Config(void);

//configure EXTI0 line to trigger an interrupt by Rising Edge on pin PA0
void Interrupt_Config(void);

//configure SysTick timer to overflows each 1 ms without ISR
void SysTick_Initialize(void);

//function to make some delays through SysTick timer
void Delay_ms(uint16_t time);

//EXTI0 ISR
void EXTI0_IRQHandler(void);




int main(void)
{
	RCC_ClockConfig();
	GPIO_Config();
	Interrupt_Config();
	SysTick_Initialize();



	while(1)
	{
		//typically blinky
		GPIOD -> ODR ^= (uint32_t)(1 << 13);
		Delay_ms(100);


	}

	return 0;
}


void RCC_ClockConfig(void)
{
	/*reset CFGR register and switch to HSI
	 * reset to zero PLLCFGR register and disable PLL and HSE
	 */
	RCC -> CFGR = (uint32_t)(0x0);
	RCC -> CR &= ~(uint32_t)(1 << 24);
	RCC -> CR &= ~(uint32_t)(1 << 16);
	RCC -> PLLCFGR = (uint32_t)(0x0);

	//enable HSE clock
	RCC -> CR |= (uint32_t)(1 << 16);
	while(!(RCC -> CR & (uint32_t)(1 << 17)));

	//switch to HSE clock
	RCC -> CFGR |= (uint32_t)(0x1);
	while(!(RCC -> CR & (uint32_t)(0x5))); //wait until HSE is main AHB bus clock

}


void GPIO_Config(void)
{
	//enable PORTA and PORTD clock
	RCC -> AHB1ENR |= (uint32_t)(0x9);

	//configure Pin PA0 as digital Input with Pull-Down
	GPIOA -> MODER &= ~(uint32_t)(0x3);
	GPIOA -> OSPEEDR &= ~(uint32_t)(0x3);
	GPIOA -> PUPDR |= (uint32_t)(0x2);

	//configure Pin PD12 as Output Push-pull (interrupt use it)
	GPIOD -> MODER |= (uint32_t)(1 << 24);
	GPIOD -> OTYPER &= ~(uint32_t)(1 << 12);
	GPIOD -> OSPEEDR &= ~(uint32_t)(0x3 << 24);

	//configure pin PD13 as Output Push-pull (general IO)
	GPIOD -> MODER |= (uint32_t)(1 << 26);
	GPIOD -> OTYPER &= ~(uint32_t)(1 << 13);
	GPIOD -> OSPEEDR &= ~(uint32_t)(0x3 << 26);

}


void Interrupt_Config(void)
{
	//enable SYSCFG clock
	RCC -> APB2ENR |= (uint32_t)(1 << 14);

	//configure EXTI0 for pin PA0
	EXTI -> IMR |= (uint32_t)(0x1); //enable Interrupt on PA0
	EXTI -> RTSR |= (uint32_t)(0x1); //trigger with a Rising Edge
	SYSCFG -> EXTICR[0] |= (uint32_t)(0x0); //addressing pin PA0 to EXTI0 line

	//configure and enable NVIC interrupt by EXTI0
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	NVIC_PriorityGroupConfig(0x0);
	NVIC_SetPriority(EXTI0_IRQn, 0x0);
	NVIC_EnableIRQ(EXTI0_IRQn);

}


void SysTick_Initialize(void)
{
	//configure SysTick to work with AHB bus clock
	SysTick -> CTRL = (uint32_t)(0x4);
	SysTick -> VAL = (uint32_t)(0x0);
	SysTick -> LOAD = AHB_FREQ/1000;

	SysTick -> CTRL |= (uint32_t)(0x1);

}


void Delay_ms(uint16_t time)
{
	//reset SysTick and clear flag
	SysTick -> VAL = (uint32_t)(0x0);
	SysTick -> CTRL &= ~(uint32_t)(1 << 16);

	for(uint16_t i = 0; i < time; i++)
	{
		while(!(SysTick -> CTRL & (uint32_t)(1 << 16)));

		SysTick -> CTRL &= ~(uint32_t)(1 << 16);

	}

}


void EXTI0_IRQHandler(void)
{
	if(EXTI -> PR & (uint32_t)(0x1))
	{
		EXTI -> PR |= (uint32_t)(0x1); //clear EXTI0 flag setting correspond bit

		for(uint32_t i = 0; i < 6; i++) //make a blink 3 times
		{
			GPIOD -> ODR ^= (uint32_t)(1 << 12);
			Delay_ms(500);
		}

	}

}
