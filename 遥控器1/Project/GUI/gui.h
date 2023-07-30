#ifndef GUI_H
#define GUI_H

#include "lcd.h"
#include "headfile.h"




typedef struct
{
	u8 r;
	u8 g;
	u8 b;

}color_t;


typedef struct
{
	u8 w_edge;
  u8 h_edge;
}edge_t;


typedef struct
{
	color_t backcolor;
	color_t gradcolor;
	u16 h;
	u16 hs;
}style_t;


typedef struct
{
	u16 xs;
	u16 ys;
	u16 xe;
	u16 ye;
}object_sect;


/**********background**************/


typedef struct
{
	style_t style;//样式
	object_sect object;//对象域
}bg_t;

/**********image******************/
typedef struct
{
	object_sect object;
	const char *image;
	style_t father_style;
	struct
	{
		u8 sta : 1;
		u8 trigger : 1;
	}mode;
	u16 color_on;
	u16 color_off;
}image_t;

/************label*******************/
typedef struct
{
	char *string;
	u16 x;
	u16 y;
	u16 color;//文本字体颜色
	style_t father_style;//父对象样式
	u8 max_l;//label最长长度
}label_t;
/************btn********************/
typedef struct
{
	label_t* label;//label         
	object_sect object;//对象域     
	style_t style_released;//未触发样式    
	style_t style_ack;//触发时样式  
	style_t father_style;//父对象样式   
	edge_t edge;//间边距   
	struct
	{
		u8 border : 1;//有无边框
		u8 trigger : 1;//是否触发态
		u8 sta : 1;//按键状态
	}mode;
	u16 line_color;//边框颜色
	u8 round_r;//圆角半径
}btn_t;
/************ta***********************/
typedef struct
{
	label_t *label;//label(text)
	struct
	{
		u8 text_mode : 1;//长短文本模式
		u8 password : 1;//密码模式
		u8 border :1;//有无边框
		u8 close : 1;
	}mode;
	edge_t edge;//间边距
	object_sect object;//对象域
	style_t style;//样式
	style_t father_style;//父样式
	u16 line_color;//边框颜色
	u8 round_r;//圆角半径
	u8 time;
	char *password;
}ta_t;
/*********btnm**********************/


typedef struct
{
	object_sect object;
	style_t style_released;
	char *string;
	edge_t edge;
	struct
	{
		u8 trigger : 1;
		u8 sta : 1;
	}mode;
}lbtn_t;



typedef struct
{
	style_t father_style;
	style_t style;
	style_t btn_ack_style;
	edge_t btn_edge;
	edge_t btnm_edge;
	object_sect object;
	u8 round_r;
	u8 row_num;
	u8 object_count;
	struct
	{
		u8 active_object : 5;
		u8 border : 1;
		u8 triger : 1;
		u8 one_btn : 1;
	}mode;
	u16 line_color;
	lbtn_t *lbtn[19];
}btnm_t;
/**********mbox****************/
typedef struct
{
	object_sect object;
	char *string;
	style_t style;
	style_t father_style;
	u16 text_color;
	edge_t edge;
	u8 round_r;	
	u8 time;
	struct
	{
		u8 sta : 1;
		u8 close_sta : 1;
	
	}mode;
}mbox_t;//消息对话框
/************lmeter*********************/
typedef struct
{
	object_sect object;
	u8 R;
	style_t father_style;
	style_t style;
	u8 r;
	struct 
	{
		u8 scale_l : 4;
	
	}mode;
	u16 pointer_color;
	u16 angle;
}lmeter_t;
/****************bar*******************/
typedef struct
{
	object_sect object;
	style_t father_style;
	style_t style;
	u16 line_color;
	float Progress;
	u8 round_r;
	u16 time;
}bar_t;

/*************cb*****************/

typedef struct
{
	object_sect object;
	style_t father_style;
	u16 off_color;
	u16 on_color;
	u16 line_color;
	label_t *label;
	struct
	{
		u8 sta : 1;
		u8 edge : 4;
	}mode;
}cb_t;



/*****************cbl***********************/



typedef struct
{
	object_sect object;
	char *string;
	struct
	{
		u8 sta : 1;
	}mode;
}lcb_t;



typedef struct
{
	object_sect object;
	style_t style;
	style_t father_style;
	lcb_t *lcb[12];
	u16 on_color;
	u16 off_color;
	u8 object_count;
}cbl_t;

/******************dl*********************/
typedef struct
{
	long data_int;
	double data_float;
}DATA_T;

