#ifndef PAGE_H
#define PAGE_H

#include "gui.h"
#include "skim_page.h"
#include "send_page.h"
#include "rov_page.h"
#include "gps_page.h"
#include "set_page.h"



typedef enum
{
	
	send_page=0,
	skim_page,
	gps_page,
	rov_page,
	set_page,
}PAGE_FLAG;



extern PAGE_FLAG page_flag;
extern bg_t *bg;
extern cb_t *page_cb;
extern mbox_t *mbox1;
extern mbox_t *mbox2;
extern style_t style1;
extern style_t style2;
extern style_t style3;
extern style_t style4;
extern style_t style5;
extern style_t style6;
extern style_t style7;
extern style_t style8;
extern style_t style9;
extern style_t style10;
















void page_create(void);
void page_style_init(void);
void page_del();
void image_handler(void);
void page_touch_handler(void);
void page_touch_list(void);


void general_handler(void);











#endif

