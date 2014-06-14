
/**
 *	USART Library for STM32F4 with receive interrupt
 *	Libraries works for all 8 U(S)ARTs which are supported in STM32F429.
 *
 *	Every USART channel has it's own receive interrupt which stores incoming data into cyclic buffer.
 *	In your project you can set buffer length, default is 32Bytes, with:
 *
 *	#define TM_USART_BUFFER_SIZE number_of_bytes
 *
 *	in your project's defines.h file.
 *
 *	@author 	Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@link		http://bit.ly/1npVXHQ
 *	@version 	v1.1
 *	@gcc		v4.7 20013qr3
 *	@ide		CooCox CoIDE v1.7.6
 */
#ifndef TM_USART_
#define TM_USART_ 110
/**
 * Library dependencies
 * - STM32F4xx
 * - STM32F4xx RCC
 * - STM32F4xx GPIO
 * - STM32F4xx USART
 * - defines.h
 */
/**
 * Includes
 */
#include <misc.h>
#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_usart.h>

//Buffer size
#ifndef TM_USART_BUFFER_SIZE
#define TM_USART_BUFFER_SIZE 		32
#endif

//NVIC Priority
#ifndef TM_USART_NVIC_PRIORITY
#define TM_USART_NVIC_PRIORITY		0x00
#endif

//U(S)ART settings, can be changed in your defines.h project file
//USART1 default settings
#ifndef TM_USART1_HARDWARE_FLOW_CONTROL
#define TM_USART1_HARDWARE_FLOW_CONTROL		USART_HardwareFlowControl_None
#endif
#ifndef TM_USART1_MODE
#define TM_USART1_MODE						USART_Mode_Tx | USART_Mode_Rx
#endif
#ifndef TM_USART1_PARITY
#define TM_USART1_PARITY					USART_Parity_No
#endif
#ifndef TM_USART1_STOP_BITS
#define TM_USART1_STOP_BITS					USART_StopBits_1
#endif
#ifndef TM_USART1_WORD_LENGTH
#define TM_USART1_WORD_LENGTH				USART_WordLength_8b
#endif

//USART2 default settings
#ifndef TM_USART2_HARDWARE_FLOW_CONTROL
#define TM_USART2_HARDWARE_FLOW_CONTROL		USART_HardwareFlowControl_None
#endif
#ifndef TM_USART2_MODE
#define TM_USART2_MODE						USART_Mode_Tx | USART_Mode_Rx
#endif
#ifndef TM_USART2_PARITY
#define TM_USART2_PARITY					USART_Parity_No
#endif
#ifndef TM_USART2_STOP_BITS
#define TM_USART2_STOP_BITS					USART_StopBits_1
#endif
#ifndef TM_USART2_WORD_LENGTH
#define TM_USART2_WORD_LENGTH				USART_WordLength_8b
#endif

//USART3 default settings
#ifndef TM_USART3_HARDWARE_FLOW_CONTROL
#define TM_USART3_HARDWARE_FLOW_CONTROL		USART_HardwareFlowControl_None
#endif
#ifndef TM_USART3_MODE
#define TM_USART3_MODE						USART_Mode_Tx | USART_Mode_Rx
#endif
#ifndef TM_USART3_PARITY
#define TM_USART3_PARITY					USART_Parity_No
#endif
#ifndef TM_USART3_STOP_BITS
#define TM_USART3_STOP_BITS					USART_StopBits_1
#endif
#ifndef TM_USART3_WORD_LENGTH
#define TM_USART3_WORD_LENGTH				USART_WordLength_8b
#endif

//UART4 default settings
#ifndef TM_UART4_HARDWARE_FLOW_CONTROL
#define TM_UART4_HARDWARE_FLOW_CONTROL		USART_HardwareFlowControl_None
#endif
#ifndef TM_UART4_MODE
#define TM_UART4_MODE						USART_Mode_Tx | USART_Mode_Rx
#endif
#ifndef TM_UART4_PARITY
#define TM_UART4_PARITY						USART_Parity_No
#endif
#ifndef TM_UART4_STOP_BITS
#define TM_UART4_STOP_BITS					USART_StopBits_1
#endif
#ifndef TM_UART4_WORD_LENGTH
#define TM_UART4_WORD_LENGTH				USART_WordLength_8b
#endif

