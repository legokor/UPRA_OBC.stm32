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
	char RX[64];
	char TX[64];
	char CMD[3];
	char msgPayload[55];
} SICL_InitTypeDef;



/* Private function prototypes -----------------------------------------------*/
int SICL_TX_msg(char* cmd, char* msg, int isReply);
void SICL_NMEA_parser(uint8_t* msg);
void clearSICL_RX(void);

#endif /* SICL_H_ */
