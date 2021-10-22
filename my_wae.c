/*
 * Expressions must be enclosed with brackets whereas numbers are NOT enclosed with it.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "syntax.h"

//#define DEBUG

#define WAE_NUM_ARGS 4

#define MAX_ARG_LEN 16
#define MAX_LEN 16

#define NUM_T 0
#define WAE_T 1
#define WITH_T 2

typedef struct _WAE_t{
    int type; // 0 : num, 1 : with, 2 : WAE
    char id[MAX_ARG_LEN];
    char data[MAX_ARG_LEN];
    struct _WAE_t * arg1; 
    struct _WAE_t * arg2; 
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
 */
void
tokenize(char tokens[WAE_NUM_ARGS][MAX_ARG_LEN], char * str){

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
            substring(str, tokens[i_token], i_start, i_end);
            i_start = i + 1;
            i_token++;
        }
    }         
}

/*
 * Parses concrete syntax into WAE abstract syntax
 * It parses conc_sync by detecting "()"s and spaces.
 */
WAE 
WAEparser(char * block){
    WAE node = newWAE(); 

    int b_len = strlen(block);
    
    char * tmp_str = (char *) malloc(b_len);
    assert(tmp_str);
    strcpy(tmp_str, block);
    

    
    if(!check_bracket(block, b_len)){
        node->type = NUM_T ;
        strncpy(node->data, block, strlen(block));
        node->arg1 = 0x0;
        node->arg2 = 0x0;
    } else {
        char * in_bracket = (char *)malloc(sizeof(char) * (b_len - 2));
        assert(in_bracket);
        
        int in_len = substring(block, in_bracket, 1, (b_len - 2));
        if(in_len == -1 && in_len != b_len - 2){
            fprintf(stderr, "Wrong input\n");
            exit(1);
        }
        
        char tokens[WAE_NUM_ARGS][MAX_ARG_LEN];
        
        tokenize(tokens, in_bracket);
        
        if(strcmp(tokens[0], "with") == 0){
            // calc token nums
            
            node->type = WITH_T;
            strncpy(node->id, tokens[1], strlen(tokens[1]));
            node->arg1 = WAEparser(tokens[2]);
            node->arg2 = WAEparser(tokens[3]);
        } else {
            node->type = WAE_T;
            strncpy(node->data, tokens[0], strlen(tokens[0]));
            node->arg1 = WAEparser(tokens[1]);
            node->arg2 = WAEparser(tokens[2]);
        }
        free(in_bracket);
    }
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
    printf("{ ");
    printf("%s", wae->data);
    WAEprint(wae->arg1);
    WAEprint(wae->arg2);
    printf(" }");
}

int WAEinterp(WAE wae){
    if(wae->type == NUM_T){
        return atoi(wae->data);
    } else {
        if(strcmp(wae->data, "+") == 0){
            return WAEinterp(wae->arg1) + WAEinterp(wae->arg2);
        } else if(strcmp(wae->data, "-") == 0){
            return WAEinterp(wae->arg1) - WAEinterp(wae->arg2);
        } else if(strcmp(wae->data, "with") == 0){
            
        } else {
            fprintf(stderr, "no operation found\n");
            exit(1);
        }
    }
}

void subst(){
    
}

#ifdef DEBUG

int main(){
    
    WAE root;
    char * tc1 = "(+ 1 2)";
    char * tc2 = "(+ 1 (+ 3 4))";
    char * tc3 = "(+ 1 (+ (- 2 4) 4))";
    char * tc4 = "(+ (- (+ 2 2) (+ (- 2 4) 4))";
    char * tc5 = "2";

    printf("\n=== TEST1 %s === \n", tc1);
    root = WAEparser(tc1);
    printf("%d\n", WAEinterp(root));


    printf("\n=== TEST2 %s === \n", tc2);
    root = WAEparser(tc2);
    printf("%d\n", WAEinterp(root));
 

    printf("\n=== TEST3 %s === \n", tc3);
    root = WAEparser(tc3);
    printf("%d\n", WAEinterp(root));
    
    
    printf("\n=== TEST4 %s === \n", tc4);
    root = WAEparser(tc4);
    printf("%d\n", WAEinterp(root));
    
    printf("\n=== TEST5 %s === \n", tc5);
    root = WAEparser(tc5);
    printf("%d\n", WAEinterp(root));

    freeWAE(root);
    return 0;
}
#endif /* DEBUG */
