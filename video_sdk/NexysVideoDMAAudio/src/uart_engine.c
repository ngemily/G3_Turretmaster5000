/*
 * uart_engine.c
 *
 *  Created on: Feb 7, 2016
 *      Author: Roberto Bortolussi
 */

#include <mb_interface.h>
#include <xparameters.h>
#include <xintc.h>
#include <xuartlite.h>
#include <xuartlite_l.h>

#include "uart_engine.h"

static UartResponseFunction uart_responses[256];

//The Receive Interrupt Handler
void _uart_recv_handler(void* CallBackRef, unsigned int EventData) {
    XUartLite *uart_ptr = (XUartLite*) CallBackRef;
    u8 recv = (u8) XUartLite_RecvByte(uart_ptr->RegBaseAddress);
    UartResponseFunction response = uart_responses[recv];

    xil_printf("in recv handler.\r\n");

    if (NULL != response) {
        response();
    } else {
        xil_printf("No handler registered for '%c' (%d)\r\n", recv, recv);
    }
}

//The Send Interrupt Handler
void _uart_send_handler(void* CallBackRef, unsigned int EventData) {
    print("Inside the send handler\r\n");
}

XStatus register_uart_response(u8 id, UartResponseFunction response) {
    if (NULL == uart_responses[id]) {
        uart_responses[id] = response;
        return XST_SUCCESS;
    } else {
        xil_printf("Handler already registered for '%c' (%d) %x\r\n", id, id, uart_responses[id]);
    }
    return XST_FAILURE;
}

XStatus initialize_debug(XUartLite *uart_ptr) {
    int status;

    print("Initializing Debug...\n\r");

    //Setup UARTLite
    if (XST_SUCCESS != (status = XUartLite_Initialize(uart_ptr, XPAR_UARTLITE_0_DEVICE_ID))) {
        xil_printf("UARTLite init failed (%d)\n\r", status);
        return XST_FAILURE;
    }
    while(XUartLite_IsSending(uart_ptr));
    XUartLite_ResetFifos(uart_ptr);
    if (XST_SUCCESS != (status = XUartLite_SelfTest(uart_ptr))) {
        xil_printf("UARTLite SelfTest failed (%d)\n\r", status);
        return XST_FAILURE;
    }

    XUartLite_SetRecvHandler(uart_ptr, _uart_recv_handler, uart_ptr);
    XUartLite_SetSendHandler(uart_ptr, _uart_send_handler, uart_ptr);
    XUartLite_EnableInterrupt(uart_ptr);

    return XST_SUCCESS;
}

