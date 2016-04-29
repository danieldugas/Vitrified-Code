#include "uart.h"
//#include <errno.h>

static struct ringbuf rx_buf = { .buf = (char[RX_SIZE]) {}, .bufsize = RX_SIZE };
static struct ringbuf tx_buf = { .buf = (char[TX_SIZE]) {}, .bufsize = TX_SIZE };

static volatile struct uart_stats {
    uint32_t    rx_overrun;
    uint32_t    rx_bytes;
    uint32_t    tx_bytes;
} uart_stats;

void ProceedData(char receivedChar);

char receivedChar;
void USART1_IRQHandler(void)
{


	// receiving
	if (USART_GetFlagStatus(USART1, (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)))
	     USART_ReceiveData(USART1); // Clear Error

	if( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) )
		    	rb_putc(&rx_buf, USART_ReceiveData(USART1)); // Collect Char


	// sending
    if (USART1->SR & USART_SR_TXE) {
    	char c;
        if (rb_getc(&tx_buf, &c)) {
            // send a queued byte
            USART1->DR = c;
        }
        else {
            // nothing to send, disable interrupt
            USART1->CR1 &= ~USART_CR1_TXEIE;
        }
        uart_stats.tx_bytes++;
    }



}


int uart_chars_avail(void)
{
    return rx_buf.len;
}


/**
 * Write *ptr contents into the ringbuffer for sending data tx_buf,
 *
 * the data in this buffer will at some point be sent by uart through
 * the interrupt handler
 *
 */
ssize_t uart_write_r(struct _reent *r, int fd, const void *ptr, size_t len)
{
    const char *c = (const char*) ptr;

    int i = 0;
    for (i = 0; i < len; i++) {
        while (!rb_putc(&tx_buf, *c));
        c++;

        // Enable TX empty interrupt
        USART1->CR1 |= USART_CR1_TXEIE;
    }


    return len;
}

/**
 * Read the ringbuffer for receiving data rx_buf into *ptr,
 *
 * received data is accumulated into this buffer through
 * the interrupt handler
 *
 */
ssize_t uart_read_r(struct _reent *r, int fd, void *ptr, size_t len)
{
    while (!rx_buf.len);

    if (len > rx_buf.len)
        len = rx_buf.len;

    char *c = (char*)ptr;
    int i = 0;
    for (i = 0; i < len; i++)
        rb_getc(&rx_buf, c++);

    return len;
}

//TODO: Find a way to not use this function
// quick and dirty way for the main to check what's in the receiving ringbuffer
ssize_t uart_dirtyread(void *ptr, size_t len)
{
	if (len > rx_buf.len)
		len = rx_buf.len;

	char *c = (char*)ptr;
	int i = 0;


	for (i = 0; i < len; i++)
		rb_getc(&rx_buf, c++);


	return len;
}

void uart_poll_send(const char *ch)
{
    while (*ch) {
        USART1->DR = *ch++ & 0xff;
        while (!(USART1->SR & USART_FLAG_TXE));
        uart_stats.tx_bytes++;
    }
}


#define UARTTestTaskT 1000
void UartTestTask(void* parameters)
{
        //uint32_t uartRxBytes = uart_stats.rx_bytes;
        portTickType lastExecutionTime = xTaskGetTickCount();
        LEDOn(LED2);
        int counter = 0;
        for (;;)
        {
                DelayUntilTime( &lastExecutionTime, UARTTestTaskT);
                //printf("\n\rcounter %i", counter);
                counter++;
        }
}

/**
 * Initialize UART.
 *
 * \param  baudrate  Baudrate
 *
 *  PB6   USART1_TXD
 *  PB7   USART1_RXD
 *
 */
