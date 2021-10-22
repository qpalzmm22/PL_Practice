#ifndef __HAV_AE_PARSER_H__
#define __HAV_AE_PARSER_H__

#include "syntax.h"

AE AEparser(char * block);
void freeAE(AE ae);

#endif /* __HAV_AE_PARSER_H__*/
