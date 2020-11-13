/*******************************************************************************
  @file     i2c_master.c
  @brief    I2C peripheral driver
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "i2c_master.h"
#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// Declaring instance data structure
typedef struct {
  i2c_state_t   currentState;     // Current state of the I2C instance
  
  /* Baudrate */
  uint32_t      baudrate;
  
  /* Transaction user data */
  uint8_t*      writeBuffer;
  uint8_t*      readBuffer;
  size_t        bytesToWrite;
  size_t        bytesToRead;
  uint32_t      address;

  /* Transaction control data */
  uint8_t       bytesWritten;     // Bytes already written by the master
  uint8_t       bytesRead;        // Bytes already read from the slave
  bool          alreadyRepeated;  // When the repeated start was already done
} i2c_instance_t;

// Baudrate config parameters

typedef struct{
  uint8_t mul;
  uint8_t icr;

} baudrate_cfg_t;
/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void      I2C_IRQDispatcher(i2c_id_t id);
__ISR__   I2C0_IRQHandler(void);
__ISR__   I2C1_IRQHandler(void);
__ISR__   I2C2_IRQHandler(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// i2c instances array
static i2c_instance_t i2cInstances[I2C_INSTANCE_COUNT] = {
  {/*I2C0 instance parameters*/},
  {/*I2C1 instance parameters*/},
  {/*I2C2 instance parameters*/}
};

static I2C_Type * i2cPointers[] = I2C_BASE_PTRS;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void i2cMasterInit(i2c_id_t id, uint32_t baudRate)
{
	// Clock gating
  SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK | SIM_SCGC4_I2C1_MASK; 
  SIM->SCGC1 |= SIM_SCGC1_I2C2_MASK;

	// Enable I2C module and interrupts
  i2cPointers[id]->C1 |= I2C_C1_IICEN_MASK | I2C_C1_IICIE_MASK;

	// Configure baud rate ( I2C BR = i2C module clk speed / (mul * SCL divider) )

}

void i2cStartTransaction(i2c_id_t id, uint8_t address, uint8_t* writeBuffer, size_t bytesToWrite, uint8_t* readBuffer, size_t bytesToRead)
{
  // Initialising transaction variables.
  i2cInstances[id].writeBuffer = writeBuffer;
  i2cInstances[id].readBuffer = readBuffer;
  i2cInstances[id].bytesToWrite = bytesToWrite;
  i2cInstances[id].bytesToRead = bytesToRead;
  i2cInstances[id].address = address;
  i2cInstances[id].alreadyRepeated = false;
}

i2c_state_t i2cQueryTransaction(i2c_id_t id)
{
  i2c_state_t result = i2cInstances[id].currentState;
  if (result == I2C_STATE_FINISHED || result == I2C_STATE_ERROR)
  {
    result = I2C_STATE_IDLE;
  }
  return result;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static baudrate_cfg_t computeBaudrateSettings(uint32_t baudrate)
{
  baudrate_cfg_t setting = {.mul = 0, .icr = 0};
  uint32_t bestBaudrate = 0;
  uint32_t bestError = 0;
  uint32_t currentBaudrate = 0;
  uint32_t currentError = 0;
  
  for(uint8_t mul = 1 ; mul <= 4 ; mul = mul << 1)
  {
    for(uint8_t icr = 0 ; icr <= 0x3F ; icr++)
    {
      currentBaudrate = 
    }
  }
}
/*******************************************************************************
 *******************************************************************************
						            INTERRUPT SERVICE ROUTINES
 *******************************************************************************
 ******************************************************************************/

void I2C_IRQDispatcher(i2c_id_t id)
{
  i2c_instance_t* instance = &(i2cInstances[id]);
  I2C_Type* pointer = &(i2cPointers[id]);

  uint8_t status = pointer->S;

  // If transfer completed
  if (status & I2C_S_TCF_MASK)
  {
    // Clear flag
    pointer->S = I2C_S_TCF_MASK;

    // If transmission mode
    if (pointer->C1 & I2C_C1_TX_MASK)
    {
      if (instance->bytesWritten == instance->bytesToWrite)
      {
        // Is this the last byte to transmit AND I don't expect to receive any bytes?
        if (instance->bytesToRead)
        {
          // Verify if the slave answered with ACK
          if (pointer->S & I2C_S_RXAK_MASK)
          {
            // An error occurred and the slave didn't answer, so the transaction
            // is stopped and the status of the instance is changed
            instance->currentState = I2C_STATE_ERROR;
            pointer->C1 = (pointer->C1 & ~I2C_C1_MST_MASK) | I2C_C1_MST(0);
          }
          else
          {
            if (instance->alreadyRepeated)
            {
              // Execution reaches this point when a transfer was completed, with transmitter
              // enabled, and the transfer was the repeated start changing the direction
              // of the data bus to read the slave, start reading the first byte
              pointer->C1 = (pointer->C1 & ~I2C_C1_TX_MASK) | I2C_C1_TX(0);
              if ( instance->bytesToRead == 1)
              {
                pointer->C1 = (pointer->C1 & ~I2C_C1_TXAK_MASK) | I2C_C1_TXAK(1);
              }
              (void)pointer->D;   // Dummy read
            }
            else
            {
              // Transmission finished and there are some bytes to be read,
              // so the master generates a repeated start to change the data direction
              // of the bus, without releasing it...
              pointer->C1 = (pointer->C1 & ~I2C_C1_RSTA_MASK) | I2C_C1_RSTA(1);
              pointer->D = ((instance->address & 0x0000007F) << 1) | 0x00000001;
              instance->alreadyRepeated = true;
            }
          }
        }
        else
        {
          // There were no bytes to read, so the master releases the I2C bus
          // and the communication is stopped, the instance status is updated
          instance->currentState = I2C_STATE_FINISHED;
          pointer->C1 = (pointer->C1 & ~I2C_C1_MST_MASK) | I2C_C1_MST(0);
        }
      }
      else if (instance->bytesWritten < instance->bytesToWrite)
      {
        // Verify if the slave answered with ACK
        if (pointer->S & I2C_S_RXAK_MASK)
        {
          // An error occurred and the slave didn't answer, so the transaction
          // is stopped and the status of the instance is changed
          instance->currentState = I2C_STATE_ERROR;
          pointer->C1 = (pointer->C1 & ~I2C_C1_MST_MASK) | I2C_C1_MST(0);
        }
        else
        {
          // Keep sending bytes
          pointer->D = instance->writeBuffer[instance->bytesWritten++];
        }
      }
    }
    else
    {
      if (instance->bytesRead == instance->bytesToRead - 1)
      {
        // The current byte received is the last one, so after
        // reading it from the peripheral, the master releases the bus.
        // Also the current state of the instance is updated.
        pointer->C1 = (pointer->C1 & ~I2C_C1_MST_MASK) | I2C_C1_MST(0);
        instance->readBuffer[instance->bytesRead++] = pointer->D;
        instance->currentState = I2C_STATE_FINISHED;
      }
      else if (instance->bytesRead < instance->bytesToRead)
      {
        // If this is the 2nd to last byte, set the ACK to stop slave from
        // continue sending data frames, and then read the current data
        if (instance->bytesRead == instance->bytesToRead - 2)
        {
          pointer->C1 = (pointer->C1 & ~I2C_C1_TXAK_MASK) | I2C_C1_TXAK(1);
        }
        instance->readBuffer[instance->bytesRead++] = pointer->D;
      }
    }
  }
}

__ISR__ I2C0_IRQHandler(void)
{
  I2C_IRQDispatcher(I2C_INSTANCE_0);
}

__ISR__ I2C1_IRQHandler(void)
{
  I2C_IRQDispatcher(I2C_INSTANCE_1);
}

__ISR__ I2C2_IRQHandler(void)
{
  I2C_IRQDispatcher(I2C_INSTANCE_2);
}

/******************************************************************************/
