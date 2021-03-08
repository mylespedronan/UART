#ifndef _BSP_H_
#define _BSP_H_

// Board Support initialization
void BSP_init(void);
void GPIO_init(void);
void UART_init(void);

// Delay loop
void Delay(unsigned long counter);

// GPIOF
void ledOn(void);
void ledOff(void);

// UART0
char UART0_Rx(void);
void UART0_printChar(unsigned char data);
void UART0_printString(char * string);

char readChar(void);
void printChar(char c);
void printString(char * string);

// UART1
char UART1_Rx(void);
void UART1_printChar(unsigned char data);
void UART1_printString(char * string);

char readChar1(void);
void printChar1(char c);
void printString1(char * string);

#endif // _BSP_H_
