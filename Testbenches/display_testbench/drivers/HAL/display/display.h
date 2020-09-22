/***************************************************************************//**
  @file     display.h
  @brief    [...]
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef HAL_DISPLAY_DISPLAY_H_
#define HAL_DISPLAY_DISPLAY_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum uint8_t{
	DISPLAY_1,
    DISPLAY_2,
    DISPLAY_3,
    DISPLAY_4,
    DISPLAY_COUNT
} display_id_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Initialise display driver and corresponding peripherals.
 */
void displayInit(void);

// Non-Blocking services ////////////////////////////////////////////////

/**
 * @brief Write character on display.
 * @param id id of the display to write to.
 * @param character character to write.
 */
void displayWrite(display_id_t id, uint8_t chr);

/**
 * @brief Enable display.
 * @param id id of the display.
 */
void displayEnable(display_id_t id);


/**
 * @brief Disable display.
 * @param id id of the display.
 */
void displayDisable(display_id_t id);

/**
 * @brief Set display's brightness.
 * @param id id of the display.
 * @param brightness brightness level.
 */
void displaySetBrightness(display_id_t id, display_brightness_t brightness);

/**
 * @brief Enable decimal point on display.
 * @param id id of the display to write to.
 */
void displayDotSet(display_id_t id);

/**
 * @brief Disable decimal point on display.
 * @param id id of the display to write to.
 */
void displayDotClear(display_id_t id);

/**
 * @brief Clears a display
 * @param id id of the display.
 */
void displayClear(display_id_t id);
/*******************************************************************************
 ******************************************************************************/

#endif
