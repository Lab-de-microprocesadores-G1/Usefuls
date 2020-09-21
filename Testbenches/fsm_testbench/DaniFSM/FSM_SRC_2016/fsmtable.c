#include <stdio.h>
#include "fsm.h"
#include "fsmtable.h"
#include "contador.h"

/*Foward Declarations*/

extern STATE estado_0[];
extern STATE estado_1[];
extern STATE estado_2[];
extern STATE estado_3[];
extern STATE estado_4[];
extern STATE estado_5[];



// prototipos

static void do_nothing(void);
static void reset_FSM(void);
static void match (void);

/*** tablas de estado ***/

/*** estado_0 ***/

 STATE estado_0[]=
{
  	{'D',estado_1,do_nothing},
  	{'R',estado_1,do_nothing},
  	{FIN_TABLA,estado_0,reset_FSM}
};


/*** estado_1 ***/

STATE estado_1[]=
{
	{'a',estado_2,do_nothing},
	{FIN_TABLA,estado_0,reset_FSM}
};

/*** estado_2 ***/

STATE estado_2[] =
{
	{'n',estado_3,do_nothing},
	{FIN_TABLA,estado_0,reset_FSM}
};

/*** estado_3 ***/

STATE estado_3[] =
{
	{'i',estado_4,do_nothing},
	{FIN_TABLA,estado_0,reset_FSM}
};

/*** estado_4 ***/

STATE estado_4[] =
{	
	{'e',estado_5,do_nothing},
	{FIN_TABLA,estado_0,reset_FSM}
};

/*** estado_5 ***/

STATE estado_5[] =
{
	{'l',estado_0,match},
   	{'n',estado_0,match},
	{FIN_TABLA,estado_0,reset_FSM}
};


//========interfaz=================


STATE *FSM_GetInitState(void)
{
 	return (estado_0);
}



///=========Rutinas de accion===============

/*Incrementa el contador de coincidencias */

static void match (void)
{
 
	 inc_cont();

}

/*Dummy function*/
static void do_nothing(void)
{

}

/*Restart FSM*/
static void reset_FSM(void)
{
	printf("Reset\n");
}

