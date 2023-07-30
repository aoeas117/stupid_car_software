#include "gui.h"
#include "stdlib.h"
#include "myfont.h"
#include "touch.h"
#include "headfile.h"
#include "mw1208.h"







void lcd_tim_init(void)
{
	tim_interrupt_init(TIM_2,1,1);//一秒一次
	lcd_tim_switch(0);
}

void lcd_tim_switch(u8 mode)
{
	if(mode)
		tim_interrupt_enable(TIM_2);
	else
		tim_interrupt_disable(TIM_2);
}




u16 lcd_makecolor(u8 r8,u8 g8,u8 b8)
{
	return ((u16)r8>>3)<<11|((u16)g8>>2)<<5|(u16)b8>>3;
}


u16 lcd_makecolor_c(color_t* color)
{
	return ((u16)color->r >>3)<<11|((u16)color->g >>2)<<5|(u16)(color->b >>3);
}

u16 lcd_make_backcolor(color_t *backcolor,color_t *gradcolor,u16 h,u16 y)//获取背景rgb565
{
	float delta_r=(float)(backcolor->r-gradcolor->r)/h,delta_g=(float)(backcolor->g-gradcolor->g)/h,delta_b=(float)(backcolor->b-gradcolor->b)/h;
	color_t temp_color;
	temp_color.r=gradcolor->r+y*delta_r;
	temp_color.g=gradcolor->g+y*delta_g;
	temp_color.b=gradcolor->b+y*delta_b;
	return lcd_makecolor(temp_color.r,temp_color.g,temp_color.b);
}


void lcd_style_set_color(style_t* style,u8 r,u8 g,u8 b,u8 r1,u8 g1,u8 b1)
{
	style->backcolor.r=r;
	style->backcolor.g=g;
	style->backcolor.b=b;
	
	style->gradcolor.r=r1;
	style->gradcolor.g=g1;
	style->gradcolor.b=b1;
	
}

void lcd_draw_grad(style_t* style,u16 xs,u16 ys,u16 w,u16 h)
{
	u16 x,y;
	u16 color;
	color_t temp_color;
	LCD_Set_Window(xs,ys,w,h);
	LCD_WriteRAM_Prepare();
	float delta_r=(float)(style->backcolor.r-style->gradcolor.r)/h,delta_g=(float)(style->backcolor.g-style->gradcolor.g)/h,delta_b=(float)(style->backcolor.b-style->gradcolor.b)/h;
	for(y=ys;y<ys+h;y++)
	{
		temp_color.r=style->gradcolor.r+y*delta_r;
		temp_color.g=style->gradcolor.g+y*delta_g;
		temp_color.b=style->gradcolor.b+y*delta_b;
		color=lcd_makecolor(temp_color.r,temp_color.g,temp_color.b);
		for(x=xs;x<xs+w;x++)
			LCD_WR_DATA(color);
	}
	LCD_Set_Window(0,0,480,320);
}





void lcd_show2image(u16 x,u16 y,u16 w,u16 h,const char* image,u16 color,style_t *style)
{
	u16 num=0;u8 temp,i=0;
	u16 width,height;
	u16 temp_color=color;
	for(height=0;height<h;height++)
	{
		if(style!=NULL)
			temp_color=lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y+height-style->hs);
		for(width=0;width<w;width++)
		{
			temp=image[num]&(0x80>>(i++));
			if(temp!=0)
			{
				LCD_DrawPoint(x+width,y+height,temp_color);
							
			}
			if(i==8)
			{
				i=0;
				num++;
			}
		}
	}
}


void LCD_ShowChar(uint16_t x,uint16_t y,const char num,uint16_t color,style_t *style)
{
	uint8_t temp, x0, y0;
	char ch=num-' ';
	for(y0=0;y0<16;y0++)
	{
		temp=asc16[ch][y0];
		for(x0=0;x0<8;x0++)
		{
			if(temp&0x80)
			{		
				if(style==NULL)
					LCD_DrawPoint(x+x0,y+y0,0xffff);
				else
					LCD_DrawPoint(x+x0,y+y0,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y+y0-style->hs));
			}
			temp<<=1;
			if(y+y0>=lcddev.height)return;      //超区域了
			if(x+x0>=lcddev.width)return;
		}
	}
}


void LCD_ShowString(uint16_t x,uint16_t y,const char* str,uint16_t color,style_t *style)
{         
		uint8_t addx=0,addy=0;
		uint16_t add=0;
    while ((*(str+add)))   //判断是不是非法字符!
    {
      LCD_ShowChar(x+addx*ASC_FONT_WIDTH,y+addy*ASC_FONT_HEIGHT,*(str+add),0xffff,style);
			addx++;
			add++;
			if(addx*ASC_FONT_WIDTH>=lcddev.width)
			{
				addx=0;
				addy++;
			}
			if(addy*ASC_FONT_HEIGHT>lcddev.height) return;
    }
}


//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void lcd_drawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color,style_t *style)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		if(style==NULL)
			LCD_DrawPoint(uRow,uCol,color);//画点
		else
			LCD_DrawPoint(uRow,uCol,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,uCol-style->hs));//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 


void lcd_drawrow(u16 x1, u16 y1, u16 x2, u16 y2,u16 color,style_t *style)
{
	u16 w=0;
	u16 color_temp;
	if(style!=0)
	color_temp=lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y1-style->hs);
	else color_temp=color;	
	LCD_Set_Window(x1,y1,x2-x1+1,1);
	LCD_WRITE_GRAM;
	for(w=x1;w<=x2;w++)
		LCD_WR_DATA(color_temp);
	LCD_Set_Window(0,0,480,320);
}
void lcd_drawcolumn(u16 x1, u16 y1, u16 x2, u16 y2,u16 color,style_t *style)
{
	u16 h=0;
	u16 color_temp;
	LCD_Set_Window(x1,y1,1,y2-y1+1);
	LCD_WRITE_GRAM;
	if(style==NULL)
	{
		color_temp=color;
		for(h=y1;h<=y2;h++)
			LCD_WR_DATA(color_temp);
	}else
	{
		for(h=y1;h<=y2;h++)
		{
			color_temp=lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,h-style->hs);
			LCD_WR_DATA(color_temp);
		}
	}
	LCD_Set_Window(0,0,480,320);
}


void lcd_drawpure(u16 xs,u16 ys,u16 w,u16 h,u16 color,style_t *style)
{
	u16 x,y;
	if(style==NULL)
	{
		LCD_Set_Window(xs,ys,w,h);
		LCD_WriteRAM_Prepare();
		for(y=ys;y<ys+h;y++)
		{
			for(x=xs;x<xs+w;x++)
				LCD_WR_DATA(color);
		}
	}else
	{
		for(y=ys;y<=ys+h;y++)
				lcd_drawrow(xs,y,xs+w,y,0,style);
	}
	LCD_Set_Window(0,0,480,320);
}




void lcd_draw_RoundRect(u16 x,u16 y,u16 width,u16 height,u8 r,style_t *style,u8 mode)//基础
{
	
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1); 
	u16 w,h;
	u16 color_h=height;u16 y0=y-(height>>1);
	if((style->h)!=height)
	{
		y0=style->hs;
		color_h=style->h;
	}

		if(width>(r<<1)&&height>(r<<1))
		{
			w=width-(r<<1);
			h=height-(r<<1);
		}
		else return;
	
	if(r!=0&&r<200)
	{
		while(a<=b)
		{ if((mode&0x01))
			{
				lcd_drawrow(x-(w>>1)-a,y-(h>>1)-b,x+(w>>1)+a,y-(h>>1)-b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,color_h,y-b-(h>>1)-y0),NULL);
				lcd_drawrow(x-(w>>1)-b,y-(h>>1)-a,x+(w>>1)+b,y-(h>>1)-a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,color_h,y-a-(h>>1)-y0),NULL);
			}
			if((mode&0x02))
			{
				lcd_drawrow(x-(w>>1)-a,y+(h>>1)+b,x+(w>>1)+a,y+(h>>1)+b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,color_h,y+b+(h>>1)-y0),NULL);
				lcd_drawrow(x-(w>>1)-b,y+(h>>1)+a,x+(w>>1)+b,y+(h>>1)+a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,color_h,y+a+(h>>1)-y0),NULL);
			}
			a++;
				 
			if(di<0)di +=4*a+6;	  
			else
			{
				di+=10+4*(a-b);   
				b--;
			} 						    
		}
	}
	for(a=y-(h>>1);a<=y+(h>>1);a++)
		lcd_drawrow(x-(width>>1),a,x+(width>>1),a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,color_h,a-y0),NULL);
}




void lcd_draw_RoundRect_border(u16 x,u16 y,u16 width,u16 height,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	u16 w,h;
	if(width>(r<<1)&&height>(r<<1))
	{
		w=width-(r<<1);
		h=height-(r<<1);
	}
	else return;
	if(r!=0&&r<200)
	{
		while(a<=b)
		{
			LCD_DrawPoint(x+(w>>1)+a,y-(h>>1)-b,color);             //0
			LCD_DrawPoint(x+(w>>1)+b,y-(h>>1)-a,color); 
			LCD_DrawPoint(x+(w>>1)+b,y+(h>>1)+a,color);             //2               
			LCD_DrawPoint(x+(w>>1)+a,y+(h>>1)+b,color);             //3 
			LCD_DrawPoint(x-(w>>1)-a,y+(h>>1)+b,color);             //4       
			LCD_DrawPoint(x-(w>>1)-b,y+(h>>1)+a,color);             //5
			LCD_DrawPoint(x-(w>>1)-a,y-(h>>1)-b,color);             //6             
			LCD_DrawPoint(x-(w>>1)-b,y-(h>>1)-a,color);         
			a++;
			//使用Bresenham算法画圆     
			if(di<0)di +=4*a+6;	  
			else
			{
				di+=10+4*(a-b);   
				b--;
			} 						    
		}
}
	
	lcd_drawcolumn(x-(width>>1),y-(h>>1),x-(width>>1),y+(h>>1),color,NULL);
	lcd_drawcolumn(x+(width>>1),y-(h>>1),x+(width>>1),y+(h>>1),color,NULL);
	lcd_drawrow(x-(w>>1),y-(height>>1),x+(w>>1),y-(height>>1),color,NULL);
	lcd_drawrow(x-(w>>1),y+(height>>1),x+(w>>1),y+(height>>1),color,NULL);
	
}



void lcd_draw_RoundRect_border_no(u16 x,u16 y,u16 width,u16 height,u8 r,u16 color,style_t *style,u8 mode)
{
	int a,b;u8 flag=1;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	u16 w,h;
	if(width>(r<<1)&&height>(r<<1))
	{
		w=width-(r<<1);
		h=height-(r<<1);
	}
	else 
	{
		w=width;
		h=height;
		flag=0;
	};
	if(r!=0&&r<200&&flag)
	{
		while(a<=b)
		{
			if(style!=NULL)
			{
				if(mode&0x01)
				{
					LCD_DrawPoint(x+(w>>1)+a,y-(h>>1)-b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y-(h>>1)-b-style->hs));             
					LCD_DrawPoint(x+(w>>1)+b,y-(h>>1)-a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y-(h>>1)-a-style->hs));
				}
				if(mode&0x02)
				{
					LCD_DrawPoint(x+(w>>1)+b,y+(h>>1)+a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y+(h>>1)+a-style->hs));                        
					LCD_DrawPoint(x+(w>>1)+a,y+(h>>1)+b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y+(h>>1)+b-style->hs));             
				}
				
				if(mode&0x04)
				{
					LCD_DrawPoint(x-(w>>1)-a,y+(h>>1)+b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y+(h>>1)+b-style->hs));                
					LCD_DrawPoint(x-(w>>1)-b,y+(h>>1)+a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y+(h>>1)+a-style->hs));            
				}
				
				if(mode&0x08)
				{
					LCD_DrawPoint(x-(w>>1)-a,y-(h>>1)-b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y-(h>>1)-b-style->hs));                         
					LCD_DrawPoint(x-(w>>1)-b,y-(h>>1)-a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y-(h>>1)-a-style->hs));         
				}
			
			}else
			{
					if(mode&0x01)
				{
					LCD_DrawPoint(x+(w>>1)+a,y-(h>>1)-b,color);                         
					LCD_DrawPoint(x+(w>>1)+b,y-(h>>1)-a,color);
				}
				if(mode&0x02)
				{
					LCD_DrawPoint(x+(w>>1)+b,y+(h>>1)+a,color);                         
					LCD_DrawPoint(x+(w>>1)+a,y+(h>>1)+b,color);             
				}
				if(mode&0x04)
				{
					LCD_DrawPoint(x-(w>>1)-a,y+(h>>1)+b,color);                         
					LCD_DrawPoint(x-(w>>1)-b,y+(h>>1)+a,color);            
				}
				if(mode&0x08)
				{
					LCD_DrawPoint(x-(w>>1)-a,y-(h>>1)-b,color);                         
					LCD_DrawPoint(x-(w>>1)-b,y-(h>>1)-a,color);        
				}
			}
			a++;
			//使用Bresenham算法画圆     
			if(di<0)di +=4*a+6;	  
			else
			{
				di+=10+4*(a-b);   
				b--;
			} 						    
		}
}
	
	lcd_drawcolumn(x-(width>>1),y-(h>>1),x-(width>>1),y+(h>>1),color,style);
	lcd_drawcolumn(x+(width>>1),y-(h>>1),x+(width>>1),y+(h>>1),color,style);
	lcd_drawrow(x-(w>>1),y-(height>>1),x+(w>>1),y-(height>>1),color,style);
	lcd_drawrow(x-(w>>1),y+(height>>1),x+(w>>1),y+(height>>1),color,style);
}



