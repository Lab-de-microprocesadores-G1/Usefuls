#include <stdio.h>

#define FALSE 0
#define TRUE  1

enum Type_State {
	
Waiting_for_25,			// Reset State
Waiting_for_10,
Waiting_for_39

};

// ------------ Initial System State --------------//
enum Type_State Estado=Waiting_for_25;
unsigned int Candado_Abierto=FALSE;


// -----------  Prototypes
unsigned int Get_Number(void);


void main (void)
{
	unsigned char Event;
	while (1)
	{
		Event=Get_Number();
		if(Event)						// Check for Valid input
		printf("Event:%d \n",Event);	// And show it
		

		if (Estado==Waiting_for_25)
		{
            if(Event==25)
				Estado=Waiting_for_10; 
                          
                 
		} 
		else if (Estado==Waiting_for_10)
		{
			if(Event==10)
				Estado=Waiting_for_39; 
			else
            {
				Estado=Waiting_for_25;
							
			}	  
 	
		}	
		else if (Estado==Waiting_for_39)
		{
			if(Event==39)
			{
				Estado=Waiting_for_25; 
				Candado_Abierto=TRUE;		//Unlock Padlock
			}	
			else
            {
				Estado=Waiting_for_25;		//Wrong input Reset FSM
							
			}	  
 	
		}
				
		// show system progress
		printf("Estado=%d Candado_Abierto=%d \n",Estado,Candado_Abierto);
		
		if(Candado_Abierto==TRUE) // Reset
		{
				
			Candado_Abierto=FALSE;
		
		}	
			
	}	
	
}



unsigned int Get_Number(void)
{
	char name[4];
	scanf("%3[^\n]",name);		// Use regex :)
	getchar();					// get rid of /n from input buffer
	
	
	//printf("Name:%s\n",name);
	
	return(atoi(name));			// atoi filters out invalid values by returning 0
	
	
}
