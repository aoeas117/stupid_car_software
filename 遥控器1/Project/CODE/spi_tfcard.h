#ifndef __SPI_TFCARD_H__
#define __SPI_TFCARD_H__	 

#include "spi_socket.h"










#define SPI_DMA_READ_SECTOR			   // ����SPIʹ��DMA�������ݶ�����
#define SPI_DMA_WRITE_SECTOR			 // ����SPIʹ��DMA��������д����
#define SPI_DMA_SEND_CMD           // ����SPIʹ��DMA�������ݷ���CMD


// TF�����Ͷ���  
#define  TF_TYPE_ERROR     0x00
#define  TF_TYPE_MMC       0x01
#define  TF_TYPE_SDV1      0x02
#define  TF_TYPE_SDV2      0x04
#define  TF_TYPE_SDHC      0x06
#define  TF_TYPE_SDXC      0x08

// TF��ָ���  	   
#define  TF_CMD0    0       // ����0 ��SD���������״̬
#define  TF_CMD1    1       // ����1 ��SD�����빤��״̬
#define  TF_CMD6    6       // ����6 ��SD���������ģʽ50MHz
#define  TF_CMD8    8       // ����8 ��SEND_IF_COND
#define  TF_CMD9    9       // ����9 ����CSD����
#define  TF_CMD10   10      // ����10����CID����
#define  TF_CMD12   12      // ����12��ֹͣ���ݴ���
#define  TF_CMD13   13      // ����13����״̬
#define  TF_CMD16   16      // ����16�����ÿ��С Ӧ����0x00
#define  TF_CMD17   17      // ����17������������
#define  TF_CMD18   18      // ����18�����������
#define  TF_CMD23   23      // ����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define  TF_CMD24   24      // ����24��д��������
#define  TF_CMD25   25      // ����25��д�������
#define  TF_CMD32   32      // ����32������Ҫ��������ʼ��ַ
#define  TF_CMD33   33      // ����33������Ҫ�����Ľ�����ַ
#define  TF_CMD38   38      // ����38������ָ�����������
#define  TF_CMD41   41      // ����41��Ӧ����0x00
#define  TF_CMD55   55      // ����55��Ӧ����0x01
#define  TF_CMD58   58      // ����58����OCR��Ϣ
#define  TF_CMD59   59      // ����59��ʹ��/��ֹCRC��Ӧ����0x00

// ����д���Ӧ������
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF

// TF����Ӧ�����
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF


/* SD����Ϣ */
typedef struct
{
  uint8_t Card_Type;
  uint32_t Card_Capacity;
	/* �û��������... */
	
} SDCard_Information_typedef;

/* SD��API */
uint8_t  SD_WaitReady(void);									// �ȴ�SD��׼��
uint8_t  SD_GetResponse(uint8_t Response);		// ��ȡSD����Ӧ
uint8_t  SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc);		// CMDָ���
uint8_t  SD_RecvData(uint8_t *buff, uint32_t len);
uint8_t  SD_SendBlock(uint8_t *buff, uint8_t cmd);
uint8_t  SD_GetCID(uint8_t *cid_data);        // ��ȡSD��CID
uint8_t  SD_GetCSD(uint8_t *csd_data);        // ��ȡSD��CSD
uint32_t SD_GetSectorCount(void);   					// ��ȡSD��������
uint32_t SD_GetCapacity(void);								// ��ȡSD������
uint8_t  SD_Set_IdleMode(void);								// SD���������ģʽ
uint8_t  SD_Set_HighSpeedMode(void);					// SD���������ģʽ
uint8_t  SD_Information_Printf(void);					// ��ӡSD�������ͺ�������Ϣ
uint8_t  SD_GetCardState(void);               // ��ȡSD��״̬

uint8_t  SD_Card_Init(void);									// SD����ʼ��
uint8_t  SD_ReadSector(uint8_t *buff, uint32_t sector, uint8_t cnt);		  // ��������ȡSD������
uint8_t  SD_WriteSector(uint8_t *buff, uint32_t sector, uint32_t cnt);		// ������д��SD������

#endif