typedef struct
{
	object_sect object;//对象域
	style_t style;//样式
	style_t father_style;//父对象样式
	struct
	{
		u8 data_mode : 1;
		u8 data_num : 5;
		u8 column : 2;
	}mode;//设置字段
	struct
	{
		u8 page_object : 4;//页面最大数据个数（4到12）
		u8 page_flag : 1;//是否有页面
		u8 current_page : 4;//当前页面max:32
		u8 page_max : 4;//页面个数max:32
	}page;
	u8 data_name_maxl;
	lbtn_t *lbtn[2];//小按钮
	struct
	{
		void *p;
		struct
		{
			u8 data_type : 4;//数据类型
			u8 index : 4;//数组索引
		}sta;
	}data_index[32];//数据缓存区指针
	char **string;
	label_t *name_label;
	label_t *data_label;
	label_t *page_label;
}dl_t;


typedef struct
{
	object_sect object;
	style_t father_style;//父对象样式
	float *latitude;
	float *longitude;
	uint8 count;//??
	float min_la;
	float min_lo;
	float la_h;
	float lo_w;
	float dx;
	float dy;
	struct
	{
		u8 grid_flag:1;
	
	}mode;
	
}coord_t;


extern label_t *label;

/******************定时器*********************/
void lcd_tim_init(void);
void lcd_tim_switch(u8 mode);

