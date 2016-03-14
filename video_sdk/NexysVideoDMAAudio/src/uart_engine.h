/*
 * uart_engine.h
 *
 *  Created on: Feb 7, 2016
 *      Author: Roberto Bortolussi
 */

#ifndef UART_ENGINE_H_
#define UART_ENGINE_H_

#include <xstatus.h>
#include <xuartlite.h>

#define MAX_COMMAND_LENGTH (15)
#define MAX_NUM_COMMANDS (50)

#define PROMPT_STRING (">>>")

typedef void (*UartResponseFunction)(void);

XStatus initialize_uart(XUartLite *uart_ptr);
XStatus register_uart_response(char *cmdString, UartResponseFunction response);

#endif /* UART_ENGINE_H_ */
