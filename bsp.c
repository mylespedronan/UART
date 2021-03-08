#include "TM4C123GH6PM.h"
#include "bsp.h"

#define PIN0	(1U << 0)
#define PIN1	(1U << 1)
#define PIN2	(1U << 2)
#define PIN3	(1U << 3)
#define PIN4	(1U << 4)

void UART1_Handler (void){
	unsigned char rx_data = 0;
	
	// Clear UART Interrupt Clear (UARTICR) register Receieve Interrupt Clear
	UART1->ICR &= ~(1U << 5);
	
	rx_data = UART1->DR;
	
	switch(rx_data){
		case 'r':
			GPIOF->DATA = (1U << 1);
			break;
		case 'g':
			GPIOF->DATA = (1U << 3);
			break;			
		case 'b':
			GPIOF->DATA = (1U << 2);
			break;
		default:
			GPIOF->DATA &= ~((1U << 1) | (1U << 2) | (1U << 3));
			break;		
	}
	
	UART0_printChar(rx_data);
	UART0_printString("\n\r");	
}

void BSP_init(void){
	GPIO_init();
	UART_init();
}

void GPIO_init(void){
	/** Enable GPIO Port F to initialize LEDs for debugging **/
	// Enable clock to GPIO Port F
	SYSCTL->RCGCGPIO |= (1U << 5);
	
	// Set DIR of Pins 1, 2, and 3 as outputs
	GPIOF->DIR |= (PIN1 | PIN2 | PIN3);
	
	// Set GPIO Pins 1, 2, and 3 as digital pins
	GPIOF->DEN |= (PIN1 | PIN2 | PIN3);	
}

void UART_init(void){
	/** Initialize and Configure UART Module 1 **/
	// Enable and provide a clock to UART module 1
	SYSCTL->RCGCUART |= (1U << 1);
	SYSCTL->RCGCUART |= (1U << 0);
	
	/* Configure UART GPIO Pins PB0 (RX) and PB1 (TX) */
	// Enable clock to GPIO Port B
	SYSCTL->RCGCGPIO |= (1U << 1);
	SYSCTL->RCGCGPIO |= (1U << 0);
	
	// Delay to make sure clocks are enabled
	Delay(1);

	/* Configure UART */
	// Disable UART by clearing UARTEN bit
	UART1->CTL = 0;
	UART0->CTL = 0;
	
	/** Calculate UART Baud Rate **/
	/* UART Baud Rate = (f / 16 * baud divisor) 						
	** f = clock frequency of the UART module = system frequency (xtal) 
	** Example: Baud rate of 115200 (115200 bits per second) 			
	** 			115200 = (16MHz / 16 * baud divisor) 					
	**			Baud divisor = (1000000/115200) = 8.680					
	**			UARTIBRD = 8											
	** Value for fractional baud rate register can be calculated by 	
	** multiplying fraction value with 64 and  adding 0.5				
	**			0.680 * 64 + 0.5 = 44									
	**			UARTFBRD = 44												*/
	// Write integer portion of BRD
	UART1->IBRD = 8;	// 16MHz/16 = 1MHz, 1MHz/104 = 9600 baud rate
	//UART1->IBRD = 104;	
	UART0->IBRD = 104;	
	
	// Write fraction portion of BRD
	UART1->FBRD = 44;	// Fraction part of baud generator register	
	//UART1->FBRD = 11;	
	UART0->FBRD = 11;	
	
	// Write the desired serial parameters to the UARTLCRH (UART Line Control)
	// Word Length 8-bit, no parity bit, no FIFO
	UART1->LCRH = 0x60;
	//UART1->LCRH = (0x3 << 5);
	UART0->LCRH = (0x3 << 5);
	
	// Configure UART clock source (set for Precision Internal Oscillator Operation (PIOSC))
	UART1->CC = 0x5;
	UART0->CC = 0x5;
	
	// Enable UART1 module, Bits - RXE (9), TXE (8), UARTEN(0)
	UART1->CTL = 0x301;
	//UART1->CTL = (1U << 0) | (1U << 8) | (1U << 9);
	UART0->CTL = (1U << 0) | (1U << 8) | (1U << 9);
	
	// Set GPIO Pin 0 and Pin 1 as digital pins
	GPIOB->DEN |= (PIN0 | PIN1);
	GPIOA->DEN |= (PIN0 | PIN1);
	
	// Set GPIO AFSEL bit for Pin 0 and Pin 1
	GPIOB->AFSEL |= (PIN0 | PIN1);
	GPIOA->AFSEL |= (PIN0 | PIN1);
	
	// Turn off analog function
	GPIOB->AMSEL = 0;
	
	// Set port control register (PCTL) for PB0 and PB1
	GPIOB->PCTL |= 0x00000011;
	//GPIOB->PCTL |= (1U << 0) | (1U << 4);
	GPIOA->PCTL |= (1U << 0) | (1U << 4);
	
	// Enable interrupt
	UART1->ICR &= ~(1U << 5);
	
	// Enable UART Interrupt Mask (UARTIM) and set UART Receive Interrupt Mask
	UART1->IM |= (1U << 4);
	
	/* Write to Interrupt Set Enable register (EN0) for interrupts 0-31 
	** UART1_Handler = Interrupt # 6										*/
	NVIC->ISER[0] = (1U << 6);
	
	Delay(1);
}

void Delay(unsigned long counter){	
	unsigned long i;
	
	for(i = 0; i < counter; i++){
		// do nothing
	}
}

void ledOn(void){
	GPIOF->DATA_Bits[PIN1] |= PIN1;
}

void ledOff(void){
	GPIOF->DATA_Bits[PIN1] = 0;
}

char UART0_Rx(void){
	char data;
	
	while((UART0-> FR & (1U << 4)) != 0);
	data = UART0->DR;
	
	return (unsigned char) data;
}

void UART0_printChar(unsigned char data){
	// Wait until TxFF (UART Transmit FIFO Full) buffer not full
	// 0 - transmitter not full
	// 1 - transmit register is full
	while((UART0->FR & (1U << 5)) != 0);
	UART0->DR = data;
}

void UART0_printString(char * string){
	while(*string){
		UART0_printChar(*(string++));
	}
}

char UART1_Rx(void){
	char data;
	
	while((UART1-> FR & (1U << 4)) != 0);
	data = UART1->DR;
	
	return (unsigned char) data;
}

void UART1_printChar(unsigned char data){
	// Wait until TxFF (UART Transmit FIFO Full) buffer not full
	// 0 - transmitter not full
	// 1 - transmit register is full
	while((UART1->FR & (1U << 5)) != 0);
	UART1->DR = data;
}

void UART1_printString(char * string){
	while(*string){
		UART1_printChar(*(string++));
	}
}

char readChar(void){
	char c;
	while((UART0->FR & (1U << 4)) != 0);
	c = UART0->DR;
	
	return c;
}

void printChar(char c){
	while((UART0->FR & (1U << 5)) != 0);
	UART0->DR = c;
}

void printString(char * string){
	while(*string){
		printChar(*(string++));
	}
}

char readChar1(void){
	char c;
	while((UART1->FR & (1U << 4)) != 0);
	c = UART1->DR;
	
	return c;
}

void printChar1(char c){
	while((UART1->FR & (1U << 5)) != 0);
	UART1->DR = c;
}

void printString1(char * string){
	while(*string){
		printChar1(*(string++));
	}
}