void lcd_draw_RoundRect_border_down(u16 x,u16 y,u16 width,u16 height,u8 r,u16 color,style_t* style)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	u16 w,h;
	if(width>(r<<1)&&height>(r<<1))
	{
		w=width-(r<<1);
		h=height-(r<<1);
	}
	else return;
	if(r!=0&&r<200)
	{
		while(a<=b)
		{
	
			if(style!=NULL)
			{
				LCD_DrawPoint(x+(w>>1)+b,y+(h>>1)+a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y+(h>>1)+a-style->hs));             //2               
				LCD_DrawPoint(x+(w>>1)+a,y+(h>>1)+b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y+(h>>1)+b-style->hs));             //3 
				LCD_DrawPoint(x-(w>>1)-a,y+(h>>1)+b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y+(h>>1)+b-style->hs));             //4       
				LCD_DrawPoint(x-(w>>1)-b,y+(h>>1)+a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y+(h>>1)+a-style->hs));             //5
			}else
			{
				LCD_DrawPoint(x+(w>>1)+b,y+(h>>1)+a,color);                         
				LCD_DrawPoint(x+(w>>1)+a,y+(h>>1)+b,color);              
				LCD_DrawPoint(x-(w>>1)-a,y+(h>>1)+b,color);                    
				LCD_DrawPoint(x-(w>>1)-b,y+(h>>1)+a,color);       
			}
			a++;
			//使用Bresenham算法画圆     
			if(di<0)di +=4*a+6;	  
			else
			{
				di+=10+4*(a-b);   
				b--;
			} 						    
		}
}
	
	lcd_drawcolumn(x-(width>>1),y-(h>>1),x-(width>>1),y+(h>>1),color,NULL);
	lcd_drawcolumn(x+(width>>1),y-(h>>1),x+(width>>1),y+(h>>1),color,NULL);
	lcd_drawrow(x-(w>>1),y+(height>>1),x+(w>>1),y+(height>>1),color,style);
	
}




//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void lcd_draw_round(u16 x0,u16 y0,u8 r,u16 color,style_t *style,u8 mode)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		if(mode&0x01)
		{
			if(style==NULL)
			{
				LCD_DrawPoint(x0+a,y0-b,color);             //0
				LCD_DrawPoint(x0+b,y0-a,color);   
			}else
			{
				LCD_DrawPoint(x0+a,y0-b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y0-b-style->hs));             //0
				LCD_DrawPoint(x0+b,y0-a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y0-a-style->hs));   
			}
		}
		if(mode&0x02)
		{
			if(style!=NULL)
			{
				LCD_DrawPoint(x0+b,y0+a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y0+a-style->hs));             //2               
				LCD_DrawPoint(x0+a,y0+b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y0+b-style->hs)); 
			}else
			{
				LCD_DrawPoint(x0+b,y0+a,color);             //2               
				LCD_DrawPoint(x0+a,y0+b,color); 
			}		 
		}
		if(mode&0x04)
		{
			if(style==NULL)
			{
				LCD_DrawPoint(x0-a,y0+b,color);             //4       
				LCD_DrawPoint(x0-b,y0+a,color);             //5
			}else
			{
				LCD_DrawPoint(x0-a,y0+b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y0+b-style->hs));             //4       
				LCD_DrawPoint(x0-b,y0+a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y0+a-style->hs));       
			
			}
		}
		if(mode&0x08)
		{			
			if(style==NULL)
			{
				LCD_DrawPoint(x0-a,y0-b,color);             //6             
				LCD_DrawPoint(x0-b,y0-a,color);             //7     	         
			}else
			{
				LCD_DrawPoint(x0-a,y0-b,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y0-b-style->hs));             //6             
				LCD_DrawPoint(x0-b,y0-a,lcd_make_backcolor(&style->backcolor,&style->gradcolor,style->h,y0-a-style->hs));  
				
			}
		}	
			a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 





void lcd_draw_pointer(u16 x,u16 y,u8 R,u16 sita,u16 color,style_t *style,u8 r)
{
	float angle=(float)sita*0.017444;
  u16	xs=x+r*cos((float)sita*0.017444);
	u16 ys=y-r*sin((float)sita*0.017444);
	u16 xe=x+R*cos(angle);
	u16 ye=y-R*sin(angle);
	lcd_drawLine(xs,ys,xe,ye,0x0000,style);
}


void lcd_draw_lmeter(u16 x,u16 y,u8 r,u16 color,style_t *style)
{
	u16 xs,ys,xe,ye;
	u16 sita;
	lcd_draw_round(x,y,r,color,style,0x11);
	for(sita=0;sita<=180;sita+=45)
	{
		xs=x+(r-9)*cos((float)sita*0.017444);
		ys=y-(r-9)*sin((float)sita*0.017444);
		xe=x+(r-1)*cos((float)sita*0.017444);
		ye=y-(r-1)*sin((float)sita*0.017444);
		lcd_drawLine(xs,ys,xe,ye,color,style);
	}
	for(sita=0;sita<=180;sita+=5)
	{
		xs=x+(r-5)*cos((float)sita*0.017444);
		ys=y-(r-5)*sin((float)sita*0.017444);
		xe=x+(r-1)*cos((float)sita*0.017444);
		ye=y-(r-1)*sin((float)sita*0.017444);
		lcd_drawLine(xs,ys,xe,ye,color,style);
	}
	
	

}





/****************object****************************/

void lcd_object_disable_display(object_sect object,style_t style,u8 r)//隐藏对象
{
	u16 x,y;
	x=(object.xs+object.xe)/2;
	y=(object.ys+object.ye)/2;
	
	u16 w,h;
	w=object.xe-object.xs;
	h=object.ye-object.ys;
	lcd_draw_RoundRect(x,y,w,h,r,&style,3);
}

u8 tp_is_sect(object_sect object)
{
	if(((tp_pos.x_current) >= object.xs && (tp_pos.x_current) <=object.xe) && ((tp_pos.y_current) >= object.ys && (tp_pos.y_current <= object.ye)))
		return 1;
	else return 0;
}



void lcd_object_enable_display(object_sect object,style_t style,u8 r,u8 mode,u16 color)
{
	u16 x,y;
	x=(object.xs+object.xe)/2;
	y=(object.ys+object.ye)/2;
	
	u16 w,h;
	w=object.xe-object.xs;
	h=object.ye-object.ys;
	if( (r<<1) > w || (r<<1) > h )
	{
		if(w>h) r=(h>>1)-2;
		else r=(w>>1)-2;
	}

	lcd_draw_RoundRect(x,y,w-2,h-2,r-2,&style,3);
	if(mode==1)
		lcd_draw_RoundRect_border(x,y,w,h,r,color);
}




void lcd_object_disable_display_animate(object_sect object,style_t style,u8 r,u8 time,u8 mode)
{
	u16 x,y;
	x=(object.xs+object.xe)/2;
	y=(object.ys+object.ye)/2;
	
	u16 w,h;
	w=object.xe-object.xs;
	h=object.ye-object.ys;
	if( (r<<1) > w || (r<<1) > h )
	{
		if(w>h) r=(h>>1)-2;
		else r=(w>>1)-2;
	}
	u16 count;
	if(mode)
	{
		for(count=h;count>0;count--)
		{
			lcd_draw_RoundRect_border_no(x,y,w,count,r,0,&style,0x0f);
			systick_delay_us(time);
		}
	}else
	{
		for(count=w;count>0;count--)
		{
			lcd_draw_RoundRect_border_no(x,y,count,h,r,0,&style,0x0f);
			systick_delay_us(time);
		}
	}
}
void lcd_object_enable_display_animate(object_sect object,style_t style,u8 r,u8 mode,u16 color,u8 time)
{
	u16 x,y;
	x=(object.xs+object.xe)/2;
	y=(object.ys+object.ye)/2;
	
	u16 w,h;
	w=object.xe-object.xs;
	h=object.ye-object.ys;
	if( (r<<1) > w || (r<<1) > h )
	{
		if(w>h) r=(h>>1)-2;
		else r=(w>>1)-2;
	}
	u16 count;
	if(mode&0x01)
	{
		for(count=1;count<=h;count++)
		{
			lcd_draw_RoundRect_border_no(x,y,w-2,count,r-2,0,&style,0x0f);
			systick_delay_us(time);
		}
	}else
	{
		for(count=1;count<=w;count++)
		{
			lcd_draw_RoundRect_border_no(x,y,count,h-2,r-2,0,&style,0x0f);
			systick_delay_us(time);
		}
	}

	if(mode&0x02)
		lcd_draw_RoundRect_border(x,y,w,h,r,color);
}




/***********************bg***********************************/
bg_t *lcd_bg_create(u16 xs,u16 ys,u16 xe,u16 ye,style_t style)
{
	bg_t *bg=(bg_t*)malloc(sizeof(bg_t));
	bg->object.xs=xs;
	bg->object.ys=ys;
	bg->object.xe=xe;
	bg->object.ye=ye;
	bg->style=style;
	bg->style.h=ye-ys;
	bg->style.hs=ys;
	lcd_draw_grad(&style,xs,ys,xe-xs,ye-ys);
	return bg;
}

void lcd_bg_del(bg_t *bg)
{
	free(bg);

}
/*******************image*************************************/
image_t *lcd_image_create(u16 xs,u16 ys,u16 w,u16 h,style_t father_style,const char *p)
{
	image_t *image=(image_t*)malloc(sizeof(image_t));
	
	image->object.xs=xs;
	image->object.ys=ys;
	image->object.xe=xs+w;
	image->object.ye=ys+h;

	image->father_style=father_style;
	
	image->color_off=0x0000;
	
	image->color_on=0x54fa;
	
	image->image=p;
	
	image->mode.sta=0;
	
	image->mode.trigger=0;
	
	return image;
}

void lcd_image_del(image_t *image)
{
	free(image);
}


void lcd_image_enable_display(image_t *image)
{
	lcd_show2image(image->object.xs,image->object.ys,image->object.xe-image->object.xs,\
	image->object.ye-image->object.ys,image->image,image->color_off,NULL);
}

void lcd_image_disable_display(image_t *image)
{
	lcd_show2image(image->object.xs,image->object.ys,image->object.xe-image->object.xs,\
	image->object.ye-image->object.ys,image->image,0,&image->father_style);
}

