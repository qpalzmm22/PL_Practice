#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ae_parser.h"
#include "syntax.h"


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


int main(){
    
    AE root;
    char * tc1 = "(+ 1 2)";
    char * tc2 = "(+ 1 (+ 3 4))";
    char * tc3 = "(+ 1 (+ (- 2 4) 4))";
    char * tc4 = "2";

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

    freeAE(root);
    return 0;
}
