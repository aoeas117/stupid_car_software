#ifndef ROV_PAGE_H
#define ROV_PAGE_H

#include "gui.h"
#include "page.h"

typedef struct
{
	lmeter_t *lmeter1;
	bar_t *bar1;
	btnm_t *btnm1;
	cbl_t *cbl1;
	btn_t *btn1;
	btn_t *btn2;


}ROV_PAGE;


extern ROV_PAGE Rov_page;
extern uint16 max_gear;


void rov_page_create(void);
void rov_page_enable_display(void);
void rov_page_disable_display(void);
void rov_page_handler(void);









#endif