/***************基础元素*******************/
u16 		lcd_makecolor												(u8 r8,u8 g8,u8 b8);//rgb888->rgb565
u16 		lcd_makecolor_c											(color_t* color);//rgb888->rgb565 形参为结构体
void 		lcd_drawgrad												(style_t* style,u16 xs,u16 ys,u16 w,u16 h);//专用于绘制大面积背景
void 		lcd_style_set_color									(style_t* style,u8 r,u8 g,u8 b,u8 r1,u8 g1,u8 b1);
u16 		lcd_make_backcolor									(color_t *backcolor,color_t *gradcolor,u16 h,u16 y);//获取在父对象擦除时的颜色值
void 		lcd_show2image											(u16 x,u16 y,u16 w,u16 h,const char* image,u16 color,style_t *style);//图像
void 		LCD_ShowChar												(uint16_t x,uint16_t y,const char num,uint16_t color,style_t *style);//显示字符
void 		LCD_ShowString											(uint16_t x,uint16_t y,const char* str,uint16_t color,style_t *style);//显示字符串
void 		lcd_drawLine												(u16 x1, u16 y1, u16 x2, u16 y2,u16 color,style_t *style);//画线
void 		lcd_drawrow													(u16 x1, u16 y1, u16 x2, u16 y2,u16 color,style_t *style);//行线
void 		lcd_drawcolumn											(u16 x1, u16 y1, u16 x2, u16 y2,u16 color,style_t *style);//列线
void 		lcd_draw_RoundRect									(u16 x,u16 y,u16 width,u16 height,u8 r,style_t *style,u8 mode);//画圆角矩形填充
void 		lcd_draw_RoundRect_border						(u16 x,u16 y,u16 width,u16 height,u8 r,u16 color);//画圆角矩框
void 		lcd_draw_RoundRect_border_no				(u16 x,u16 y,u16 width,u16 height,u8 r,u16 color,style_t *style,u8 mode);
void 		lcd_draw_RoundRect_border_down			(u16 x,u16 y,u16 width,u16 height,u8 r,u16 color,style_t* style);
void 		lcd_draw_pointer										(u16 x,u16 y,u8 R,u16 sita,u16 color,style_t *style,u8 r);
void 		lcd_draw_lmeter											(u16 x,u16 y,u8 r,u16 color,style_t *style);
void	  lcd_draw_round											(u16 x0,u16 y0,u8 r,u16 color,style_t *style,u8 mode);//画圆
void 		lcd_create_random_style							(style_t *style);//随机样式
/*****************object******************************/
u8 			tp_is_sect													(object_sect object);
void 		lcd_object_enable_display						(object_sect object,style_t style,u8 r,u8 mode,u16 color);
void 		lcd_object_disable_display					(object_sect object,style_t style,u8 r);
void 		lcd_object_disable_display_animate	(object_sect object,style_t style,u8 r,u8 time,u8 mode);
void	  lcd_object_enable_display_animate		(object_sect object,style_t style,u8 r,u8 mode,u16 color,u8 time);
void 		lcd_drawpure												(u16 xs,u16 ys,u16 w,u16 h,u16 color,style_t *style);
/******************bg*************************/
bg_t 	 *lcd_bg_create									(u16 xs,u16 ys,u16 xe,u16 ye,style_t style);
void 		lcd_bg_del										(bg_t *bg);
/*****************image*********************/
image_t*lcd_image_create							(u16 xs,u16 ys,u16 w,u16 h,style_t father_style,const char *p);
void		lcd_image_del									(image_t *image);
void 		lcd_image_enable_display			(image_t *image);
void 		lcd_image_disable_display			(image_t *image);
void 		lcd_image_ack									(image_t *image);
u8 			lcd_image_get_response				(image_t *image);
void 		lcd_image_set_color						(image_t *image,u16 color_on,u16 color_off);
void 		lcd_image_set_trigger					(image_t *image,u8 mode);
void 		lcd_image_set_active					(image_t *image,u8 mode);
/******************label*****************/
label_t  *lcd_label_create						(u16 x,u16 y,u16 color,char *string,style_t father_style);
void 			lcd_label_del								(label_t *label);
void 			lcd_label_enable_display		(label_t *label);
void 			lcd_label_disable_display		(label_t *label);
void			lcd_label_add_char					(label_t *label,char ch);
void 			lcd_label_del_char					(label_t *label);
char 		 *lcd_label_get_text					(label_t *label);
void 			lcd_label_set_text					(label_t *label,char* string);
void 			lcd_label_set_max_length		(label_t *label,u8 max_length);
void 			lcd_label_set_pos           (label_t *label,u16 x,u16 y);
void 			lcd_label_set_color					(label_t *label,u16 color);
/*****************btn******************/
btn_t  *lcd_btn_create								(u16 x,u16 y,char *btn_name,style_t style_released,style_t father_style);
void 		lcd_btn_del										(btn_t *btn);
void 		lcd_btn_enable_display				(btn_t *btn);
void 		lcd_btn_disable_display				(btn_t *btn);
void 		lcd_btn_set_text_color				(btn_t *btn,u16 color);
void 		lcd_btn_set_border_color			(btn_t *btn,u16 color);
void 		lcd_btn_set_round_r						(btn_t *btn,u8 round_r);
void 		lcd_btn_set_edge							(btn_t *btn,u8 w_edge,u8 h_edge);
void 		lcd_btn_set_w									(btn_t *btn,u8 w);
void	  lcd_btn_set_ack_style					(btn_t *btn,style_t style);
void		lcd_btn_ack										(btn_t *btn);
u8	 		lcd_btn_get_response					(btn_t *btn);
void 		lcd_btn_set_border						(btn_t *btn,u8 mode);
void 		lcd_btn_set_trigger						(btn_t *btn,u8 mode);
char 	 *lcd_btn_get_text							(btn_t *btn);
/******************ta************************/
ta_t 	 *lcd_ta_create									(u16 x,u16 y,style_t style,char *string,style_t father_style);
void 		lcd_ta_del										(ta_t *ta);
void 		lcd_ta_enable_display					(ta_t *ta);
void 		lcd_ta_enable_display_animate	(ta_t *ta);
void 		lcd_ta_disable_display				(ta_t *ta);
void 		lcd_ta_disable_display_animate(ta_t *ta);
void 		lcd_ta_set_edge								(ta_t *ta,u8 w_edge,u8 h_edge);
void	  lcd_ta_set_border_color				(ta_t *ta,u16 color);
void 		lcd_ta_set_text								(ta_t *ta,char *string);
void 		lcd_ta_set_text_color					(ta_t *ta,u16 color);
char 	 *lcd_ta_get_text								(ta_t *ta);
void 		lcd_ta_set_round_r						(ta_t *ta,u8 round_r);
void 		lcd_ta_set_border							(ta_t *ta,u8 mode);
void 		lcd_ta_set_close							(ta_t *ta,u8 mode);
void 		lcd_ta_set_time								(ta_t *ta,u8 time);
void 		lcd_ta_set_password						(ta_t *ta,u8 mode,char *password);
u8 	 		lcd_ta_match_password					(ta_t *ta);
void 		lcd_ta_set_text_mode					(ta_t *ta,u8 mode);
void 		lcd_ta_add_char								(ta_t *ta,char ch);
void 		lcd_ta_del_char								(ta_t *ta);
void 		lcd_ta_set_max_length					(ta_t *ta,u16 max_l);
/**************btnm************************/
lbtn_t *lcd_lbtn_create								(u16 x,u16 y,char *string,style_t style,u8 w);
void 		lcd_lbtn_del									(lbtn_t *lbtn);
void 		lcd_lbtn_set_style						(lbtn_t *lbtn,style_t style);
void 		lcd_lbtn_enable_display				(lbtn_t *lbtn);
void 		lcd_lbtn_ack									(lbtn_t *lbtn,style_t style);
void 		lcd_lbtn_set_active						(lbtn_t *lbtn,u8 mode,style_t style);
btnm_t *lcd_btnm_create								(u16 x,u16 y,char * map[],style_t style,style_t father_style);
void 		lcd_btnm_del									(btnm_t *btnm);
void 		lcd_btnm_enable_display				(btnm_t *btnm);
void 		lcd_btnm_disable_display			(btnm_t *btnm);
char 	 *lcd_btnm_get_text							(btnm_t *btnm,u8 index);
void 		lcd_btnm_set_trigger					(btnm_t *btnm,u8 mode);
void 		lcd_btnm_set_btn_style				(btnm_t *btnm,style_t style);
void 		lcd_btnm_set_one_btn					(btnm_t *btnm,u8 mode);
void 		lcd_btnm_set_border						(btnm_t *btnm,u8 mode);
void 		lcd_btnm_set_border_color			(btnm_t *btnm,u16 color);
void 		lcd_btnm_set_btn_ack_style		(btnm_t *btnm,style_t style);
void 		lcd_btnm_set_round_r					(btnm_t *btnm,u8 round_r);
u8 			lcd_btnm_get_active_btn				(btnm_t *btnm);
u8 			lcd_btnm_ack									(btnm_t *btnm);
u8 			lcd_btnm_get_btn_trigger_sta	(btnm_t *btnm,u8 index);
void 		lcd_btnm_set_active_btn				(btnm_t *btnm,u8 index);
void 		lcd_btnm_sta_save							(btnm_t *btnm);
/**************mbox********************************/
mbox_t *lcd_mbox_create								(u16 x,style_t style,style_t father,u8 round_r);
void 		lcd_mbox_del									(mbox_t *mbox);
void		lcd_mbox_enable_display				(mbox_t *mbox);
void 		lcd_mbox_disable_display			(mbox_t *mbox);
void 		lcd_mbox_display_message			(mbox_t *mbox,char *text,u8 mode);
void 		lcd_mbox_set_message					(mbox_t *mbox,char *text);
void 		lcd_mbox_close_message				(mbox_t *mbox);
void 		lcd_mbox_tim_ack							(mbox_t *mbox);
void 		lcd_mbox_set_text_color				(mbox_t *mbox,u16 color);
void 		lcd_mbox_set_action_time			(mbox_t *mbox,u8 time);
void 		lcd_mbox_set_width						(mbox_t *mbox,u8 char_num);
void 		lcd_mbox_set_height						(mbox_t *mbox,u8 h);
void		lcd_mbox_ack									(mbox_t *mbox);
/*****************lmeter**********************************/
lmeter_t *lcd_lmeter_create						(u16 x,u16 y,u8 R,style_t style,style_t  father);
void 			lcd_lmeter_del							(lmeter_t *lmeter);
void		 	lcd_lmeter_enable_display		(lmeter_t *lmeter);
void 			lcd_lmeter_disable_display	(lmeter_t *lmeter);
void 			lcd_lmeter_set_angle				(lmeter_t *lmeter,u16 angle);
/*****************bar***********/
bar_t  *lcd_bar_create								(u16 x,u16 y,style_t father,style_t style);
void 		lcd_bar_del										(bar_t *bar);
void	  lcd_bar_enable_display				(bar_t *bar);
void 		lcd_bar_disable_display				(bar_t *bar);
void 		lcd_bar_set_progress					(bar_t *bar,float progress);
void 		lcd_bar_set_line_color				(bar_t *bar,u16 color);
void 		lcd_bar_set_length						(bar_t *bar,u16 l);
void 		lcd_bar_set_round_r						(bar_t *bar,u8 round_r);
void 		lcd_bar_set_width							(bar_t *bar,u16 w);
void 		lcd_bar_set_time							(bar_t *bar,u16 t);
/*********************cb*************************************/
cb_t 	 *lcd_cb_create									(u16 x,u16 y,char *string,style_t father);
void 		lcd_cb_del										(cb_t *cb);
void 		lcd_cb_enable_display					(cb_t *cb);
void 		lcd_cb_disable_display				(cb_t *cb);
u8 			lcd_cb_ack										(cb_t *cb);
void		lcd_cb_set_line_color					(cb_t *cb,u16 color);
void 		lcd_cb_set_color							(cb_t *cb,u16 on_color,u16 off_color);
void 		lcd_cb_set_size								(cb_t *cb,u8 size);
void 		lcd_cb_set_edge								(cb_t *cb,u8 edge);
void 		lcd_cb_set_text_color					(cb_t *cb,u16 color);
/*********************cbl**********************************/
lcb_t  *lcd_lcb_create								(u16 x,u16 y,char *string);																			//创建复选单元
void 		lcd_lcb_del										(lcb_t *lcb);																										//删除复选单元（如果实现了内存管理可以用）
void 		lcd_lcb_enable_display				(lcb_t *lcb,u16 off_color);																			//显示复选单元
u8 			lcd_lcb_ack										(lcb_t *lcb,u16 on_color,u16 off_color);												//复选单元触摸响应（内部调用）
void 		lcd_lcb_set_active						(lcb_t *lcb,u16 on_color,u16 off_color,u8 mode);								//指定复选单元的状态并显示（内部调用）