//UART5 default settings
#ifndef TM_UART5_HARDWARE_FLOW_CONTROL
#define TM_UART5_HARDWARE_FLOW_CONTROL		USART_HardwareFlowControl_None
#endif
#ifndef TM_UART5_MODE
#define TM_UART5_MODE						USART_Mode_Tx | USART_Mode_Rx
#endif
#ifndef TM_UART5_PARITY
#define TM_UART5_PARITY						USART_Parity_No
#endif
#ifndef TM_UART5_STOP_BITS
#define TM_UART5_STOP_BITS					USART_StopBits_1
#endif
#ifndef TM_UART5_WORD_LENGTH
#define TM_UART5_WORD_LENGTH				USART_WordLength_8b
#endif

//USART6 default settings
#ifndef TM_USART6_HARDWARE_FLOW_CONTROL
#define TM_USART6_HARDWARE_FLOW_CONTROL		USART_HardwareFlowControl_None
#endif
#ifndef TM_USART6_MODE
#define TM_USART6_MODE						USART_Mode_Tx | USART_Mode_Rx
#endif
#ifndef TM_USART6_PARITY
#define TM_USART6_PARITY					USART_Parity_No
#endif
#ifndef TM_USART6_STOP_BITS
#define TM_USART6_STOP_BITS					USART_StopBits_1
#endif
#ifndef TM_USART6_WORD_LENGTH
#define TM_USART6_WORD_LENGTH				USART_WordLength_8b
#endif

//UART7 default settings
#ifndef TM_UART7_HARDWARE_FLOW_CONTROL
#define TM_UART7_HARDWARE_FLOW_CONTROL		USART_HardwareFlowControl_None
#endif
#ifndef TM_UART7_MODE
#define TM_UART7_MODE						USART_Mode_Tx | USART_Mode_Rx
#endif
#ifndef TM_UART7_PARITY
#define TM_UART7_PARITY						USART_Parity_No
#endif
#ifndef TM_UART7_STOP_BITS
#define TM_UART7_STOP_BITS					USART_StopBits_1
#endif
#ifndef TM_UART7_WORD_LENGTH
#define TM_UART7_WORD_LENGTH				USART_WordLength_8b
#endif

//UART8 default settings
#ifndef TM_UART8_HARDWARE_FLOW_CONTROL
#define TM_UART8_HARDWARE_FLOW_CONTROL		USART_HardwareFlowControl_None
#endif
#ifndef TM_UART8_MODE
#define TM_UART8_MODE						USART_Mode_Tx | USART_Mode_Rx
#endif
#ifndef TM_UART8_PARITY
#define TM_UART8_PARITY						USART_Parity_No
#endif
#ifndef TM_UART8_STOP_BITS
#define TM_UART8_STOP_BITS					USART_StopBits_1
#endif
#ifndef TM_UART8_WORD_LENGTH
#define TM_UART8_WORD_LENGTH				USART_WordLength_8b
#endif

/**
 * When you initialize USARTx, you have to select which pins pack you will use
 *
 * Pinspack 3 can be used only for USART3
 */
typedef enum {
	TM_USART_PinsPack_1,
	TM_USART_PinsPack_2,
	TM_USART_PinsPack_3
} TM_USART_PinsPack_t;

/**
 * Initialize USARTx
 *
 * Parameters:
 * 	- USART_TypeDef* USARTx: which USART channel
 * 		USART1, USART2, USART3, UART4, UART5, USART6, UART7. UART8
 * 	- TM_USART_PinsPack_t pinspack: Pinspack used
 * 		- TM_USART_PinsPack_1
 * 		- TM_USART_PinsPack_2
 * 		- TM_USART_PinsPack_3 (only for USART3)
 *	- uint32_t baudrate: baudrate for USARTx
 */
