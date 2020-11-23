/*******************************************************************************
  @file     FXOS8700.c
  @brief    FXOS8700 accelerometer driver
  @author   G. Davidov, F. Farall, J. Gaytán, L. Kammann, N. Trozzo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "fxos8700_accelerometer.h"

#include "drivers/MCAL/i2c/i2c_master.h"
#include "drivers/HAL/timer/timer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// General settings and configuration values
#define FXOS8700CQ_I2C_BAUD_RATE        100000U
#define FXOS8700CQ_UPDATE_TICK_MS       50
#define FXOS8700CQ_UPDATE_TICK          TIMER_MS2TICKS(FXOS8700CQ_UPDATE_TICK_MS)
#define FXOS8700CQ_WRITE_BUFFER_SIZE    2
#define FXOS8700CQ_READ_BUFFER_SIZE     2
#define FXOS8700CQ_WHOAMI_VALUE         0xC7

// FXOS8700CQ I2C Slave Address
#define FXOS8700CQ_SLAVE_ADDRESS        0x1D

// FXOS8700CQ Register Addresses
#define FXOS8700CQ_STATUS_REG           0x00
#define FXOS8700CQ_WHOAMI_REG           0x0D
#define FXOS8700CQ_CTRL_REG1            0x2A
#define FXOS8700CQ_XYZ_DATA_CFG_REG     0x0E
#define FXOS8700CQ_OUT_X_MSB_REG        0x01
#define FXOS8700CQ_OUT_X_LSB_REG        0x02
#define FXOS8700CQ_OUT_Y_MSB_REG        0x03
#define FXOS8700CQ_OUT_Y_LSB_REG        0x04
#define FXOS8700CQ_OUT_Z_MSB_REG        0x05
#define FXOS8700CQ_OUT_Z_LSB_REG        0x06
#define FXOS8700CQ_PL_STATUS_REG        0x10
#define FXOS8700CQ_PL_CFG_REG           0x11
#define FXOS8700CQ_PL_COUNT_REG         0x12
#define FXOS8700CQ_PL_BF_ZCOMP_REG      0x13
#define FXOS8700CQ_PL_THS_REG           0x14

#define FXOS_REG2INT(x)                 (*(uint8_t *)(&x))
#define FXOS_INT2REG(x, y)              (*(y*)(&x))

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// Declaring the status values of the accelerometer
typedef enum {
  ACC_STATUS_INITIALIZATION,
  ACC_STATUS_RUNNING,
  ACC_STATUS_ERROR
} acc_status_t;

// PL Back-Front Z-lockout angles 
typedef enum {
  FXOS_PL_BF_ZLOCK_13_DEG,
  FXOS_PL_BF_ZLOCK_17_DEG,
  FXOS_PL_BF_ZLOCK_20_DEG,
  FXOS_PL_BF_ZLOCK_24_DEG,
  FXOS_PL_BF_ZLOCK_28_DEG,
  FXOS_PL_BF_ZLOCK_32_DEG,
  FXOS_PL_BF_ZLOCK_36_DEG,
  FXOS_PL_BF_ZLOCK_40_DEG,
} fxos_pl_bf_zlock_t;

// Back-Front Trip Angle Threshold
typedef enum {
  FXOS_PL_BKFR_0,
  FXOS_PL_BKFR_1,
  FXOS_PL_BKFR_2,
  FXOS_PL_BKFR_3
} fxos_pl_bf_bkfr_t;

// PL Threshold angles 
typedef enum {
  FXOS_PL_THS_15_DEG = 0x07,
  FXOS_PL_THS_20_DEG = 0x09,
  FXOS_PL_THS_30_DEG = 0x0C,
  FXOS_PL_THS_35_DEG = 0x0D,
  FXOS_PL_THS_40_DEG = 0x0F,
  FXOS_PL_THS_45_DEG = 0x10,
  FXOS_PL_THS_55_DEG = 0x13,
  FXOS_PL_THS_60_DEG = 0x14,
  FXOS_PL_THS_70_DEG = 0x17,
  FXOS_PL_THS_75_DEG = 0x19,
} fxos_pl_threshold_t;

// PL Hysteresis angles
typedef enum {
  FXOS_PL_HYS_4_DEG,
  FXOS_PL_HYS_7_DEG,
  FXOS_PL_HYS_11_DEG,
  FXOS_PL_HYS_14_DEG,
  FXOS_PL_HYS_17_DEG,
  FXOS_PL_HYS_21_DEG,
  FXOS_PL_HYS_24_DEG
} fxos_pl_hys_t;

// Accelerometer context data structure
typedef struct {
  /* Status and control fields of the context */
  bool                  alreadyInit;            // When the driver is already initialized
  acc_status_t          status;                 // Global status of the accelerometer driver
  uint8_t               counter;                // Counter of steps for simple state machine   

  /* Measurement results with Ping Pong buffers */
  acc_vector_t          acceleration[2];        // Vector acceleration
  acc_orientation_t     orientation[2];         // Orientation
  uint8_t               outputBufferIndex;      // Index of the output buffer being used
  bool                  updated;                // Flag used to notify a new measurement

  /* I2C message buffers */
  uint8_t               writeBuffer[FXOS8700CQ_WRITE_BUFFER_SIZE];
  uint8_t               readBuffer[FXOS8700CQ_READ_BUFFER_SIZE];
} acc_context_t;

