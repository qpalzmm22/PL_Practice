
#ifndef _HAV_MY_LFAE_H_
#define _HAV_MY_LFAE_H_

#define MAX_ARG_LEN 256

#define FREED 0
// LFAE
#define NUM_T 1
#define ADD_T 2
#define SUB_T 3
#define ID_T 4
#define FUN_T 5
#define APP_T 6

// LFAE_Value
#define NUMV_T 1
#define CLOSUREV_T 2
#define EXPRV_T 3

// Defrdsub
#define MTSUB_T 1
#define ASUB_T 2

// BOX
#define EMPTY 1
#define NOT_EMPTY 2

// WHEN TYPE IS -1, IT's FREED! 

typedef struct _LFAE_t{
    int type;               // 1 : num | 2 : add, 3: sub | 4: id | 5: fun  | 6: app
    char data[MAX_ARG_LEN]; //  n      |  0x0            |  name |  param  |  0x0
    struct _LFAE_t * arg1;  //  0x0    |  lhs            |  0x0  |  0x0    |  ftn
    struct _LFAE_t * arg2;  //  0x0    |  rhs            |  0x0  |  0x0    |  arg
}LFAE_t, *LFAE;

typedef struct _LFAE_Value_t{
    int type;                   // 1 : numV  | 2 : closureV | 3 : exprV
    char data[MAX_ARG_LEN];     //  n        |  symbol      |  0x0    
    struct _LFAE_t * body;      //  0x0      |  body        |  expr
    struct _DefrdSub_t * ds;    //  0x0      |  ds          |  ds
    struct _Box_t * box;        //  0x0      |  0x0         |  value 
}LFAE_Value_t, *LFAE_Value;

typedef struct _DefrdSub_t{
    int type;                       // 1 : mtSub | 2 : aSub     |
    char name[MAX_ARG_LEN];         //  0x0      |  name        |
    struct _LFAE_Value_t * value;   //  0x0      |  value       |
    struct _DefrdSub_t * ds ;       //  0x0      |  ds          |
}DefrdSub_t, *DefrdSub;

// ? What can go in to Box.. => LFAE_Value 
typedef struct _Box_t{
    int isempty;                    // 0 : empty 1 : filled
    struct _LFAE_Value_t * value;
}Box_t, *Box;

void
DefrdSub_print(DefrdSub ds);

void
LFAE_Value_print(LFAE_Value lfae_value);

LFAE_Value
strict(LFAE_Value lfae_val);

LFAE_Value
LFAEinterp(LFAE lfae, DefrdSub ds);

void
freeLFAE(LFAE node);

void
freeLFAE_Value(LFAE_Value node);

void
freeDefrdSub(DefrdSub node);

void
freeBox(Box node);

#endif /* _HAV_MY_LFAE_H_ */
