#ifndef __SPI_SOCKET_H__
#define __SPI_SOCKET_H__	 


#include <stdio.h>
#include <string.h>

/* SPI外设初始化由STM32CubeMX生成spi.c完成，使用时需包含其头文件spi.h */
#include "headfile.h"

/* SPI外设初始化由STM32CubeMX生成spi.c完成，需根据实际使用的SPI外设配置以下宏 */



/* SPI外设初始化由STM32CubeMX生成spi.c完成，需根据实际使用SPI外设的CS引脚配置以下宏 */



#define TF_MO B14
#define TF_MI B15
#define TF_CS B9
#define TF_CLK B12



#define TF_CLK_SET gpio_set(TF_CLK,1)
#define TF_MO_SET gpio_set(TF_MO,1)
/*****复位*****/
#define TF_CLK_CLE gpio_set(TF_CLK,0)
#define TF_MO_CLE gpio_set(TF_MO,0)
/*****读引脚*****/
#define TF_MI_GET gpio_get(TF_MI)



#define SPI_TF_CS_LOW   gpio_set(B9,0)	// CS输出低电平
#define SPI_TF_CS_HIGH  gpio_set(B9,1) 		// CS输出高电平







/* 移植时用户需要修改的接口函数 */
int8_t SD_SPI_SetSpeed(uint8_t SPI_BaudRatePrescaler);
uint8_t SD_ReadWriteByte(uint8_t Byte);
void tf_init(void);

#endif


