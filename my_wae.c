/*:
 * Expressions must be enclosed with brackets whereas numbers are NOT enclosed with it.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>


#define WAE_NUM_ARGS 4

#define MAX_ARG_LEN 256
#define MAX_LEN 16

#define NUM_T 0
#define WITH_T 1
#define ADD_T 2
#define SUB_T 3
#define ID_T 4


typedef struct _WAE_t{
    int type; // 0 : num, 1 : with, 2 : add, 3: sub, 4: id
    char data[MAX_ARG_LEN];
    struct _WAE_t * arg1; // value lhs
    struct _WAE_t * arg2; // expr, rhs
}WAE_t, *WAE;

/*
 * allocate WAE with 0 (calloc)
 */
WAE
newWAE()
{
    WAE new = (WAE)calloc(1, sizeof(WAE_t));
    assert(new);

    return new; 
}

/*
 * Frees all the elements inside. Then frees the root.
 */
void 
freeWAE(WAE root) 
{
    if(root == 0x0)
        return;

    freeWAE(root->arg1);
    freeWAE(root->arg2);
    free(root);
}

/*
 * check if a block starts with "(" and end with ")"
 */
int 
check_bracket(char * block, int b_len){
    if(*block == '{' && *(block + b_len - 1) == '}')
        return 1;
    else 
        return 0;
}

/*
 * Gets the subsring of i_begin to i_end of str and return it to ret_str
 * i_begin must be allocated already.
 */
int
substring(char *in_str, char *out_str,  int i_begin, int i_end){
    
    if( !in_str || !out_str || i_begin > i_end){
        return -1; // Wrong input
    }

    int cnt = 0;

    for(int i = i_begin; i <= i_end ; i++){
        out_str[cnt] = in_str[i]; 
        cnt++;
    }
    out_str[cnt] = 0x0;
    
    return cnt;
}

/*
 * Needs tokens to be already allocated
 * Need concrete syntax to be in exact form( exactly one space, no errors, etc)
 * Returns number of tokens */
int
tokenize(char tokens[WAE_NUM_ARGS][MAX_ARG_LEN], char * str){

    int len = strlen(str);
    int b_stack = 0;
    int i_start = 0;
    int i_end = 0;
    int i_token = 0;

    for(int i = 0; i <= len; i++){
        if(str[i] == '{')
            b_stack++;
        if(str[i] == '}')
            b_stack--;

        if((b_stack == 0 && str[i] == ' ') || str[i] == 0x0){
            if(i_token == WAE_NUM_ARGS){
                fprintf(stderr, "Invalid Syntax !! : Too many args");
                exit(1);
            }
            i_end = i - 1;
            substring(str, tokens[i_token], i_start, i_end);
            i_start = i + 1;
            i_token++;
            //printf("tokens[%d] = %s\n", i_token - 1, tokens[i_token - 1]); 
           // wae->type = N
        }
    }
    return i_token;
}

/*
 * Unwraps bracket if it does contain it.
 * returns 1 if it did indeed was enclosed with bracket. If not, it returns 0
 * caller must free char * after.
 */
int
unwrap_bracket(char *prev, char *after){
    
    int b_len = strlen(prev);

    int is_wrapped = check_bracket(prev, b_len);
    
    if(is_wrapped){
    
        substring(prev, after, 1, (b_len - 2));
    
    } else {

        strcpy(after, prev);
    
    }
    return is_wrapped;
}

/*
 * Check if str is all digits.
 * If it is, returns 1
 * It not, returns 0.
 *
 */
int aredigit(char *str){
    for(int i = 0; *(str +i) != 0x0; i++){
        if(!isdigit(str[i])) return 0;
    }
    return 1;
}
/*
 * Parses concrete syntax into WAE abstract syntax
 * It parses conc_sync by detecting "()"s and spaces.
 */
