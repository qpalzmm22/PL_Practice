

#debug=-DDEBUG

all : mjyint

mjyint : 
	gcc -o mjyint mjyint.c ${debug}

fuzz : 
	gcc -o fuzz_mjyint fuzz_mjyint.c

clean :
	rm -rf mjyint out seeds fuzz 