void lcd_image_ack(image_t *image)
{
	if(image->mode.trigger==1)
	{
		if(image->mode.sta==0)
		{
			image->mode.sta=1;
			lcd_show2image(image->object.xs,image->object.ys,image->object.xe-image->object.xs,\
			image->object.ye-image->object.ys,image->image,image->color_on,NULL);
		}else
		{
			image->mode.sta=0;
			lcd_show2image(image->object.xs,image->object.ys,image->object.xe-image->object.xs,\
			image->object.ye-image->object.ys,image->image,image->color_off,NULL);
		}
	}else
	{
		lcd_show2image(image->object.xs,image->object.ys,image->object.xe-image->object.xs,\
		image->object.ye-image->object.ys,image->image,image->color_on,NULL);
		systick_delay_ms(10);
		lcd_show2image(image->object.xs,image->object.ys,image->object.xe-image->object.xs,\
		image->object.ye-image->object.ys,image->image,image->color_off,NULL);
	}

}

u8 lcd_image_get_response(image_t *image)
{
	if(tp_is_sect(image->object))
	{
		lcd_image_ack(image);
		return 1;
	}else
	return 0;

}
	
void lcd_image_set_color(image_t *image,u16 color_on,u16 color_off)
{
	image->color_off=color_off;
	image->color_on=color_on;
}

void lcd_image_set_trigger(image_t *image,u8 mode)
{
	image->mode.trigger=mode;
}

void lcd_image_set_active(image_t *image,u8 mode)
{
	image->mode.sta=mode;
	if(mode)
	{
		lcd_show2image(image->object.xs,image->object.ys,image->object.xe-image->object.xs,\
			image->object.ye-image->object.ys,image->image,image->color_on,NULL);
	
	}else
	{
		lcd_show2image(image->object.xs,image->object.ys,image->object.xe-image->object.xs,\
			image->object.ye-image->object.ys,image->image,image->color_off,NULL);
	}

}


/**************label api function************************/
//本模块用到的api函数有malloc,free,sprintf,LCD_ShowString,LCD_ShowChar



label_t *lcd_label_create(u16 x,u16 y,u16 color,char *string,style_t father_style)//创建一个label对象并返回其指针
{
	label_t *label=(label_t*)malloc(sizeof(label_t));               //string长度受max_l参数限制
	label->max_l=6;
	label->x=x;
	label->y=y;
	label->color=color;
	label->string=(char*)malloc(sizeof(char)*15);
	sprintf(label->string,"%s",string);
	label->string[label->max_l]=0;
	label->father_style=father_style;
	return label;
}


void lcd_label_del(label_t *label)//删除label对象
{
	free(label->string);
	free(label);
}


void lcd_label_enable_display(label_t *label)//显示当前的label对象
{
	u8 char_count=strlen(label->string);  
	if(char_count>label->max_l)
			label->string[label->max_l]=0;
	LCD_ShowString(label->x,label->y,label->string,label->color,NULL);
}



void lcd_label_disable_display(label_t *label)//隐藏当前的label对象
{
	LCD_ShowString(label->x,label->y,label->string,0,&label->father_style);
}

void lcd_label_add_char(label_t *label,char ch)//为label对象添加一个字符，会同步显示在屏幕上
{
	u8 char_count=strlen(label->string);   //受max_l参数限制
	if(char_count>=label->max_l) return; 
	u16 x=label->x+char_count*8;
	LCD_ShowChar(x,label->y,ch,label->color,NULL);
	label->string[char_count]=ch;
	label->string[char_count+1]=0;
	
}
void lcd_label_del_char(label_t *label)//删掉label对象的一个字符，会同步删除在屏幕上
{
	u8 char_count=strlen(label->string);
	if(char_count==0) return;
	u16 x=label->x+(char_count-1)*8;
  LCD_ShowChar(x,label->y,label->string[char_count-1],0,&label->father_style);
	label->string[char_count-1]=0;
	
}

char *lcd_label_get_text(label_t *label)//获取label对象的文本指针
{
	return label->string;
}

void lcd_label_set_text(label_t *label,char* string)//设置label对象的文本，超过max_length会被截断
{
	u8 char_count=strlen(string);
	lcd_label_disable_display(label);
	sprintf(label->string,"%s",string);
	if(char_count>label->max_l)
		label->string[label->max_l]=0;
	lcd_label_enable_display(label);
}



void lcd_label_set_max_length(label_t *label,u8 max_length)//设置label对象的最大长度，默认为6，太大会浪费空间
{
	label->max_l=max_length;
	char *string=(char*)malloc(sizeof(char)*max_length);
	sprintf(string,"%s",label->string);
	free(label->string);
	label->string=string;
}



void lcd_label_set_color(label_t *label,u16 color)//设置label对象的字体颜色
{
		label->color=color;
		lcd_label_enable_display(label);

}

void lcd_label_set_pos(label_t *label,u16 x,u16 y)
{
	label->x=x;
	label->y=y;

}

/********************btn*******************************/

btn_t *lcd_btn_create(u16 x,u16 y,char *btn_name,style_t style_released,style_t father_style)
{
	u8 char_count=strlen(btn_name);
	
	btn_t *btn=(btn_t*)malloc(sizeof(btn_t));
	btn->style_released=style_released;
	btn->father_style=father_style;
	
	btn->edge.w_edge=5;
	btn->edge.h_edge=4;
	
	btn->object.xs=x;
	btn->object.ys=y;
	btn->object.xe=x+char_count*8+2*btn->edge.w_edge;
	btn->object.ye=y+16+2*btn->edge.h_edge;
	
	btn->style_released.h=16+2*btn->edge.h_edge-2;
	btn->style_ack.backcolor=btn->style_released.gradcolor;
	btn->style_ack.gradcolor=btn->style_released.backcolor;
	btn->style_ack.h=16+2*btn->edge.h_edge-2;
	
	btn->line_color=0x0000;
	
	btn->round_r=6;
	
	btn->mode.border=1;
	
	btn->mode.trigger=0;
	
	
	btn->mode.sta=0;
	
	btn->label=lcd_label_create(x+btn->edge.w_edge,y+btn->edge.h_edge,0x0000,btn_name,btn->style_released);
	lcd_label_set_max_length(btn->label,char_count);
	sprintf(btn->label->string,"%s",btn_name);
	
	return btn;

}


void lcd_btn_del(btn_t *btn)
{
	lcd_label_del(btn->label);
	free(btn);
}



void lcd_btn_enable_display(btn_t *btn)
{
	lcd_object_enable_display(btn->object,btn->style_released,btn->round_r,btn->mode.border,btn->line_color);
	lcd_label_enable_display(btn->label);
}



void lcd_btn_disable_display(btn_t *btn)
{
	u16 x,y;
	x=(btn->object.xs+btn->object.xe)/2;
	y=(btn->object.ys+btn->object.ye)/2;
	
	u16 w,h;
	w=btn->object.xe-btn->object.xs;
	h=btn->object.ye-btn->object.ys;
	lcd_draw_RoundRect(x,y,w,h,btn->round_r,&btn->father_style,3);

}


void lcd_btn_set_text_color(btn_t *btn,u16 color)
{
	btn->label->color=color;
}



void lcd_btn_set_border_color(btn_t *btn,u16 color)
{
	btn->line_color=color;
}

void lcd_btn_set_round_r(btn_t *btn,u8 round_r)
{
	btn->round_r=round_r;
}

void lcd_btn_set_edge(btn_t *btn,u8 w_edge,u8 h_edge)
{
	btn->edge.h_edge=h_edge;
	btn->edge.w_edge=w_edge;
	btn->object.xe=btn->object.xs+btn->label->max_l*8+2*btn->edge.w_edge;
	btn->object.ye=btn->object.ys+16+2*btn->edge.h_edge;
	btn->style_released.h=16+2*btn->edge.h_edge-2;
	btn->style_ack.h=btn->style_released.h-2;
	btn->label->x=btn->object.xs+w_edge;
	btn->label->y=btn->object.ys+h_edge;
}

void lcd_btn_set_w(btn_t *btn,u8 w)
{
	u8 char_count=strlen(btn->label->string);
	if(w<char_count*8+2*btn->edge.w_edge) w=char_count*8+2*btn->edge.w_edge;
	u8 w_edge=(w-char_count*8)>>1;
	lcd_btn_set_edge(btn,w_edge,btn->edge.h_edge);
}



void lcd_btn_set_ack_style(btn_t *btn,style_t style)
{
	btn->style_ack=style;
	btn->style_ack.h=btn->object.ye-btn->object.ys-2;
}



void lcd_btn_ack(btn_t *btn)//按钮按下动作（内部调用）
{
	
	if(btn->mode.trigger==0)//非触发态
	{
		lcd_object_enable_display(btn->object,btn->style_ack,btn->round_r,0,0x0000);
		lcd_label_enable_display(btn->label);
		systick_delay_ms(1);
		lcd_object_enable_display(btn->object,btn->style_released,btn->round_r,0,0x0000);
		lcd_label_enable_display(btn->label);
	}else //触发态
	{
		if(btn->mode.sta==0)
		{
			btn->mode.sta=1;
			lcd_object_enable_display(btn->object,btn->style_ack,btn->round_r,0,0x0000);
		lcd_label_enable_display(btn->label);
		}else
		{
			btn->mode.sta=0;
			lcd_object_enable_display(btn->object,btn->style_released,btn->round_r,0,0x0000);
			lcd_label_enable_display(btn->label);
		}
	}
}



u8 lcd_btn_get_response(btn_t *btn)
{
	if(tp_is_sect(btn->object))
	{
		lcd_btn_ack(btn);
		return 1;
	}

	return 0;

}



void lcd_btn_set_border(btn_t *btn,u8 mode)
{
	btn->mode.border=mode;
}


void lcd_btn_set_trigger(btn_t *btn,u8 mode)
{
	btn->mode.trigger=mode;
}


char *lcd_btn_get_text(btn_t *btn)
{
	return lcd_label_get_text(btn->label);
}


/****************ta****************************/
ta_t *lcd_ta_create(u16 x,u16 y,style_t style,char *string,style_t father_style)
{
	ta_t *ta=(ta_t*)malloc(sizeof(ta_t));
	
	ta->edge.w_edge=5;
	ta->edge.h_edge=3;
	
	ta->father_style=father_style;
	
	ta->style=style;
	ta->style.h=16+2*ta->edge.h_edge-2;
	ta->style.hs=y;
	
	ta->label=lcd_label_create(x+ta->edge.w_edge,y+ta->edge.h_edge,0x0000,string,ta->style);
	lcd_label_set_max_length(ta->label,15);
	sprintf(ta->label->string,"%s",string);

	
	ta->object.xs=x;
	ta->object.ys=y;
	ta->object.xe=x+ta->label->max_l*8+2*ta->edge.w_edge;
	ta->object.ye=y+16+2*ta->edge.h_edge;
	
	ta->line_color=0x0000;
	
	ta->round_r=6;
	
	ta->mode.password=0; //关闭密码
	
	ta->mode.border=1;   //有边框
	
	ta->mode.text_mode=0; //短文本
	
	ta->mode.close=1;
	
	ta->time=5;
	
	return ta;
}

void lcd_ta_del(ta_t *ta)
{
	lcd_label_del(ta->label);
	lcd_ta_disable_display(ta);
	if(ta->mode.password==1)
		free(ta->password);
	free(ta);
}

void lcd_ta_enable_display(ta_t *ta)
{
	lcd_object_enable_display(ta->object,ta->style,ta->round_r,ta->mode.border,ta->line_color);
	lcd_label_enable_display(ta->label);
	
}

void lcd_ta_enable_display_animate(ta_t *ta)
{
	lcd_object_enable_display_animate(ta->object,ta->style,ta->round_r,((0x01)&(ta->mode.close))|((0x01)&(ta->mode.border))<<1,ta->line_color,ta->time);
	lcd_label_enable_display(ta->label);
}

void lcd_ta_disable_display(ta_t *ta)
{
	u16 x,y;
	x=(ta->object.xs+ta->object.xe)/2;
	y=(ta->object.ys+ta->object.ye)/2;
	
	u16 w,h;
	w=ta->object.xe-ta->object.xs;
	h=ta->object.ye-ta->object.ys;
	
	lcd_draw_RoundRect(x,y,w,h,ta->round_r-2,&ta->father_style,3);
}


void lcd_ta_disable_display_animate(ta_t *ta)
{
	lcd_object_disable_display_animate(ta->object,ta->father_style,ta->round_r,ta->time,ta->mode.close);
}