WAE 
WAEparser(char * block){
    WAE node = newWAE(); 

    int b_len = strlen(block);
    char *tmp_str = (char*) malloc(sizeof(char) * b_len);
    assert(tmp_str);

    int is_wrapped = unwrap_bracket(block, tmp_str);
    
    char tokens[WAE_NUM_ARGS][MAX_ARG_LEN];
    
    int num_tok = tokenize(tokens, tmp_str);
    
    if(num_tok == 1 && aredigit(tokens[0])){ // num
    
        node->type = NUM_T ;
        strcpy(node->data, tokens[0]);
        node->arg1 = 0x0;
        node->arg2 = 0x0;
    
    } else if(num_tok == 1){ // id

        node->type = ID_T ;
        strcpy(node->data, tokens[0]);
        node->arg1 = 0x0;
        node->arg2 = 0x0;

    } else if(num_tok == 3 && strcmp(tokens[0], "with") == 0){
        // calc token nums
        
        node->type = WITH_T;
        
        char with_tokens[2][MAX_ARG_LEN];
        char with_tmp_str[MAX_ARG_LEN];

        unwrap_bracket(tokens[1], with_tmp_str);
        tokenize(with_tokens, with_tmp_str);

        strcpy(node->data, with_tokens[0]);
        node->arg1 = WAEparser(with_tokens[1]);
        node->arg2 = WAEparser(tokens[2]);
    
    } else if(num_tok == 3 && strcmp(tokens[0],"+") == 0){
        
        node->type = ADD_T ;
        //strcpy(node->data, "add");
        node->arg1 = WAEparser(tokens[1]);
        node->arg2 = WAEparser(tokens[2]);
    
    } else if(num_tok == 3 && strcmp(tokens[0], "-") == 0) {
        
        node->type = SUB_T ;
        //strcpy(node->data, "sub");
        node->arg1 = WAEparser(tokens[1]);
        node->arg2 = WAEparser(tokens[2]);
    
    } else {
        fprintf(stderr, "Invalid Syntax\n");
        exit(1);
    }
    //printf("tmp_str: %s\n", tmp_str);
    free(tmp_str);

    return node;
}

/*
 * TODO
 * prints abstract syntax of WAE recursively
 */
void
WAEprint(WAE wae){
    if(wae == 0x0)
        return;

    switch(wae->type){

        case NUM_T:
            printf("(num %s)", wae->data);
            break;

        case ID_T:
            
            printf("(id '%s)", wae->data);
            break;
        
        case ADD_T:
            
            printf("(add ");
            WAEprint(wae->arg1);
            printf(" ");
            WAEprint(wae->arg2);
            printf(")");
            break;

        case SUB_T:
 
            printf("(sub ");
            WAEprint(wae->arg1);
            printf(" ");
            WAEprint(wae->arg2);
            printf(")");
            break;
        
        case WITH_T:
            
            printf("(with %s ", wae->data);
            WAEprint(wae->arg1);
            printf(" ");
            WAEprint(wae->arg2);
            printf(")");
            break;

        default:
            break;
    }

}

/*
 * Calcuate addition of subtraction.
 * make new node and returns it. original ones are freed.
 *
 */
WAE 
calc_arith(int op, WAE lhs, WAE rhs){
    WAE node = newWAE();
    
    int num1 = atoi(lhs->data);
    int num2 = atoi(rhs->data);
    
    node->type == NUM_T;

    if( op == ADD_T){
        sprintf(node->data, "%d", num1 + num2);
    } else if( op == SUB_T){
        sprintf(node->data, "%d", num1 - num2);
    } else {
        fprintf(stderr, "Error in calc...\n");
        exit(1);
    }

#ifdef DEBUG
    printf("calc_arith : %s\n", node->data); 
#endif /* DEBUG */
    
    
    return node;
}




