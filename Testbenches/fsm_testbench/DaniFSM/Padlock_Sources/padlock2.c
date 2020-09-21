#include <stdio.h>

#define FALSE 0
#define TRUE  1

enum Type_State {
	
Waiting_for_25,			// Reset State
Waiting_for_10,
Waiting_for_39

};

enum Type_State Estado=Waiting_for_25; // Initial (Reset) State

unsigned int Candado_Abierto=FALSE;
	
#define BUF_LEN 4	

unsigned int Get_Number(void);


void main (void)
{
	unsigned char Event;
	while (1)
	{
		Event=Get_Number();
		if(Event)
		printf("Event:%d \n",Event);
		
		switch(Estado)
		{
			case Waiting_for_25:
		            if(Event==25)
						Estado=Waiting_for_10;  // Right Input --> Next State
                          
        	break;
        	
			case Waiting_for_10:
		
					if(Event==10)
						Estado=Waiting_for_39;  // Right Input --> Next State
					else
						Estado=Waiting_for_25; // Wrong input Reset FSM
						  
			break;
			
			case Waiting_for_39:
			
					if(Event==39)
					{
						Candado_Abierto=TRUE;		// Open padlock and 
						Estado=Waiting_for_25;		// Reset FSM
					}
					else
						Estado=Waiting_for_25;		// Wrong input Reset FSM
			break;	 	
		
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
	scanf("%3[^\n]",name);		// Use regex :) i.e Any char but '\n' and no more than 3
	getchar();					// get rid of \n from input buffer
	
	
	//printf("Name:%s\n",name);
	
	return(atoi(name));			// atoi filters out invalid values by returning 0
	
	
}


