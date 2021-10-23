

#debug=-DDEBUG

all : WAE

AE:
	gcc -o my_ae my_ae.c ${debug}

WAE: 
	gcc -o my_wae my_wae.c ${debug}
