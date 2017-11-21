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
#include "flight_data.h"
#include "usart.h"

UART_HandleTypeDef huart3;
SICL_InitTypeDef SICL;
Telemetry_InitTypeDef TM;
HouseKeeping_InitTypeDef com;
HouseKeeping_InitTypeDef obc;
IWDG_HandleTypeDef hiwdg;



int SICL_TX_msg(char* cmd, char* msg)
{
	int msg_len = 3; // = strlen(cmd) + strlen(msg) + 6;

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
	msg_len += 5;

	HAL_UART_Transmit(&huart3, (uint8_t*)SICL.TX, msg_len, 100);

	clearSICL_TX();
	return 0;
}

void SICL_NMEA_parser(void)
// TODO : add checksum check
{
	int i, j, k, IntegerPart;


	IntegerPart = 1;

	// TCHKD - COM house-keeping msg processing
	if ((SICL.RX[1] == 'T') && (SICL.RX[2] == 'C') && (SICL.RX[3] == 'H') && (SICL.RX[4] == 'K') && (SICL.RX[5] == 'D'))
	{
		com.temp = 0;
		com.VCC = 0;
		TM.msg_count = 0;

		for (i=0, j=0, k=0; (i<SICL.RXindex) && (j<10); i++)
		{
			if (SICL.RX[i] == ',')
			{
				j++;    // Segment index
				k=0;    // Index into target variable
				IntegerPart = 1;
			}
			else
			{
				if (j == 1)
				{
					if ((SICL.RX[i] >= '0') && (SICL.RX[i] <= '9'))
					{
						com.temp *= 10;
						com.temp += (SICL.RX[i] - '0');
						k++;
					}
				}
				else if (j == 2)
				{
					if ((SICL.RX[i] >= '0') && (SICL.RX[i] <= '9'))
					{
						com.VCC *= 10;
						com.VCC += (SICL.RX[i] - '0');
						k++;
					}
				}
				else if (j == 3)
				{
					if ((SICL.RX[i] >= '0') && (SICL.RX[i] <= '9'))
					{
						TM.msg_count *= 10;
						TM.msg_count += (SICL.RX[i] - '0');
						k++;
					}
					else if(SICL.RX[i] == '*')
					{
						j++;
					}
				}
			}
		}
		return;
	}
	// CMDTA - UPRA-CAM datastream porcessing
	if ((SICL.RX[1] == 'C') && (SICL.RX[2] == 'M') && (SICL.RX[3] == 'D') && (SICL.RX[4] == 'T') && (SICL.RX[5] == 'A'))
	{
		for (i=0, j=0, k=0; (i<SICL.RXindex) && (j<10); i++)
		{
			if (SICL.RX[i] == ',')
			{
				j++;    // Segment index
				k=0;    // Index into target variable
				IntegerPart = 1;
			}
			else
			{
				if (j == 1)
				{
					if(SICL.RX[i] == 'S')
					{
						// TODO : start image dumping
					}
					else if(SICL.RX[i] == '*')
					{
						j++;
					}
				}
			}
		}
		return;
	}

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

	char* debug[30]; //for debug only

	for(;;)
	{
		if(HAL_UART_Receive(&huart3, (uint8_t*)&tmp, 1, 1000) == HAL_OK)
		{
			HAL_IWDG_Refresh(&hiwdg);
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
		    	SICL_NMEA_parser();

				sprintf((char*)debug, "OBC: com temp: %d, com VCC: %d, message#: %d", com.temp, com.VCC, TM.msg_count);
				sendStatusln((char*)debug);
				//HAL_UART_Transmit(&huart3, (uint8_t*)debug, strlen((char*)debug), 100); //debug only
		    	return 0; //for debug only
		    	//SICL.RXindex = 0;
		    }
		    HAL_IWDG_Refresh(&hiwdg);
		}
		else
		{
			HAL_IWDG_Refresh(&hiwdg);
			timeout++;
			if(timeout > 3)
			{
				sendErrorln("OBC: HK request timeout");
				return 1;
			}
		}
	}
	return 3;
}

void getTChousekeeping(submodule module)
{
	int error=10;

	HAL_IWDG_Refresh(&hiwdg);
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

	char* tmp[85];

	int ext_temp = 236; // for debug only
	obc.temp = 235; //for debug only


	for(;;)
	{
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		vTaskSuspendAll();
		HAL_IWDG_Refresh(&hiwdg);
		getTChousekeeping(COM);

		HAL_IWDG_Refresh(&hiwdg);
		sprintf((char*)tmp, "%s,%s,%s,%05d,%04d,%04d", TM.time, TM.lati, TM.longi, TM.altitude, ext_temp, obc.temp);


		SICL_TX_msg("LTM", (char*)tmp);
		HAL_IWDG_Refresh(&hiwdg);
		xTaskResumeAll();

	}

}

/* USER CODE END 0 */