/*********************************
* FXOS8700 REGISTERS DECLARATION *
*********************************/

typedef struct {
  uint8_t NEWLP       : 1;
  uint8_t LO          : 1;
  uint8_t RESERVED    : 3;
  uint8_t LAPO        : 2;
  uint8_t BAFRO       : 1;
} fxos_pl_status_reg_t;

typedef struct {
  uint8_t DBCNTM      : 1;
  uint8_t PLEN        : 1;
  uint8_t RESERVED    : 6;
} fxos_pl_cfg_reg_t;

typedef struct {
  uint8_t BKFR        : 2;
  uint8_t RESERVED    : 3;
  uint8_t ZLOCK       : 3;
} fxos_bf_zcomp_reg_t;

typedef struct {
  uint8_t PL_THS      : 5;
  uint8_t HYS         : 3;
} fxos_pl_ths_reg_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Periodic service routine to run the FXOS driver background services
 */
static void FXOSPeriodicISR(void);

/**
 * @brief Routine to be called when the I2C transaction has finished
 */
static void FXOSOnI2CFinished(void);

/**
 * @brief Routine to be called when an error occurs in the I2c transaction
 */
static void FXOSOnI2CError(void);

/**
 * @brief Executes the initalization sequence, cycling step by step when is called.
 * @param reset   Reset the simple state machine
 */
static void FXOSInitSequence(bool reset);

/*
 * @brief Executes the running sequence, cycling step by step when is called.
 * @param reset    Reset the simple state machine
 */
static void FXOSRunningSequence(bool reset);

/**
 * @brief Starts a reading procedure
 * @param address   Register address on the FXOS8700
 */
static void FXOSStartRead(uint8_t address);

/**
 * @brief Starts a writing procedure
 * @param address   Register address on the FXOS8700
 * @param value     Value to write
 */
static void FXOSStartWrite(uint8_t address, uint8_t value);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static acc_context_t  context;              // Accelerometer data context

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool FXOSInit(void)
{
  if (!context.alreadyInit)
  {
    // Initialize the I2C driver
    i2cMasterInit(I2C_INSTANCE_0, FXOS8700CQ_I2C_BAUD_RATE);
    i2cOnFinished(I2C_INSTANCE_0, FXOSOnI2CFinished);
    i2cOnError(I2C_INSTANCE_0, FXOSOnI2CError);
    
    // Initialize and set the periodic service routine
    timerInit();
    timerStart(timerGetId(), FXOS8700CQ_UPDATE_TICK, TIM_MODE_PERIODIC, FXOSPeriodicISR);

    // Raise the already initialized flag
    context.alreadyInit = true;
  }
  
  return context.alreadyInit;  
}

bool FXOSIsRunning(void)
{
  return (context.status == ACC_STATUS_RUNNING);
}

bool FXOSMeasurementAvailable(void)
{
  bool result = context.updated;
  if (result)
  {
    context.updated = false;
  }
  return result;
}

bool FXOSGetAcceleration(acc_vector_t* vector)
{
  bool successful = false;
  if (context.status == ACC_STATUS_RUNNING)
  {
    *vector = context.acceleration[context.outputBufferIndex];
    successful = true;
  }
  return successful;
}

