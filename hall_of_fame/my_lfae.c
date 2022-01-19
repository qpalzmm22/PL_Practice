/*
 * Expressions must be enclosed with brackets whereas numbers are NOT enclosed with it.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>

#include "my_lfae.h"

#define LFAE_NUM_ARGS 4


/*
 * allocate LFAE with 0 (calloc)
 */
LFAE
newLFAE()
{
    LFAE new = (LFAE)calloc(1, sizeof(LFAE_t));
    assert(new);

    new->type = NUM_T;
    new->arg1 = 0x0;
    new->arg1 = 0x0;
    
    return new; 
}

/*
 * allocate LFAE_Value with 0 (calloc)
 */
LFAE_Value
newLFAE_Value()
{
    LFAE_Value new = (LFAE_Value)calloc(1, sizeof(LFAE_Value_t));
    assert(new);
    
    new->type = NUMV_T; 
    new->body = 0x0;
    new->ds = 0x0;
    new->box = 0x0;

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

    new->type = MTSUB_T;
    new->value = 0x0;
    new->ds = 0x0;
    return new;
}

/*
 * allocate LFAE with 0 (calloc)
 */
Box
newBox()
{
    Box new = (Box)calloc(1, sizeof(Box_t));
    assert(new);
    
    new->isempty = EMPTY;
    new->value = 0x0;
    return new; 
}
/*
 * Frees all the elements inside. Then frees the root.
 */
void 
freeLFAE(LFAE node) 
{
    printf("Freeing LFAE \t\t%p\n", node);
    if(node == NULL || node->type == FREED)
        return;

    LFAE arg1 = node->arg1;
    LFAE arg2 = node->arg2;
    
    printf("Actually Freeing LFAE\n");
    
    //node->type = FREED;
    free(node);
    node = 0x0;
    
    freeLFAE(arg1);
    freeLFAE(arg2);
}

void
freeLFAE_Value(LFAE_Value node)
{
    printf("Freeing LFAE_Value \t\t%p\n", node);
    if(node == NULL || node->type == FREED)
        return ;
    
    LFAE body = node->body;
    DefrdSub ds = node->ds;
    Box box = node->box;
    
    printf("Actually Freeing LFAE_Value\n");

    node->type = FREED;
    free(node);
    node = 0x0;

    freeLFAE(body);
    freeDefrdSub(ds);
    freeBox(box);
}

void
freeDefrdSub(DefrdSub node)
{
    printf("Freeing DefrdSub \t\t%p\n",node);
    if(node == NULL || node->type == FREED)
        return ;
    
    LFAE_Value value = node->value;
    DefrdSub ds = node->ds;
    
    printf("Actually Freeing DefrdSub\n");

    node->type = FREED;
    free(node);
    node = 0x0;

    freeLFAE_Value(value);
    if(node == ds)
        freeDefrdSub(ds);
}