extern void TM_USART_Init(USART_TypeDef* USARTx, TM_USART_PinsPack_t pinspack, uint32_t baudrate);

#ifdef USART1
/**
 * Initialize USART1 pins
 *
 * Parameters:
 * 	- TM_USART_PinsPack_t pinspack: Pinspack used
 * 		- TM_USART_PinsPack_1
 * 			- TX: PA9
 * 			- RX: PA10
 * 		- TM_USART_PinsPack_2
 * 			- TX: PB6
 * 			- RX: PB7
 *
 * Called from TM_USART_Init()
 */
extern void TM_USART1_InitPins(TM_USART_PinsPack_t pinspack);
#endif

#ifdef USART2
/**
 * Initialize USART2 pins
 *
 * Parameters:
 * 	- TM_USART_PinsPack_t pinspack: Pinspack used
 * 		- TM_USART_PinsPack_1
 * 			- TX: PA2
 * 			- RX: PA3
 * 		- TM_USART_PinsPack_2
 * 			- TX: PD5
 * 			- RX: PD6
 *
 * Called from TM_USART_Init()
 */
extern void TM_USART2_InitPins(TM_USART_PinsPack_t pinspack);
#endif

#ifdef USART3
/**
 * Initialize USART3 pins
 *
 * Parameters:
 * 	- TM_USART_PinsPack_t pinspack: Pinspack used
 * 		- TM_USART_PinsPack_1
 * 			- TX: PB10
 * 			- RX: PB11
 * 		- TM_USART_PinsPack_2
 * 			- TX: PC10
 * 			- RX: PC11
 * 		- TM_USART_PinsPack_3
 * 			- TX: PD8
 * 			- RX: PD9
 *
 * Called from TM_USART_Init()
 */
extern void TM_USART3_InitPins(TM_USART_PinsPack_t pinspack);
#endif

#ifdef UART4
/**
 * Initialize UART4 pins
 *
 * Parameters:
 * 	- TM_USART_PinsPack_t pinspack: Pinspack used
 * 		- TM_USART_PinsPack_1
 * 			- TX: PA0
 * 			- RX: PA1
 * 		- TM_USART_PinsPack_2
 * 			- TX: PC10
 * 			- RX: PC11
 *
 * Called from TM_USART_Init()
 */
extern void TM_UART4_InitPins(TM_USART_PinsPack_t pinspack);
#endif

#ifdef UART5
/**
 * Initialize UART5 pins
 *
 * Parameters:
 * 	- TM_USART_PinsPack_t pinspack: Pinspack used
 * 		- TM_USART_PinsPack_1
 * 			- TX: PC12
 * 			- RX: PD2
 *
 * Called from TM_USART_Init()
 */
extern void TM_UART5_InitPins(TM_USART_PinsPack_t pinspack);
#endif

#ifdef USART6
/**
 * Initialize USART6 pins
 *
 * Parameters:
 * 	- TM_USART_PinsPack_t pinspack: Pinspack used
 * 		- TM_USART_PinsPack_1
 * 			- TX: PC6
 * 			- RX: PC7
 * 		- TM_USART_PinsPack_2
 * 			- TX: PG14
 * 			- RX: PG9
 *
 * Called from TM_USART_Init()
 */
extern void TM_USART6_InitPins(TM_USART_PinsPack_t pinspack);
#endif

#ifdef UART7
/**
 * Initialize UART7 pins
 *
 * Parameters:
 * 	- TM_USART_PinsPack_t pinspack: Pinspack used
 * 		- TM_USART_PinsPack_1
 * 			- TX: PE8
 * 			- RX: PE7
 * 		- TM_USART_PinsPack_2
 * 			- TX: PF7
 * 			- RX: PF6
 *
 * Called from TM_USART_Init()
 */
extern void TM_UART7_InitPins(TM_USART_PinsPack_t pinspack);
#endif