WAE
subst(WAE wae, char idtf[MAX_ARG_LEN], WAE val){

    WAE node = newWAE();

#ifdef DEBUG
    printf("subst=> wae%d : %s, val%d : %s\n", wae->type, wae->data, val->type,val->data);
#endif /* DEBUG */

    switch(wae->type){
        case NUM_T:
            
            node->type = NUM_T;
            strcpy(node->data, wae->data);
            node->arg1 = 0x0;
            node->arg2 = 0x0;
            break;

        case ADD_T:

            node->type = ADD_T;
            node->arg1 = subst(wae->arg1, idtf, val);
            node->arg2 = subst(wae->arg2, idtf, val);
            break;

        case SUB_T:
            
            node->type = SUB_T;
            node->arg1 = subst(wae->arg1, idtf, val);
            node->arg2 = subst(wae->arg2, idtf, val);
            break;

        case WITH_T:
            
            node->type = WITH_T;
            strcpy(node->data, wae->data);
            node->arg1 = subst(wae->arg1, idtf, val);
            node->arg2 = (strcmp(idtf, wae->data) == 0)? wae->arg2 : subst(wae->arg2, idtf, val);
            break;

        case ID_T:

            if(strcmp(idtf, wae->data) == 0){ // val is already interpreted
                
                node->type = NUM_T;
                strcpy(node->data, val->data);
                node->arg1 = 0x0;
                node->arg2 = 0x0;
            
            } else{
                
                node->type = ID_T;
                strcpy(node->data, wae->data);
                node->arg1 = wae->arg1;
                node->arg2 = wae->arg2;
            
            }

            break;

        default:
            fprintf(stderr, "Invalid Abstract Syntax !!\n");
            exit(1);
            break;
    }
    //free(wae);
    //free(val);
    return node;
}

/*
 * Interpreter..
 *
 */
WAE 
WAEinterp(WAE wae){

#ifdef DEBUG
    printf("interp=> wae%d : %s\n",wae->type, wae->data);
#endif /* DEBUG */
    
    WAE ret;
    switch( wae->type ){
        
        case NUM_T :
         
            ret = wae;
            break;
        
        case ID_T :
        
            fprintf(stderr, "Free variable !!\n");
            exit(1);
            break;
        
        case ADD_T :
            
            ret = calc_arith(ADD_T, WAEinterp(wae->arg1), WAEinterp(wae->arg2));
            break;
        
        case SUB_T :
        
            ret = calc_arith(SUB_T, WAEinterp(wae->arg1), WAEinterp(wae->arg2));
            break;
        
        case WITH_T :

            ret = WAEinterp( subst(wae->arg2, wae->data, WAEinterp(wae->arg1)));
            break;
        
        default:
            fprintf(stderr, "no operation found\n");
            exit(1);
            break;
    }
    return ret;
}

void
printHelp(){
    printf("\nUsuage : ./my_wae [-p] [-i] conc_syntax\n"
                    "\n"
                    "\tDefault is printing both parser and interpreter\n"
                    "\tIf you wish to see only parsed wae or interpreted wae, use following options."
                    "\n"
                    "\t [-p] : only parser option\n"
                    "\t\tprints only abstract syntax of parser\n"
                    "\n"
                    "\t [-i] : only interprepter option\n"
                    "\t\tprints only result of interpreting it\n"
                    "\n");
}

int main(int argc, char ** argv){
    
    int p_flag = 0;
    int i_flag = 0;

    int opt;
    while((opt = getopt(argc, argv, "phi")) != -1){
        switch(opt){
            case 'p':
                p_flag = 1;
                break;
            case 'i':
                i_flag = 1;
                break;
            case 'h':
                printHelp();
                break;
            default : /* ? */
                printHelp();
                exit(1);
         }
    }
    if (optind >= argc){
        fprintf(stderr, "Expects concrete syntax after options\n");
        exit(1);
    }
    
    WAE root = WAEparser(argv[optind]); 
    WAE result = WAEinterp(root);

    if( p_flag){
        WAEprint(root);
        printf("\n\n");
    } 
    if( i_flag ){
        WAEprint(result);
        printf("\n\n");
    }
    if( !i_flag && !p_flag ){
        WAEprint(root);
        printf("\n\n");
        WAEprint(result);
        printf("\n\n");
    
    }
    return 0;
}

