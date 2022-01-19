/*
 * Expressions must be enclosed with brackets whereas numbers are NOT enclosed with it.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>

#include "mjyint.h"

#define LFAEDS_NUM_ARGS 4
#define MAXSTR 1024

/*
 * allocate LFAEDS with 0 (calloc)
 */
LFAEDS
newLFAEDS()
{
    LFAEDS new = (LFAEDS)calloc(1, sizeof(LFAEDS_t));
    assert(new);

    new->type = NUM_T;
    new->arg1 = 0x0;
    new->arg1 = 0x0;
    
    return new; 
}

/*
 * allocate LFAEDS_Value with 0 (calloc)
 */
LFAEDS_Value
newLFAEDS_Value()
{
    LFAEDS_Value new = (LFAEDS_Value)calloc(1, sizeof(LFAEDS_Value_t));
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
 * allocate LFAEDS with 0 (calloc)
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
freeLFAEDS(LFAEDS node) 
{
    if(node == NULL || node->type == FREED)
        return;

    LFAEDS arg1 = node->arg1;
    LFAEDS arg2 = node->arg2;
    
    
    node->type = FREED;
    free(node);
    node = 0x0;
    
    freeLFAEDS(arg1);
    freeLFAEDS(arg2);
}

void
freeLFAEDS_Value(LFAEDS_Value node)
{
    if(node == NULL || node->type == FREED)
        return ;
    
    LFAEDS body = node->body;
    DefrdSub ds = node->ds;
    Box box = node->box;
    
    node->type = FREED;
    free(node);
    node = 0x0;

    freeLFAEDS(body);
    freeDefrdSub(ds);
    freeBox(box);
}

void
freeDefrdSub(DefrdSub node)
{
    if(node == NULL || node->type == FREED)
        return ;
    
    LFAEDS_Value value = node->value;
    DefrdSub ds = node->ds;
    

    node->type = FREED;
    free(node);
    node = 0x0;

    freeLFAEDS_Value(value);
    if(node == ds)
        freeDefrdSub(ds);
}

void
freeBox(Box node)
{
    if(node == NULL || node->isempty == FREED)
        return;
    
    LFAEDS_Value value = node->value;

    node->isempty = FREED;
    free(node);
    node = 0x0;
    
    freeLFAEDS_Value(value);
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
tokenize(char tokens[LFAEDS_NUM_ARGS][MAX_ARG_LEN], char * str){

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
            if(i_token == LFAEDS_NUM_ARGS){
                fprintf(stderr, "Invalid Syntax !! : Too many args");
                exit(0);
            }
            i_end = i - 1;
            substring(str, tokens[i_token], i_start, i_end);
            i_start = i + 1;
            i_token++;
            //printf("tokens[%d] = %s\n", i_token - 1, tokens[i_token - 1]); 
           // lfaeds->type = N
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
LFAEDS_Value
lookup(char name[MAX_ARG_LEN], DefrdSub ds){
    
#ifdef DEBUG
    printf("\nlookup=> name[%s], ds->name[%s] \n", name, ds->name);
 //   if(ds->value)LFAEDS_Value_print(ds->value);
    printf("type : %d\n", ds->type);
#endif /* DEBUG */

    LFAEDS_Value ret;

    if (ds->type == MTSUB_T){
        
        fprintf(stderr, "Free identifier in lookup\n");
        exit(0);

    } else if(ds->type == ASUB_T){
     
        if(strcmp(ds->name, name) == 0){

            ret = strict(ds->value);
            
        } else {
            
            ret = lookup(name, ds->ds);
        }
    
    } else {
        
        fprintf(stderr, "Wrong type in lookup\n");
        exit(0);
    
    }
    return ret;
}

/*
 * Unboxes LFAEDS-Value
 */
LFAEDS_Value
strict(LFAEDS_Value lfaeds_val){
    LFAEDS_Value ret;

    if(lfaeds_val->type == EXPRV_T){
        if(lfaeds_val->box->isempty == EMPTY){
            
            LFAEDS_Value v = strict( LFAEDSinterp( lfaeds_val->body, lfaeds_val->ds ) );
            
            lfaeds_val->box->value = v;
            ret = v;
        
        }else {

            ret = lfaeds_val->box->value;      
            lfaeds_val->box->isempty = EXPRV_T ;

        }
    } else {

        ret = lfaeds_val;
    
    }
    return ret; 
}

/*
 * Parses concrete syntax into LFAEDS abstract syntax
 * It parses conc_sync by detecting "()"s and spaces.
 */
LFAEDS 
LFAEDSparser(char * block){
    LFAEDS node = newLFAEDS(); 

    int b_len = strlen(block);
    char *tmp_str = (char*) malloc(sizeof(char) * b_len);
    assert(tmp_str);

    int is_wrapped = unwrap_bracket(block, tmp_str);
    
    char tokens[LFAEDS_NUM_ARGS][MAX_ARG_LEN];
    
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

        LFAEDS node2 = newLFAEDS(); 
        node2->type = FUN_T;

        strcpy(node2->data, with_tokens[0]);
        node2->arg1 = LFAEDSparser(tokens[2]);
        node2->arg2 = 0x0;

        node->arg1 = node2;
        node->arg2 = LFAEDSparser(with_tokens[1]);
        
    
    } else if(num_tok == 3 && strcmp(tokens[0], "fun") == 0){
    
        node->type = FUN_T ;
        
        char fun_tokens[1][MAX_ARG_LEN];
        char fun_tmp_str[MAX_ARG_LEN];

        unwrap_bracket(tokens[1], fun_tmp_str);
        tokenize(fun_tokens, fun_tmp_str);

        strcpy(node->data, fun_tokens[0]);
        node->arg1 = LFAEDSparser(tokens[2]);
        node->arg2 = 0x0;
   
    } else if(num_tok == 3 && strcmp(tokens[0], "dsfun") == 0){
    
        node->type = DSFUN_T ;
        
        char dsfun_tokens[1][MAX_ARG_LEN];
        char dsfun_tmp_str[MAX_ARG_LEN];

        unwrap_bracket(tokens[1], dsfun_tmp_str);
        tokenize(dsfun_tokens, dsfun_tmp_str);

        strcpy(node->data, dsfun_tokens[0]);
        node->arg1 = LFAEDSparser(tokens[2]);
        node->arg2 = 0x0;
   
    } else if(num_tok == 3 && strcmp(tokens[0],"+") == 0){
        
        node->type = ADD_T ;
        //strcpy(node->data, "add");
        node->arg1 = LFAEDSparser(tokens[1]);
        node->arg2 = LFAEDSparser(tokens[2]);
    
    } else if(num_tok == 3 && strcmp(tokens[0], "-") == 0) {
        
        node->type = SUB_T ;
        //strcpy(node->data, "sub");
        node->arg1 = LFAEDSparser(tokens[1]);
        node->arg2 = LFAEDSparser(tokens[2]);
    
    } else if(num_tok == 2) {
        
        node->type = APP_T ;
        node->arg1 = LFAEDSparser(tokens[0]);
        node->arg2 = LFAEDSparser(tokens[1]);

    } else {

        fprintf(stderr, "Invalid Syntax\n");
		exit(0);
    }
    //printf("tmp_str: %s\n", tmp_str);
    free(tmp_str);

    return node;
}

/*
 * TODO
 * prints abstract syntax of LFAEDS recursively
 */
void
LFAEDSprint(LFAEDS lfaeds){
    if(lfaeds == 0x0)
        return;

    switch(lfaeds->type){

        case NUM_T:

            printf("(num %s)", lfaeds->data);
            break;

        case ID_T:
            
            printf("(id '%s)", lfaeds->data);
            break;
        
        case ADD_T:
            
            printf("(add ");
            LFAEDSprint(lfaeds->arg1);
            printf(" ");
            LFAEDSprint(lfaeds->arg2);
            printf(")");
            break;

        case SUB_T:
 
            printf("(sub ");
            LFAEDSprint(lfaeds->arg1);
            printf(" ");
            LFAEDSprint(lfaeds->arg2);
            printf(")");
            break;
        
        case FUN_T:
            
            printf("(fun '%s ", lfaeds->data);
            LFAEDSprint(lfaeds->arg1);
            printf(")");
            break;
        
        case APP_T:
            
            printf("(app ");
            LFAEDSprint(lfaeds->arg1);
            printf(" ");
            LFAEDSprint(lfaeds->arg2);
            printf(")");
            break;

        case DSFUN_T:
            
            printf("(dsfun '%s ", lfaeds->data);
            LFAEDSprint(lfaeds->arg1);
            printf(")");
            break;

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
        LFAEDS_Value_print(box->value);
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
        LFAEDS_Value_print(ds->value);
        printf(" ");
        DefrdSub_print(ds->ds);
    }

}
 
/*
 * prints LFAEDS_Value recursively
 */
void
LFAEDS_Value_print(LFAEDS_Value lfaeds_value){
    if(lfaeds_value == 0x0){
        //printf("no LFAEDS_val\n");
        return ;
    }
    switch(lfaeds_value->type){
        
        case NUMV_T :
            
            printf("(numV %s)\n", lfaeds_value->data);
            break;

        case CLOSUREV_T : 
            
            printf("(closureV %s ", lfaeds_value->data);
            LFAEDSprint(lfaeds_value->body);
            printf(" ");
            DefrdSub_print(lfaeds_value->ds);
            printf(")");
            break;

        case EXPRV_T : 
            
            printf("(exprV ");
            LFAEDSprint(lfaeds_value->body);
            printf(" ");
            DefrdSub_print(lfaeds_value->ds);
            printf(" ");
            Box_print(lfaeds_value->box);
            printf(")");

            break ;

        case DSCLOSUREV_T : 
            
            printf("(dsclosureV %s ", lfaeds_value->data);
            LFAEDSprint(lfaeds_value->body);
            printf(")");

        default : 
            fprintf(stderr, "Error in printing LFAEDS_Value\n");
			exit(0);
    }


}

/*
 * Calcuate addition of subtraction.
 * make new node and returns it. original ones are freed.
 *
 */
LFAEDS_Value
calc_arith(int op, LFAEDS_Value lhs, LFAEDS_Value rhs){
    LFAEDS_Value node = newLFAEDS_Value();
    
    int num1 = atoi(lhs->data);
    int num2 = atoi(rhs->data);

    node->type = NUMV_T;

    if( op == ADD_T){
        sprintf(node->data, "%d", num1 + num2);
    } else if( op == SUB_T){
        sprintf(node->data, "%d", num1 - num2);
    } else {
		fprintf(stderr, "Error in calc...\n");
		exit(0);
    }

    node->body = 0x0;
    node->ds = 0x0;
    node->box = 0x0;

#ifdef DEBUG
    printf("calc_arith : %s\n", node->data); 
#endif /* DEBUG */
   

    return node;
}


/*
 * Copies all the values from src to dest
*/ 
void
copy_DefrdSub(DefrdSub dest, DefrdSub src){

    if(src == 0x0){
        return;
    }

    DefrdSub ds = newDefrdSub();

    dest->type = src->type;
    strcpy(dest->name, src->name);
    dest->value = src->value;
    copy_DefrdSub(dest->ds, src->ds);

    

}
/*
 * Interpreter..
 *
 */
LFAEDS_Value
LFAEDSinterp(LFAEDS lfaeds, DefrdSub ds){

#ifdef DEBUG
    printf("\ninterp=> lfaeds %d\n", lfaeds->type);
    printf("data : %s\n", lfaeds->data);
    printf("ds-type : %d =>  %s\n", ds->type, ds->name);
    if(lfaeds->arg1) printf("arg1->type : %d\n", lfaeds->arg1->type);
    if(lfaeds->arg2) printf("arg2->type : %d\n", lfaeds->arg2->type);
#endif /* DEBUG */
    
    LFAEDS_Value ret = newLFAEDS_Value();

    switch( lfaeds->type ){
        case NUM_T :
            
            ret->type = NUMV_T;
            strcpy(ret->data, lfaeds->data);
            ret->body = 0x0;
            ret->ds = ds;

            break;
        
        case ADD_T :
            
            free(ret);
            ret = calc_arith(ADD_T, LFAEDSinterp(lfaeds->arg1, ds), LFAEDSinterp(lfaeds->arg2, ds));
            
            break;
        
        case SUB_T :

            free(ret);
            ret = calc_arith(SUB_T, LFAEDSinterp(lfaeds->arg1, ds), LFAEDSinterp(lfaeds->arg2, ds));
            break;
        
        case ID_T :
        
            ret = lookup(lfaeds->data, ds); // ds->LFAEDS_Value
            break;
        
        case FUN_T :

            ret->type = CLOSUREV_T;
            strcpy(ret->data, lfaeds->data);
            ret->body = lfaeds->arg1;
            ret->ds = ds;

            break;

        case DSFUN_T :

            ret->type = DSCLOSUREV_T;
            strcpy(ret->data, lfaeds->data);
            ret->body = lfaeds->arg1;
            ret->ds = ds;
            break;

        case APP_T :;


            // interp must allocate ..?
            LFAEDS_Value f_val = strict(LFAEDSinterp(lfaeds->arg1, ds));
            LFAEDS_Value a_val = newLFAEDS_Value();
            
            
            a_val->type = EXPRV_T ;
            a_val->body = lfaeds->arg2;
            a_val->ds = ds;
            a_val->box = newBox();
            a_val->box->isempty = EMPTY;

            LFAEDS cV_body = f_val->body;
            char* cV_param = f_val->data;
            
            DefrdSub cV_ds = f_val->ds;
            
            DefrdSub inner_ds = newDefrdSub();
            
            
            if(f_val->type == CLOSUREV_T){

                inner_ds->ds = cV_ds;

            } else if (f_val->type == DSCLOSUREV_T){

                inner_ds->ds = ds;

            } else {

                fprintf(stderr, "Error in LFAEDS_Value type");
				exit(0);

            }

            inner_ds->type = ASUB_T;
            strcpy(inner_ds->name, cV_param);
            inner_ds->value = a_val;
            
            //free(ret);

            ret = LFAEDSinterp(cV_body, inner_ds);

            break;

        default:
            fprintf(stderr, "no operation found\n");
			exit(0);
    }
    return ret;
}

void
printHelp(){
    printf("\nUsuage : ./my_lfaeds [-p] [-i] conc_syntax\n"
                    "\n"
                    "\tDefault is printing both parser and interpreter\n"
                    "\tIf you wish to see only parsed lfaeds or interpreted lfaeds, use following options.\n"
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
    char in_str[MAXSTR];
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
                return 0;
				break;
            
            default : /* ? */
            
                printHelp();
                exit(1);
        
        }
    }
    //if (optind >= argc){
        //fprintf(stderr, "Expects concrete syntax after options\n");
        //return 0;
    //}
    
   // LFAEDS root = LFAEDSparser(argv[optind]);
	
	//if(root == 0x0)
	//return 0;
//	while(1){
      fgets(in_str, MAXSTR, stdin);
      in_str[strcspn(in_str, "\n")] = 0x0;
      
      LFAEDS root = LFAEDSparser(in_str); 
      
      DefrdSub ds = newDefrdSub();

      if( p_flag){

          LFAEDSprint(root);
          printf("\n\n");

      } 
      if( i_flag ){

          LFAEDS_Value result = LFAEDSinterp(root, ds);
          LFAEDS_Value_print(result);
          printf("\n\n");
      
          freeLFAEDS_Value(result);

      }
      if( !i_flag && !p_flag ){

          LFAEDSprint(root);
          printf("\n\n");
          LFAEDS_Value result = LFAEDSinterp(root, ds);
          LFAEDS_Value_print(result);
          printf("\n\n");
          
        // freeLFAEDS_Value(result);

      }

      free(ds);
      freeLFAEDS(root);
//    }

    return 0;
}
