#include <stdio.h>
#include "contador.h"
                  
static unsigned int contador;

void inc_cont(void)
 {
 	contador++;
  printf("cont: %d\n" ,contador);     //just for test (debug)
 }


unsigned int leer_cont(void)
 {
	return (contador);
 
 }

void reset_contador(void)
 {
  	contador=0;

 }
