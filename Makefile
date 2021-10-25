

#debug=-DDEBUG

all : FAE

AE:
	gcc -o my_ae my_ae.c ${debug}

WAE: 
	gcc -o my_wae my_wae.c ${debug}

FAE: 
	gcc -o my_fae my_fae.c ${debug}