void lcd_ta_set_edge(ta_t *ta,u8 w_edge,u8 h_edge)
{
	ta->edge.h_edge=h_edge;
	ta->edge.w_edge=w_edge;
	
	ta->object.xe=2*ta->edge.w_edge+ta->object.xs+8*ta->label->max_l;
	ta->object.ye=2*ta->edge.h_edge+ta->object.ys+16;
	
	ta->style.h=16+2*ta->edge.h_edge-2;
	ta->label->father_style.h=ta->style.h-2;
	
	ta->label->x=ta->object.xs+w_edge;
	ta->label->y=ta->object.ys+h_edge;
}


void lcd_ta_set_border_color(ta_t *ta,u16 color)
{
	ta->line_color=color;
}

void lcd_ta_set_text(ta_t *ta,char *string)
{
	if(ta->mode.password==0)
		lcd_label_set_text(ta->label,string);
	else
	{
		u8 char_count=strlen(ta->label->string);u8 i=0;
		for(i=0;i<char_count;i++)
			lcd_ta_del_char(ta);
	
	}
}




void lcd_ta_set_text_color(ta_t *ta,u16 color)
{
	lcd_label_set_color(ta->label,color);
}

char *lcd_ta_get_text(ta_t *ta)
{
	return lcd_label_get_text(ta->label);
}

void lcd_ta_set_round_r(ta_t *ta,u8 round_r)
{
	ta->round_r=round_r;
}

void lcd_ta_set_border(ta_t *ta,u8 mode)
{
	ta->mode.border=mode;
}

void lcd_ta_set_close(ta_t *ta,u8 mode)
{
	ta->mode.close=mode;
}

void lcd_ta_set_time(ta_t *ta,u8 time)
{
	ta->time=time;
}
void lcd_ta_set_password(ta_t *ta,u8 mode,char *password)
{
	ta->mode.password=mode;
	if(mode)//开启密码模式，仅单行有效
	{
		u8 char_count=strlen(password);
		ta->password=(char*)malloc(sizeof(char)*char_count);
		sprintf(ta->password,"%s",password);
	}
}

u8 lcd_ta_match_password(ta_t *ta)//1匹配成功 0匹配失败
{
	if(ta->mode.password==0) return 0;
	char *password=lcd_ta_get_text(ta);
	if(strcmp(password,ta->password)==0)
		return 1;
	else
		return 0;

}



	
void lcd_ta_set_text_mode(ta_t *ta,u8 mode)
{
		ta->mode.text_mode=mode;
}


void lcd_ta_add_char(ta_t *ta,char ch)//待完善
{
	if(ta->mode.text_mode==0)
	{
		if(ta->mode.password==1)
		{
			u8 char_count=strlen(ta->label->string);
			if(char_count>ta->label->max_l) return;
			ta->label->string[char_count]=ch;
			ta->label->string[char_count+1]=0;
			u16 x=ta->label->x+8*char_count;
			LCD_ShowChar(x,ta->label->y,'*',0x0000,NULL);
		}else
			lcd_label_add_char(ta->label,ch);
	}
}

void lcd_ta_del_char(ta_t *ta)//待完善
{
	if(ta->mode.text_mode==0)
	{
		if(ta->mode.password==1)
		{
			u8 char_count=strlen(ta->label->string);
			if(char_count==0) return;
			ta->label->string[char_count-1]=0;
			u16 x=ta->label->x+8*(char_count-1);
			LCD_ShowChar(x,ta->label->y,'*',0,&ta->label->father_style);
		}else
			lcd_label_del_char(ta->label);
	}
}


void lcd_ta_set_max_length(ta_t *ta,u16 max_l)
{
	lcd_label_set_max_length(ta->label,max_l);
	
	ta->object.xe=ta->object.xs+2*ta->edge.w_edge+8*max_l;
	ta->object.ye=ta->object.ys+2*ta->edge.h_edge+16;
}

/*****************btnm对象***************************************/

lbtn_t *lcd_lbtn_create(u16 x,u16 y,char *string,style_t style,u8 w)
{
	lbtn_t *lbtn=(lbtn_t*)malloc(sizeof(lbtn_t));
	
	u8 char_count=strlen(string);
	
	lbtn->edge.w_edge=5;
	lbtn->edge.h_edge=4;
	
	if(w>char_count*8)
	lbtn->edge.w_edge=(w-char_count*8)>>1;
	
	lbtn->object.xs=x;
	lbtn->object.ys=y;
	
	lbtn->object.ye=y+16+2*lbtn->edge.h_edge;
	lbtn->object.xe=x+char_count*8+2*lbtn->edge.w_edge;
	
	lbtn->mode.trigger=0;
	lbtn->mode.sta=0;
	
	lbtn->string=(char*)malloc(sizeof(char)*char_count+1);
	sprintf(lbtn->string,"%s",string);
	
	lbtn->style_released=style;
	lbtn->style_released.h=14+2*lbtn->edge.h_edge;

	return lbtn;
}

void lcd_lbtn_del(lbtn_t *lbtn)
{
	free(lbtn->string);
	free(lbtn);
};

void lcd_lbtn_set_style(lbtn_t *lbtn,style_t style)
{
	lbtn->style_released=style;
	lbtn->style_released.h=lbtn->object.ye-lbtn->object.ys-2;
	lbtn->style_released.hs=lbtn->object.ys;

}




void lcd_lbtn_enable_display(lbtn_t *lbtn)
{	
	lcd_object_enable_display(lbtn->object,lbtn->style_released,8,1,0x0000);
	LCD_ShowString(lbtn->object.xs+lbtn->edge.w_edge,lbtn->object.ys+lbtn->edge.h_edge,lbtn->string,0x0000,NULL);
	
}





void lcd_lbtn_ack(lbtn_t *lbtn,style_t style)
{
	if(lbtn->mode.trigger)
	{
		if(lbtn->mode.sta==0)
		{
			lbtn->mode.sta=1;
			lcd_object_enable_display(lbtn->object,style,8,0,0x0000);
			LCD_ShowString(lbtn->object.xs+lbtn->edge.w_edge,lbtn->object.ys+lbtn->edge.h_edge,lbtn->string,0x0000,NULL);
		}else
		{
			lbtn->mode.sta=0;
			lcd_object_enable_display(lbtn->object,lbtn->style_released,8,0,0x0000);
			LCD_ShowString(lbtn->object.xs+lbtn->edge.w_edge,lbtn->object.ys+lbtn->edge.h_edge,lbtn->string,0x0000,NULL);
		}
	}else
	{
		lcd_object_enable_display(lbtn->object,style,8,0,0x0000);
		LCD_ShowString(lbtn->object.xs+lbtn->edge.w_edge,lbtn->object.ys+lbtn->edge.h_edge,lbtn->string,0x0000,NULL);
		systick_delay_ms(50);
		lcd_object_enable_display(lbtn->object,lbtn->style_released,8,0,0x0000);;
		LCD_ShowString(lbtn->object.xs+lbtn->edge.w_edge,lbtn->object.ys+lbtn->edge.h_edge,lbtn->string,0x0000,NULL);
	}
}


void lcd_lbtn_set_active(lbtn_t *lbtn,u8 mode,style_t style)
{
	lbtn->mode.sta=mode;
	if(mode)
	{
		lcd_object_enable_display(lbtn->object,style,8,0,0x0000);
		LCD_ShowString(lbtn->object.xs+lbtn->edge.w_edge,lbtn->object.ys+lbtn->edge.h_edge,lbtn->string,0x0000,NULL);
	}
	else
	{
		lcd_object_enable_display(lbtn->object,lbtn->style_released,8,0,0x0000);
		LCD_ShowString(lbtn->object.xs+lbtn->edge.w_edge,lbtn->object.ys+lbtn->edge.h_edge,lbtn->string,0x0000,NULL);
	}
}




btnm_t *lcd_btnm_create(u16 x,u16 y,char * map[],style_t style,style_t father_style)
{
	btnm_t *btnm=(btnm_t*)malloc(sizeof(btnm_t));
	
	btnm->father_style=father_style;
	
	btnm->btnm_edge.h_edge=8;
	btnm->btnm_edge.w_edge=6;
	
	btnm->btn_edge.h_edge=4;
	btnm->btn_edge.w_edge=4;
	/***************************/
	u8 count1=0,count2=1,index=0;
	while(map[index][0])
	{
		if(map[index++][0]!='\n')
		{
			count1++;//对象数
		}
		else
			count2++;//行数
	}
	
	btnm->object_count=count1;
	btnm->row_num=count2;
	/**********************/
	u8 line[btnm->row_num],max_l=0,flag=0,count=0,width_m=0;
	count1=0;count2=0;index=0;
	while(map[index][0])
	{
		if(map[index][0]!='\n')
		{
			count1=strlen(map[index]);
			if(count1>max_l)
				max_l=count1;
			count++;
		}else
		{
			line[count2++]=count;
			flag=(max_l*8+btnm->btn_edge.w_edge)*count+(count+1)*btnm->btnm_edge.w_edge;
			if(flag>width_m)
				width_m=flag;
			max_l=0;
			count=0;
		}
	index++;
	
	}

	line[count2]=count;
	flag=(max_l*8+btnm->btn_edge.w_edge)*count+(count+1)*btnm->btnm_edge.w_edge;
	if(flag>width_m)
		width_m=flag;
	u16 h=(16+2*btnm->btn_edge.h_edge)*btnm->row_num+(btnm->row_num+1)*btnm->btnm_edge.h_edge;
	u16 w=width_m;
	
	btnm->object.xs=x;
	btnm->object.ys=y;
	btnm->object.xe=x+w;
	btnm->object.ye=y+h;
	
	btnm->style=style;
	btnm->style.h=h-2;
	
	btnm->btn_ack_style=style;
	btnm->btn_ack_style.h=2*btnm->btn_edge.h_edge+14;
	
	btnm->line_color=0x0000;
	
	btnm->round_r=15;
	
	btnm->mode.triger=0;                                      
	btnm->mode.active_object=btnm->object_count;
	btnm->mode.border=1;
	btnm->mode.one_btn=0;
	
	for(index=0;index<btnm->row_num;index++)
		line[index]=(w-(line[index]+1)*btnm->btnm_edge.w_edge)/line[index];
		
	style_t style_btn={0x13,0xfa,0xb1,0xf1,0x56,0x11,0,0};
	count1=0;count2=0;index=0;
	u16 x0=x+btnm->btnm_edge.w_edge,y0=y+btnm->btnm_edge.h_edge;
	while(map[index][0])
	{
		if(map[index][0]!='\n')
		{
			btnm->lbtn[count1++]=lcd_lbtn_create(x0,y0,map[index],style_btn,line[count2]);
			x0+=(btnm->btnm_edge.w_edge+line[count2]);
		}else
		{
			count2++;
			x0=x+btnm->btnm_edge.w_edge;
			y0+=(btnm->btnm_edge.h_edge+16+2*btnm->btn_edge.h_edge);
		}
		index++;
	}

	return btnm;
}

void lcd_btnm_del(btnm_t *btnm)
{
	u8 index=0;
	for(index=0;index<btnm->object_count;index++)
		free(btnm->lbtn[index]);
	free(btnm);

}




void lcd_btnm_enable_display(btnm_t *btnm)
{
	u8 index=0;
	lcd_object_enable_display(btnm->object,btnm->style,btnm->round_r,btnm->mode.border,btnm->line_color);
	for(index=0;index<btnm->object_count;index++)
		lcd_lbtn_enable_display(btnm->lbtn[index]);

}

void lcd_btnm_disable_display(btnm_t *btnm)
{
	lcd_object_disable_display(btnm->object,btnm->father_style,btnm->round_r);
}




char *lcd_btnm_get_text(btnm_t *btnm,u8 index)
{
	if(index>=btnm->object_count)return NULL;
	return btnm->lbtn[index]->string;
}



void lcd_btnm_set_trigger(btnm_t *btnm,u8 mode)
{
	btnm->mode.triger=mode;
	u8 index=0;
	for(index=0;index<btnm->object_count;index++)
		btnm->lbtn[index]->mode.trigger=mode;
}

