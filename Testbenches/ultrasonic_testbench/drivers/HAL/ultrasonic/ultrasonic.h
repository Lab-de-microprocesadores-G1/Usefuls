/*******************************************************************************
  @file     ultrasonic.h
  @brief    Ultrasonic HC-SR04 Module Driver
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef HAL_ULTRASONIC_H_
#define HAL_ULTRASONIC_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

typedef void	(*us_callback)(double	distance);

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/********************
 *                  *
 * GENERAL SERVICES *
 *                  *
 *******************/

/**
 * @brief Initializes peripheral needed to use the ultrasonic module.
 */
void ultrasonicInit(void);

/**
 * @brief Starts the measurement process, it is highly recommended to leave
 * 		  at least 60mS between each measurement process to avoid error in the
 * 		  distance values.
 * 		  Returns whether it could be started or not.
 */
bool ultrasonicStart(void);

/**
 * @brief Returns whether the ultrasonic driver is busy or not, if measurement process has not finished.
 */
bool ultrasonicBusy(void);

/********************
 *                  *
 * POLLING SERVICES *
 *                  *
 *******************/

/**
 * @brief Returns whether the ultrasonic driver has a measurement not read yet,
 * 		  if that the case returns true and clears the internal flag.
 */
bool ultrasonicHasDistance(void);

/**
 * @brief Returns the current measured distance, should be called after verifying
 * 		  that the driver has a distance saved using ultrasonicHasDistance().
 * 		  The distance returned is measured in millimeters.
 */
double ultrasonicGetDistance(void);

/*************************
 *                       *
 * EVENT DRIVEN SERVICES *
 *                       *
 ************************/

/**
 * @brief Registers a callback to be called when measurement process finished and
 * 		  a valid value is available, which is given to the callback.
 * 		  The distance is passed in millimeters.
 * @param callback		Callback for the measurement finished event
 */
void ultrasonicSubscribe(us_callback callback);

/*******************************************************************************
 ******************************************************************************/


#endif /* HAL_ULTRASONIC_H_ */
