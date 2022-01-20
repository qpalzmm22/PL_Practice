

#debug=-DDEBUG

all : mjyint

mjyint : 
	gcc -o mjyint mjyint.c ${debug}

clean :
	rm -rf mjyint out seeds fuzz 
