/*******************************************************************************
  @file     FXOS8700.h
  @brief    FXOS8700 accelerometer driver
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef HAL_FXOS8700_FXOS8700_H_
#define HAL_FXOS8700_FXOS8700_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// Declaring the acceleration data structure
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} acc_vector_t;

// Declaring landscape and portrait orientations
typedef enum {
  ACC_PORTRAIT_UP,
  ACC_PORTRAIT_DOWN,
  ACC_LANDSCAPE_RIGHT,
  ACC_LANDSCAPE_LEFT,
} acc_lp_orientations_t;

// Declaring back and front orientations
typedef enum {
  ACC_FRONT,
  ACC_BACK
} acc_bf_orientations_t;

// Declaring the orientation data structure
typedef struct {
  uint8_t   landscapePortrait   : 2;    // Returns a acc_lp_orientations_t value
  uint8_t   backFront           : 1;    // Returns a acc_bf_orientations_t value
} acc_orientation_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*
 * @brief Initialized the FXOS8700 accelerometer
 * @returns True if successful init
 */
bool FXOSInit(void);

/*
 * @brief Returns whether the driver is running
 */
bool FXOSIsRunning(void);

/**
 * @brief Returns when a new measurement is available
 */
bool FXOSMeasurementAvailable(void);

/*
 * @brief Instantaneous acceleration vector getter
 * @param coord         Pointer for acceleration in x, y, z axes
 * @returns status, false on error
 */
bool FXOSGetAcceleration(acc_vector_t* vector);

/*
 * @brief Instantaneous orientation getter
 * @param orientation   Pointer for orientation
 * @returns status, false on error 
 */
bool FXOSGetOrientation(acc_orientation_t* orientation);

/*******************************************************************************
 ******************************************************************************/

#endif /* HAL_FXOS8700_FXOS8700_H_ */
