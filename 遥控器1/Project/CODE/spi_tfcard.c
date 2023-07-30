#include "spi_tfcard.h"		
#include "page.h"

/* SD卡信息 */
SDCard_Information_typedef SDCard_Information;

/**
  * @brief  取消选择, 释放SPI总线
  * @note   无
  * @param  无
  * @retval 无
  */
void SD_DisSelect(void)
{
	SPI_TF_CS_HIGH;
 	SD_ReadWriteByte(0xFF); // 延时8个时钟
}






/**
  * @brief  选中SD卡, 等待SD卡准备
  * @note   SD卡返回0x00时表示忙，返回0xFF表示准备就绪
  * @param  无
  * @retval 0: 成功, 其他: 失败
  */
uint8_t SD_Select(void)
{
	SPI_TF_CS_LOW;
 	SD_ReadWriteByte(0xFF); // 延时8个时钟
	
	if (SD_WaitReady() == 0)
	{
		return 0; // 等待成功
	}
	else
	{
		SD_DisSelect();
		return 1; // 等待失败
	}
}






/**
  * @brief  等待SD卡准备
  * @note   SD卡返回0x00时表示忙，返回0xFF表示准备就绪
  * @param  无
  * @retval 0: 成功, 其他: 失败
  */
uint8_t SD_WaitReady(void)
{
	uint16_t count = 0;
	
	do
	{
		if (SD_ReadWriteByte(0xFF) == 0xFF)
		{
			return 0;
		}
		count++;		  	
	}
	while (count < 0xFFF0); 
	
	return 1;
}


/**
  * @brief  等待SD卡回应
  * @note   无
  * @param  Response: 回应值
  * @retval 0: 成功, 其他: 失败
  */
uint8_t SD_GetResponse(uint8_t Response)
{
	uint16_t count = 0x1FFF; //等待次数
	
	while ((SD_ReadWriteByte(0xFF) != Response) && count)
	{
		count--;  // 等待得到准确的回应  	  
	}
	
	if (count == 0)		// 超时退出
	{
		return MSD_RESPONSE_FAILURE;  // 回应失败  
	}		
	else		// 正常退出
	{
		return MSD_RESPONSE_NO_ERROR; // 回应成功
	}
}


/**
  * @brief  按扇区从SD卡读取数据
  * @note   SD卡的1个扇区固定为512字节
  * @param  buf: 数据缓存区
  * @param  len: 数据缓存区长度
  * @retval 0: 成功, 其他: 失败
  */
uint8_t SD_RecvData(uint8_t *buff, uint32_t len)
{			  	  
	if (SD_GetResponse(0xFE) != MSD_RESPONSE_NO_ERROR)    // 等待SD卡发回数据起始指令0xFE
	{
		return 1;   // 读取失败
	}


  while (len--)  // 开始接收数据
  {
		*buff = SD_ReadWriteByte(0xFF);   // 接收数据
    buff++;
  }
	SD_ReadWriteByte(0xFF);   // 忽略CRC
	SD_ReadWriteByte(0xFF);

  
  return 0;   // 读取成功
}

	
/**
  * @brief  按扇区向SD卡发送数据
  * @note   SD卡的1个扇区固定为512字节
  * @param  buff: 数据缓存区
  * @param  cmd: 指令
  * @retval 0: 成功, 其他: 失败
  */
uint8_t SD_SendBlock(uint8_t *buff, uint8_t cmd)
{	
	uint8_t retval;
	uint8_t crcdata[2] = {0xFF, 0xFF};
	
	if (SD_WaitReady() == 1)
	{
		return 1;  // 等待准备失效
	}
	
	SD_ReadWriteByte(cmd);
	if (cmd != 0xFD)   // 不是结束指令
	{
		
   
		uint16_t count;	
		for (count = 0; count < 512; count++)
		{
		  SD_ReadWriteByte(buff[count]);   // 发送数据
		}
	  SD_ReadWriteByte(crcdata[0]);   	// 忽略CRC
		SD_ReadWriteByte(crcdata[1]);

		
		retval = SD_ReadWriteByte(0xFF);   // 接收响应
		if ((retval & 0x1F) != MSD_DATA_OK)			 // 正常响应为xxx00101
		{
			return 2;    // 响应错误		
		}
		
		/* 延时等待SD卡内部数据写入完成，上面的操作只是把数据发送到SD卡控制器缓存中，还没有写入到SD卡闪存内 */
		if (SD_WaitReady() == 1)
		{
			return 1;  // 等待准备失效
		}

	}						 									  					    
  return 0;   // 写入成功
}