void
freeBox(Box node)
{
    printf("Freeing Box \t\t%p\n", node);
    if(node == NULL || node->isempty == FREED)
        return;
    
    LFAE_Value value = node->value;

    printf("Actually Freeing Box\n");
    
    node->isempty = FREED;
    free(node);
    node = 0x0;
    
    freeLFAE_Value(value);
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
tokenize(char tokens[LFAE_NUM_ARGS][MAX_ARG_LEN], char * str){

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
            if(i_token == LFAE_NUM_ARGS){
                fprintf(stderr, "Invalid Syntax !! : Too many args");
                exit(1);
            }
            i_end = i - 1;
            substring(str, tokens[i_token], i_start, i_end);
            i_start = i + 1;
            i_token++;
            //printf("tokens[%d] = %s\n", i_token - 1, tokens[i_token - 1]); 
           // lfae->type = N
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
 * Looks up name from ds and substitue to it if id is found
 */
LFAE_Value
lookup(char name[MAX_ARG_LEN], DefrdSub ds){
    
#ifdef DEBUG
    printf("\nlookup=> name[%s], ds->name[%s] \n", name, ds->name);
 //   if(ds->value)LFAE_Value_print(ds->value);
    printf("type : %d\n", ds->type);
#endif /* DEBUG */

    LFAE_Value ret;

    if (ds->type == MTSUB_T){
        
        fprintf(stderr, "Free identifier in lookup\n");
        exit(1);

    } else if(ds->type == ASUB_T){
     
        if(strcmp(ds->name, name) == 0){

            ret = strict(ds->value);
            
        } else {
            
            ret = lookup(name, ds->ds);
        }
    
    } else {
        
        fprintf(stderr, "Wrong type in lookup\n");
        exit(1);
    
    }
    return ret;
}

/*
 * Unboxes LFAE-Value
 */
LFAE_Value
strict(LFAE_Value lfae_val){
    LFAE_Value ret;

    if(lfae_val->type == EXPRV_T){
        if(lfae_val->box->isempty == EMPTY){
            
            LFAE_Value v = strict( LFAEinterp( lfae_val->body, lfae_val->ds ) );
            
            lfae_val->box->value = v;
            ret = v;
        
        }else {

            ret = lfae_val->box->value;      

        }
    } else {

        ret = lfae_val;
    
    }
    return ret; 
}

/*
 * Parses concrete syntax into LFAE abstract syntax
 * It parses conc_sync by detecting "()"s and spaces.
 */
LFAE 
LFAEparser(char * block){
    LFAE node = newLFAE(); 

    int b_len = strlen(block);
    char *tmp_str = (char*) malloc(sizeof(char) * b_len);
    assert(tmp_str);

    int is_wrapped = unwrap_bracket(block, tmp_str);
    
    char tokens[LFAE_NUM_ARGS][MAX_ARG_LEN];
    
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

        LFAE node2 = newLFAE(); 
        node2->type = FUN_T;

        strcpy(node2->data, with_tokens[0]);
        node2->arg1 = LFAEparser(tokens[2]);
        node2->arg2 = 0x0;

        node->arg1 = node2;
        node->arg2 = LFAEparser(with_tokens[1]);
        
    
    } else if(num_tok == 3 && strcmp(tokens[0], "fun") == 0){
    
        node->type = FUN_T ;
        
        char fun_tokens[1][MAX_ARG_LEN];
        char fun_tmp_str[MAX_ARG_LEN];

        unwrap_bracket(tokens[1], fun_tmp_str);
        tokenize(fun_tokens, fun_tmp_str);

        strcpy(node->data, fun_tokens[0]);
        node->arg1 = LFAEparser(tokens[2]);
        node->arg2 = 0x0;
   
    } else if(num_tok == 3 && strcmp(tokens[0],"+") == 0){
        
        node->type = ADD_T ;
        //strcpy(node->data, "add");
        node->arg1 = LFAEparser(tokens[1]);
        node->arg2 = LFAEparser(tokens[2]);
    
    } else if(num_tok == 3 && strcmp(tokens[0], "-") == 0) {
        
        node->type = SUB_T ;
        //strcpy(node->data, "sub");
        node->arg1 = LFAEparser(tokens[1]);
        node->arg2 = LFAEparser(tokens[2]);
    
    } else if(num_tok == 2) {
        
        node->type = APP_T ;
        node->arg1 = LFAEparser(tokens[0]);
        node->arg2 = LFAEparser(tokens[1]);

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
 * prints abstract syntax of LFAE recursively
 */
void
LFAEprint(LFAE lfae){
    if(lfae == 0x0)
        return;

    switch(lfae->type){

        case NUM_T:

            printf("(num %s)", lfae->data);
            break;

        case ID_T:
            
            printf("(id '%s)", lfae->data);
            break;
        
        case ADD_T:
            
            printf("(add ");
            LFAEprint(lfae->arg1);
            printf(" ");
            LFAEprint(lfae->arg2);
            printf(")");
            break;

        case SUB_T:
 
            printf("(sub ");
            LFAEprint(lfae->arg1);
            printf(" ");
            LFAEprint(lfae->arg2);
            printf(")");
            break;
        
        case FUN_T:
            
            printf("(fun '%s ", lfae->data);
            LFAEprint(lfae->arg1);
            printf(")");
            break;
        
        case APP_T:
            
            printf("(app ");
            LFAEprint(lfae->arg1);
            printf(" ");
            LFAEprint(lfae->arg2);
            printf(")");

        default:
            break;
    }

}

/*
 * prints Box recursively
 */
void
Box_print(Box box){
    
    if(box == 0x0){
        return ;
    }

    if(box->isempty == EMPTY){
        
        printf("'f");
    
    } else { // won't be executed since if it exist, that means there's an error.
        LFAE_Value_print(box->value);
    }
}
/*
 * prints DefrdSub recursively
 */
void
DefrdSub_print(DefrdSub ds){
    
    if(ds == 0x0 ){
        return ;
    }

    if(ds->type == MTSUB_T){
        printf("(mtSub)");
        return ;
    } else if(ds->type == ASUB_T) { 

        printf("(aSub %s ", ds->name);
        LFAE_Value_print(ds->value);
        printf(" ");
        DefrdSub_print(ds->ds);
    }

}
 
/*
 * prints LFAE_Value recursively
 */
void
LFAE_Value_print(LFAE_Value lfae_value){
    if(lfae_value == 0x0){
        //printf("no LFAE_val\n");
        return ;
    }
    switch(lfae_value->type){
        
        case NUMV_T :
            
            printf("(numV %s)\n", lfae_value->data);
            break;

        case CLOSUREV_T : 
            
            printf("(closureV %s ", lfae_value->data);
            LFAEprint(lfae_value->body);
            printf(" ");
            DefrdSub_print(lfae_value->ds);
            printf(")");
            break;

        case EXPRV_T : 
            
            printf("(exprV ");
            LFAEprint(lfae_value->body);
            printf(" ");
            DefrdSub_print(lfae_value->ds);
            printf(" ");
            Box_print(lfae_value->box);
            printf(")");

            break ;

        default : 
            fprintf(stderr, "Error in printing LFAE_Value\n");
            exit(1);
            break;
    }


}

/*
 * Calcuate addition of subtraction.
 * make new node and returns it. original ones are freed.
 *
 */
LFAE_Value
calc_arith(int op, LFAE_Value lhs, LFAE_Value rhs){
    LFAE_Value node = newLFAE_Value();
    
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
copy_LFAE_Value(LFAE_Value dest, LFAE_Value src){
    
}
/*
 * Interpreter..
 *
 */
LFAE_Value
LFAEinterp(LFAE lfae, DefrdSub ds){

#ifdef DEBUG
    printf("\ninterp=> lfae %d\n", lfae->type);
    printf("data : %s\n", lfae->data);
    if(lfae->arg1) printf("arg1->type : %d\n", lfae->arg1->type);
    if(lfae->arg2) printf("arg2->type : %d\n", lfae->arg2->type);
    printf("\n");
#endif /* DEBUG */
    
    LFAE_Value ret = newLFAE_Value();

    switch( lfae->type ){
        
        case NUM_T :
            
            ret->type = NUMV_T;
            strcpy(ret->data, lfae->data);
            ret->body = 0x0;
            ret->ds = 0x0;

            break;
        
        case ADD_T :
            
            free(ret);
            ret = calc_arith(ADD_T, LFAEinterp(lfae->arg1, ds), LFAEinterp(lfae->arg2, ds));
            
            break;
        
        case SUB_T :

            free(ret);
            ret = calc_arith(SUB_T, LFAEinterp(lfae->arg1, ds), LFAEinterp(lfae->arg2, ds));
            break;
        
        case ID_T :
        
            ret = lookup(lfae->data, ds); // ds->LFAE_Value
            break;
        
        case FUN_T :

            ret->type = CLOSUREV_T;
            strcpy(ret->data, lfae->data);
            ret->body = lfae->arg1;
            ret->ds = ds;

            break;

        case APP_T :;

            // interp must allocate ..?
            LFAE_Value f_val = strict(LFAEinterp(lfae->arg1, ds));
            LFAE_Value a_val = newLFAE_Value();

            a_val->body = lfae->arg2;
            a_val->type = EXPRV_T ;
            a_val->ds = ds;
            a_val->box = newBox();
            a_val->box->isempty = EMPTY;
            //LFAE a_val = LFAEinterp(lfae->arg2, ds);

            LFAE cV_body = f_val->body;
            char* cV_param = f_val->data;
            DefrdSub cV_ds = f_val->ds;

            DefrdSub outer_ds = newDefrdSub();
     
            outer_ds->type = ASUB_T;
            strcpy(outer_ds->name, cV_param);
            outer_ds->value = a_val;
            outer_ds->ds = cV_ds;
            
            free(ret);

            ret = LFAEinterp(cV_body, outer_ds);


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
    printf("\nUsuage : ./my_lfae [-p] [-i] conc_syntax\n"
                    "\n"
                    "\tDefault is printing both parser and interpreter\n"
                    "\tIf you wish to see only parsed lfae or interpreted lfae, use following options.\n"
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
                exit(0);
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
    
    LFAE root = LFAEparser(argv[optind]); 
    
    DefrdSub ds = newDefrdSub();

    if( p_flag){

        LFAEprint(root);
        printf("\n\n");

    } 
    if( i_flag ){

        LFAE_Value result = LFAEinterp(root, ds);
        LFAE_Value_print(result);
        printf("\n\n");
    
        freeLFAE_Value(result);

    }
    if( !i_flag && !p_flag ){

        LFAEprint(root);
        printf("\n\n");
        LFAE_Value result = LFAEinterp(root, ds);
        LFAE_Value_print(result);
        printf("\n\n");
        
       freeLFAE_Value(result);

    }

    //free(ds);
    //freeLFAE(root);

    return 0;
}
