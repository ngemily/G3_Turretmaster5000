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
#include <string.h>
#include <ctype.h>

#include "uart_engine.h"

static char commandBuffer[MAX_COMMAND_LENGTH + 1];
static char argBuffer[MAX_ARG_LENGTH + 1];
static u8 commandIndex;
static u8 argIndex;
static XUartLite *sUartLite;

static struct {
    char *cmdString;
    UartResponseFunction response;
} commandTable[MAX_NUM_COMMANDS];

static int numCommands;


static enum {
    PROC_COMMAND = 0,
    PROC_ARG,
} uartState;


int getCommandIndex(char *cmdString) {
    int i = 0;
    while (i < numCommands) {
        if (strcmp(commandTable[i].cmdString, cmdString) == 0) {
            break;
        }
        i++;
    }

    if (i == numCommands) {
        i = -1;
    }

    return i;
}

void uartCommandRecvHandler(u8 recv) {
    if (isprint(recv) || isspace(recv)) {
        xil_printf("%c", recv);
    }

    if (recv == '\r') {
        int handlerIndex;
        xil_printf("\r\n");

        // Null terminate the command string.
        commandBuffer[commandIndex] = '\0';

        // Find the command in the command table and call it.
        handlerIndex = getCommandIndex(commandBuffer);
        if (handlerIndex == -1) {
            xil_printf("No handler was registered for string '%s'\r\n", commandBuffer);
        } else {
            commandTable[handlerIndex].response();
        }

        xil_printf(PROMPT_STRING);

        // Reset the UART state machine.
        commandIndex = 0;
    } else if (recv == '\b') {
        if (commandIndex > 0) {
            commandIndex--;
            xil_printf("\b \b");
        }
    } else {
        if (commandIndex >= MAX_COMMAND_LENGTH) {
            xil_printf("\r\nCommand too long!\r\n");
            xil_printf(PROMPT_STRING);
            commandIndex = 0;
        }

        commandBuffer[commandIndex++] = recv;
    }
}


void uartRecvHandler(void* CallBackRef, unsigned int EventData) {
    XUartLite *uart_ptr = (XUartLite*) CallBackRef;
    u8 recv = (u8) XUartLite_RecvByte(uart_ptr->RegBaseAddress);

    switch (uartState) {
        case PROC_COMMAND:
            uartCommandRecvHandler(recv);
            break;

        case PROC_ARG:
            break;

        default:
            xil_printf("Invalid UART state!\r\n");
            uartState = PROC_COMMAND;
            break;
    }

}


//The Send Interrupt Handler
void _uart_send_handler(void* CallBackRef, unsigned int EventData) {
    print("Inside the send handler\r\n");
}


XStatus register_uart_response(char *cmdString, UartResponseFunction response) {
    int foundIndex = getCommandIndex(cmdString);
    XStatus result = XST_SUCCESS;

    if (foundIndex != -1) {
        xil_printf("Command already has registered handler: %s\r\n", cmdString);
        result = XST_FAILURE;
    } else if (numCommands >= MAX_NUM_COMMANDS) {
        xil_printf("Reached maximum number of commands. \r\n");
        result = XST_FAILURE;
    } else {
        commandTable[numCommands].cmdString = cmdString;
        commandTable[numCommands].response = response;
        numCommands++;
    }

    return result;
}


XStatus initialize_uart(XUartLite *uart_ptr) {
    int status;

    print("Initializing Debug...\n\r");
    sUartLite = uart_ptr;

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

    // Set up the UART interrupt handlers.
    XUartLite_SetRecvHandler(uart_ptr, uartRecvHandler, uart_ptr);
    XUartLite_SetSendHandler(uart_ptr, _uart_send_handler, uart_ptr);
    XUartLite_EnableInterrupt(uart_ptr);

    // Initialize the state machine and the buffers.
    commandIndex = 0;
    argIndex = 0;
    uartState = PROC_COMMAND;
    numCommands = 0;
    return XST_SUCCESS;
}

bool consume_uart_arg(char *prompt, char *buf, int size) {
    u8 recv;
    bool keepGoing = true;
    int i;

    xil_printf(prompt);
    xil_printf(ARG_PROMPT);
    while (keepGoing) {
        recv = (u8) XUartLite_RecvByte(sUartLite->RegBaseAddress);

        if (isprint(recv) || isspace(recv)) {
            xil_printf("%c", recv);
        }

        if (recv == '\r') {
            // Let's move the argument to the user-supplied buffer.

            for (i = 0; i < argIndex; i++) {
                buf[i] = argBuffer[i];
            }
            buf[argIndex] = '\0';
            argIndex = 0;
            keepGoing = false;
            xil_printf("\r\n");
        } else if (recv == '\b') {
            if (argIndex > 0) {
                argIndex--;
                xil_printf("\b \b");
            }
        } else {
            if (argIndex >= MAX_COMMAND_LENGTH) {
                xil_printf("\r\nArg too long!\r\n");
                xil_printf(prompt);
                xil_printf(ARG_PROMPT);
                argIndex = 0;
            }

            argBuffer[argIndex++] = recv;
        }
    }

    return true;
}


bool get_int_from_string(char *str, int *result) {
    int temp = 0;
    int i;
    int length = strlen(str);

    for (i = 0; i < length; i++) {
        if (str[i] < '0' || '9' < str[i]) {
            return false;
        }
        temp *= 10;
        temp += str[i] - '0';
    }

    *result = temp;
    return true;
}