void lcd_btnm_set_btn_style(btnm_t *btnm,style_t style)
{
	u8 index;
	for(index=0;index<btnm->object_count;index++)
		lcd_lbtn_set_style(btnm->lbtn[index],style);
}




void lcd_btnm_set_one_btn(btnm_t *btnm,u8 mode)
{
	btnm->mode.one_btn=mode;
}


void lcd_btnm_set_border(btnm_t *btnm,u8 mode)
{
	btnm->mode.border=mode;
}

void lcd_btnm_set_border_color(btnm_t *btnm,u16 color)
{
	btnm->line_color=color;
}

void lcd_btnm_set_btn_ack_style(btnm_t *btnm,style_t style)
{
	btnm->btn_ack_style=style;
	btnm->btn_ack_style.h=14+2*btnm->btn_edge.h_edge;
}


void lcd_btnm_set_round_r(btnm_t *btnm,u8 round_r)
{
	if(round_r>3*btnm->btnm_edge.w_edge)
		btnm->round_r=3*btnm->btnm_edge.w_edge;
	else
		btnm->round_r=round_r;
}



u8 lcd_btnm_get_active_btn(btnm_t *btnm)//内部调用
{
	u8 index=0,flag=btnm->object_count;
	if(tp_is_sect(btnm->object))
	{
		for(index=0;index<btnm->object_count;index++)
			if(tp_is_sect(btnm->lbtn[index]->object))
				flag=index;
		return flag;
	}
	return btnm->object_count;
	

}






u8 lcd_btnm_ack(btnm_t *btnm)
{
	u8 active=btnm->object_count;
	active=lcd_btnm_get_active_btn(btnm);
	if(active>=btnm->object_count) return active;
	else
	{
		if(btnm->mode.triger)
		{
			if(btnm->mode.one_btn==0)
				lcd_lbtn_ack(btnm->lbtn[active],btnm->btn_ack_style);
			else
			{
				if(active==btnm->mode.active_object)
					lcd_lbtn_ack(btnm->lbtn[active],btnm->btn_ack_style);
				else
				{
					lcd_lbtn_set_active(btnm->lbtn[active],1,btnm->btn_ack_style);
					if(btnm->mode.active_object!=btnm->object_count)
						lcd_lbtn_set_active(btnm->lbtn[btnm->mode.active_object],0,btnm->btn_ack_style);
					btnm->mode.active_object=active;
				}	
			}
		}else
		lcd_lbtn_ack(btnm->lbtn[active],btnm->btn_ack_style);
	}
	return active;
}


u8 lcd_btnm_get_btn_trigger_sta(btnm_t *btnm,u8 index)//error:2
{
	if(index>=btnm->object_count) return 2;
  if(btnm->mode.triger==0) return 2;
	else
	{
		return btnm->lbtn[index]->mode.sta; //0 or 1
	}

}



void lcd_btnm_set_active_btn(btnm_t *btnm,u8 index)
{
	if(btnm->mode.one_btn)
	{
		lcd_lbtn_set_active(btnm->lbtn[index],1,btnm->btn_ack_style);
		btnm->mode.active_object=index;
	
	}

}


void lcd_btnm_sta_save(btnm_t *btnm)
{
	lcd_btnm_set_active_btn(btnm,btnm->mode.active_object);
}





/**************mbox********************************/

mbox_t *lcd_mbox_create(u16 x,style_t style,style_t father,u8 round_r)
{
	mbox_t *mbox=(mbox_t*)malloc(sizeof(mbox_t));
	
	mbox->string=(char*)malloc(sizeof(char)*15);
	
	mbox->text_color=0x0000;
	
	
	mbox->edge.w_edge=4;
	mbox->edge.h_edge=6;
	
	mbox->style=style;
	mbox->style.h=16+2*mbox->edge.h_edge+round_r;
	mbox->style.hs=0;
	
	mbox->father_style=father;
	
	mbox->time=5;
	
	mbox->object.xs=x;
	mbox->object.xe=x+2*mbox->edge.w_edge+15*8;
	mbox->object.ys=0;
	mbox->object.ye=16+2*mbox->edge.h_edge+round_r;
	
	mbox->round_r=round_r;
	
	mbox->mode.sta=0;
	mbox->mode.close_sta=0;
	
	return mbox;
}

void lcd_mbox_del(mbox_t *mbox)
{
	free(mbox->string);
	free(mbox);
}

void lcd_mbox_enable_display(mbox_t *mbox)
{
	u16 w=mbox->object.xe-mbox->object.xs;
	u16 x=(mbox->object.xe+mbox->object.xs)>>1;
	lcd_draw_RoundRect(x,1,w,2*mbox->round_r+2,mbox->round_r,&mbox->style,2);
	lcd_draw_RoundRect_border_down(x,1,w+2,2*mbox->round_r+5,mbox->round_r+1,0x0000,NULL);
}

void lcd_mbox_disable_display(mbox_t *mbox)
{
	u16 w=mbox->object.xe-mbox->object.xs;
	u16 x=(mbox->object.xe+mbox->object.xs)>>1;
	if(mbox->mode.sta==1)
		lcd_mbox_close_message(mbox);
	lcd_draw_RoundRect(x,2,w+4,2*mbox->round_r+4,mbox->round_r,&mbox->father_style,2);

	}


void lcd_mbox_display_message(mbox_t *mbox,char *text,u8 mode)
{
	mbox->mode.sta=1;
	u16 w=mbox->object.xe-mbox->object.xs;
	u16 x=(mbox->object.xe+mbox->object.xs)>>1;
	u16 count=0;
	
	
	for(count=1;count<=mbox->object.ye-mbox->round_r-6;count++)
	{
		lcd_draw_RoundRect_border_down(x,1+count,w+2,2*mbox->round_r+3,mbox->round_r+1,0x0000,NULL);
		lcd_draw_RoundRect_border_down(x,1+count,w+2,2*mbox->round_r+3,mbox->round_r+1,0x0000,&mbox->father_style);
	}
	lcd_draw_RoundRect_border_down(x,2+count,w+2,2*mbox->round_r+5,mbox->round_r+1,0x0000,NULL);
	lcd_draw_RoundRect(x,(mbox->object.ye>>1)-mbox->round_r/2,w,mbox->object.ye+mbox->round_r-3,mbox->round_r,&mbox->style,2);
	u8 char_count;
	if(text!=NULL)
		char_count=strlen(text);
	else
			char_count=strlen(mbox->string);
	w=mbox->round_r+mbox->edge.h_edge/2;
	x=((mbox->object.xe-mbox->object.xs)-char_count*8)/2+mbox->object.xs;
	if(text!=NULL)
		sprintf(mbox->string,"%s",text);
	LCD_ShowString(x,w,mbox->string,mbox->text_color,NULL);
	if(mode)
	{
		lcd_tim_switch(1);
		mbox->mode.close_sta=1;
	}
}






void lcd_mbox_set_message(mbox_t *mbox,char *text)
{
	if(mbox->mode.sta==1)
	{
		u8 char_count=strlen(mbox->string);
		u16 w=mbox->round_r+mbox->edge.h_edge/2;
		u16 x=((mbox->object.xe-mbox->object.xs)-char_count*8)/2+mbox->object.xs;
		LCD_ShowString(x,w,mbox->string,0,&mbox->style);
		
		char_count=strlen(text);
		x=((mbox->object.xe-mbox->object.xs)-char_count*8)/2+mbox->object.xs;
		sprintf(mbox->string,"%s",text);
		LCD_ShowString(x,w,mbox->string,mbox->text_color,NULL);
	}else
	{
		lcd_mbox_display_message(mbox,text,0);
	}
}



	

void lcd_mbox_close_message(mbox_t *mbox)
{
	
		mbox->mode.sta=0;
		u16 w=mbox->object.xe-mbox->object.xs;
		u16 x=(mbox->object.xe+mbox->object.xs)>>1;
		u16 count=0;
		for(count=mbox->object.ye-mbox->round_r-3;count>1;count--)
		{
			lcd_draw_RoundRect_border_down(x,1+count,w+2,2*mbox->round_r+3,mbox->round_r+1,0x0000,&mbox->father_style);
		}
		lcd_draw_RoundRect_border_down(x,1+count,w+2,2*mbox->round_r+3,mbox->round_r+1,0x0000,NULL);

}



void lcd_mbox_tim_ack(mbox_t *mbox)
{
	if(mbox->mode.close_sta==1)
	{
		lcd_mbox_close_message(mbox);
		mbox->mode.close_sta=0;
		lcd_tim_switch(0);
	}
}




void lcd_mbox_set_text_color(mbox_t *mbox,u16 color)
{
	mbox->text_color=color;
}



void lcd_mbox_set_action_time(mbox_t *mbox,u8 time)
{
	mbox->time=time;
}




void lcd_mbox_set_width(mbox_t *mbox,u8 char_num)
{
	mbox->object.xe=mbox->object.xs+char_num*8+2*mbox->edge.w_edge;
}



void lcd_mbox_set_height(mbox_t *mbox,u8 h)
{
	mbox->edge.h_edge=h;
	mbox->object.ye=16+2*mbox->edge.h_edge+mbox->round_r;
}


void lcd_mbox_ack(mbox_t *mbox)
{
	if(mbox->mode.sta==1)
	{
		if(tp_is_sect(mbox->object))
			lcd_mbox_close_message(mbox);
	}else
	{
		mbox->object.ys=mbox->round_r+4;
		if(tp_is_sect(mbox->object))
			lcd_mbox_display_message(mbox,NULL,0);
		mbox->object.ye=16+2*mbox->edge.h_edge+mbox->round_r;
	}
}



/*****************lmeter**********************************/

lmeter_t *lcd_lmeter_create(u16 x,u16 y,u8 R,style_t style,style_t  father)
{
	lmeter_t *lmeter=(lmeter_t*)malloc(sizeof(lmeter_t));
	
	lmeter->father_style=father;
	
	lmeter->object.xs=x;
	lmeter->object.ys=y;
	
	lmeter->R=R;
	
	lmeter->r=4;
	
	lmeter->style=style;
	lmeter->style.h=R;
	lmeter->style.hs=y-R;
	
	lmeter->pointer_color=0x0000;
	
	lmeter->angle=180;
	
	return lmeter;
}

void lcd_lmeter_del(lmeter_t *lmeter)
{
	free(lmeter);
}

void lcd_lmeter_enable_display(lmeter_t *lmeter)
{
	style_t style={0x12,0x1c,0x29,0xc3,0xff,0xbf,10,110};
	lcd_draw_lmeter(lmeter->object.xs,lmeter->object.ys,lmeter->R,0x0000,&lmeter->style);
	lcd_draw_RoundRect(lmeter->object.xs,lmeter->object.ys,lmeter->r*2+2,lmeter->r*2+2,lmeter->r,&style,3);
	lcd_lmeter_set_angle(lmeter,90);
	
	

}

void lcd_lmeter_disable_display(lmeter_t *lmeter)
{
	lcd_draw_lmeter(lmeter->object.xs,lmeter->object.ys,lmeter->R,0x0000,&lmeter->father_style);
	lcd_draw_RoundRect(lmeter->object.xs,lmeter->object.ys,lmeter->r*2+2,lmeter->r*2+2,lmeter->r,&lmeter->father_style,3);
	lcd_draw_pointer(lmeter->object.xs,lmeter->object.ys,lmeter->R-10,lmeter->angle,lmeter->pointer_color,&lmeter->father_style,lmeter->r+2);
}

void lcd_lmeter_set_angle(lmeter_t *lmeter,u16 angle)
{
	lcd_draw_pointer(lmeter->object.xs,lmeter->object.ys,lmeter->R-10,lmeter->angle,lmeter->pointer_color,&lmeter->father_style,lmeter->r+2);
	lcd_draw_pointer(lmeter->object.xs,lmeter->object.ys,lmeter->R-10,angle,lmeter->pointer_color,NULL,lmeter->r+2);
	lmeter->angle=angle;
}