cbl_t  *lcd_cbl_create								(u16 x,u16 y,char * map[],style_t style,style_t father);        //创建复选框
void 	  lcd_cbl_del										(cbl_t *cbl);																										//删除复选框（如果实现了内存管理可以用）
void 		lcd_cbl_enable_display				(cbl_t *cbl);																										//显示复选框
void 		lcd_cbl_disable_display				(cbl_t *cbl);																										//隐藏复选框
u8 			lcd_cbl_ack										(cbl_t *cbl);                                                   //复选框触摸响应并返回触摸对象号
void 		lcd_cbl_set_color							(cbl_t *cbl,u16 on_color,u16 off_color);                        //设置不同状态下的颜色
void 		lcd_cbl_set_active						(cbl_t *cbl,u8 index,u8 mode);																	//指定复选框的一个对象的状态并显示
void 		lcd_cbl_sta_save							(cbl_t *cbl);                                                   //显示复选框各状态

/**********************dl***********************************/
dl_t 	 *lcd_dl_create									(u16 x,u16 y,char * map[],style_t style,style_t father,u8 mode);//创建数据列表
void 		lcd_dl_enable_display					(dl_t *dl);                                                     //显示数据列表
void 		lcd_dl_disable_display				(dl_t *dl);																											//隐藏数据列表
void 		lcd_dl_set_data_length				(dl_t *dl,u8 l);																								//设置数据长度
void 		lcd_dl_set_column							(dl_t *dl,u8 num);																							//设置数据列表的列数
void 		lcd_dl_set_data								(dl_t *dl ,u8 index_n,DATA_T *data);														//为数据列表内的数据赋值
void 		lcd_dl_set_data_index					(dl_t *dl,void *p,u8 index_n,u8 type);													//设置数据内存索引
void 		lcd_dl_set_page_object_num		(dl_t *dl,u8 n);																								//设置单页数据个数（行数）
void 		lcd_dl_sprintf								(dl_t *dl,u8 index_n);																					//将列表索引数据输出到屏幕显示缓存（内部调用）
void 		lcd_dl_data_enable_display		(dl_t *dl,u8 index_list);																				//显示数据
void 		lcd_dl_data_disable_display		(dl_t *dl,u8 index_list);																				//隐藏数据
void 		lcd_dl_data_update						(dl_t *dl,u8 index_list);																				//更新数据
void 		lcd_dl_turn_page							(dl_t *dl,u8 ud);																								//换页
void 		lcd_dl_lbtn_action						(dl_t *dl);																											//按钮动作
/*********************coord********************************/
void 		lcd_coord_create							(coord_t *coord,u16 x,u16 y,u16 w,u16 h,style_t *style);
void 		lcd_coord_set_data						(coord_t *coord,float *la,float *lo,u8 n);
void 		lcd_coord_enable_display			(coord_t *coord);
void 		lcd_coord_disable_display			(coord_t *coord);
void 		lcd_coord_draw_point					(u16 x,u16 y,u16 color);
void 		lcd_coord_pram_update					(coord_t *coord);
void 		lcd_coord_draw								(coord_t *coord,u8 cmd);
void 		lcd_coord_distance						(coord_t *coord,float min_latitude, float min_longitude, float max_latitude, float	max_longitude);
void 		lcd_coord_draw_grid						(coord_t *coord,u16 color);//画网格线
void 		lcd_coord_grid								(coord_t *coord);
void 		lcd_coord_single_point				(coord_t *coord,uint16 n,u16 color);








#endif