/**
  * @brief  向SD卡发送命令
  * @note   无
  * @param  cmd: 命令
  * @param  arg: 命令参数
  * @param  crc: CRC
  * @retval SD卡返回的响应
  */
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  uint8_t retval;	
	uint8_t count = 0xFF; 
	
	SD_DisSelect();  // 取消上次片选
	if (SD_Select() == 1)
	{
		return 0xFF;  // 片选失效 
	}


  SD_ReadWriteByte(cmd | 0x40);   // 分别写入命令
  SD_ReadWriteByte(arg >> 24);
  SD_ReadWriteByte(arg >> 16);
  SD_ReadWriteByte(arg >> 8);
  SD_ReadWriteByte(arg);	  
  SD_ReadWriteByte(crc); 

	
	if (cmd == TF_CMD12)
	{
		SD_ReadWriteByte(0xFF);  // Skip a stuff byte when stop reading
	}
	
  // 等待响应, 或超时退出
	do
	{
		retval = SD_ReadWriteByte(0xFF);
	}
	while ((retval & 0x80) && count--);	 
	
  return retval;  	// 返回状态值
}		


/**
  * @brief  获取SD卡的CID信息
  * @note   包括制造商信息
  * @param  cid: 存放CID的缓冲区，至少16Byte
  * @retval 0: 成功, 其他: 失败
  */
uint8_t SD_GetCID(uint8_t *cid)
{
	uint8_t retval;	
	
	retval = SD_SendCmd(TF_CMD10, 0, 0x39);  // 发CMD10命令, 读CID
  if (retval == 0x00)
	{
		retval = SD_RecvData(cid, 16);  // 接收16个字节的数据	 
  }

	SD_DisSelect();  // 取消片选
	if (retval == 1)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}		

	
/**
  * @brief  获取SD卡的CSD信息
  * @note   包括容量和速度信息
  * @param  csd: 存放CSD的缓冲区，至少16Byte
  * @retval 0: 成功, 其他: 失败
  */
uint8_t SD_GetCSD(uint8_t *csd)
{
  uint8_t retval;	 
  retval = SD_SendCmd(TF_CMD9, 0, 0xAF);  // 发CMD9命令, 读CSD
  if (retval == 0)
	{
    retval = SD_RecvData(csd, 16);   // 接收16个字节的数据 
  }
	
	SD_DisSelect();  // 取消片选
	if (retval == 1)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}


/**
  * @brief  获取SD卡的总扇区数
  * @note   1扇区等于512字节
  * @param  无
  * @retval SD卡的总扇区数
  */