/***********bar***************************/
bar_t *lcd_bar_create(u16 x,u16 y,style_t father,style_t style)
{
	bar_t *bar=(bar_t*)malloc(sizeof(bar_t));
	
	bar->object.xs=x;
	bar->object.ys=y;
	bar->object.xe=x+20;
	bar->object.ye=y+100;
	
	bar->father_style=father;
	
	bar->line_color=0x0000;
	
	bar->Progress=0;
	
	bar->style=style;
	bar->style.h=112;
	bar->style.hs=y;
	
	bar->round_r=6;
	bar->time=1;
	
	return bar;
}

void lcd_bar_del(bar_t *bar)
{
	free(bar);
}
void lcd_bar_enable_display(bar_t *bar)
{
	u16 x,y;
	x=(bar->object.xs+bar->object.xe)/2;
	y=(bar->object.ys+bar->object.ye)/2;
	
	u16 w,h;
	w=bar->object.xe-bar->object.xs;
	h=bar->object.ye-bar->object.ys;
	if( (bar->round_r<<1) > w || (bar->round_r<<1) > h )
	{
		if(w>h) bar->round_r=(h>>1)-2;
		else bar->round_r=(w>>1)-2;
	}

	lcd_draw_RoundRect_border(x,y,w,h,bar->round_r,bar->line_color);
}

void lcd_bar_disable_display(bar_t *bar)
{
	lcd_object_disable_display(bar->object,bar->father_style,bar->round_r);
}

void lcd_bar_set_progress(bar_t *bar,float progress)
{
	if(progress>1 || progress<0) return;
	u16 h=bar->object.ye-bar->object.ys-2*bar->round_r+4;
	u16 ys=bar->object.ye-bar->round_r+2-h*bar->Progress,ye=bar->object.ye-bar->round_r+2-h*progress;
	u16 count=0;
	u16 delta=fabs(ys-ye);
	if(ys>ye)
		for(count=0;count<delta;count++)
		{
			lcd_drawrow(bar->object.xs+2,ys-count,bar->object.xe-2,ys-count,0x0000,&bar->style);
		
		}
	else
		for(count=0;count<delta;count++)
		{
			lcd_drawrow(bar->object.xs+2,ys+count,bar->object.xe-2,ys+count,0x0000,&bar->father_style);
			
		}
	bar->Progress=progress;
}


void lcd_bar_set_line_color(bar_t *bar,u16 color)
{
	bar->line_color=color;
}

void lcd_bar_set_length(bar_t *bar,u16 l)
{
	bar->object.ye=bar->object.ys+l;
	bar->style.h=l;
	
}

void lcd_bar_set_round_r(bar_t *bar,u8 round_r)
{
	bar->round_r=round_r;
}

void lcd_bar_set_width(bar_t *bar,u16 w)
{
	bar->object.xe=bar->object.xs+w;
}

void lcd_bar_set_time(bar_t *bar,u16 t)
{
	bar->time=t;
}



/*********************cb*************************************/
cb_t *lcd_cb_create(u16 x,u16 y,char *string,style_t father)
{
	cb_t *cb=(cb_t*)malloc(sizeof(cb_t));
	
	cb->father_style=father;
	
	cb->off_color=0xffff;
	
	cb->on_color=0xfa42;
	
	cb->line_color=0x0000;
	
	cb->object.xs=x;
	cb->object.ys=y;
	cb->object.xe=x+16;
	cb->object.ye=y+16;
	
	cb->mode.sta=0;
	cb->mode.edge=4;
	
	cb->label=lcd_label_create(cb->object.xe+4,cb->object.ys,0x0000,string,father);
	u8 char_count=strlen(string);
	lcd_label_set_max_length(cb->label,char_count);
	sprintf(cb->label->string,"%s",string);
	
	return cb;

}



void lcd_cb_del(cb_t *cb)
{
	lcd_label_del(cb->label);
	free(cb);

}



void lcd_cb_enable_display(cb_t *cb)
{
	u16 x=(cb->object.xs+cb->object.xe)/2;
	u16 y=(cb->object.ys+cb->object.ye)/2;
	u16 w=cb->object.xe-cb->object.xs;
	u16 h=cb->object.ye-cb->object.ys;
	
	lcd_draw_RoundRect_border(x,y,w,h,0,cb->line_color);
	lcd_drawpure(cb->object.xs+1,cb->object.ys+1,w-1,h-1,cb->off_color,NULL);
	lcd_label_enable_display(cb->label);
	
	if(cb->mode.sta)
	{
		lcd_drawpure(cb->object.xs+cb->mode.edge,cb->object.ys+cb->mode.edge,w-2*cb->mode.edge+1,\
			h-2*cb->mode.edge+1,cb->on_color,NULL);
	}
	
	
}

void lcd_cb_disable_display(cb_t *cb)
{
	u16 w=cb->object.xe-cb->object.xs;
	u16 h=cb->object.ye-cb->object.ys;
	lcd_drawpure(cb->object.xs,cb->object.ys,w,h,0x0000,&cb->father_style);
	lcd_label_disable_display(cb->label);
}



u8 lcd_cb_ack(cb_t *cb)
{
	if(tp_is_sect(cb->object))
	{
		u16 w=cb->object.xe-cb->object.xs;
		u16 h=cb->object.ye-cb->object.ys;
		if(cb->mode.sta==0)
		{
			lcd_drawpure(cb->object.xs+cb->mode.edge,cb->object.ys+cb->mode.edge,w-2*cb->mode.edge+1,\
			h-2*cb->mode.edge+1,cb->on_color,NULL);
			cb->mode.sta=1;
			return 1;
		}
		else
		{
			lcd_drawpure(cb->object.xs+cb->mode.edge,cb->object.ys+cb->mode.edge,w-2*cb->mode.edge+1,\
			h-2*cb->mode.edge+1,cb->off_color,NULL);
			cb->mode.sta=0;
			return 0;
		}
	}else
	return 2;
}


void lcd_cb_set_sta(cb_t *cb,u8 sta)
{
	u16 w=cb->object.xe-cb->object.xs;
	u16 h=cb->object.ye-cb->object.ys;
	if(sta==1)
	{
		if(cb->mode.sta==0)
		{
			lcd_drawpure(cb->object.xs+cb->mode.edge,cb->object.ys+cb->mode.edge,w-2*cb->mode.edge+1,\
			h-2*cb->mode.edge+1,cb->on_color,NULL);
			cb->mode.sta=1;
		}
	}else if(sta==0)
	{
		if(cb->mode.sta==1)
		{
			lcd_drawpure(cb->object.xs+cb->mode.edge,cb->object.ys+cb->mode.edge,w-2*cb->mode.edge+1,\
			h-2*cb->mode.edge+1,cb->off_color,NULL);
			cb->mode.sta=0;
		}
	}

}



void lcd_cb_set_line_color(cb_t *cb,u16 color)
{
	cb->line_color=color;
}

	
void lcd_cb_set_color(cb_t *cb,u16 on_color,u16 off_color)
{
	cb->on_color=on_color;
	cb->off_color=off_color;
}

void lcd_cb_set_size(cb_t *cb,u8 size)
{
	if(size<16)
		return;
	else
	{
		cb->object.xe=cb->object.xs+size;
		cb->object.ye=cb->object.ys+size;
		
		cb->label->x=cb->object.xe+4;
		cb->label->y=(cb->object.ys+cb->object.ye)/2-8;
	}
}

void lcd_cb_set_edge(cb_t *cb,u8 edge)
{
	if(edge*2>cb->object.xe-cb->object.xs)
		return;
	if(edge>15)
		cb->mode.edge=15;
	cb->mode.edge=edge;
}
void lcd_cb_set_text_color(cb_t *cb,u16 color)
{
	cb->label->color=color;
}



/*************cbl**************/


lcb_t *lcd_lcb_create(u16 x,u16 y,char *string)
{
	lcb_t *lcb=(lcb_t*)malloc(sizeof(lcb_t));
	
	lcb->object.xs=x;
	lcb->object.ys=y;
	lcb->object.xe=x+16;
	lcb->object.ye=y+16;
	
	u8 char_count=strlen(string);
	lcb->string=(char*)malloc(sizeof(char)*char_count);
	sprintf(lcb->string,"%s",string);
	
	lcb->mode.sta=0;
	
	return lcb;
}


void lcd_lcb_del(lcb_t *lcb)
{
	free(lcb->string);
	free(lcb);
}


void lcd_lcb_enable_display(lcb_t *lcb,u16 off_color)
{
	u16 x=(lcb->object.xs+lcb->object.xe)/2;
	u16 y=(lcb->object.ys+lcb->object.ye)/2;
	u16 w=lcb->object.xe-lcb->object.xs;
	u16 h=lcb->object.ye-lcb->object.ys;
	
	lcd_draw_RoundRect_border(x,y,w,h,0,0x0000);
	lcd_drawpure(lcb->object.xs+1,lcb->object.ys+1,w-1,h-1,off_color,NULL);
	LCD_ShowString(lcb->object.xe+4,lcb->object.ys,lcb->string,0x0000,NULL);
}


u8 lcd_lcb_ack(lcb_t *lcb,u16 on_color,u16 off_color)
{
	if(tp_is_sect(lcb->object))
	{
		u16 w=lcb->object.xe-lcb->object.xs;
		u16 h=lcb->object.ye-lcb->object.ys;
		if(lcb->mode.sta==0)
		{
			lcd_drawpure(lcb->object.xs+4,lcb->object.ys+4,w-7,\
			h-7,on_color,NULL);
			lcb->mode.sta=1;
			return 1;
		}
		else
		{
			lcd_drawpure(lcb->object.xs+4,lcb->object.ys+4,w-7,\
			h-7,off_color,NULL);
			lcb->mode.sta=0;
			return 0;
		}
	}else
	return 2;
}


void lcd_lcb_set_active(lcb_t *lcb,u16 on_color,u16 off_color,u8 mode)
{
	u16 w=lcb->object.xe-lcb->object.xs;
	u16 h=lcb->object.ye-lcb->object.ys;
	lcb->mode.sta=mode;
	if(mode)
	{
		lcd_drawpure(lcb->object.xs+4,lcb->object.ys+4,w-7,\
			h-7,on_color,NULL);
	}else
		lcd_drawpure(lcb->object.xs+4,lcb->object.ys+4,w-7,\
			h-7,off_color,NULL);
}




cbl_t *lcd_cbl_create(u16 x,u16 y,char * map[],style_t style,style_t father)
{
	cbl_t *cbl=(cbl_t*)malloc(sizeof(cbl_t));
	
	cbl->father_style=father;
	
	cbl->off_color=0xffff;
	cbl->on_color=0xfa42;
	
	u8 index=0,max_count=0,char_count=0;
	while(map[index][0])
	{
		char_count=strlen(map[index]);
		if(char_count>max_count)
			max_count=char_count;
		index++;
	}
	
	cbl->object_count=index;
	
	
	cbl->object.xs=x;
	cbl->object.ys=y;
	cbl->object.xe=x+16+max_count*8+4+2*6;
	cbl->object.ye=y+(cbl->object_count+1)*5+cbl->object_count*16;
	
	
	cbl->style=style;
	cbl->style.h=cbl->object.ye-cbl->object.ys-2;
	cbl->style.hs=y;
	for(index=0;index<cbl->object_count;index++)
		cbl->lcb[index]=lcd_lcb_create(x+6,index*16+(index+1)*5+y,map[index]);

	return cbl; 

}


void lcd_cbl_del(cbl_t *cbl)
{
	u8 index;
	for(index=0;index<cbl->object_count;index++)
		lcd_lcb_del(cbl->lcb[index]);
	free(cbl);
}

void lcd_cbl_enable_display(cbl_t *cbl)
{
	lcd_object_enable_display(cbl->object,cbl->style,3,1,0x0000);
	u8 index;
	for(index=0;index<cbl->object_count;index++)
		lcd_lcb_enable_display(cbl->lcb[index],cbl->off_color);
}




void lcd_cbl_disable_display(cbl_t *cbl)
{
	lcd_object_disable_display(cbl->object,cbl->father_style,1);

}

u8 lcd_cbl_ack(cbl_t *cbl)
{
	if(tp_is_sect(cbl->object))
	{
		u8 flag=2,index=0;short flag_index=cbl->object_count;
		for(index=0;index<cbl->object_count;index++)
		{
			flag=lcd_lcb_ack(cbl->lcb[index],cbl->on_color,cbl->off_color);
			if(flag==0 || flag==1)
			{	
				flag_index=index;
				break;
			}
		}
		if(flag==0)
			return index;
		else
			return index|0x80;
	}else
	return cbl->object_count;
}



