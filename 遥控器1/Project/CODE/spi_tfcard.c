#include "spi_tfcard.h"		
#include "page.h"

/* SD����Ϣ */
SDCard_Information_typedef SDCard_Information;

/**
  * @brief  ȡ��ѡ��, �ͷ�SPI����
  * @note   ��
  * @param  ��
  * @retval ��
  */
void SD_DisSelect(void)
{
	SPI_TF_CS_HIGH;
 	SD_ReadWriteByte(0xFF); // ��ʱ8��ʱ��
}






/**
  * @brief  ѡ��SD��, �ȴ�SD��׼��
  * @note   SD������0x00ʱ��ʾæ������0xFF��ʾ׼������
  * @param  ��
  * @retval 0: �ɹ�, ����: ʧ��
  */
uint8_t SD_Select(void)
{
	SPI_TF_CS_LOW;
 	SD_ReadWriteByte(0xFF); // ��ʱ8��ʱ��
	
	if (SD_WaitReady() == 0)
	{
		return 0; // �ȴ��ɹ�
	}
	else
	{
		SD_DisSelect();
		return 1; // �ȴ�ʧ��
	}
}






/**
  * @brief  �ȴ�SD��׼��
  * @note   SD������0x00ʱ��ʾæ������0xFF��ʾ׼������
  * @param  ��
  * @retval 0: �ɹ�, ����: ʧ��
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
  * @brief  �ȴ�SD����Ӧ
  * @note   ��
  * @param  Response: ��Ӧֵ
  * @retval 0: �ɹ�, ����: ʧ��
  */
uint8_t SD_GetResponse(uint8_t Response)
{
	uint16_t count = 0x1FFF; //�ȴ�����
	
	while ((SD_ReadWriteByte(0xFF) != Response) && count)
	{
		count--;  // �ȴ��õ�׼ȷ�Ļ�Ӧ  	  
	}
	
	if (count == 0)		// ��ʱ�˳�
	{
		return MSD_RESPONSE_FAILURE;  // ��Ӧʧ��  
	}		
	else		// �����˳�
	{
		return MSD_RESPONSE_NO_ERROR; // ��Ӧ�ɹ�
	}
}


/**
  * @brief  ��������SD����ȡ����
  * @note   SD����1�������̶�Ϊ512�ֽ�
  * @param  buf: ���ݻ�����
  * @param  len: ���ݻ���������
  * @retval 0: �ɹ�, ����: ʧ��
  */
uint8_t SD_RecvData(uint8_t *buff, uint32_t len)
{			  	  
	if (SD_GetResponse(0xFE) != MSD_RESPONSE_NO_ERROR)    // �ȴ�SD������������ʼָ��0xFE
	{
		return 1;   // ��ȡʧ��
	}


  while (len--)  // ��ʼ��������
  {
		*buff = SD_ReadWriteByte(0xFF);   // ��������
    buff++;
  }
	SD_ReadWriteByte(0xFF);   // ����CRC
	SD_ReadWriteByte(0xFF);

  
  return 0;   // ��ȡ�ɹ�
}

	
/**
  * @brief  ��������SD����������
  * @note   SD����1�������̶�Ϊ512�ֽ�
  * @param  buff: ���ݻ�����
  * @param  cmd: ָ��
  * @retval 0: �ɹ�, ����: ʧ��
  */
uint8_t SD_SendBlock(uint8_t *buff, uint8_t cmd)
{	
	uint8_t retval;
	uint8_t crcdata[2] = {0xFF, 0xFF};
	
	if (SD_WaitReady() == 1)
	{
		return 1;  // �ȴ�׼��ʧЧ
	}
	
	SD_ReadWriteByte(cmd);
	if (cmd != 0xFD)   // ���ǽ���ָ��
	{
		
   
		uint16_t count;	
		for (count = 0; count < 512; count++)
		{
		  SD_ReadWriteByte(buff[count]);   // ��������
		}
	  SD_ReadWriteByte(crcdata[0]);   	// ����CRC
		SD_ReadWriteByte(crcdata[1]);

		
		retval = SD_ReadWriteByte(0xFF);   // ������Ӧ
		if ((retval & 0x1F) != MSD_DATA_OK)			 // ������ӦΪxxx00101
		{
			return 2;    // ��Ӧ����		
		}
		
		/* ��ʱ�ȴ�SD���ڲ�����д����ɣ�����Ĳ���ֻ�ǰ����ݷ��͵�SD�������������У���û��д�뵽SD�������� */
		if (SD_WaitReady() == 1)
		{
			return 1;  // �ȴ�׼��ʧЧ
		}

	}						 									  					    
  return 0;   // д��ɹ�
}


