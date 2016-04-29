#ifndef UART_H
#define UART_H

#pragma once

#include "stm32f4xx.h"
#include "ringbuf.h"
#include <stdlib.h>
#include <stdio.h>
#include "stdbool.h"
#include "Shortcuts.h"

#include <sys/types.h>
#include <reent.h>

//TODO: Can we make the buffer larger?
#define RX_SIZE  128
#define TX_SIZE  128

extern void  UARTInit(int baudrate);
extern void  uart_poll_send(const char *s);
extern int   uart_chars_avail(void);

extern ssize_t uart_write_r(struct _reent *r, int fd, const void *ptr, size_t len);
extern ssize_t uart_read_r(struct _reent *r, int fd, void *ptr, size_t len);

extern ssize_t uart_dirtyread(void *ptr, size_t len);

extern ssize_t uart_buffersize();
extern ssize_t uart_buffervalue(void *ptr, size_t len);

void UartTestTask(void *pvParameters);


#endif
