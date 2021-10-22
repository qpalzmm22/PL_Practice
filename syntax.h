#ifndef __HAV_SYNTAX_H__
#define __HAV_SYNTAX_H__


#define AE_MAX_LEN 16
#define NUM_T 0
#define AE_T 1

typedef struct _AE_t{
    int type; // 0 : num, 1 : AE
    char data[AE_MAX_LEN];
    struct _AE_t * lhs; 
    struct _AE_t * rhs; 
}AE_t, *AE;

#endif /* __HAV_SYNTAX_H__ */
