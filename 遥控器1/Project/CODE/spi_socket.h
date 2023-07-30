#ifndef __SPI_SOCKET_H__
#define __SPI_SOCKET_H__	 


#include <stdio.h>
#include <string.h>

/* SPI�����ʼ����STM32CubeMX����spi.c��ɣ�ʹ��ʱ�������ͷ�ļ�spi.h */
#include "headfile.h"

/* SPI�����ʼ����STM32CubeMX����spi.c��ɣ������ʵ��ʹ�õ�SPI�����������º� */



/* SPI�����ʼ����STM32CubeMX����spi.c��ɣ������ʵ��ʹ��SPI�����CS�����������º� */



#define TF_MO B14
#define TF_MI B15
#define TF_CS B9
#define TF_CLK B12



#define TF_CLK_SET gpio_set(TF_CLK,1)
#define TF_MO_SET gpio_set(TF_MO,1)
/*****��λ*****/
#define TF_CLK_CLE gpio_set(TF_CLK,0)
#define TF_MO_CLE gpio_set(TF_MO,0)
/*****������*****/
#define TF_MI_GET gpio_get(TF_MI)



#define SPI_TF_CS_LOW   gpio_set(B9,0)	// CS����͵�ƽ
#define SPI_TF_CS_HIGH  gpio_set(B9,1) 		// CS����ߵ�ƽ







/* ��ֲʱ�û���Ҫ�޸ĵĽӿں��� */
int8_t SD_SPI_SetSpeed(uint8_t SPI_BaudRatePrescaler);
uint8_t SD_ReadWriteByte(uint8_t Byte);
void tf_init(void);

#endif


