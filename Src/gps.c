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
#define TIMESYNCTIMEOUT					0x00001fff//0x00001fff

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
char getGPS(void);
int getGGA(void);
void clearGPS_Buffer(void);
char CheckNSEW(char nsew);
void sendUBX(uint8_t *MSG, int len);
int getUBX_ACK(uint8_t* MSG);
int setGPS_PORT(void);
int setGPS_DynamicModel6(void);

/********************************************* FUNCTION DEFINITIONS *************************************************/
/********************************************************************************************************************/

void InitGPS(void)
{
	int ret = 100;

	sendStatus("OBC: Init GPS...");
	HAL_IWDG_Refresh(&hiwdg);
	ret = setGPS_DynamicModel6();
	if( ret != 0)
	{
		sendError("DM_error...");
	}
	HAL_IWDG_Refresh(&hiwdg);
	ret = setGPS_PORT();
	if( ret !=0)
	{
		sendErrorln("PRT_error");
		return;
	}
	HAL_IWDG_Refresh(&hiwdg);
	sendStatusln("OK");
}

void clearGPS_Buffer(void)
{
	for(int i=0; i<82; i++)
	{
		TM.GPSBuffer[i] = 0;
	}
	TM.GPSIndex = 0;
}

/*int getGPS(void)
{
	char tmp=0;
	clearGPS_Buffer();
	int timeout=0;

	for(;;)
	{
		HAL_IWDG_Refresh(&hiwdg);
		if(HAL_UART_Receive(&huart2, (uint8_t*)&tmp, 1, 500) == HAL_OK)
		{
//			HAL_IWDG_Refresh(&hiwdg);
			if ((tmp =='$') || (TM.GPSIndex >= 200))
		    {
		      TM.GPSIndex = 0;
		    }

		    if (tmp != '\r')
		    {
		      TM.GPSBuffer[TM.GPSIndex++] = tmp;
		    }

		    if (tmp == '\n')
		    {
		    	sendDebug(TM.GPSBuffer);
		    	return 0;
		    }
		}
		else
		{
			HAL_IWDG_Refresh(&hiwdg);
			timeout++;
			if(timeout > 10)
			{
				sendError("lTO...");
				return 1;
			}
		}
	}
	return 3;
}
*/


// read from GPS port
char getGPS(void)
{
	char tmp=0;
	int timeout=0;

	for(;;)
	{
		HAL_IWDG_Refresh(&hiwdg);
		if(HAL_UART_Receive(&huart2, (uint8_t*)&tmp, 1, 100) == HAL_OK)
		{
			return (char)tmp;
		}
		else
		{
			HAL_IWDG_Refresh(&hiwdg);
			timeout++;
			if(timeout > 10)
			{
				sendError("lTO...");
				return 0;
			}
		}
	}
	return 3;
}

/*char getGPS(void)
{
	char tmp=0;
	int timeout=0;

	while(((huart2.Instance->SR) & UART_FLAG_RXNE) == RESET)
	{

	}
	tmp = huart2.Instance->DR;

	return tmp;

	return 3;
}*/


//get a GGA message
int get_GGA()
{
	char inByte;
	int i=0;
	long int wtchdg=0;
//	sendUSARTstr("get_gga\r\n");
	TM.GPSIndex = 0;
	while (1)
	{
		inByte = getGPS();
		//sendDebugch(inByte);
		HAL_IWDG_Refresh(&hiwdg);
		if ((inByte =='$') || (TM.GPSIndex >= 80))
		{
			TM.GPSIndex = 0;
			TM.GPSBuffer[TM.GPSIndex] = inByte;
			TM.GPSIndex++;
			inByte = getGPS();
			//sendDebugch(inByte);
			HAL_IWDG_Refresh(&hiwdg);
			if((inByte =='G') && (inByte != 0))
			{
				TM.GPSBuffer[TM.GPSIndex] = inByte;
				TM.GPSIndex++;
				inByte = getGPS();
				//sendDebugch(inByte);
				HAL_IWDG_Refresh(&hiwdg);
				if((inByte =='P') && (inByte != 0))
				{
					TM.GPSBuffer[TM.GPSIndex] = inByte;
					TM.GPSIndex++;
					inByte = getGPS();
					//sendDebugch(inByte);
					HAL_IWDG_Refresh(&hiwdg);
					if((inByte =='G') && (inByte != 0))
					{
						TM.GPSBuffer[TM.GPSIndex] = inByte;
						TM.GPSIndex++;
						inByte = getGPS();
						//sendDebugch(inByte);
						HAL_IWDG_Refresh(&hiwdg);

						if((inByte =='G') && (inByte != 0))
						{
							TM.GPSBuffer[TM.GPSIndex] = inByte;
							TM.GPSIndex++;
							inByte = getGPS();
							//sendDebugch(inByte);
							HAL_IWDG_Refresh(&hiwdg);

							if((inByte =='A') && (inByte != 0))
							{
								//inByte = getGPS();
								TM.GPSBuffer[TM.GPSIndex] = inByte;
								TM.GPSIndex++;
								HAL_IWDG_Refresh(&hiwdg);

								for( i = TM.GPSIndex;((TM.GPSIndex>80) || (inByte != '\r')); i++)
//								for( ;((TM.GPSIndex>80) || (inByte != '\r')); TM.GPSIndex++)
								{
									inByte = getGPS();
									//sendDebugch(inByte);
									if(inByte != 0)
									{
										TM.GPSBuffer[i] = inByte;
										TM.GPSIndex++;
									}
									HAL_IWDG_Refresh(&hiwdg);
								}
								return 0;
							}
						}
					}
				}
			}
		}

		if (wtchdg > 100)
		{
			return 1;
		}

		wtchdg++;

	}

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
	//HAL_Delay(5);
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
	int ret_gga = 10;
	int timeout = 0;
	int timeout2 = 0;
	for(;;)
	{
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		vTaskSuspendAll();
		ret = 10;
		ret_gga = 10;
		timeout = 0;
		timeout2 = 0;
		sendStatus("OBC: Get GPS...");
		HAL_IWDG_Refresh(&hiwdg);
		while(ret_gga != 0)
		{
			while(ret != 0)
			{
//				sendDebug("w");
				timeout++;
				if(timeout >10)
				{
					sendErrorln("PORT timeout");
					break;
				}
				HAL_IWDG_Refresh(&hiwdg);
				ret = get_GGA();
				//sendDebugln(TM.GPSBuffer);
			}
			ret = 10;
			timeout=0;
			HAL_IWDG_Refresh(&hiwdg);
			timeout2++;
			if(timeout2 >10)
			{
				sendErrorln("No GPGAA");
				break;
			}
			ret_gga = GPS_NMEA_parser();

		}
		xTaskResumeAll();
	}
}