void lcd_cbl_set_color(cbl_t *cbl,u16 on_color,u16 off_color)
{
	cbl->on_color=on_color;
	cbl->off_color=off_color;
}

void lcd_cbl_set_active(cbl_t *cbl,u8 index,u8 mode)
{
	lcd_lcb_set_active(cbl->lcb[index],cbl->on_color,cbl->off_color,mode);
}


void lcd_cbl_sta_save(cbl_t *cbl)
{
	u8 index=0;
	for(index=0;index<cbl->object_count;index++)
	{
		if(cbl->lcb[index]->mode.sta==1)
			lcd_cbl_set_active(cbl,index,1);
	}

}

/***************dl*************************/


dl_t *lcd_dl_create(u16 x,u16 y,char * map[],style_t style,style_t father,u8 mode)
{
	dl_t *dl=(dl_t*)malloc(sizeof(dl_t));
	
	u8 index=0,count=0;
	while(map[index++][0])
		count++;
	
	dl->mode.data_num=count;
	dl->mode.data_mode=mode;
	
	dl->father_style=father;
	
	dl->style=style;
	dl->style.hs=y;
	dl->mode.column=0;
	
	
	dl->page.page_object=4;
	dl->page.current_page=0;
	if(dl->mode.data_num>dl->page.page_object)
		dl->page.page_flag=1;
	else
		dl->page.page_flag=0;
	dl->page.page_max=dl->mode.data_num/dl->page.page_object;
	
	for(u8 index=0;index<32;index++)
		dl->data_index[index].p=NULL;
		
	dl->string=map;
	if(mode==0)//mode0:多数据单值
	{
		u16 height=0;
		if(dl->page.page_flag)
			height=dl->page.page_object*16+(dl->page.page_object-1)*7+6;
		else
			height=dl->mode.data_num*16+(dl->mode.data_num-1)*7+6;
		u8 max_count=0,count=0;index=0;
		while(map[index][0])
		{
			count=strlen(map[index]);
			if(max_count<count)
				max_count=count;
			index++;
		}
		dl->data_name_maxl=max_count;
		u16 width=12+max_count*8+(dl->mode.column+1)*76;
		
		dl->style.h=height-2;
		
		dl->object.xs=x;
		dl->object.ys=y;
		dl->object.xe=dl->object.xs+width;
		dl->object.ye=dl->object.ys+height;
		
	}
	dl->data_label=lcd_label_create(0,0,0,"",dl->style);
	dl->name_label=lcd_label_create(0,0,0,"",dl->style);
	lcd_label_set_max_length(dl->data_label,8);
	lcd_label_set_max_length(dl->name_label,16);
	
	for(index=0;index<32;index++)
		dl->data_index[index].p=NULL;
	
	
	if(dl->page.page_flag)
	{
		style_t style_temp={0x13,0x51,0x3f,0x50,0xff,0xbf,0,0};
		
		dl->lbtn[0]=lcd_lbtn_create(dl->object.xs,dl->object.ye,"<-",style_temp,60);
		dl->lbtn[1]=lcd_lbtn_create(dl->object.xe-60,dl->object.ye,"->",style_temp,60);
		
		
		
		dl->page_label=lcd_label_create((dl->object.xe-dl->object.xs)/2+dl->object.xs-16,dl->object.ye+4,0x0000,"1\2",dl->father_style);
		sprintf(dl->page_label->string,"%u\\%u",dl->page.current_page+1,dl->page.page_max+1);
	}
	
	return dl;

}

void lcd_dl_del(dl_t *dl)
{
	lcd_label_del(dl->data_label);
	lcd_label_del(dl->name_label);
	free(dl);

}


void lcd_dl_enable_display(dl_t *dl)
{
	if(dl->mode.data_mode==0)
	{
		lcd_object_enable_display(dl->object,dl->style,6,1,0x0000);
		
		u8 index=0;
		if(dl->page.page_flag==0)
		{
			for(index=0;index<dl->mode.data_num;index++)
			{
				lcd_label_set_pos(dl->name_label,dl->object.xs+6,dl->object.ys+3+23*index);
				sprintf(dl->name_label->string,"%s",dl->string[index]);
				lcd_label_enable_display(dl->name_label);
				if(index<dl->mode.data_num-1)
				lcd_drawLine(dl->object.xs,dl->object.ys+22+index*23,dl->object.xe,dl->object.ys+22+index*23,0x0000,NULL);
			}
		}else
		{
			for(index=0;index<dl->page.page_object&&index+dl->page.current_page*dl->page.page_object<dl->mode.data_num;index++)
			{
				lcd_label_set_pos(dl->name_label,dl->object.xs+6,dl->object.ys+3+23*index);
				sprintf(dl->name_label->string,"%s",dl->string[index+dl->page.current_page*dl->page.page_object]);
				lcd_label_enable_display(dl->name_label);
			}
			
			for(index=0;index<dl->page.page_object;index++)
			{
				if(index<dl->page.page_object-1)
				lcd_drawLine(dl->object.xs,dl->object.ys+22+index*23,dl->object.xe,dl->object.ys+22+index*23,0x0000,NULL);
			}
			
		}
		lcd_drawLine(12+dl->data_name_maxl*8+dl->object.xs,dl->object.ys,12+dl->data_name_maxl*8+dl->object.xs,dl->object.ye-1,0x0000,NULL);
		for(index=0;index<dl->mode.column;index++)
			lcd_drawLine(12+dl->data_name_maxl*8+dl->object.xs+(index+1)*((dl->data_label->max_l)*8+12),dl->object.ys,\
		12+dl->data_name_maxl*8+dl->object.xs+(index+1)*((dl->data_label->max_l)*8+12),dl->object.ye-1,0,NULL);
	}
	if(dl->page.page_flag)
	{
		lcd_lbtn_enable_display(dl->lbtn[0]);
		lcd_lbtn_enable_display(dl->lbtn[1]);
		
		char t[6];
		sprintf(t,"%u\\%u",dl->page.current_page+1,dl->page.page_max+1);
		lcd_label_set_text(dl->page_label,t);
	}

}


void lcd_dl_disable_display(dl_t *dl)
{
	lcd_object_disable_display(dl->object,dl->father_style,6);
	if(dl->page.page_flag)
	{
		lcd_object_disable_display(dl->lbtn[0]->object,dl->father_style,6);
		lcd_object_disable_display(dl->lbtn[1]->object,dl->father_style,6);
			lcd_label_disable_display(dl->page_label);
	}
}

void lcd_dl_set_data_length(dl_t *dl,u8 l)
{
	if(l>16)return;
	dl->data_label->max_l=l;
	dl->object.xe=dl->object.xs+12+dl->data_name_maxl*8+(dl->mode.column+1)*((dl->data_label->max_l)*8+12);
	if(dl->page.page_flag)
	{
		dl->lbtn[1]->object.xs=dl->object.xe-60;
		dl->lbtn[1]->object.xe=dl->object.xe;
		
		lcd_label_set_pos(dl->page_label,(dl->object.xe-dl->object.xs)/2+dl->object.xs-16,dl->object.ye+4);
	}
}

void lcd_dl_set_column(dl_t *dl,u8 num)
{
	if(num>3)return;
	dl->mode.column=num;
	dl->object.xe=dl->object.xs+12+dl->data_name_maxl*8+(dl->mode.column+1)*((dl->data_label->max_l)*8+12);
	if(dl->page.page_flag)
	{
		dl->lbtn[1]->object.xs=dl->object.xe-60;
		dl->lbtn[1]->object.xe=dl->object.xe;
		
		lcd_label_set_pos(dl->page_label,(dl->object.xe-dl->object.xs)/2+dl->object.xs-16,dl->object.ye+4);
	}
}

void lcd_dl_set_data_index(dl_t *dl,void *p,u8 index_n,u8 type)
{
	if(index_n>dl->mode.data_num) return;
	dl->data_index[index_n].p=p;
	dl->data_index[index_n].sta.data_type=type;
}

void lcd_dl_set_data(dl_t *dl ,u8 index_n,DATA_T *data)
{
	if(index_n>32) return;
	if(dl->data_index[index_n].p==NULL) return;
	void * p=dl->data_index[index_n].p;
	if(dl->data_index[index_n].sta.data_type==int_type)
		*(int*)p=data->data_int;
	else if(dl->data_index[index_n].sta.data_type==float_type)
		*(float*)p=data->data_float;
	else if(dl->data_index[index_n].sta.data_type==unsigned_char_type)
		*(u8*)p=data->data_int;
	else if(dl->data_index[index_n].sta.data_type==unsigned_short_type)//uint16_t
		*(u16*)p=data->data_int;
	else if(dl->data_index[index_n].sta.data_type==double_type)//double
		*(double*)p=data->data_float;
	else if(dl->data_index[index_n].sta.data_type==long_type)//long
		*(long*)p=data->data_int;
}


void lcd_dl_sprintf(dl_t *dl,u8 index_n)
{
	
	if(dl->data_index[index_n].p==NULL) return;
	if(dl->data_index[index_n].sta.data_type==int_type)//int
		sprintf(dl->data_label->string,"%d",*(int*)dl->data_index[index_n].p);
	else if(dl->data_index[index_n].sta.data_type==float_type)//float
		sprintf(dl->data_label->string,"%f",*(float*)dl->data_index[index_n].p);
	else if(dl->data_index[index_n].sta.data_type==char_type)//char
		sprintf(dl->data_label->string,"%c",*(char*)dl->data_index[index_n].p);
	else if(dl->data_index[index_n].sta.data_type==string_type)//str
		sprintf(dl->data_label->string,"%s",(char*)dl->data_index[index_n].p);
	else if(dl->data_index[index_n].sta.data_type==unsigned_char_type)//uint8_t
		sprintf(dl->data_label->string,"%u",*(uint8_t*)dl->data_index[index_n].p);
	else if(dl->data_index[index_n].sta.data_type==unsigned_short_type)//uint16_t
		sprintf(dl->data_label->string,"%u",*(uint16_t*)dl->data_index[index_n].p);
	else if(dl->data_index[index_n].sta.data_type==double_type)//double
		sprintf(dl->data_label->string,"%lf",*(double*)dl->data_index[index_n].p);
	else if(dl->data_index[index_n].sta.data_type==long_type)//long
		sprintf(dl->data_label->string,"%ld",*(long*)dl->data_index[index_n].p);
	else if(dl->data_index[index_n].sta.data_type==long_long_type)//long
		sprintf(dl->data_label->string,"%lld",*(long long*)dl->data_index[index_n].p);
	
	dl->data_label->string[dl->data_label->max_l]=0;
}

void lcd_dl_data_enable_display(dl_t *dl,u8 index_list)
{
	if(dl->mode.data_mode==0)
	{
		u8 index=0;
		if(index_list>dl->mode.column)
		index_list=dl->mode.column;
		u8 count=0,dx=0;
			for(index=0;index<dl->page.page_object&&index+dl->page.current_page*dl->page.page_object<dl->mode.data_num;index++)
			{
				if(dl->data_index[index+dl->page.current_page*dl->page.page_object].sta.data_type!=0)
				{
					lcd_dl_sprintf(dl,index+dl->page.current_page*dl->page.page_object);
					count=strlen(dl->data_label->string)*8;
					dx=(12+dl->data_label->max_l*8-count)/2;
					lcd_label_set_pos(dl->data_label,dl->object.xs+12+dl->data_name_maxl*8+dx+index_list*((dl->data_label->max_l)*8+12),dl->object.ys+3+23*index);
					lcd_label_enable_display(dl->data_label);
				}					
			}
		
	}
}


