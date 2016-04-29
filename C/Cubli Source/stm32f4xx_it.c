/**
*****************************************************************************
**
**  File        : stm32f4xx_it.c
**
**  Abstract    : Main Interrupt Service Routines.
**                This file provides template for all exceptions handler and
**                peripherals interrupt service routine.
**
**  Environment : Atollic TrueSTUDIO(R)
**                STMicroelectronics STM32F4xx Standard Peripherals Library
**
**  Distribution: The file is distributed “as is,” without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the Atollic TrueSTUDIO(R) toolchain.
**
**
*****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "UtilityTask.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/


CanRxMsg RxMessage;
//extern uint8_t KeyNumber;

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}


//#define USE_CAN1
//
//
//#ifdef USE_CAN1



extern xQueueHandle _velocityQueue[3];
extern xQueueHandle _currentQueue[3];
/**
  * @brief  This function handles CAN1 RX0 request.
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
//  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
//
//  if ((RxMessage.StdId == 0x321)&&(RxMessage.IDE == CAN_ID_STD) && (RxMessage.DLC == 1))
//  {
//    LED_Display(RxMessage.Data[0]);
//    //KeyNumber = RxMessage.Data[0];
//  }
//
//  //portEND_SWITCHING_ISR()


	//printf("Can Input Interrupt\n\r");

	long xHigherPriorityTaskWoken = 0; //Means false

	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	//LEDOff(LED3);LEDOff(LED4);LEDOff(LED5);LEDOff(LED6);
	//LEDOn(LED3);
	//printf("Msg StdId: %i\n\r", (int)RxMessage.StdId);
	//LEDOn(LED4);
	if (RxMessage.StdId == 0x581) //Depends on Node ID of component in CAN network
	{
		if(RxMessage.Data[0] == 0x43 && RxMessage.Data[1] == 0x28 && RxMessage.Data[2] == 0x20){
			xQueueSendToBackFromISR( _velocityQueue[0] , &RxMessage, &xHigherPriorityTaskWoken );}
		else if(RxMessage.Data[0] == 0x4B && RxMessage.Data[1] == 0x78 && RxMessage.Data[2] == 0x60){
			xQueueSendToBackFromISR( _currentQueue[0] , &RxMessage, &xHigherPriorityTaskWoken );}
	}
	if (RxMessage.StdId == 0x582)
	{
		if(RxMessage.Data[0] == 0x43 && RxMessage.Data[1] == 0x28 && RxMessage.Data[2] == 0x20){
			xQueueSendToBackFromISR( _velocityQueue[1] , &RxMessage, &xHigherPriorityTaskWoken );}
		else if(RxMessage.Data[0] == 0x4B && RxMessage.Data[1] == 0x78 && RxMessage.Data[2] == 0x60){
			xQueueSendToBackFromISR( _currentQueue[1] , &RxMessage, &xHigherPriorityTaskWoken );}
		//xQueueSendToBackFromISR( _canInputQueue[1] , &RxMessage, &xHigherPriorityTaskWoken );
	}
	if (RxMessage.StdId == 0x583)
	{
		if(RxMessage.Data[0] == 0x43 && RxMessage.Data[1] == 0x28 && RxMessage.Data[2] == 0x20){
			xQueueSendToBackFromISR( _velocityQueue[2] , &RxMessage, &xHigherPriorityTaskWoken );}
		else if(RxMessage.Data[0] == 0x4B && RxMessage.Data[1] == 0x78 && RxMessage.Data[2] == 0x60){
			xQueueSendToBackFromISR( _currentQueue[2] , &RxMessage, &xHigherPriorityTaskWoken );}
		//xQueueSendToBackFromISR( _canInputQueue[2] , &RxMessage, &xHigherPriorityTaskWoken );
	}

	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}



///**
//  * @brief  This function handles CAN1 RX0 request.
//  * @param  None
//  * @retval None
//  */
//void CAN1_RX0_IRQHandler(void)
//{
////  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
////
////  if ((RxMessage.StdId == 0x321)&&(RxMessage.IDE == CAN_ID_STD) && (RxMessage.DLC == 1))
////  {
////    LED_Display(RxMessage.Data[0]);
////    //KeyNumber = RxMessage.Data[0];
////  }
////
////  //portEND_SWITCHING_ISR()
//
//
//	//printf("Can Input Interrupt\n\r");
//
//	LEDToggle(LED3);
//
//	long xHigherPriorityTaskWoken = 0; //Means false
//
//	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
//	//LEDOff(LED3);LEDOff(LED4);LEDOff(LED5);LEDOff(LED6);
//	//LEDOn(LED3);
//	//printf("Msg StdId: %i\n\r", (int)RxMessage.StdId);
//	//LEDOn(LED4);
//	if (RxMessage.StdId == 0x581) //Depends on Node ID of component in CAN network
//	//if ((RxMessage.StdId == 0x582)/*&& (RxMessage.Data[0] == 0x60)*/)
//	{
//		//CAN_Process_Rx_message(RxMessage);
//		xQueueSendToBackFromISR( xCAN_Velocity_Queue , &RxMessage, &xHigherPriorityTaskWoken );
//		//LEDOn(LED5);
//	}
//
//	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
//	//LEDOn(LED6);
//}

