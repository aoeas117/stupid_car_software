#ifndef SKIM_PAGE_H
#define SKIM_PAGE_H


#include "mw1208.h"
#include "page.h"





typedef struct
{
	dl_t* dl1;
	btnm_t *btnm1;
	cbl_t *cbl1;
	uint8 update_flag;
	
	
}SKIM_PAGE;

extern SKIM_PAGE Skim_page; 
extern DATA_T data_buffer;
extern double Longtitude,Latitude,Height;


void skim_page_create(void);

void skim_page_enable_display(void);

void skim_page_disable_display(void);

void skim_page_handler(void);

uint8 decode_str_data(char *string);

void skim_page_get_data();



#endif
