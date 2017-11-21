/* USER CODE BEGIN 0 */

/**
**********************************************************************************************************************
* @file    gps.h
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

#ifndef GPS_H_			// Header's name
#define GPS_H_

/**************************************************** INCLUDES ******************************************************/
/********************************************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "stm32f4xx_hal.h"
#include <string.h>


/***************************************************** DEFINES ******************************************************/
/********************************************************************************************************************/



/************************************************ TYPE DEFINITIONS **************************************************/
/********************************************************************************************************************/


/************************************************ MODULE VARIABLES **************************************************/
/********************************************************************************************************************/


/********************************************* FUNCTION DECLARATIONS ************************************************/
/********************************************************************************************************************/
void InitGPS(void);
void GPS_Process(void const * argument);

#endif  /* GPS_H_ */

/* USER CODE END 0 */
