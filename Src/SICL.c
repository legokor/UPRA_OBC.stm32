/* USER CODE BEGIN 0 */

/*
 * SICL.c
 *
 *  Created on: 2017. okt. 28.
 *      Author: Komposzt
 */

#include "SICL.h"
#include "checksum.h"
#include <stdio.h>

UART_HandleTypeDef huart3;
SICL_InitTypeDef SICL;


int SICL_TX_msg(char* cmd, char* msg)
{
	int msg_len = 3; // = strlen(cmd) + strlen(msg) + 6;
	int i=0;

	// $TM cmd , msg * cc
	// 012 345 6
	SICL.TX[0] = '$';
	SICL.TX[1] = 'T';
	SICL.TX[2] = 'M';


	sprintf(SICL.TX, "%s%s,%s*", SICL.TX, cmd, msg );

	msg_len += strlen(msg) + 4;

	/*SICL.TX[msg_len++] = 'c';
	SICL.TX[msg_len++] = 'c';*/

	sprintf(SICL.TX, "%s%02X\n\r", SICL.TX, NMEAchecksum(SICL.TX));
	msg_len += 4;

	HAL_UART_Transmit(&huart3, (uint8_t*)SICL.TX, msg_len, 100);

	clearSICL_TX();
	return 0;
}

void SICL_NMEA_parser(uint8_t* msg)
{

}

void clearSICL_RX(void)
{
	for(int i=0; i<82; i++)
	{
		SICL.RX[i] = 0;
	}
}

void clearSICL_TX(void)
{
	for(int i=0; i<82; i++)
	{
		SICL.TX[i] = 0;
	}
}


int SICL_RX_msg(void)
{
	char tmp=0;
	SICL.RXindex=0;
	int timeout=0;

	for(;;)
	{
		if(HAL_UART_Receive(&huart3, (uint8_t*)&tmp, 1, 1000) == HAL_OK)
		{
		    if ((tmp =='$') || (SICL.RXindex >= 80))
		    {
		      SICL.RXindex = 0;
		    }

		    if (tmp != '\r')
		    {
		      SICL.RX[SICL.RXindex++] = tmp;
		    }

		    if (tmp == '\n')
		    {
		    	// TODO : Processing function
				HAL_UART_Transmit(&huart3, (uint8_t*)SICL.RX, SICL.RXindex, 100); //debug only
		    	return 0; //for debug only
		    	SICL.RXindex = 0;
		    }
		}
		else
		{
			timeout++;
			if(timeout > 5)
			{
				HAL_UART_Transmit(&huart3, (uint8_t*)"OBC: HK request timeout\n\r", 25, 100); //TODO: create easy serial TX function
				return 1;
			}
		}
	}
	return 3;
}

void getTChousekeeping(submodule module)
{
	int error=10;

	switch (module)
	{
		case COM: SICL_TX_msg("HKR", "C"); break;
		default: break;
	}

	error = SICL_RX_msg();

	//... TODO: multiple tries and local timer watchdog should be implemented
}

void TMLTM_TX(void const * argument)
{
//	uint32_t ulNotificationValue;

	for(;;)
	{
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

		getTChousekeeping(COM);


		//TODO : Add proper message structure
		SICL_TX_msg("LTM", "336677,+4545.222,+01133.555,00236,0236,0235");

	}

}

/* USER CODE END 0 */
