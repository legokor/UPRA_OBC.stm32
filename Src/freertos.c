/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "stm32f4xx_hal.h"
#include <string.h>
#include "SICL.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
//osTimerId mainTimerHandle;

/* USER CODE BEGIN Variables */
UART_HandleTypeDef huart3;
SICL_InitTypeDef SICL;

uint8_t last=0;
char tmp=0;
static TaskHandle_t xTask1 = NULL, xTask2 = NULL;
TaskHandle_t ltmTaskHandle = NULL;
char SICL_RX[64];

int period = 0;

TimerHandle_t mainTimer;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);


extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
void SICL_process(void const * argument);
void proba1(void const * argument);
void proba2(void const * argument);
void mainTimerCallback(TimerHandle_t xTimer);

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart3);
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of mainTimer */
//  osTimerDef(mainTimer, mainTimerCallback);
//  mainTimerHandle = osTimerCreate(osTimer(mainTimer), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */

  mainTimer = xTimerCreate("Timer", ( 1000 / portTICK_PERIOD_MS), pdTRUE, ( void * ) 0, mainTimerCallback );
  if( mainTimer == NULL)
  {
	  HAL_UART_Transmit(&huart3, (uint8_t*)"timer not created\n\r", 19, 100);
  }
  else
  {
	  if( xTimerStart(mainTimer, 0) != pdPASS)
	  {
		  HAL_UART_Transmit(&huart3, (uint8_t*)"timer not started\n\r", 19, 100);
	  }
  }

  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */

//  xTaskCreate(SICL_process, "SICL_RX", 500, NULL, osPriorityNormal, &commTaskHandle);
  xTaskCreate(TMLTM_TX, "LowSpeedTelemetry TX", 500, NULL, osPriorityNormal, &ltmTaskHandle);
  xTaskCreate(proba2, "p2", 1500, NULL, 2, &xTask2);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for FATFS */
  MX_FATFS_Init();

  /* USER CODE BEGIN StartDefaultTask */
  int i=0;
  /* Infinite loop */
  for(;;)
  {
      /* Send a notification to prvTask2(), bringing it out of the Blocked
      state. */
/*	  if(HAL_UART_Receive(&huart3, (uint8_t*)&tmp, 1, 3000) == HAL_OK)
	  {
		  SICL.RX[i] = tmp;
		  i++;
		  if(tmp == '\n')
		  {
			  tmp = 0;
			  i = 0;
			  xTaskNotifyGive( xTask2 );
		  }
	  }
	  else
	  {
		  HAL_UART_Transmit(&huart3, (uint8_t*)"timeout\n\r", 9, 100);
	  }*/
	  osDelay(0);


      /* Block to wait for prvTask2() to notify this task. */
 //     ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
  }
  /* USER CODE END StartDefaultTask */
}

/* mainTimerCallback function */


/* USER CODE BEGIN Application */


void SICL_process(void const * argument)
{


}

void proba1(void const * argument)
{
    for( ;; )
    {
        /* Send a notification to prvTask2(), bringing it out of the Blocked
        state. */
    	HAL_UART_Transmit(&huart3, (uint8_t*)"task1\n\r", 7, 100);
    	xTaskNotifyGive( xTask2 );
    	osDelay(200);
        /* Block to wait for prvTask2() to notify this task. */
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
    }
}

void proba2(void const * argument)
{
	for( ;; )
    {
        /* Block to wait for prvTask1() to notify this task. */
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

        /* Send a notification to prvTask1(), bringing it out of the Blocked
        state. */
 //       xTaskNotifyGive( xTask1 );
        HAL_UART_Transmit(&huart3, (uint8_t*)"task2\n\r", 7, 100);
/*        for(int i = 0; i < strlen(SICL.RX)+1; i++)
        {
        	tmp = SICL.RX[i];
            HAL_UART_Transmit(&huart3, &tmp, 1, 100);
        }*/
 //       HAL_UART_Transmit(&huart3, (uint8_t*)&SICL.RX, strlen(SICL.RX), 100);
        xTimerReset( mainTimer, 0);
        clearSICL_RX();
        char proba[] = {'1', '2' , ',' , '3'};
        SICL_TX_msg("com", proba);
        osDelay(200);
    }
}


void mainTimerCallback(TimerHandle_t xTimer)
{
  /* USER CODE BEGIN mainTimerCallback */
	period++;
	if( period > 10)
	{
		period = 0;
		xTaskNotifyGive( ltmTaskHandle );
	}

  /* USER CODE END mainTimerCallback */
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
