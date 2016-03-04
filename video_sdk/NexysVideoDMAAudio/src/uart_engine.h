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

typedef void (*UartResponseFunction)(void);

XStatus initialize_debug(XUartLite *uart_ptr);
XStatus register_uart_response(u8 id, UartResponseFunction response);

#endif /* UART_ENGINE_H_ */