void sendUBX(uint8_t* MSG, int len)
{
	int i=0;

	HAL_UART_Abort(&huart2);
	if (HAL_UART_Transmit(&huart2, (uint8_t*)(0xFF), 1, 500) != HAL_OK)
	{
		sendDebug("this shall not be seen");
		return;
	}

	HAL_Delay(500);
//	delay(5);
/*	for(i=0; i<len; i++)
	{
		if (HAL_UART_Transmit(&huart2, (uint8_t*)MSG[i], 1, 500) != HAL_OK)
		{
			sendDebug("this shall not be seen either");
			return;
		}*/
		HAL_UART_Transmit(&huart2, &MSG, len, 500);

//		USART_SendData(USART2, MSG[i]);
//	}
}

//NEO6M SETTINGS

int setGPS_DynamicModel6(void)
{
	int gps_set_sucess=0;
	int wtchdg=0;
	char start[] = { 0xFF, '\0'};
	uint8_t setdm6[] = {
						0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06,
						0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,
						0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C,
						0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC };

	sendDebug("dm");
	while(!gps_set_sucess)
	{
		sendDebug(".");
		HAL_IWDG_Refresh(&hiwdg);
		sendUBX(setdm6, sizeof(setdm6)/sizeof(char));
		sendDebug(".");
		gps_set_sucess=getUBX_ACK(setdm6);
		wtchdg++;
		if( wtchdg > 3) //32700
		{
			return 1;
		}
	}
	return 0;
}

int setGPS_PORT(void)
{
	int gps_set_sucess=0;
	int wtchdg=0;
	char start[] = { 0xFF, '\0'};
	uint8_t setport[] = {
						0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00,
						0x00, 0xD0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00,
						0x07, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA1,
						0xAF };


	sendDebug("prt");
	while(!gps_set_sucess)
	{
		HAL_IWDG_Refresh(&hiwdg);
		sendDebug(".");
		sendUBX(setport, sizeof(setport)/sizeof(char));
		sendDebug(".");
		gps_set_sucess=getUBX_ACK(setport);
		wtchdg++;
		if( wtchdg > 3) //32700
		{
			return 1;
		}
	}
	return 0;
}

int getUBX_ACK(uint8_t* MSG)
{
	uint32_t 		wtchdg=0;
	char	 		b;
	int 			ackByteID = 0;
	uint8_t	 		ackPacket[10];
	int		 		ubxi;

	// Construct the expected ACK packet
	ackPacket[0] = 0xB5; // header
	ackPacket[1] = 0x62; // header
	ackPacket[2] = 0x05; // class
	ackPacket[3] = 0x01; // id
	ackPacket[4] = 0x02; // length
	ackPacket[5] = 0x00;
	ackPacket[6] = MSG[2]; // ACK class
	ackPacket[7] = MSG[3]; // ACK id
	ackPacket[8] = 0; // CK_A
	ackPacket[9] = 0; // CK_B

	// Calculate the checksums
	for (ubxi=2; ubxi<8; ubxi++)
	{
		ackPacket[8] = ackPacket[8] + ackPacket[ubxi];
		ackPacket[9] = ackPacket[9] + ackPacket[8];
	}

	HAL_IWDG_Refresh(&hiwdg);

	while (1)
	{
		// Test for success

		if(HAL_UART_Receive(&huart2, (uint8_t*)&b, 1, 100) == HAL_OK)
		{
			sendDebug("!");
			HAL_IWDG_Refresh(&hiwdg);
			if (ackByteID > 9)
			{
				// All packets in order!
				return 1;
			}

			// Timeout if no valid response in 3 seconds
			if (wtchdg > (20))
			{
				return 0;
			}
			wtchdg++;
			// Make sure data is available to read

			// Check that bytes arrive in sequence as per expected ACK packet
			if (b == ackPacket[ackByteID])
			{
				ackByteID++;
			}
			else
			{
				ackByteID = 0; // Reset and look again, invalid order
			}			//b = getGPS();
			HAL_IWDG_Refresh(&hiwdg);
		}

	}
}







/* USER CODE END 0 */
