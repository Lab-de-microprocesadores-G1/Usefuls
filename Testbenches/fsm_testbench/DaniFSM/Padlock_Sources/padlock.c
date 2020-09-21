#include <stdio.h>

#define FALSE 0
#define TRUE  1
unsigned int Vino_25=FALSE;
unsigned int Vino_10=FALSE;
unsigned int Vino_39=FALSE;
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
		

		if ( !Vino_25 && !Vino_10 && !Vino_39 )
		{
            if(Event==25)
				Vino_25=TRUE; 
            /*else
            {
				Vino_10=FALSE;
				Vino_39=FALSE;
			
			}	     
            */     
                 
		} 
		else if (Vino_25 && !Vino_10 && !Vino_39)
		{
			if(Event==10)
				Vino_10=TRUE; 
			else
            {
				Vino_25=FALSE;
							
			}	  
 	
		}	
		else if (Vino_25 && Vino_10 && !Vino_39)
		{
			if(Event==39)
				Vino_39=TRUE; 
				
			else
            {
				Vino_10=FALSE;
				Vino_25=FALSE;
			
			}	  
			
		}
		
		if (Vino_25 && Vino_10 && Vino_39)  // Moore Machine Style
		{
				
				
				Candado_Abierto=TRUE;		//Output
			
		}
		
		// show system progress
		printf("Vino_25=%d Vino_10=%d Vino_39=%d Candado_Abierto=%d \n",\
		Vino_25,Vino_10,Vino_39,Candado_Abierto);
		
		if(Candado_Abierto==TRUE) // Reset 
		{
		
				Vino_25=FALSE;
				Vino_10=FALSE;
				Vino_39=FALSE; 
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
