/*******************************************************************************
  @file     i2c_master.h
  @brief    I2C peripheral driver
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

#ifndef MCAL_I2C_I2C_H_
#define MCAL_I2C_I2C_H_

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

typedef enum {
  I2C_INSTANCE_0,
  I2C_INSTANCE_1,
  I2C_INSTANCE_2,
  I2C_INSTANCE_COUNT
} i2c_id_t;

typedef enum {
  I2C_STATE_IDLE,
  I2C_STATE_IN_PROGRESS,
  I2C_STATE_FINISHED,
  I2C_STATE_ERROR
} i2c_state_t;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes the I2C peripheral driver
 * @param id            I2C Instance Id
 * @param baudRate      I2C Baud Rate
 */
void i2cMasterInit(i2c_id_t id, uint32_t baudRate);

/**
 * @brief Starts a transaction on the I2C bus
 * @param id            I2C Instance id
 * @param address       Slave address
 * @param writeBuffer   Data to be sent
 * @param bytesToWrite  Number of bytes to be sent
 * @param readBuffer    Buffer to place read data
 * @param bytesToRead   Number of bytes to be sent
 */
void i2cStartTransaction(i2c_id_t id, uint8_t address, uint8_t* writeBuffer, size_t bytesToWrite, uint8_t* readBuffer, size_t bytesToRead);

/**
 * @brief Returns the current status of the transaction
 * @param id            I2C Instance Id
 * @return  Returns i2c_state_t
 */
i2c_state_t i2cQueryTransaction(i2c_id_t id);

/*******************************************************************************
 ******************************************************************************/

#endif /* MCAL_I2C_I2C_H_ */