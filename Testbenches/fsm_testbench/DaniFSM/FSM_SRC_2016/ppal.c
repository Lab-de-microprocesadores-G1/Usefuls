#include <stdio.h>

#include "contador.h"   /*Objeto contador usado para contar coincidencias*/ 
#include "fsm.h"	    /*FSM engine (interprete)*/
#include "fsmtable.h"   /*FSM Table*/
#include "termlib.h"    /*Enable the use of getch and getche see termlib.c*/


STATE *p2state=NULL;	/*Used to store FSM state*/

#define FIN_ARCHIVO  'Q'

int main (void)
{

    int evento;


	clrscr();		 // Clear Screen
	gotoxy(20,3);
	printf(GREEN_TEXT "FSM Test (Press 'Q' to quit) \n" WHITE_TEXT);
	


    changemode(BUFFERED_OFF); // Turns terminal line buffering off (see termlib.c)
	
	
	p2state=FSM_GetInitState();// Inicializo la FSM con el estado inicial

	while((evento = getch())!= FIN_ARCHIVO)  //Si hay evento
	
	p2state=fsm(p2state,evento);      //Se lo paso a la maquina de estados

	printf("\n\nTotal de coincidencias: %d\n\n\n", leer_cont());
  	

    changemode(BUFFERED_ON);  // Turns terminal line buffering on (see termlib.c)
}