void UARTInit(int baudrate)
{
    // Enable peripheral clocks
    //
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOB;
    RCC->APB2ENR |= RCC_APB2Periph_USART1;

    // Initialize Serial Port
    //
//    GPIO_Init(GPIOB, &(GPIO_InitTypeDef) {
//        .GPIO_Pin   = GPIO_Pin_6,
//        .GPIO_Speed = GPIO_Speed_50MHz,
//        .GPIO_Mode  = GPIO_Mode_AF,
//        .GPIO_OType = GPIO_OType_PP
//    });
//
//    GPIO_Init(GPIOB, &(GPIO_InitTypeDef) {
//        .GPIO_Pin = GPIO_Pin_7,
//        .GPIO_Mode = GPIO_Mode_IN,
//        .GPIO_PuPd = GPIO_PuPd_UP
//    });
//
//    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
//    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
    GPIO_Init(GPIOB, &(GPIO_InitTypeDef) {
		.GPIO_Pin   = GPIO_Pin_6,
		.GPIO_Speed = GPIO_Speed_50MHz,
		.GPIO_Mode  = GPIO_Mode_AF,
		.GPIO_PuPd = GPIO_PuPd_UP,
		.GPIO_OType = GPIO_OType_PP
	});

	GPIO_Init(GPIOB, &(GPIO_InitTypeDef) {
		.GPIO_Pin = GPIO_Pin_7,
		.GPIO_Mode = GPIO_Mode_AF,
		.GPIO_PuPd = GPIO_PuPd_UP
	});

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

    USART_Init(USART1, &(USART_InitTypeDef) {
        .USART_BaudRate = baudrate,
        .USART_WordLength = USART_WordLength_8b,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity_No ,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx
    });

    NVIC_Init(&(NVIC_InitTypeDef) {
        .NVIC_IRQChannel = USART1_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 0,//configLIBRARY_KERNEL_INTERRUPT_PRIORITY//0
        .NVIC_IRQChannelSubPriority = 0,
        .NVIC_IRQChannelCmd = ENABLE
    });

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

/*
// -------------------- Shell commands --------------------
//
static void cmd_baudrate(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: %s <baudrate>\n", argv[0]);
        return;
    }

    uart_init(atoi(argv[1]));
}

static void cmd_uart_stats(void)
{
    printf("rx_bytes:   %8lu\n", uart_stats.rx_bytes);
    printf("rx_overrun: %8lu\n", uart_stats.rx_overrun);
    printf("tx_bytes:   %8lu\n", uart_stats.tx_bytes);
}


SHELL_CMD(baudrate,   (cmdfunc_t)cmd_baudrate,   "set baudrate")
SHELL_CMD(uart_stats, (cmdfunc_t)cmd_uart_stats, "show UART statistics")
*/

enum ProceedingState {WaitingForData = 0, ReadMessID = 1, ReadData = 2};
uint8_t _ProceedingState = WaitingForData;
int8_t receivedData[10]={};
uint8_t dataCounter = 0;
char messID;



void ProceedData(char receivedChar){

	switch(_ProceedingState)
	{
		case WaitingForData:
			if(receivedChar=='#'){
				_ProceedingState = ReadMessID;
				LEDToggle(LED2);
			}
	    break;
		case ReadMessID:
			if(receivedChar=='1'||receivedChar=='2'||receivedChar=='3'){
				messID = receivedChar;
				_ProceedingState = ReadData;
			}
	  	break;
		case ReadData:
			switch(messID)
			{
				case '1':
					receivedData[dataCounter]=(int8_t)receivedChar;
					dataCounter++;
					if(dataCounter>9){
						_ProceedingState = WaitingForData;
						dataCounter=0;
						//variables need to be globally defined (include the .h file in which the variable is made accessible through extern... ) to assign values to them

						//omega_E2C[0] = (int16_t)(- 8192 + ((receivedData[0]) << 7 | ( receivedData[1])));
						//omega_E2C[1] = (int16_t)(- 8192 + ((receivedData[2]) << 7 | ( receivedData[3])));

						//_staticCurrent[0] = ( (int16_t)(- 8192 + ((receivedData[4]) << 7 | ( receivedData[5]))) )/1000.0;
						//_staticCurrent[1] = ( (int16_t)(- 8192 + ((receivedData[6]) << 7 | ( receivedData[7]))) )/1000.0;

						//breakDelay = (int8_t)(-64  + receivedData[8]);
						//_startController = (bool)(-1 + (int8_t)receivedData[9]);
						}
					break;
				case '2':
					receivedData[dataCounter]=(int8_t)receivedChar;
					dataCounter++;
					if(dataCounter>1){
						_ProceedingState = WaitingForData;
						dataCounter=0;
						//omega_R_0 = (int16_t)(- 8192 + ((receivedData[0]) << 7 | ( receivedData[1])));
						//printf("\n\r%i", (int16_t)omega_R_0);
						}
					break;

			}
		break;
	}
}