uint32_t SD_GetSectorCount(void)
{
  uint8_t csd[16];
  uint32_t capacity, csize, n;
	  
	// 获取CSD信息
  if (SD_GetCSD(csd) != 0)
	{
		return 0;
	}

  // 如果为SDHC卡, 按照下面方式计算
  if((csd[0] & 0xC0) == 0x40)	 // SDV2.0的卡
  {
	  csize = csd[9] + ((uint32_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1;
		capacity = csize << 9;
  }
	else   // SDV1.0的卡
  {	
	  n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
		capacity= (uint32_t)csize << (n - 9);  // 得到扇区数   
  }
	
	capacity = SD_GetCapacity() / 512;
  return capacity;
}


/**
  * @brief  获取SD卡的容量
  * @note   1扇区等于512字节
  * @param  无
  * @retval SD卡的容量
  */
uint32_t SD_GetCapacity(void)
{
  uint8_t csd[16];
  uint32_t capacity;
  uint16_t n;
  uint32_t csize; 

  // 获取CSD信息
  if (SD_GetCSD(csd) != 0)
	{
		return 0;
	}	 

  if ((csd[0] & 0xC0) == 0x40)  // 判断bit126是否为1
  { 
    csize = csd[9] + ((uint32_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1;
    capacity = csize << 9; 
  }
  else
  { 
    n = (csd[5] & 0x0F) + ((csd[10] & 0x80) >> 7) + ((csd[9] & 0x03) << 1) + 2;
    csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 0x03) << 10) + 1;
    capacity = (uint32_t)csize << (n - 10);
  }
  return capacity;
} 


/**
  * @brief  初始化SD卡
  * @note   SD卡的1个扇区固定为512字节
  * @param  无
  * @retval 0: 成功, 其他: 失败
  */
uint8_t SD_Card_Init(void)
{
	uint8_t retval;
	uint16_t count = 0x20;
	uint8_t rxbuff[4];  
	uint8_t i;
	SDCard_Information.Card_Type = TF_TYPE_ERROR;

	tf_init();
	
	for (i = 0; i < 16; i++)
	{
		SD_ReadWriteByte(0xFF); 	// 延时128个时钟
	}

	SPI_TF_CS_LOW;
	do
	{
		retval = SD_SendCmd(TF_CMD0, 0, 0x95); // 进入空闲状态
	}
	while((retval != 0x01) && count--);
	
	
	char string[12];
	
	sprintf(string,"%u",retval);
	
	lcd_mbox_set_message(mbox2,string);
		
	
	
	if (retval == 0x01)
	{
		
		if (SD_SendCmd(TF_CMD8, 0x1AA, 0x87) == 0x01) // SD卡V2.0
		{
			for (i = 0; i < 4; i++) 	// 接收SD卡返回数据
			{
				rxbuff[i] = SD_ReadWriteByte(0xFF);
			}
			
			if (rxbuff[2] == 0x01 && rxbuff[3] == 0xAA)  // SD卡是否支持2.7~3.6V
			{
				count = 0x1FFF;
				do 	// 等待退出空闲模式
				{
					SD_SendCmd(TF_CMD55, 0, 0x01);
					retval = SD_SendCmd(TF_CMD41, 0x40000000, 0x01);
				}
				while (retval && count--);
					
				
				if (count && SD_SendCmd(TF_CMD58, 0, 0x01) == 0)   // 鉴别SD卡2.0版本开始
				{
					for (i = 0; i < 4; i++)
					{
						rxbuff[i] = SD_ReadWriteByte(0xFF);  // 得到OCR值
					}
					if (rxbuff[0] & 0x40)
					{
						SDCard_Information.Card_Type = TF_TYPE_SDHC;    // 检查CCS
					}
					else 
					{
						SDCard_Information.Card_Type = TF_TYPE_SDV2;
					}
				}
			}
		}
		
		else  // SD卡V1.0 / MMC
		{
			SD_SendCmd(TF_CMD55, 0, 0x01);
			retval = SD_SendCmd(TF_CMD41, 0, 0x01);
			if (retval <= 1)
			{		
				SDCard_Information.Card_Type = TF_TYPE_SDV1;
				count = 0x1FFF;
				do 	// 等待退出空闲模式
				{
					SD_SendCmd(TF_CMD55, 0, 0x01);
					retval = SD_SendCmd(TF_CMD41, 0, 0x01);
				}
				while (retval && count--);
			}
			
			else  // MMC卡不支持CMD55+CMD41识别
			{
				SDCard_Information.Card_Type = TF_TYPE_MMC;   // MMC
				count = 0x1FFF;
				do
				{
					retval = SD_SendCmd(TF_CMD1, 0, 0x01);  // 发送CMD1
				}
				while(retval && count--);
			}
			
			if(count == 0 || SD_SendCmd(TF_CMD16, 512, 0x01) != 0)
			{
				SDCard_Information.Card_Type = TF_TYPE_ERROR;  // 错误的卡
			}
		}
	}
	
	SD_DisSelect();  // 取消片选
	//SD_SPI_SetSpeed(SPI_BaudRatePrescaler_4);   // 高速20MHz
	
	if(SDCard_Information.Card_Type >= 1)
	{
		return 0;
	}
	else
	{
		return 0xEE;   // 其他错误
	}
}


/**
  * @brief  按扇区读取SD卡数据
  * @note   SD卡的1个扇区固定为512字节
  * @param  buff: 数据缓冲区
  * @param  sector: 起始扇区
  * @param  cnt: 扇区数
  * @retval 0: 成功, 其他: 失败
  */
uint8_t SD_ReadSector(uint8_t *buff, uint32_t sector, uint8_t cnt)
{
	uint8_t retval;

	if (SDCard_Information.Card_Type != TF_TYPE_SDHC)
	{
		sector *= 512;   // 转换为字节地址
	}

	if (cnt == 1)
	{
		retval = SD_SendCmd(TF_CMD17, sector, 0x01);  // 读命令
		if(retval == 0)  // 指令发送成功
		{
			retval = SD_RecvData(buff, 512);   // 接收512个字节	   
		}
	}
	
	else
	{
		retval = SD_SendCmd(TF_CMD18, sector, 0x01);   // 连续读命令
		do
		{
			retval = SD_RecvData(buff, 512);   // 接收512个字节	 
			buff += 512;
		}
		while (--cnt && retval == 0); 
		
		SD_SendCmd(TF_CMD12, 0, 0x01);	  // 发送停止命令
	}
	
	SD_DisSelect();  // 取消片选
	return retval;
}


/**
  * @brief  按扇区写入SD卡数据
  * @note   SD卡的1个扇区固定为512字节
  * @param  buff: 数据缓冲区
  * @param  sector: 起始扇区
  * @param  cnt: 扇区数
  * @retval 0: 成功, 其他: 失败
  */
uint8_t SD_WriteSector(uint8_t *buff, uint32_t sector, uint32_t cnt)
{
	uint8_t retval;

	if (SDCard_Information.Card_Type != TF_TYPE_SDHC)
	{
		sector *= 512;  // 转换为字节地址
	}
	
	if (cnt == 1)
	{
		retval = SD_SendCmd(TF_CMD24, sector, 0x01);  // 写命令
		if (retval == 0x00)  // 指令发送成功
		{
			retval = SD_SendBlock(buff, 0xFE);  // 写512个字节
		}
		
		else
		{
			SD_Card_Init();
		}
	}
	
	else
	{
		if (SDCard_Information.Card_Type != TF_TYPE_MMC)
		{
			SD_SendCmd(TF_CMD55, 0, 0x01);	
			SD_SendCmd(TF_CMD23, cnt, 0x01);
		}
		
 		retval = SD_SendCmd(TF_CMD25, sector, 0x01);  // 连续写命令
		if (retval == 0)
		{
			do
			{
				retval = SD_SendBlock(buff, 0xFC);  // 发送512个字节	 
				buff += 512;
			}
			while (--cnt && retval == 0);
			retval = SD_SendBlock(0, 0xFD);  // 接收512个字节 
		}
	}
	
	SD_DisSelect();  // 取消片选
	return retval;
}


/**
  * @brief  Returns the SD status.
  * @param  None
  * @retval The SD status.
  */
uint8_t SD_GetCardState(void)
{
  uint8_t retval[2];

  /* Send CMD13 (SD_SEND_STATUS) to get SD status */
  retval[0] = SD_SendCmd(TF_CMD13, 0, 0xFF);
	retval[1] = SD_ReadWriteByte(0xFF);
  SD_DisSelect();

  /* Find SD status according to card state */
  if (( retval[0] == 0) && ( retval[1] == 0))
  {
    return 0;
  }
	else
	{
		return 1;
	}
}

/**
  * @brief  SD卡进入空闲模式
  * @note   无
  * @param  无
  * @retval 0: 成功, 其他: 失败
  */
uint8_t SD_Set_IdleMode(void)
{
	uint8_t retval;
	uint8_t count = 0x20;
	uint8_t i;
	
	SPI_TF_CS_HIGH;
	for (i = 0; i < 16; i++)
	{
		SD_ReadWriteByte(0xFF); // 延时128个时钟
	}

	SPI_TF_CS_LOW;
	do
	{
		retval = SD_SendCmd(TF_CMD0, 0, 0x95); // 进入空闲状态
	}
	while((retval != 0x01) && count--);
	
	return 0;
}



/**
  * @brief  SD卡进入高速模式
  * @note   无
  * @param  无
	* @retval 0: 成功，其他: 失败
  */
uint8_t SD_Set_HighSpeedMode(void)
{
	uint8_t i;
	uint8_t retval;
	uint8_t temp[64];
	uint8_t count = 0xFF;

	SD_SPI_SetSpeed(SPI_BaudRatePrescaler_256);	// 312.5KHz = 0.3125MHz
	
	do
	{
		retval = SD_SendCmd(TF_CMD6, 0x00FFFFF1, 0x01);
	}
	while(retval && count--);
	
	if (SD_GetResponse(0xFE) != 0)
	{
		return 1;   // 等待SD卡发回数据起始指令0xFE
	}
	
	for (i = 0; i < 64; i++)  // 开始接收数据
  {
		temp[i] = SD_ReadWriteByte(0xFF); 	// 接收SD卡返回数据
  }
	SD_ReadWriteByte(0xFF);   // 接收CRC
	SD_ReadWriteByte(0xFF);   // 接收CRC
	
	if ((temp[16] & 0x0F) != 0x01)
	{
		SD_SPI_SetSpeed(SPI_BaudRatePrescaler_4);   // 20MHz
		return 1;	// 不支持HighSpeedMode
	}
	
	do
	{
		retval = SD_SendCmd(TF_CMD6, 0x80FFFFF1, 0x01);
	}
	while(retval && count--);
	
	if (SD_GetResponse(0xFE) != 0)
	{
		return 1;   // 等待SD卡发回数据起始指令0xFE
	}
	
	for (i = 0; i < 64; i++)  // 开始接收数据
  {
		temp[i] = SD_ReadWriteByte(0xFF); 	// 接收SD卡返回数据
  }
	SD_ReadWriteByte(0xFF);   // 接收CRC
	SD_ReadWriteByte(0xFF);   // 接收CRC
	
	if ((temp[16] & 0x0F) != 0x01)
	{
		SD_SPI_SetSpeed(SPI_BaudRatePrescaler_4);   // 20MHz
		return 1;	// 不支持HighSpeedMode
	}
	
	SD_SPI_SetSpeed(SPI_BaudRatePrescaler_2);   // 40MHz
	return retval;
}


/**
  * @brief  打印SD卡的类型和容量信息
  * @note   其中调用了printf函数，注意包含头文件stdio.h
  * @param  无
	* @retval 0: 成功，其他: 失败
  */
uint8_t SD_Information_Printf(void)
{
	/* 获取TF卡大小 */
	SDCard_Information.Card_Capacity = SD_GetCapacity();
	
	/* 判断是否为SD_XC的卡 */
	if ((SDCard_Information.Card_Capacity / 1024 > 1024 * 32)
				&& (SDCard_Information.Card_Type == TF_TYPE_SDHC))
	{
		SDCard_Information.Card_Type = TF_TYPE_SDXC;
	}
	
	/* 打印TF卡类型 */
	if (SDCard_Information.Card_Type == TF_TYPE_MMC)
	{
		printf("\r\nSD_MMC,");
	}
	else if (SDCard_Information.Card_Type == TF_TYPE_SDXC)
	{
		printf("\r\nSD_XC,");
	}
	else if (SDCard_Information.Card_Type == TF_TYPE_SDHC)
	{
		printf("\r\nSD_HC,");
	}
	else if (SDCard_Information.Card_Type == TF_TYPE_SDV2)
	{
		printf("\r\nSD_V2,");
	}
	else if (SDCard_Information.Card_Type == TF_TYPE_SDV1)
	{
		printf("\r\nSD_V1,");
	}
	else
	{
		printf("\r\nSD ERROR,");
	}
	
	/* 打印TF卡容量 */
	if (SDCard_Information.Card_Capacity / 1024 > 1024)
	{
		printf("Capacity:%4.1lfGB\r\n", (float)SDCard_Information.Card_Capacity / 1024 / 1024);
	}
	else
	{
		printf("Capacity:%4.1lfMB\r\n", (float)SDCard_Information.Card_Capacity / 1024);
	}
	
	return 0;
}





