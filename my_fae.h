
#ifndef _HAV_MY_FAE_H_
#define _HAV_MY_FAE_H_

#define MAX_ARG_LEN 256

#define NUM_T 0
#define ADD_T 1
#define SUB_T 2
#define ID_T 3
#define FUN_T 4
#define APP_T 5

#define NUMV_T 0
#define CLOSUREV_T 1

#define MTSUB_T 0
#define ASUB_T 1

typedef struct _FAE_t{
    int type; // 0 : num, 1 : add, 2: sub, 3: id, 4: fun, 5: app
    char data[MAX_ARG_LEN]; // id, number value, 
    struct _FAE_t * arg1; // value, lhs , function id 
    struct _FAE_t * arg2; // expr, rhs, function param
}FAE_t, *FAE;

typedef struct _FAE_Value_t{
    int type; // 0 : numV, 1 : closureV
    char data[MAX_ARG_LEN]; 
    struct _FAE_t * body;
    struct _DefrdSub_t * ds;
}FAE_Value_t, *FAE_Value;

typedef struct _DefrdSub_t{
    int type; // 0 : mtSub, 1 : aSub
    char name[MAX_ARG_LEN];
    FAE_Value value;
    struct _DefrdSub_t * ds ;
}DefrdSub_t, *DefrdSub;

#endif /* _HAV_MY_FAE_H_ */
