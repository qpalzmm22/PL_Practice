

#debug=-DDEBUG

all : LFAEDS

LFAEDS: 
	gcc -o my_lfaeds my_lfaeds.c ${debug}

clean :
	rm my_lfaeds
