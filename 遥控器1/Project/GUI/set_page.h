#ifndef SET_PAGE_H
#define SET_PAGE_H



#include "gui.h"
#include "page.h"



typedef struct
{
	btnm_t* btnm1;


}SET_PAGE;


extern SET_PAGE Set_page;


void set_page_create(void);
void set_page_enable_display(void);
void set_page_disable_display(void);
void set_page_handler(void);




#endif