/**
  * @brief  ��SD����������
  * @note   ��
  * @param  cmd: ����
  * @param  arg: �������
  * @param  crc: CRC
  * @retval SD�����ص���Ӧ
  */
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  uint8_t retval;	
	uint8_t count = 0xFF; 
	
	SD_DisSelect();  // ȡ���ϴ�Ƭѡ
	if (SD_Select() == 1)
	{
		return 0xFF;  // ƬѡʧЧ 
	}


  SD_ReadWriteByte(cmd | 0x40);   // �ֱ�д������
  SD_ReadWriteByte(arg >> 24);
  SD_ReadWriteByte(arg >> 16);
  SD_ReadWriteByte(arg >> 8);
  SD_ReadWriteByte(arg);	  
  SD_ReadWriteByte(crc); 

	
	if (cmd == TF_CMD12)
	{
		SD_ReadWriteByte(0xFF);  // Skip a stuff byte when stop reading
	}
	
  // �ȴ���Ӧ, ��ʱ�˳�
	do
	{
		retval = SD_ReadWriteByte(0xFF);
	}
	while ((retval & 0x80) && count--);	 
	
  return retval;  	// ����״ֵ̬
}		


/**
  * @brief  ��ȡSD����CID��Ϣ
  * @note   ������������Ϣ
  * @param  cid: ���CID�Ļ�����������16Byte
  * @retval 0: �ɹ�, ����: ʧ��
  */
