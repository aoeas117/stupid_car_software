#ifndef GPS_PAGE_H
#define GPS_PAGE_H



#include "gui.h"
#include "page.h"
#define COORD_NUM 32



typedef struct
{
	dl_t *dl1;
	btnm_t *btnm1;
	ta_t *ta1;
	btn_t *btn1;
	label_t *label1;
	label_t *label2;
	label_t *label3;
	coord_t coord;
	u8 flag :1;
	struct
	{
		u8 gps_point_total;//�����ȡ����
		u8 gps_point_saved;//�Ѵ����(������)
		u8 gps_point_index;//��ǰָ���(�ɶԵ�ǰָ�����д�ȡ��ɾ���Ȳ���)
		u8 gps_data_auto_update :1;//GPS�����Զ����±�־λ���ù��ܿ���ʱ�����û�ȡ��ȡ����Ϣ���ܣ�
		u8 gps_save_valid :1;
		u8 gps_delete_valid :1;	
		u8 gps_receive_flag:1;
		u8 gps_receive_count;
		u8 gps_receive_max;
	}gps_point;

	}GPS_PAGE;

extern GPS_PAGE Gps_page;
extern float gps_la[COORD_NUM];
extern float gps_lo[COORD_NUM];


void 	gps_page_create						(void);

void 	gps_page_enable_display		(void);

void 	gps_page_disable_display	(void);
	
void 	gps_page_handler					(void);
	
u16 	gps_page_string2int				(char *string);
	
void 	strat_gps_lalo_get_data		(void);
	
void 	gps_get_lalo_data					(void);



#endif

