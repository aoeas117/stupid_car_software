#include "spi_socket.h"		



/**
  * @brief  设置SPI总线速度
  * @note   移植时用户需要修改的接口函数
  * @param  SPI_BaudRate_Prescaler: SPI总线分频系数, SPI_BAUDRATEPRESCALER_2 ~ SPI_BAUDRATEPRESCALER_256
  * @retval 无
  */
int8_t SD_SPI_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
	
	
	return 0;
}


/**
  * @brief  SPI总线的CS引脚配置
  * @note   无
  * @param  无
	* @retval 0: 成功，其他: 失败
  */


void tf_init(void)
{
	gpio_init(TF_CLK,GPO,1,GPO_PUSH_PULL);
	gpio_init(TF_CS,GPO,1,GPO_PUSH_PULL);
	gpio_init(TF_MO,GPO,1,GPO_PUSH_PULL);
	gpio_init(TF_MI,GPI,0,GPI_PULL_UP);



}


void TF_Write_Byte(uint8 num)    
{  
	uint8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TF_MO_SET;  
		else TF_MO_CLE;   
		num<<=1;  
		TF_CLK_CLE;		 
		systick_delay_us(5);
		TF_CLK_SET;//上升沿有效	        
	}		 			    
} 		 




/**
  * @brief  通过SPI总线读写一个字节数据
  * @note   无
  * @param  txdata: 写入的数据
  * @retval 读取的数据
  */
uint8_t SD_ReadWriteByte(uint8_t Byte)
{
	uint8_t rxdata; 
	TF_CLK_SET;		
	TF_MO_CLE; 	
	SPI_TF_CS_LOW; 		
	TF_Write_Byte(Byte);
	for(uint8 count=0;count<8;count++)
	{ 				  
		rxdata<<=1; 	
		TF_CLK_CLE;		 
		systick_delay_us(5);    
 		TF_CLK_SET;	//下降沿有效  
 		if(TF_MI_GET)rxdata++; 		 
	}  	 

	SPI_TF_CS_HIGH;	
 	return rxdata;	
}







