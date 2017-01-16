#ifndef __SPI_MMC_H__
#define __SPI_MMC_H__
/* SPI select pin */
#define SPI1_SEL 0x00100000
/* The SPI data is 8 bit long, the MMC use 48 bits, 6 bytes */
#define MMC_CMD_SIZE 6
/* The max MMC flash size is 256MB */
#define MMC_DATA_SIZE 512 /* 16-bit in size, 512 bytes */
#define MAX_TIMEOUT 0xFF
#define IDLE_STATE_TIMEOUT 1
#define OP_COND_TIMEOUT 2
#define SET_BLOCKLEN_TIMEOUT 3
#define WRITE_BLOCK_TIMEOUT 4
#define WRITE_BLOCK_FAIL 5
#define READ_BLOCK_TIMEOUT 6
#define READ_BLOCK_DATA_TOKEN_MISSING 7
#define DATA_TOKEN_TIMEOUT 8
#define SELECT_CARD_TIMEOUT 9
#define SET_RELATIVE_ADDR_TIMEOUT 10


void SPI1_Init( void );
void SPI1_Send( BYTE *Buf, DWORD Length );
void SPI1_Receive( BYTE *Buf, DWORD Length );
BYTE SPI1_ReceiveByte( void );
int mmc_init(void);
int mmc_response(BYTE response);
int mmc_read_block(WORDi block_number);
int mmc_write_block(WORDi block_number);
int mmc_wait_for_write_finish(void);
#endif /* __SPI1_MMC_H__ */

