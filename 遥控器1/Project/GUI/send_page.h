#ifndef SEND_PAGE_H
#define SEND_PAGE_H

#include "gui.h"
#include "page.h"


typedef struct
{
	ta_t *ta1;
	btnm_t *btnm1;
	btnm_t *btnm2;
}SEND_PAGE;



extern SEND_PAGE Send_page;



void sned_page_create(void);
void send_page_enable_display(void);
void sned_page_disable_display(void);
void send_page_handler(void);
void send_page_btnm1_handler(void);
void Send_page_btnm2_handler(void);

u16 send_page_string2int(char *string);
u8 send_page_str_judge(char *str);















#endif

