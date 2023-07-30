#ifndef _KEY_H
#define _KEY_H
#include "common.h"
#define u8 unsigned char

#define KEY3 C5
#define KEY6 C4


void key_init();
u8 KEY_Scan(u8 mode);
void key_function(u8 function_number);





#endif