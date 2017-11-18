/* USER CODE BEGIN 0 */

/**
**********************************************************************************************************************
* @file    flight_data.h
* @author  Bence Goczan
* @version 1.0
* @date    2017. nov. 18.
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

#ifndef FLIGHT_DATA_H_			// Header's name
#define FLIGHT_DATA_H_

/**************************************************** INCLUDES ******************************************************/
/********************************************************************************************************************/


/***************************************************** DEFINES ******************************************************/
/********************************************************************************************************************/


/************************************************ TYPE DEFINITIONS **************************************************/
/********************************************************************************************************************/
typedef struct
{
	int temp;
	int VCC;
	int curr;
} HouseKeeping_InitTypeDef;

typedef struct
{
	char 			GPSBuffer[82];
	int 			GPSIndex;
	char 			value;
	char 			_time[10];// ="hhmmss.sss";
	char 			time[10];// ="hhmmss.sss";
	char 			lati[12];//= "+yyyy.yyyy";
	char 			_lati[12];//= "+yyyy.yyyy";
	char			lati_dir;
	char 			longi[13];// = "-lllll.llll";
	char 			_longi[13];// = "-lllll.llll";
	char 			longi_dir;
	volatile char 	alti[8];
	unsigned int 	altitude; //altitude in integer
	char	 		geo[8];
	char 			temp[6];
	char 			volt[4];
	char 			frequ;
	char 			lastcmdtime[7];
	char			pos[32];
	volatile char 	alt[18];
	unsigned int 	fix;
	unsigned int 	sat;
	int				msg_count;

} Telemetry_InitTypeDef;


/************************************************ MODULE VARIABLES **************************************************/
/********************************************************************************************************************/


/********************************************* FUNCTION DECLARATIONS ************************************************/
/********************************************************************************************************************/


#endif  /* FLIGHT_DATA_H_ */

/* USER CODE END 0 */
