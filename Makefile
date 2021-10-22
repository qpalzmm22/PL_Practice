parser_c=ae_parser.c
interp_c=ae_interp.c

all : parser interpreter

parser:
	gcc -o my_parser $(parser_c)

interpreter:
	gcc -o my_interp $(interp_c) $(parser_c)

