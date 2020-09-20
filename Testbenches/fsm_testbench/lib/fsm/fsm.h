/***************************************************************************//**
  @file     fsm.h
  @brief    FSM Library
  @author   Nicolás Trozzo
 ******************************************************************************/

#ifndef FSM_FSM_H_
#define FSM_FSM_H

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

typedef uint8_t fsmEvent_t;

typedef struct fsmState fsmState_t;

typedef struct {
	fsmEvent_t ev;
	void (*action) (void);
	fsmState_t nextState;
} fsmTransition_t;

typedef fsmState_t	(fsmTransition_t[]);

typedef fsmTable	fsmState_t[];

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

#endif /* FSM_FSM_H_ */
