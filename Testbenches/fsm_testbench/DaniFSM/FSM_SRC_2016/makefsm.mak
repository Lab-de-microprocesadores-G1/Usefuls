ppal:ppal.o contador.o fsm.o fsmtable.o termlib.o
	gcc ppal.o contador.o fsm.o fsmtable.o termlib.o -o ppal

#  la opción -c no genera el ejecutable, sino el código objeto
#  (compilo No link-edito )
		
ppal.o: ppal.c 
	gcc -c ppal.c

contador.o: contador.c contador.h
	gcc -c contador.c

fsm.o: fsm.c fsm.h
	gcc -c fsm.c

fsmtable.o: fsmtable.c fsmtable.h
	gcc -c fsmtable.c

termlib.o: termlib.c termlib.h
	gcc -c termlib.c