void ButtonInterupptServo()
{
	//_servoPos += _servoIncrement;
	LEDToggle(LED2);
}

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */

extern enum CubeModes;
extern cube_mode _modeNumber;
extern uint8_t _jumpMode;	//ADDED To DEBUG
extern enum JumpMode;


//This interrupt occurs when the user button is pressed.
//Then, _modeNumber cycles through the button modes
void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
	  //_jumpMode=F2CJump;
	if(_attitudeReady){
		_modeNumber = cycleButtonMode( _modeNumber );
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

bool _OnPressed=false;
extern bool _shutDown;
xTaskHandle _OnButtonHandler;

//This interrupt occurs when the on button is pressed.
void EXTI9_5_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line6);		// Clear interrupt flag
	if(!_OnPressed){
		_OnPressed=true;
		xTaskCreate( OnButtonHandler, "OnButtonHandler", configMINIMAL_STACK_SIZE + 250, NULL,	tskIDLE_PRIORITY + 2, &_OnButtonHandler);
	}else if(!_shutDown){
		vTaskDelete(_OnButtonHandler);
		_OnPressed=false;
	}
}



//
//#define USARTx_IRQHANDLER USART3_IRQHandler
//
//#define TXBUFFERSIZE (countof(TxBuffer) - 1)
//#define RXBUFFERSIZE 0x20
//
///* Private macro -------------------------------------------------------------*/
//#define countof(a) (sizeof(a) / sizeof(*(a)))
//
///* Private variables ---------------------------------------------------------*/
/*uint8_t TxBuffer[] = "\n\rUSART Hyperterminal Interrupts Example: USART-Hyperterminal\ */
// communication using Interrupt\n\r";
//uint8_t RxBuffer[RXBUFFERSIZE];
//uint8_t NbrOfDataToTransfer = TXBUFFERSIZE;
//uint8_t NbrOfDataToRead = RXBUFFERSIZE;
//__IO uint8_t TxCounter = 0;
//__IO uint16_t RxCounter = 0;
//
//
////void Test()
////{
//// USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
////}
//
//
///**
// * @brief This function handles USARTx global interrupt request.
// * @param None
// * @retval None
// */
//void USARTx_IRQHANDLER(void)
//{
// //uint16_t interruptType = USART_IT_TC;
//
// LEDToggle(LED4);
//
// if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
// {
// LEDToggle(LED5);
//
// /* Read one byte from the receive data register */
// RxBuffer[RxCounter++] = (USART_ReceiveData(USART1) & 0x7F);
//
// if(RxCounter == NbrOfDataToRead)
// {
// /* Disable the EVAL_COM1 Receive interrupt */
// USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
// }
// }
//
//// if(USART_GetITStatus(interruptType, USART_IT_TXE) != RESET)
//// {
//// /* Write one byte to the transmit data register */
//// USART_SendData(USART1, TxBuffer[TxCounter++]);
////
//// if(TxCounter == NbrOfDataToTransfer)
//// {
//// /* Disable the EVAL_COM1 Transmit interrupt */
//// USART_ITConfig(interruptType, USART_IT_TXE, DISABLE);
//// }
//// }
//}



/**
* @brief This function handles CAN1 Handler.
* @param None
* @retval None
*/
//void USB_LP_CAN1_RX0_IRQHandler(void)
//{
// CanTxMsg TxMessage;
//
//
// portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
//
// CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
// if ((RxMessage.StdId == 0x581)/*&& (RxMessage.Data[0] == 0x60)*/)
// {
// //CAN_Process_Rx_message(RxMessage);
// xQueueSendToBackFromISR( xCAN_Velocity_Queue , &RxMessage, &xHigherPriorityTaskWoken );
// }
//
// portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
//}

//#endif /* USE_CAN1 */

//#ifdef USE_CAN2
///**
// * @brief This function handles CAN2 RX0 request.
// * @param None
// * @retval None
// */
//void CAN2_RX0_IRQHandler(void)
//{
// CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
//
// if ((RxMessage.StdId == 0x321)&&(RxMessage.IDE == CAN_ID_STD) && (RxMessage.DLC == 1))
// {
// LED_Display(RxMessage.Data[0]);
// KeyNumber = RxMessage.Data[0];
// }
//}
//#endif /* USE_CAN2 */




/**
* @brief This function handles PendSVC exception.
* @param None
* @retval None
*/
//void PendSV_Handler(void)
//{
//}

/**
* @brief This function handles SysTick Handler.
* @param None
* @retval None
*/
//void SysTick_Handler(void)
//{
//}

/******************************************************************************/
/* STM32F4xx Peripherals Interrupt Handlers */
/* Add here the Interrupt Handler for the used peripheral(s) (PPP), for the */
/* available peripheral interrupt handler's name please refer to the startup */
/* file (startup_stm32f4xx.s). */
/******************************************************************************/

/**
* @brief This function handles PPP interrupt request.
* @param None
* @retval None
*/
/*void PPP_IRQHandler(void)
{
}*/
