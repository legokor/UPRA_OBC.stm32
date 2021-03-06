/* USER CODE BEGIN 0 */

/*
 * SICL.h
 *
 *  Created on: 2017. okt. 28.
 *      Author: Komposzt
 */

#ifndef SICL_H_
#define SICL_H_

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "stm32f4xx_hal.h"
#include <string.h>

/* MACROS BEGIN */



/* MACROS END */

/* Private variables ---------------------------------------------------------*/


typedef struct
{
	char RX[82];
	int  RXindex;
	char TX[82];
	char CMD[3];
	char msgPayload[73];
} SICL_InitTypeDef;

typedef enum
{
	COM = 1
} submodule;


/* Private function prototypes -----------------------------------------------*/
int SICL_TX_msg(char* cmd, char* msg);
void SICL_NMEA_parser(void);
void TMLTM_TX(void const * argument);
void getTChousekeeping(submodule module);

int SICL_RX_msg(void);

void clearSICL_RX(void);
void clearSICL_TX(void);

#endif /* SICL_H_ */

/* USER CODE END 0 */