void lcd_dl_data_disable_display(dl_t *dl,u8 index_list)
{
	if(dl->mode.data_mode==0)
	{
		if(index_list>dl->mode.column)
			index_list=dl->mode.column;
		u8 index=0;
		object_sect object;
		object.xs=dl->object.xs+dl->data_name_maxl*8+12+((dl->data_label->max_l)*8+12)*index_list+6;
		
		object.xe=object.xs+dl->data_label->max_l*8;
		if(dl->page.page_flag==0)
		{
			for(index=0;index<dl->mode.data_num;index++)
			{
					object.ys=dl->object.ys+3+index*22;
					object.ye=object.ys+18;
					lcd_object_disable_display(object,dl->style,0);
			}	
		}else
		{
			for(index=0;index<dl->page.page_object;index++)
			{
					object.ys=dl->object.ys+3+index*23;
					object.ye=object.ys+16;
					lcd_object_disable_display(object,dl->style,0);
			}	
		}
	}
}


void lcd_dl_data_update(dl_t *dl,u8 index_list)
{
	lcd_dl_data_disable_display(dl,index_list);
	lcd_dl_data_enable_display(dl,index_list);
}


void lcd_dl_turn_page(dl_t *dl,u8 ud)//0上1下
{
	if(dl->page.page_flag==0) return;
	if(ud==0 && dl->page.current_page==0) return;
	if(ud==1 && dl->page.current_page==dl->page.page_max) return;
	u8 index=0;
	for(index=0;index<dl->page.page_object&&index+dl->page.current_page*dl->page.page_object<dl->mode.data_num;index++)
	{
			lcd_label_set_pos(dl->name_label,dl->object.xs+6,dl->object.ys+3+23*index);
			sprintf(dl->name_label->string,"%s",dl->string[index+dl->page.current_page*dl->page.page_object]);
			lcd_label_disable_display(dl->name_label);
	}
	
	if(ud==0)
			dl->page.current_page--;
	else if(ud==1)
			dl->page.current_page++;

	for(index=0;index<dl->page.page_object&&index+dl->page.current_page*dl->page.page_object<dl->mode.data_num;index++)
	{
			lcd_label_set_pos(dl->name_label,dl->object.xs+6,dl->object.ys+3+23*index);
			sprintf(dl->name_label->string,"%s",dl->string[index+dl->page.page_object*dl->page.current_page]);
			lcd_label_enable_display(dl->name_label);
	}
	char t[6];
	sprintf(t,"%u\\%u",dl->page.current_page+1,dl->page.page_max+1);
	lcd_label_set_text(dl->page_label,t);
	lcd_dl_data_update(dl,0);
		
}
	

void lcd_dl_lbtn_action(dl_t *dl)
{
	style_t style_temp={0x8f,0x64,0x3a,0xa2,0xb1,0xf3,0,0};
	if(tp_is_sect(dl->lbtn[0]->object))
	{
			lcd_lbtn_ack(dl->lbtn[0],style_temp);
			lcd_dl_turn_page(dl,0);
	}
	if(tp_is_sect(dl->lbtn[1]->object))
	{
			lcd_lbtn_ack(dl->lbtn[1],style_temp);
			lcd_dl_turn_page(dl,1);
	}



}


void lcd_dl_set_page_object_num(dl_t *dl,u8 n)
{
	if(n<2 ||n>11) return;
	dl->page.page_object=n;
	if(dl->mode.data_num>dl->page.page_object)
		dl->page.page_flag=1;
	else
		dl->page.page_flag=0;
	dl->page.page_max=dl->mode.data_num/dl->page.page_object;
	
	u16 height=0;
		if(dl->page.page_flag)
			height=dl->page.page_object*16+(dl->page.page_object-1)*7+6;
		else
			height=dl->mode.data_num*16+(dl->mode.data_num-1)*7+6;
	dl->object.ye=dl->object.ys+height;
	dl->style.h=height-2;
		
	dl->data_label->father_style=dl->style;
	dl->name_label->father_style=dl->style;
		
	if(dl->page.page_flag)
	{
		dl->lbtn[0]->object.ys=dl->object.ye;
		dl->lbtn[0]->object.ye=dl->object.ye+24;
		
		dl->lbtn[1]->object.ys=dl->object.ye;
		dl->lbtn[1]->object.ye=dl->object.ye+24;
		
		lcd_label_set_pos(dl->page_label,(dl->object.xe-dl->object.xs)/2+dl->object.xs-16,dl->object.ye+4);
	
	}
		
}


/*******coord**************/
#define COORD_BACK lcd_makecolor(128,128,128)
#define COORD_PNE  0x0000
void lcd_coord_create(coord_t *coord,u16 x,u16 y,u16 w,u16 h,style_t *style)
{
	coord->object.xs=x;
	coord->object.ys=y;
	coord->object.xe=x+w;
	coord->object.ye=y+h;
	coord->latitude=NULL;
	coord->longitude=NULL;
	coord->count=0;
	coord->mode.grid_flag=0;
	
	coord->father_style=*style;
}

void lcd_coord_set_data(coord_t *coord,float *la,float *lo,u8 n)
{
	coord->count=n;
	coord->latitude=la;
	coord->longitude=lo;
}

void lcd_coord_draw_grid(coord_t *coord,u16 color)//画网格线
{
	u16 x=(coord->object.xs+coord->object.xe)/2;
	u16 y=(coord->object.ys+coord->object.ye)/2;
	u16 w=coord->object.xe-coord->object.xs;
	u16 h=coord->object.ye-coord->object.ys;

	u8 n=w/10;
	
	for(u8 i=1;i<n;i++)
	{
		x=coord->object.xs+i*10;
		y=coord->object.ye;
		lcd_drawcolumn(x,y-h,x,y,color,NULL);
	}
	
	n=h/10;
	for(u8 i=1;i<n;i++)
	{
		x=coord->object.xs;
		y=coord->object.ys+i*10;
		lcd_drawrow(x,y,x+w,y,color,NULL);
	}
	

}

void lcd_coord_enable_display(coord_t *coord)
{
	//axis
	u16 x=(coord->object.xs+coord->object.xe)/2;
	u16 y=(coord->object.ys+coord->object.ye)/2;
	u16 w=coord->object.xe-coord->object.xs;
	u16 h=coord->object.ye-coord->object.ys;
	
	lcd_drawpure(coord->object.xs+1,coord->object.ys+1,w-1,h-1,COORD_BACK,NULL);
	lcd_draw_RoundRect_border(x,y,w,h,0,0x0000);
	lcd_coord_draw_grid(coord,COORD_PNE);
	
}


void lcd_coord_disable_display(coord_t *coord)
{

	u16 w=coord->object.xe-coord->object.xs;
	u16 h=coord->object.ye-coord->object.ys;
	lcd_drawpure(coord->object.xs,coord->object.ys,w,h,0x0000,&coord->father_style);

}

void lcd_coord_draw_point(u16 x,u16 y,u16 color)
{
	u8 l=3;//点的大小
	for(u8 i=0;i<l;i++)
		for(u8 j=0;j<l;j++)
		LCD_DrawPoint(x+i,y+j,color);
}

void lcd_coord_pram_update(coord_t *coord)
{
	float min_latitude=coord->latitude[0];
	float max_latitude=coord->latitude[0];
	float min_longitude=coord->longitude[0];
	float max_longitude=coord->longitude[0];
	for(u8 i=1;i<coord->count;i++)
		if(coord->longitude[i]&&coord->latitude[i])//经纬度都有效
		{
			if(coord->latitude[i]>max_latitude)
				max_latitude=coord->latitude[i];
			if(coord->latitude[i]<min_latitude)
				min_latitude=coord->latitude[i];
			if(coord->longitude[i]>max_longitude)
				max_longitude=coord->longitude[i];
			if(coord->longitude[i]<min_longitude)
				min_longitude=coord->longitude[i];
		}
	coord->min_la=min_latitude;
	coord->min_lo=min_longitude;
	coord->la_h=max_latitude-min_latitude;
	coord->lo_w=max_longitude-min_longitude;
	lcd_coord_distance(coord,min_latitude,min_longitude,max_latitude,max_longitude);
		
}

#define ANGLE_TO_RAD(x)    ((x) * PI / 180.0)                                   // 角度转换为弧度
#define RAD_TO_ANGLE(x)    ((x) * 180.0 / PI)                                   // 弧度转换为角度
#define PI 3.1415926535
void lcd_coord_distance(coord_t *coord,float min_latitude, float min_longitude, float max_latitude, float	max_longitude)
{
    const double EARTH_RADIUS = 6378137;                                        // 地球半径(单位：m)
    double min_rad_latitude;
    double max_rad_latitude;
    double min_rad_longitude;
    double max_rad_longitude;
    double distance;
		double a;
    double b;

    min_rad_latitude = ANGLE_TO_RAD(min_latitude);                                    // 根据角度计算弧度
    max_rad_latitude = ANGLE_TO_RAD(max_latitude);
    min_rad_longitude = ANGLE_TO_RAD(min_longitude);
    max_rad_longitude = ANGLE_TO_RAD(max_longitude);

		u8 n=(coord->object.ye-coord->object.ys)/10;
    a=max_rad_latitude - min_rad_latitude;
		b=0;
		distance = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(min_rad_latitude) * cos(max_rad_latitude) * pow(sin(b / 2), 2))); 
		coord->dx=distance*EARTH_RADIUS/n; 
	
		n=(coord->object.xe-coord->object.xs)/10;
		a=0;
		b=max_rad_longitude-min_rad_longitude;
		distance = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(min_rad_latitude) * cos(max_rad_latitude) * pow(sin(b / 2), 2))); 
		coord->dy=distance*EARTH_RADIUS/n; 
}

void lcd_coord_draw(coord_t *coord,u8 cmd)//1:显示点，0:隐藏点
{
	u16 x0=coord->object.xs+1;
	u16 y0=coord->object.ye-4;
	u16 w=coord->object.xe-coord->object.xs-6;
	u16 h=coord->object.ye-coord->object.ys-6;
	u16 x=0,y=0;
	u16 color=0;
	float p;
	for(u8 i=0;i<coord->count;i++)
		if(coord->longitude[i]&&coord->latitude[i])//经纬度都有效
		{
			p=(coord->longitude[i]-coord->min_lo)/coord->lo_w;//x轴权重
			x=x0+w*p;
			p=(coord->latitude[i]-coord->min_la)/coord->la_h;//y轴权重
			y=y0-h*p;
			if(i==0)
				color=0x6743;
			else
				color=0xffff;
			if(cmd)
				lcd_coord_draw_point(x,y,color);	
			else
				lcd_coord_draw_point(x,y,COORD_BACK);	
		}
}


void lcd_coord_grid(coord_t *coord)
{
	if(coord->mode.grid_flag)//隐藏网格线
	{
		coord->mode.grid_flag=0;
		lcd_coord_draw_grid(coord,COORD_BACK);//隐藏网格线
		lcd_coord_draw(coord,1);//显示点
	}else//显示网格线
	{
		coord->mode.grid_flag=1;
		lcd_coord_draw_grid(coord,COORD_PNE);//显示网格线
		lcd_coord_draw(coord,1);//显示点
	}
}



void lcd_coord_single_point	(coord_t *coord,uint16 n,u16 color)
{
	if(n>=coord->count) return;
	u16 x0=coord->object.xs+1;
	u16 y0=coord->object.ye-4;
	u16 w=coord->object.xe-coord->object.xs-6;
	u16 h=coord->object.ye-coord->object.ys-6;
	float p=0;
	u16 x=0,y=0;
	if(coord->longitude[n]&&coord->latitude[n])//经纬度都有效
	{
		p=(coord->longitude[n]-coord->min_lo)/coord->lo_w;//x轴权重
		x=x0+w*p;
		p=(coord->latitude[n]-coord->min_la)/coord->la_h;//y轴权重
		y=y0-h*p;
		lcd_coord_draw_point(x,y,color);	
	}
}


void lcd_create_random_style(style_t *style)
{
	u16 seed=adc_mean_filter(ADC_1,ADC2_CH05_A05,1);
	srand(seed);
	style->backcolor.r=rand()%255;
	style->backcolor.g=rand()%1000;
	style->backcolor.b=rand()%400+style->backcolor.r+style->backcolor.g;
	
	seed=adc_mean_filter(ADC_1,ADC2_CH05_A05,1);
	srand(seed);
	style->gradcolor.r=rand()%800;
	style->gradcolor.g=rand()%255;
	style->gradcolor.b=rand()%400+style->backcolor.r+style->backcolor.g;
}






















