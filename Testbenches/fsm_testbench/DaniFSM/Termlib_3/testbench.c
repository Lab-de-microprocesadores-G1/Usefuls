#include <stdio.h>
#include "termlib.h"

// Test Bench for kblib //
char c; 

int main(void)
{

 changemode(BUFFERED_OFF); // Turns terminal line buffering off
 
 clrscr();
 
 puts("Test1: Testing kbhit() and getch() Press a key!");

 while(!kbhit());  // wait for a key to be pressed
 c=getch();        // Get the Key
 printf("You pressed '%c'!\n", c);
 
 puts("Test2: Testing kbhit() and getche() Press a key!");

 while(!kbhit()); // wait for a key to be pressed
 c=getche();      // Get the Key
 printf(" You pressed '%c'!\n", c);

 puts("Test3: Testing kbhit() and getchar() Press a key!");

 while(!kbhit()); // wait for a key to be pressed
 c=getchar();     // Get the Key
 printf("You pressed '%c'!\n", c);

 gotoxy(10,15);		// Col 10  Row 15
 printf(GREEN_TEXT "Test4: Testing gotoxy(10,15). Press any key! to quit\n\n" WHITE_TEXT);
 
 while(!kbhit()); // wait for a key to be pressed
 c=getchar();     // Get the Key
 
 changemode(BUFFERED_ON);  // Turns terminal line buffering on

  return 0;
}