uint8_t SD_GetCID(uint8_t *cid)
{
	uint8_t retval;	
	
	retval = SD_SendCmd(TF_CMD10, 0, 0x39);  // ��CMD10����, ��CID
  if (retval == 0x00)
	{
		retval = SD_RecvData(cid, 16);  // ����16���ֽڵ�����	 
  }

	SD_DisSelect();  // ȡ��Ƭѡ
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
  * @brief  ��ȡSD����CSD��Ϣ
  * @note   �����������ٶ���Ϣ
  * @param  csd: ���CSD�Ļ�����������16Byte
  * @retval 0: �ɹ�, ����: ʧ��
  */
uint8_t SD_GetCSD(uint8_t *csd)
{
  uint8_t retval;	 
  retval = SD_SendCmd(TF_CMD9, 0, 0xAF);  // ��CMD9����, ��CSD
  if (retval == 0)
	{
    retval = SD_RecvData(csd, 16);   // ����16���ֽڵ����� 
  }
	
	SD_DisSelect();  // ȡ��Ƭѡ
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
  * @brief  ��ȡSD������������
  * @note   1��������512�ֽ�
  * @param  ��
  * @retval SD������������
  */
uint32_t SD_GetSectorCount(void)
{
  uint8_t csd[16];
  uint32_t capacity, csize, n;
	  
	// ��ȡCSD��Ϣ
  if (SD_GetCSD(csd) != 0)
	{
		return 0;
	}

  // ���ΪSDHC��, �������淽ʽ����
  if((csd[0] & 0xC0) == 0x40)	 // SDV2.0�Ŀ�
  {
	  csize = csd[9] + ((uint32_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1;
		capacity = csize << 9;
  }
	else   // SDV1.0�Ŀ�
  {	
	  n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
		capacity= (uint32_t)csize << (n - 9);  // �õ�������   
  }
	
	capacity = SD_GetCapacity() / 512;
  return capacity;
}


/**
  * @brief  ��ȡSD��������
  * @note   1��������512�ֽ�
  * @param  ��
  * @retval SD��������
  */
uint32_t SD_GetCapacity(void)
{
  uint8_t csd[16];
  uint32_t capacity;
  uint16_t n;
  uint32_t csize; 

  // ��ȡCSD��Ϣ
  if (SD_GetCSD(csd) != 0)
	{
		return 0;
	}	 

  if ((csd[0] & 0xC0) == 0x40)  // �ж�bit126�Ƿ�Ϊ1
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
  * @brief  ��ʼ��SD��
  * @note   SD����1�������̶�Ϊ512�ֽ�
  * @param  ��
  * @retval 0: �ɹ�, ����: ʧ��
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
		SD_ReadWriteByte(0xFF); 	// ��ʱ128��ʱ��
	}

	SPI_TF_CS_LOW;
	do
	{
		retval = SD_SendCmd(TF_CMD0, 0, 0x95); // �������״̬
	}
	while((retval != 0x01) && count--);
	
	
	char string[12];
	
	sprintf(string,"%u",retval);
	
	lcd_mbox_set_message(mbox2,string);
		
	
	
	if (retval == 0x01)
	{
		
		if (SD_SendCmd(TF_CMD8, 0x1AA, 0x87) == 0x01) // SD��V2.0
		{
			for (i = 0; i < 4; i++) 	// ����SD����������
			{
				rxbuff[i] = SD_ReadWriteByte(0xFF);
			}
			
			if (rxbuff[2] == 0x01 && rxbuff[3] == 0xAA)  // SD���Ƿ�֧��2.7~3.6V
			{
				count = 0x1FFF;
				do 	// �ȴ��˳�����ģʽ
				{
					SD_SendCmd(TF_CMD55, 0, 0x01);
					retval = SD_SendCmd(TF_CMD41, 0x40000000, 0x01);
				}
				while (retval && count--);
					
				
				if (count && SD_SendCmd(TF_CMD58, 0, 0x01) == 0)   // ����SD��2.0�汾��ʼ
				{
					for (i = 0; i < 4; i++)
					{
						rxbuff[i] = SD_ReadWriteByte(0xFF);  // �õ�OCRֵ
					}
					if (rxbuff[0] & 0x40)
					{
						SDCard_Information.Card_Type = TF_TYPE_SDHC;    // ���CCS
					}
					else 
					{
						SDCard_Information.Card_Type = TF_TYPE_SDV2;
					}
				}
			}
		}
		
		else  // SD��V1.0 / MMC
		{
			SD_SendCmd(TF_CMD55, 0, 0x01);
			retval = SD_SendCmd(TF_CMD41, 0, 0x01);
			if (retval <= 1)
			{		
				SDCard_Information.Card_Type = TF_TYPE_SDV1;
				count = 0x1FFF;
				do 	// �ȴ��˳�����ģʽ
				{
					SD_SendCmd(TF_CMD55, 0, 0x01);
					retval = SD_SendCmd(TF_CMD41, 0, 0x01);
				}
				while (retval && count--);
			}
			
			else  // MMC����֧��CMD55+CMD41ʶ��
			{
				SDCard_Information.Card_Type = TF_TYPE_MMC;   // MMC
				count = 0x1FFF;
				do
				{
					retval = SD_SendCmd(TF_CMD1, 0, 0x01);  // ����CMD1
				}
				while(retval && count--);
			}
			
			if(count == 0 || SD_SendCmd(TF_CMD16, 512, 0x01) != 0)
			{
				SDCard_Information.Card_Type = TF_TYPE_ERROR;  // ����Ŀ�
			}
		}
	}
	
	SD_DisSelect();  // ȡ��Ƭѡ
	//SD_SPI_SetSpeed(SPI_BaudRatePrescaler_4);   // ����20MHz
	
	if(SDCard_Information.Card_Type >= 1)
	{
		return 0;
	}
	else
	{
		return 0xEE;   // ��������
	}
}


/**
  * @brief  ��������ȡSD������
  * @note   SD����1�������̶�Ϊ512�ֽ�
  * @param  buff: ���ݻ�����
  * @param  sector: ��ʼ����
  * @param  cnt: ������
  * @retval 0: �ɹ�, ����: ʧ��
  */
uint8_t SD_ReadSector(uint8_t *buff, uint32_t sector, uint8_t cnt)
{
	uint8_t retval;

	if (SDCard_Information.Card_Type != TF_TYPE_SDHC)
	{
		sector *= 512;   // ת��Ϊ�ֽڵ�ַ
	}

	if (cnt == 1)
	{
		retval = SD_SendCmd(TF_CMD17, sector, 0x01);  // ������
		if(retval == 0)  // ָ��ͳɹ�
		{
			retval = SD_RecvData(buff, 512);   // ����512���ֽ�	   
		}
	}
	
	else
	{
		retval = SD_SendCmd(TF_CMD18, sector, 0x01);   // ����������
		do
		{
			retval = SD_RecvData(buff, 512);   // ����512���ֽ�	 
			buff += 512;
		}
		while (--cnt && retval == 0); 
		
		SD_SendCmd(TF_CMD12, 0, 0x01);	  // ����ֹͣ����
	}
	
	SD_DisSelect();  // ȡ��Ƭѡ
	return retval;
}


/**
  * @brief  ������д��SD������
  * @note   SD����1�������̶�Ϊ512�ֽ�
  * @param  buff: ���ݻ�����
  * @param  sector: ��ʼ����
  * @param  cnt: ������
  * @retval 0: �ɹ�, ����: ʧ��
  */
uint8_t SD_WriteSector(uint8_t *buff, uint32_t sector, uint32_t cnt)
{
	uint8_t retval;

	if (SDCard_Information.Card_Type != TF_TYPE_SDHC)
	{
		sector *= 512;  // ת��Ϊ�ֽڵ�ַ
	}
	
	if (cnt == 1)
	{
		retval = SD_SendCmd(TF_CMD24, sector, 0x01);  // д����
		if (retval == 0x00)  // ָ��ͳɹ�
		{
			retval = SD_SendBlock(buff, 0xFE);  // д512���ֽ�
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
		
 		retval = SD_SendCmd(TF_CMD25, sector, 0x01);  // ����д����
		if (retval == 0)
		{
			do
			{
				retval = SD_SendBlock(buff, 0xFC);  // ����512���ֽ�	 
				buff += 512;
			}
			while (--cnt && retval == 0);
			retval = SD_SendBlock(0, 0xFD);  // ����512���ֽ� 
		}
	}
	
	SD_DisSelect();  // ȡ��Ƭѡ
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
  * @brief  SD���������ģʽ
  * @note   ��
  * @param  ��
  * @retval 0: �ɹ�, ����: ʧ��
  */
uint8_t SD_Set_IdleMode(void)
{
	uint8_t retval;
	uint8_t count = 0x20;
	uint8_t i;
	
	SPI_TF_CS_HIGH;
	for (i = 0; i < 16; i++)
	{
		SD_ReadWriteByte(0xFF); // ��ʱ128��ʱ��
	}

	SPI_TF_CS_LOW;
	do
	{
		retval = SD_SendCmd(TF_CMD0, 0, 0x95); // �������״̬
	}
	while((retval != 0x01) && count--);
	
	return 0;
}



/**
  * @brief  SD���������ģʽ
  * @note   ��
  * @param  ��
	* @retval 0: �ɹ�������: ʧ��
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
		return 1;   // �ȴ�SD������������ʼָ��0xFE
	}
	
	for (i = 0; i < 64; i++)  // ��ʼ��������
  {
		temp[i] = SD_ReadWriteByte(0xFF); 	// ����SD����������
  }
	SD_ReadWriteByte(0xFF);   // ����CRC
	SD_ReadWriteByte(0xFF);   // ����CRC
	
	if ((temp[16] & 0x0F) != 0x01)
	{
		SD_SPI_SetSpeed(SPI_BaudRatePrescaler_4);   // 20MHz
		return 1;	// ��֧��HighSpeedMode
	}
	
	do
	{
		retval = SD_SendCmd(TF_CMD6, 0x80FFFFF1, 0x01);
	}
	while(retval && count--);
	
	if (SD_GetResponse(0xFE) != 0)
	{
		return 1;   // �ȴ�SD������������ʼָ��0xFE
	}
	
	for (i = 0; i < 64; i++)  // ��ʼ��������
  {
		temp[i] = SD_ReadWriteByte(0xFF); 	// ����SD����������
  }
	SD_ReadWriteByte(0xFF);   // ����CRC
	SD_ReadWriteByte(0xFF);   // ����CRC
	
	if ((temp[16] & 0x0F) != 0x01)
	{
		SD_SPI_SetSpeed(SPI_BaudRatePrescaler_4);   // 20MHz
		return 1;	// ��֧��HighSpeedMode
	}
	
	SD_SPI_SetSpeed(SPI_BaudRatePrescaler_2);   // 40MHz
	return retval;
}


/**
  * @brief  ��ӡSD�������ͺ�������Ϣ
  * @note   ���е�����printf������ע�����ͷ�ļ�stdio.h
  * @param  ��
	* @retval 0: �ɹ�������: ʧ��
  */
uint8_t SD_Information_Printf(void)
{
	/* ��ȡTF����С */
	SDCard_Information.Card_Capacity = SD_GetCapacity();
	
	/* �ж��Ƿ�ΪSD_XC�Ŀ� */
	if ((SDCard_Information.Card_Capacity / 1024 > 1024 * 32)
				&& (SDCard_Information.Card_Type == TF_TYPE_SDHC))
	{
		SDCard_Information.Card_Type = TF_TYPE_SDXC;
	}
	
	/* ��ӡTF������ */
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
	
	/* ��ӡTF������ */
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





