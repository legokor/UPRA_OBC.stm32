/* USER CODE BEGIN 0 */

/**
*********************************************************************************************************************
* @file    gps.c
* @author  Your name
* @version 1.0
* @date    2017. nov. 19.
* @brief   Brief overview of the file
* @bug	   Known bugs
* @test	   Tests made to prove the correct functioning
* @todo	   TODO
**********************************************************************************************************************
* @attention
*
*
* <h2><center>&copy; COPYRIGHT 2017 UPRA TEAM </center></h2>
**********************************************************************************************************************
*/


/**************************************************** INCLUDES ******************************************************/
/********************************************************************************************************************/
#include "gps.h"
#include "checksum.h"
#include <stdio.h>
#include "flight_data.h"
#include "usart.h"

/***************************************************** DEFINES ******************************************************/
/********************************************************************************************************************/

/************************************************ TYPE DEFINITIONS **************************************************/
/********************************************************************************************************************/

/************************************************ MODULE VARIABLES **************************************************/
/********************************************************************************************************************/
UART_HandleTypeDef huart2;
Telemetry_InitTypeDef TM;
IWDG_HandleTypeDef hiwdg;

/****************************************** LOCAL FUNCTION DECLARATIONS *********************************************/
/********************************************************************************************************************/
int GPS_NMEA_parser(void);
int getGPS(void);
void clearGPS_Buffer(void);
char CheckNSEW(char nsew);

/********************************************* FUNCTION DEFINITIONS *************************************************/
/********************************************************************************************************************/

void clearGPS_Buffer(void)
{
	for(int i=0; i<82; i++)
	{
		TM.GPSBuffer[i] = 0;
	}
	TM.GPSIndex = 0;
}

int getGPS(void)
{
	char tmp=0;
	clearGPS_Buffer();
	int timeout=0;

	for(;;)
	{
		if(HAL_UART_Receive(&huart2, (uint8_t*)&tmp, 1, 500) == HAL_OK)
		{
			HAL_IWDG_Refresh(&hiwdg);
			if ((tmp =='$') || (TM.GPSIndex >= 80))
		    {
		      TM.GPSIndex = 0;
		    }

		    if (tmp != '\r')
		    {
		      TM.GPSBuffer[TM.GPSIndex++] = tmp;
		    }

		    if (tmp == '\n')
		    {
		    	GPS_NMEA_parser();

		    	return 0;
		    }
		    HAL_IWDG_Refresh(&hiwdg);
		}
		else
		{
			HAL_IWDG_Refresh(&hiwdg);
			timeout++;
			if(timeout > 3)
			{
				sendError("GPS line timeout...");
				return 1;
			}
		}
	}
	return 3;
}

