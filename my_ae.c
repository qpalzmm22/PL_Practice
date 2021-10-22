/*
 * Expressions must be enclosed with brackets whereas numbers are NOT enclosed with it.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "syntax.h"

//#define DEBUG

#define AE_NUM_ARGS 3

#define MAX_ARG_LEN 16
#define MAX_LEN 16
#define NUM_T 0
#define AE_T 1

typedef struct _AE_t{
    int type; // 0 : num, 1 : AE
    char data[MAX_ARG_LEN];
    struct _AE_t * lhs; 
    struct _AE_t * rhs; 
}AE_t, *AE;

/*
 * allocate AE with 0 (calloc)
 */
AE
newAE()
{
    AE new = (AE)calloc(1, sizeof(AE_t));
    assert(new);

    return new; 
}

/*
 * Frees all the elements inside. Then frees the root.
 */
void 
freeAE(AE root) 
{
    if(root == 0x0)
        return;

    freeAE(root->lhs);
    freeAE(root->rhs);
    free(root);
}

/*
 * check if a block starts with "(" and end with ")"
 */
int 
check_bracket(char * block, int b_len){
    if(*block == '(' && *(block + b_len - 1) == ')')
        return 1;
    else 
        return 0;
}

/*
 * Gets the subsring of i_begin to i_end of str and return it to ret_str
 * i_begin must be allocated already.
 */
int
substr(char *in_str, char *out_str,  int i_begin, int i_end){
    
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
 */
void
tokenize(char tokens[AE_NUM_ARGS][MAX_ARG_LEN], char * str){

    int len = strlen(str);
    int b_stack = 0;
    int i_start = 0;
    int i_end = 0;
    int i_token = 0;

    for(int i = 0; i <= len; i++){
        if(str[i] == '(')
            b_stack++;
        if(str[i] == ')')
            b_stack--;

        if((b_stack == 0 && str[i] == ' ') || str[i] == 0x0){
            i_end = i - 1;
            substr(str, tokens[i_token], i_start, i_end);
            i_start = i + 1;
            i_token++;
        }
    }         
}

/*
 * Parses concrete syntax into AE abstract syntax
 * It parses conc_sync by detecting "()"s and spaces.
 */
AE 
AEparser(char * block){
    AE node = newAE(); 

    int b_len = strlen(block);
    
    char * tmp_str = (char *) malloc(b_len);
    strcpy(tmp_str, block);
    
    if(!check_bracket(block, b_len)){
        node->type = NUM_T ;
        strncpy(node->data, block, strlen(block));
        node->lhs = 0x0;
        node->rhs = 0x0;
    } else {
        char * in_bracket = (char *)malloc(sizeof(char) * (b_len - 2));
        
        int in_len = substr(block, in_bracket, 1, (b_len - 2));
        if(in_len == -1 && in_len != b_len - 2){
            fprintf(stderr, "Wrong input\n");
            exit(1);
        }
        
        char tokens[AE_NUM_ARGS][MAX_ARG_LEN];
        
        tokenize(tokens, in_bracket);
        
        node->type = AE_T;
        strncpy(node->data, tokens[0], strlen(tokens[0]));
        node->lhs = AEparser(tokens[1]);
        node->rhs = AEparser(tokens[2]);

        free(in_bracket);
    }

    return node;
}

/*
 * prints abstract syntax of AE recursively
 */
void
AEprint(AE ae){
    if(ae == 0x0)
        return;
    printf("{ ");
    printf("%s", ae->data);
    AEprint(ae->lhs);
    AEprint(ae->rhs);
    printf(" }");
}

int AEinterp(AE ae){
    if(ae->type == NUM_T){
        return atoi(ae->data);
    } else {
        if(strcmp(ae->data, "+") == 0){
            return AEinterp(ae->lhs) + AEinterp(ae->rhs);
        } else if(strcmp(ae->data, "-") == 0){
            return AEinterp(ae->lhs) - AEinterp(ae->rhs);
        } else {
            fprintf(stderr, "no operation found\n");
            exit(1);
        }
    }
}

#ifdef DEBUG

int main(){
    
    AE root;
    char * tc1 = "(+ 1 2)";
    char * tc2 = "(+ 1 (+ 3 4))";
    char * tc3 = "(+ 1 (+ (- 2 4) 4))";
    char * tc4 = "(+ (- (+ 2 2) (+ (- 2 4) 4))";
    char * tc5 = "2";

    printf("\n=== TEST1 %s === \n", tc1);
    root = AEparser(tc1);
    printf("%d\n", AEinterp(root));


    printf("\n=== TEST2 %s === \n", tc2);
    root = AEparser(tc2);
    printf("%d\n", AEinterp(root));
 

    printf("\n=== TEST3 %s === \n", tc3);
    root = AEparser(tc3);
    printf("%d\n", AEinterp(root));
    
    
    printf("\n=== TEST4 %s === \n", tc4);
    root = AEparser(tc4);
    printf("%d\n", AEinterp(root));
    
    printf("\n=== TEST5 %s === \n", tc5);
    root = AEparser(tc5);
    printf("%d\n", AEinterp(root));

    freeAE(root);
    return 0;
}
#endif /* DEBUG */