#ifdef UART8
/**
 * Initialize UART8 pins
 *
 * Parameters:
 * 	- TM_USART_PinsPack_t pinspack: Pinspack used
 * 		- TM_USART_PinsPack_1
 * 			- TX: PE1
 * 			- RX: PE0
 *
 * Called from TM_USART_Init()
 */
extern void TM_UART8_InitPins(TM_USART_PinsPack_t pinspack);
#endif

/**
 * Put character to USARTx
 *
 * Parameters:
 * 	- USART_TypeDef* USARTx: which USART channel
 * 		USART1, USART2, USART3, UART4, UART5, USART6, UART7. UART8
 *	- volatile char c: char to be sent to USART
 */
extern void TM_USART_Putc(USART_TypeDef* USARTx, volatile char c);

/**
 * Put string to USARTx
 *
 * Parameters:
 * 	- USART_TypeDef* USARTx: which USART channel
 * 		USART1, USART2, USART3, UART4, UART5, USART6, UART7. UART8
 *	- char* str: pointer to first character in string
 */
extern void TM_USART_Puts(USART_TypeDef* USARTx, char* str);

/**
 * Get character from internal buffer
 *
 * Parameters:
 * 	- USART_TypeDef* USARTx: which USART channel
 * 		USART1, USART2, USART3, UART4, UART5, USART6, UART7. UART8
 *
 * Returned character if exists in buffer, otherwise zero
 */
extern uint8_t TM_USART_Getc(USART_TypeDef* USARTx);

#ifdef USART1
/**
 * USART1 Interrupt handler
 *
 * Called by systen when data is received on USART1
 */
extern void USART1_IRQHandler(void);
#endif

#ifdef USART2
/**
 * USART2 Interrupt handler
 *
 * Called by systen when data is received on USART2
 */
extern void USART2_IRQHandler(void);
#endif

#ifdef USART3
/**
 * USART3 Interrupt handler
 *
 * Called by systen when data is received on USART3
 */
extern void USART3_IRQHandler(void);
#endif

#ifdef UART4
/**
 * UART4 Interrupt handler
 *
 * Called by systen when data is received on UART4
 */
extern void UART4_IRQHandler(void);
#endif

#ifdef UART5
/**
 * UART5 Interrupt handler
 *
 * Called by systen when data is received on UART5
 */
extern void UART5_IRQHandler(void);
#endif

#ifdef USART6
/**
 * USART6 Interrupt handler
 *
 * Called by systen when data is received on USART6
 */
extern void USART6_IRQHandler(void);
#endif

#ifdef UART7
/**
 * UART7 Interrupt handler
 *
 * Called by systen when data is received on UART7
 */
extern void UART7_IRQHandler(void);
#endif

#ifdef UART8
/**
 * UART8 Interrupt handler
 *
 * Called by systen when data is received on UART8
 */
extern void UART8_IRQHandler(void);
#endif

/**
 * Insert received data into internal buffer
 * Called in interrupt
 *
 * Parameters:
 * 	- uint8_t USARTnum
 * 		- USART number: starting from 0
 * 	- char c: character to be inserted in buffer
 */
extern void TM_USART_InsertToBuffer(uint8_t USARTnum, char c);

/**
 * Check's if internal buffer is empty
 *
 * Parameters:
 * 	- USART_TypeDef* USARTx: which USART channel
 * 		USART1, USART2, USART3, UART4, UART5, USART6, UART7. UART8
 *
 * Returns 1 if buffer is empty, otherwise 0
 */
extern uint8_t TM_USART_BufferEmpty(USART_TypeDef* USARTx);

/**
 * Get USART number
 *
 * Called internally
 *
 * Parameters:
 * 	- USART_TypeDef* USARTx: which USART channel
 * 		USART1, USART2, USART3, UART4, UART5, USART6, UART7. UART8
 *
 * Returns x - 1 number.
 */
uint8_t TM_USART_GetUsartNumber(USART_TypeDef* USARTx);

#endif