int GPS_NMEA_parser(void)
{
	int i, j, k, IntegerPart;


	IntegerPart = 1;

	// GPGGA processing
	if ((TM.GPSBuffer[1] == 'G') && (TM.GPSBuffer[2] == 'P') && (TM.GPSBuffer[3] == 'G') && (TM.GPSBuffer[4] == 'G') && (TM.GPSBuffer[5] == 'A'))
	{

		IntegerPart = 1;
		TM.sat = 0;
		TM.fix = 0;
		int Altitude = 0;
		sprintf(TM._lati, "%s", TM.lati);
		sprintf(TM._longi, "%s", TM.longi);
		sprintf(TM._time, "%s", TM.time);

		// $GPGGA,015258,3818.4698,N,02423.6403,E,1,04,5.6,1615.7,M,34.5,M,,*7F
		//	0		1		2	   3	4		5 6  7  8    9

		for (i=0, j=0, k=0; (i<TM.GPSIndex) && (j<10); i++) // We start at 7 so we ignore the '$GPGGA,'
		{
			if (TM.GPSBuffer[i] == ',')
			{
				j++;    // Segment index
				k=0;    // Index into target variable
				IntegerPart = 1;
			}
			else
			{
				if (j == 1)
				{
					//read time
					if ((TM.GPSBuffer[i] >= '0') && (TM.GPSBuffer[i] <= '9') && (TM.GPSBuffer[i] != '.') && IntegerPart)
					{
						TM.time[k] = TM.GPSBuffer[i];
						k++;
					}
					else
					{
						IntegerPart = 0;
					}
				}
				else if (j == 2)
				{
					//read latitude
					TM.lati[k+1] = TM.GPSBuffer[i];
					k++;
				}
				else if (j == 3)
				{
					//read nsew
					TM.lati[0] = CheckNSEW(TM.GPSBuffer[i]);
				}
				else if (j == 4)
				{
					//read longitude
					TM.longi[k+1] = TM.GPSBuffer[i];
					k++;
				}
				else if (j == 5)
				{
					//read nsew
					TM.longi[0] = CheckNSEW(TM.GPSBuffer[i]);
				}
				else if (j == 6)
				{
					// Fix read
					if ((TM.GPSBuffer[i] >= '0') && (TM.GPSBuffer[i] <= '9'))
					{
						//TM.fix = TM.fix * 10;
						//TM.fix += (unsigned int)(TM.GPSBuffer[i] - 48);
						TM.fix = (TM.GPSBuffer[i] - '0');
					}
				}
				else if (j == 7)
				{
					// Satellite Count
					if ((TM.GPSBuffer[i] >= '0') && (TM.GPSBuffer[i] <= '9'))
					{
						TM.sat = TM.sat * 10;
						TM.sat += (unsigned int)(TM.GPSBuffer[i] - '0');
					}
				}
				else if (j == 9)
				{
					// Altitude
//					if ((TM.GPSBuffer[i] >= '0') && (TM.GPSBuffer[i] <= '9') && IntegerPart)
					if ((TM.GPSBuffer[i] >= '0') && (TM.GPSBuffer[i] <= '9') && (TM.GPSBuffer[i] != '.') && IntegerPart)
					{
						Altitude = Altitude * 10;
						Altitude += (unsigned int)(TM.GPSBuffer[i] - '0'); //-48
					}
					else
					{
						IntegerPart = 0;
					}
				}
			}

		}

		// check fix
		if( TM.fix !=0)
		{
			sprintf((char*)TM._lati, "%s", TM.lati);
			sprintf((char*)TM._longi, "%s", TM.longi);
			sprintf((char*)TM._time, "%s", TM.time);
/*			TM._lati[11] ='\0';
			TM._longi[12] ='\0';
			TM._time[7] ='\0';*/
			TM.altitude = Altitude;
		//	gps_read_error = 0;
		}
		else
		{
		//	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
			sprintf(TM.lati, "%s", TM._lati);
			sprintf(TM.longi, "%s", TM._longi);
		//	sprintf(TM.time, "%02d%02d%02d", RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
			TM.lati[11] ='\0';
			TM.longi[12] ='\0';
			TM.time[7] ='\0';
			//gps_read_error = 2;
		}
		sendStatusln("OK");
		return 0;
	}
	sendStatus("...");
	return 1;
}

char CheckNSEW(char nsew)
{
	if(( nsew == 'N') || ( nsew == 'E'))
	{
		return '+';
	}
	else if(( nsew == 'S') || ( nsew == 'W'))
	{
		return '-';
	}
	else return 0;
}



void GPS_Process(void const * argument)
{
	int ret = 10;
	int timeout = 0;
	for(;;)
	{
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		ret = 10;
		timeout = 0;
		sendStatus("OBC: Get GPS...");
		HAL_IWDG_Refresh(&hiwdg);
		while(ret != 0)
		{
			timeout++;
			if(timeout >5)
			{
				sendErrorln("GPS-GGA timeout");
				break;
			}
			HAL_IWDG_Refresh(&hiwdg);
			ret = getGPS();
		}
		HAL_IWDG_Refresh(&hiwdg);

	}
}

/* USER CODE END 0 */
