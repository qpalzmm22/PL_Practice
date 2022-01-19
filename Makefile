

#debug=-DDEBUG

all : mjyint

mjyint : 
	gcc -o mjyint mjyint.c ${debug}

clean :
	rm mjyint