bool FXOSGetOrientation(acc_orientation_t* orientation)
{
  bool successful = false;
  if (context.status == ACC_STATUS_RUNNING)
  {
    *orientation = context.orientation[context.outputBufferIndex];
    successful = true;
  }
  return successful;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void FXOSPeriodicISR(void)
{
  if (context.status == ACC_STATUS_INITIALIZATION)
  {
    FXOSInitSequence(true);
  }
  else if (context.status == ACC_STATUS_RUNNING)
  {
    FXOSRunningSequence(true);
  } 
}

static void FXOSOnI2CFinished(void)
{
  for(uint8_t i = 0 ; i < 1000 ; i++);

  if (context.status == ACC_STATUS_INITIALIZATION)
  {
    FXOSInitSequence(false);
  } 
  else if (context.status == ACC_STATUS_RUNNING)
  {
    FXOSRunningSequence(false);
  }
}

static void FXOSOnI2CError(void)
{
  context.status = ACC_STATUS_ERROR;
}

static void FXOSInitSequence(bool reset)
{
  fxos_pl_cfg_reg_t plConfig = { .PLEN = 1 };
  fxos_bf_zcomp_reg_t bfConfig = {
    .BKFR = FXOS_PL_BKFR_0,
    .ZLOCK = FXOS_PL_BF_ZLOCK_13_DEG
  };
  fxos_pl_ths_reg_t thsConfig = {
    .PL_THS = FXOS_PL_THS_15_DEG,
    .HYS    = FXOS_PL_HYS_4_DEG
  };

  if (reset)
  {
	  context.counter = 0;
  }

  switch (context.counter++)
  {
    case 0: // Read WHO_AM_I register to check device presence
      FXOSStartRead(FXOS8700CQ_WHOAMI_REG);
		  break;

    case 1: // Check WHO_AM_I value read matches
      if (context.readBuffer[0] == FXOS8700CQ_WHOAMI_VALUE)
      {
        // Set FXOS to active mode
        FXOSStartWrite(FXOS8700CQ_CTRL_REG1, 1);
		  }
      else 
      {
		    context.status = ACC_STATUS_ERROR;
		  }
      break;
    
    case 2: // Enable Portrait-Landscape detection
      FXOSStartWrite(FXOS8700CQ_PL_CFG_REG, FXOS_REG2INT(plConfig));
      break;
    
    case 3: // Configure Z Compensation register
      FXOSStartWrite(FXOS8700CQ_PL_BF_ZCOMP_REG, FXOS_REG2INT(bfConfig));
      break;
    
    case 4: // Configure PL threshold and hysteresis
      FXOSStartWrite(FXOS8700CQ_PL_THS_REG, FXOS_REG2INT(thsConfig));
      break;
    
    default:
      break;
  }
}

static void FXOSRunningSequence(bool reset)
{
  fxos_pl_status_reg_t status;
  if (reset)
  {
    acc_vector_t vector = {0, 0, 0};
    context.counter = 0;
    context.acceleration[(context.outputBufferIndex + 1) % 2] = vector;
  }

  switch (context.counter++)
  {
    case 0: // Start reading acceleration X MSB
      FXOSStartRead(FXOS8700CQ_OUT_X_MSB_REG);
      break;

    case 1: // Start reading acceleration X LSB
      context.acceleration[(context.outputBufferIndex + 1) % 2].x = (context.readBuffer[0] << 8);
      FXOSStartRead(FXOS8700CQ_OUT_X_LSB_REG);
      break;

    case 2: // Start reading acceleration Y MSB
      context.acceleration[(context.outputBufferIndex + 1) % 2].x |= (context.readBuffer[0]);
      context.acceleration[(context.outputBufferIndex + 1) % 2].x >>= 2;
      FXOSStartRead(FXOS8700CQ_OUT_Y_MSB_REG);
      break;

    case 3: // Start reading acceleration Y LSB
      context.acceleration[(context.outputBufferIndex + 1) % 2].y = (context.readBuffer[0] << 8);
      FXOSStartRead(FXOS8700CQ_OUT_Y_LSB_REG);
      break;
      
    case 4: // Start reading acceleration Z MSB
      context.acceleration[(context.outputBufferIndex + 1) % 2].y |= (context.readBuffer[0]);
      context.acceleration[(context.outputBufferIndex + 1) % 2].y >>= 2;
      FXOSStartRead(FXOS8700CQ_OUT_Z_MSB_REG);
      break;
      
    case 5: // Start reading acceleration Z LSB
      context.acceleration[(context.outputBufferIndex + 1) % 2].z = (context.readBuffer[0] << 8);
      FXOSStartRead(FXOS8700CQ_OUT_Z_LSB_REG);
      break;
    
    case 6: // Start reading the orientation
      context.acceleration[(context.outputBufferIndex + 1) % 2].z |= (context.readBuffer[0]);
      context.acceleration[(context.outputBufferIndex + 1) % 2].z >>= 2;
      FXOSStartRead(FXOS8700CQ_PL_STATUS_REG);
      break;

    case 7: // End measurement cycle...
      status = FXOS_INT2REG((context.readBuffer[0]), fxos_pl_status_reg_t);
      context.orientation[(context.outputBufferIndex + 1) % 2].landscapePortrait = status.LAPO;
      context.orientation[(context.outputBufferIndex + 1) % 2].backFront = status.BAFRO;
      context.updated = true;
      context.outputBufferIndex = (context.outputBufferIndex + 1) % 2;
      break;
      
    default:
      break;
  }
}

static void FXOSStartRead(uint8_t address)
{
  context.writeBuffer[0] = address;
  i2cStartTransaction(I2C_INSTANCE_0, FXOS8700CQ_SLAVE_ADDRESS, context.writeBuffer, 1, context.readBuffer, 1);
}

static void FXOSStartWrite(uint8_t address, uint8_t value)
{
  context.writeBuffer[0] = address;
  context.writeBuffer[1] = value;
  i2cStartTransaction(I2C_INSTANCE_0, FXOS8700CQ_SLAVE_ADDRESS, context.writeBuffer, 2, NULL, 0);
}

/*******************************************************************************
 *******************************************************************************
						  INTERRUPT SERVICE ROUTINES
 *******************************************************************************
 ******************************************************************************/

/******************************************************************************/
