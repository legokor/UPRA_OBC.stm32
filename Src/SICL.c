/* USER CODE BEGIN 0 */

/*
 * SICL.c
 *
 *  Created on: 2017. okt. 28.
 *      Author: Komposzt
 */

#include "SICL.h"
#include <stdio.h>

UART_HandleTypeDef huart3;
SICL_InitTypeDef SICL;


int SICL_TX_msg(char* cmd, char* msg, int isReply)
{
	int msg_len = 3; // = strlen(cmd) + strlen(msg) + 6;
	int i=0;

	// $TM cmd , msg * cc
	// 012 345 6
	SICL.TX[0] = '$';
	SICL.TX[1] = 'T';
	SICL.TX[2] = 'M';

	for( i = 0; i < 3; i++)
	{
		SICL.TX[msg_len + i] = cmd[i];
	}

	msg_len += 3;

	SICL.TX[msg_len++] = ',';

	for( i = 0; i < strlen(msg)-2; i++)
	{
		SICL.TX[msg_len + i] = msg[i];
	}

	msg_len += i;

	SICL.TX[msg_len++] = '*';


	/*SICL.TX[msg_len++] = 'c';
	SICL.TX[msg_len++] = 'c';*/

	sprintf(SICL.TX, "%s%02X\n", SICL.TX, NMEAchecksum(SICL.TX));
	msg_len += 3;

	HAL_UART_Transmit(&huart3, SICL.TX, msg_len, 100);


/*	HAL_UART_Transmit(&huart3, "$TM", 3, 100);
	HAL_UART_Transmit(&huart3, cmd, strlen(cmd), 100);
	HAL_UART_Transmit(&huart3, ",", 1, 100);
	HAL_UART_Transmit(&huart3, cmd, strlen(cmd), 100);*/
	return 0;
}

void SICL_NMEA_parser(uint8_t* msg)
{

}

void clearSICL_RX(void)
{
	for(int i=0; i<64; i++)
	{
		SICL.RX[i] = 0;
	}
}

/* USER CODE END 0 */
