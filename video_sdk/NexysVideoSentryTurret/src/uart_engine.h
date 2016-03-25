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
#include <stdbool.h>

#define MAX_COMMAND_LENGTH (15)
#define MAX_ARG_LENGTH (15)
#define MAX_NUM_COMMANDS (50)

#define PROMPT_STRING (">>>")
#define ARG_PROMPT ("$")

typedef void (*UartResponseFunction)(void);

XStatus initialize_uart(XUartLite *uart_ptr);
XStatus register_uart_response(char *cmdString, UartResponseFunction response);
bool consume_uart_arg(char *prompt, char *buf, int size);

bool get_int_from_string(char *str, int *result);


#endif /* UART_ENGINE_H_ */
