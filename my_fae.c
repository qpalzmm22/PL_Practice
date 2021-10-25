/*:
 * Expressions must be enclosed with brackets whereas numbers are NOT enclosed with it.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>

#include "my_fae.h"

#define FAE_NUM_ARGS 4

#define MAX_LEN 16

#define WITH_T 6

/*
 * allocate FAE with 0 (calloc)
 */
FAE
newFAE()
{
    FAE new = (FAE)calloc(1, sizeof(FAE_t));
    assert(new);

    return new; 
}

/*
 * allocate FAE_Value with 0 (calloc)
 */
FAE_Value
newFAE_Value()
{
    FAE_Value new = (FAE_Value)calloc(1, sizeof(FAE_Value_t));
    assert(new);

    return new; 
}

/*
 * allocate DefrdSub with 0 (calloc)
 * There's default value for ds.
 */
DefrdSub
newDefrdSub()
{
    DefrdSub new = (DefrdSub)calloc(1, sizeof(DefrdSub_t));
    assert(new);

    ds->type = MTSUB_T;
    ds->value = 0x0;
    return new;
}

/*
 * Frees all the elements inside. Then frees the root.
 */
void 
freeFAE(FAE root) 
{
    if(root == 0x0)
        return;

    freeFAE(root->arg1);
    freeFAE(root->arg2);
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
 * Looks up name from ds and substitue to it if id is found
 */
FAE_Value
lookup(char name[MAX_ARG_LEN], DefrdSub ds){
    
    if (ds->type == MTSUB_T){
        
        fprintf(stderr, "Free identifier in lookup\n");
        exit(1);

    } else if(ds->type == ASUB_T){
     
        if(strcmp(ds->name, name) == 0){

            return ds->value;
            
        } else {
            
            return lookup(name, ds->ds);
        }
    
    } else {
        
        fprintf(stderr, "Wrong type in lookup\n");
        exit(1);
    
    }
}

/*
 * Needs tokens to be already allocated
 * Need concrete syntax to be in exact form( exactly one space, no errors, etc)
 * Returns number of tokens */
int
tokenize(char tokens[FAE_NUM_ARGS][MAX_ARG_LEN], char * str){

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
            if(i_token == FAE_NUM_ARGS){
                fprintf(stderr, "Invalid Syntax !! : Too many args");
                exit(1);
            }
            i_end = i - 1;
            substring(str, tokens[i_token], i_start, i_end);
            i_start = i + 1;
            i_token++;
            //printf("tokens[%d] = %s\n", i_token - 1, tokens[i_token - 1]); 
           // fae->type = N
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
 * Parses concrete syntax into FAE abstract syntax
 * It parses conc_sync by detecting "()"s and spaces.
 */
FAE 
FAEparser(char * block){
    FAE node = newFAE(); 

    int b_len = strlen(block);
    char *tmp_str = (char*) malloc(sizeof(char) * b_len);
    assert(tmp_str);

    int is_wrapped = unwrap_bracket(block, tmp_str);
    
    char tokens[FAE_NUM_ARGS][MAX_ARG_LEN];
    
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
        
        node->type = APP_T;
        
        char with_tokens[2][MAX_ARG_LEN];
        char with_tmp_str[MAX_ARG_LEN];

        unwrap_bracket(tokens[1], with_tmp_str);
        tokenize(with_tokens, with_tmp_str);

        FAE node2 = newFAE(); 
        node2->type = FUN_T;

        strcpy(node2->data, with_tokens[0]);
        node2->arg1 = FAEparser(tokens[2]);
        node2->arg2 = 0x0;

        node->arg1 = node2;
        node->arg2 = FAEparser(with_tokens[1]);
        
    
    } else if(num_tok == 3 && strcmp(tokens[0], "fun") == 0){
    
        node->type = FUN_T ;
        
        char fun_tokens[1][MAX_ARG_LEN];
        char fun_tmp_str[MAX_ARG_LEN];

        unwrap_bracket(tokens[1], fun_tmp_str);
        tokenize(fun_tokens, fun_tmp_str);

        strcpy(node->data, fun_tokens[0]);
        node->arg1 = FAEparser(tokens[2]);
        node->arg2 = 0x0;
   
    } else if(num_tok == 3 && strcmp(tokens[0],"+") == 0){
        
        node->type = ADD_T ;
        //strcpy(node->data, "add");
        node->arg1 = FAEparser(tokens[1]);
        node->arg2 = FAEparser(tokens[2]);
    
    } else if(num_tok == 3 && strcmp(tokens[0], "-") == 0) {
        
        node->type = SUB_T ;
        //strcpy(node->data, "sub");
        node->arg1 = FAEparser(tokens[1]);
        node->arg2 = FAEparser(tokens[2]);
    
    } else if(num_tok == 2) {
        
        node->type = APP_T ;
        node->arg1 = FAEparser(tokens[0]);
        node->arg2 = FAEparser(tokens[1]);

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
 * prints abstract syntax of FAE recursively
 */
void
FAEprint(FAE fae){
    if(fae == 0x0)
        return;

    switch(fae->type){

        case NUM_T:
            printf("(num %s)", fae->data);
            break;

        case ID_T:
            
            printf("(id '%s)", fae->data);
            break;
        
        case ADD_T:
            
            printf("(add ");
            FAEprint(fae->arg1);
            printf(" ");
            FAEprint(fae->arg2);
            printf(")");
            break;

        case SUB_T:
 
            printf("(sub ");
            FAEprint(fae->arg1);
            printf(" ");
            FAEprint(fae->arg2);
            printf(")");
            break;
        
        case FUN_T:
            
            printf("(fun '%s ", fae->data);
            FAEprint(fae->arg1);
            printf(")");
            break;
        
        case APP_T:
            
            printf("(app ");
            FAEprint(fae->arg1);
            printf(" ");
            FAEprint(fae->arg2);
            printf(")");

        default:
            break;
    }

}

/*
 * Calcuate addition of subtraction.
 * make new node and returns it. original ones are freed.
 *
 */
FAE_Value
calc_arith(int op, FAE lhs, FAE rhs){
    FAE_Value node = newFAE_Value();
    
    int num1 = atoi(lhs->data);
    int num2 = atoi(rhs->data);
    
    node->type == NUMV_T;

    if( op == ADD_T){
        sprintf(node->data, "%d", num1 + num2);
    } else if( op == SUB_T){
        sprintf(node->data, "%d", num1 - num2);
    } else {
        fprintf(stderr, "Error in calc...\n");
        exit(1);
    }

    node->body = 0x0;
    node->ds = 0x0;

#ifdef DEBUG
    printf("calc_arith : %s\n", node->data); 
#endif /* DEBUG */
    
    return node;
}


/*
 * Copies all the values from src to dest
*/ 
void
copy_FAE_Value(FAE_Value dest, FAE_Value src){
    
}
/*
 * Interpreter..
 *
 */
FAE_Value 
FAEinterp(FAE fae, DefrdSub ds){

#ifdef DEBUG
    printf("interp=> fae%d : %s\n",fae->type, fae->data);
#endif /* DEBUG */
    
    FAE_Value ret = newFAE_Value();
    switch( fae->type ){
        
        case NUM_T :
            
            ret->NUMV_T;
            strcpy(ret->data, fae->data);
            ret->body = 0x0;
            ret->ds = 0x0;

            break;
        
        case ID_T :
        
            ret = lookup(fae->value, ds);
            break;
        
        case ADD_T :
    
            // needs to be freed
            calc_result = calc_arith(ADD_T, FAEinterp(fae->arg1, ds), FAEinterp(fae->arg2, ds));
            
            break;
        
        case SUB_T :
        
            // needs to be freed
            ret = calc_arith(SUB_T, FAEinterp(fae->arg1, ds), FAEinterp(fae->arg2, ds));
            break;
        
        case FUN_T :

            // needs to be allocated
            ret->CLOSUREV_T;
            strcpy(ret->data, fae->data);
            ret->body = fae->arg1;
            ret->ds = ds;

            break;

        case APP_T :;

            // interp must allocate ..?
            FAE_Value f_val = FAEinterp(fae->arg1, ds);
            FAE_Value a_val = FAEinterp(fae->arg2, ds);

            FAE cV_body = f_val->body;
            char* cV_param = f_val->data;
            DefrdSub cV_ds = f_val->ds;

            DefrdSub outer_ds = newDefrdSub();
            outer_ds->type = ASUB_T;
            strcpy(outer_ds->name, cV_param);
            outer_ds->value = a_val;
            outer_ds->ds = cV_ds;

            return FAEinterp(cV_body, outer_ds);

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
    printf("\nUsuage : ./my_fae [-p] [-i] conc_syntax\n"
                    "\n"
                    "\tDefault is printing both parser and interpreter\n"
                    "\tIf you wish to see only parsed fae or interpreted fae, use following options."
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
    
    FAE root = FAEparser(argv[optind]); 
    
    DefredSub ds = newDefrdSub();

    if( p_flag){
        FAEprint(root);
        printf("\n\n");
    } 
    if( i_flag ){
        FAE result = FAEinterp(root, ds);
        FAEprint(result);
        printf("\n\n");
    }
    if( !i_flag && !p_flag ){
        FAEprint(root);
        printf("\n\n");
        FAE result = FAEinterp(root, ds);
        FAEprint(result);
        printf("\n\n");
    
    }
    return 0;
}
