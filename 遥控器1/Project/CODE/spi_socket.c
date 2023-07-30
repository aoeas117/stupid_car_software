#include "spi_socket.h"		



/**
  * @brief  ����SPI�����ٶ�
  * @note   ��ֲʱ�û���Ҫ�޸ĵĽӿں���
  * @param  SPI_BaudRate_Prescaler: SPI���߷�Ƶϵ��, SPI_BAUDRATEPRESCALER_2 ~ SPI_BAUDRATEPRESCALER_256
  * @retval ��
  */
int8_t SD_SPI_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
	
	
	return 0;
}


/**
  * @brief  SPI���ߵ�CS��������
  * @note   ��
  * @param  ��
	* @retval 0: �ɹ�������: ʧ��
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
		TF_CLK_SET;//��������Ч	        
	}		 			    
} 		 




/**
  * @brief  ͨ��SPI���߶�дһ���ֽ�����
  * @note   ��
  * @param  txdata: д�������
  * @retval ��ȡ������
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
 		TF_CLK_SET;	//�½�����Ч  
 		if(TF_MI_GET)rxdata++; 		 
	}  	 

	SPI_TF_CS_HIGH;	
 	return rxdata;	
}







