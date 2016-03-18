/******************************************************************************
 * Author: Patrick Payne
 * This file contains a low level driver for the MicroSD(HC) card connected
 * to the Microblaze soft processor via an AXI QUAD SPI peripheral. FatFS
 * is built on top of this "glue layer" to provide full FAT16/FAT32
 * functionality.
 *
 *****************************************************************************/

#include <stdbool.h>
#include "xspi.h"
#include "xspi_l.h"
#include "xil_types.h"
#include "microblaze_sleep.h"
#include "xil_printf.h"

#include "diskio.h"

//#define SLOW_MODE
#define DEBUG_SD
#ifdef DEBUG_SD
	#define SD_DEBUG_PRINT(...) xil_printf(__VA_ARGS__)
#else
	#define SD_DEBUG_PRINT(...)
#endif

// SD commands (SPI mode).
// See http://www.chlazza.net/sdcardinfo.html
#define CMD0	(0)			// GO_IDLE_STATE
#define CMD1	(1)			// SEND_OP_COND
#define	ACMD41	(0x80+41)	// SEND_OP_COND (SDC)
#define CMD8	(8)			// SEND_IF_COND
#define CMD9	(9)			// SEND_CSD
#define CMD10	(10)		// SEND_CID
#define CMD12	(12)		// STOP_TRANSMISSION
#define CMD13	(13)		// SEND_STATUS
#define ACMD13	(0x80+13)	// SD_STATUS (SDC)
#define CMD16	(16)		// SET_BLOCKLEN
#define CMD17	(17)		// READ_SINGLE_BLOCK
#define CMD18	(18)		// READ_MULTIPLE_BLOCK
#define CMD23	(23)		// SET_BLOCK_COUNT
#define	ACMD23	(0x80+23)	// SET_WR_BLK_ERASE_COUNT (SDC)
#define CMD24	(24)		// WRITE_BLOCK
#define CMD25	(25)		// WRITE_MULTIPLE_BLOCK
#define CMD32	(32)		// ERASE_ER_BLK_START
#define CMD33	(33)		// ERASE_ER_BLK_END
#define CMD38	(38)		// ERASE
#define CMD55	(55)		// APP_CMD
#define CMD58	(58)		// READ_OCR

// SD response bits: see http://www.chlazza.net/sdcardinfo.html
// Type R1: (note: MSB always zero)
#define SD_R1_IDLE ((u8) 0x01)
#define SD_R1_ERASE_RESET ((u8) 0x02)
#define SD_R1_ILLEGAL_CMD ((u8) 0x04)
#define SD_R1_CRC_ERR ((u8) 0x08)
#define SD_R1_ERASE_SEQ_ERR ((u8) 0x10)
#define SD_R1_ADDR_ERR ((u8) 0x20)
#define SD_R1_PARAM_ERR ((u8) 0x40)

// Type R2:
#define SD_R2_CARD_LOCKED ((u8) 0x01)
#define SD_R2_WRITE_PROT_ERASE ((u8) 0x02)
#define SD_R2_ERROR ((u8) 0x04)
#define SD_R2_CC_ERROR ((u8) 0x08)
#define SD_R2_ECC_ERROR ((u8) 0x01)
#define SD_R2_WRITE_PROT_VIOLATION ((u8) 0x02)
#define SD_R2_ERASE_PARAM ((u8) 0x04)
#define SD_R2_OUT_OF_RANGE ((u8) 0x08)

#define SPI_ADDR XPAR_SPI_0_BASEADDR
#define SPI_DEVICE_ID		XPAR_SPI_0_DEVICE_ID

#define SPI_INTR_ID		XPAR_INTC_0_SPI_0_VEC_ID
#define CARD_PRESENT_BASE_ADDR XPAR_GPIO_0_BASEADDR

#define SPI_TIMEOUT (50)
#define MAX_RESP_SIZE (10)
#define MAX_RESET_RETRIES (10)
#define MAX_ACMD41_RETRIES (100)


/******************************************************************************
 * Static Function Declarations
 *****************************************************************************/

static u8 SdCrc(u8* chr, int cnt, u8 crc);
static void InitializeSpi(void);
static inline void InhibitSpi(void);
static inline void DisInhibitSpi(void);
static u8 SendByte(u8 Byte);
static void SendBytes(const u8 *Bytes, int NumBytes);
static void ReadBytes(u8 *Bytes, int NumBytes);
static inline bool WaitReady(void);
static inline void DeselectSdSlave(void);
static inline bool SelectSdSlave(void);
static int SendCmd(u8 Cmd, u32 Arg, u8 *Resp);
static bool ReceiveDatablock(u8 *Data, int ByteCount);
static bool TransmitDatablock(const u8 *Data, int ByteCount, u8 Token);


/******************************************************************************
 * Static Variable Declarations
 *****************************************************************************/

// SdStatus is used to maintain a state for the disk_status call.
static DSTATUS SdStatus = STA_NOINIT;

// CardType is used internally to represent what version of the SD
// Protocol and hardware is being used.
// b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing
static u8 CardType;


/******************************************************************************
 * Public FatFS interface
 *****************************************************************************/

DSTATUS disk_status (BYTE pdrv) {
	//We only support the single SD card.
	if (pdrv) return STA_NOINIT;

	return SdStatus;
}


DSTATUS disk_initialize (BYTE pdrv) {
	u8 ReadBuf[MAX_RESP_SIZE];
	int NumRespBytes;
	bool Idle;
	int RetriesRemaining;

	//We only support the single SD card.
	if (pdrv) {
		SD_DEBUG_PRINT("Attempted to initialize nonzero drive number.\n\r");
		return STA_NOINIT;
	}

	InitializeSpi();
	DeselectSdSlave();

	// The SDSPI protocol requires us to provide at least 80 clocks before
	// selecting the device and sending commands.
	for (int i = 0; i < 15; i++) {
		SendByte(0xFF);
	}

	// Reset the SD card through the SPI interface.
	RetriesRemaining = MAX_RESET_RETRIES;
	while (RetriesRemaining > 0) {
		// CMD0 is the command for software reset.
		NumRespBytes = SendCmd(CMD0, 0, ReadBuf);
		if ((NumRespBytes > 0) && (ReadBuf[0] == SD_R1_IDLE)) {
			break;
		}
	}

	if (RetriesRemaining <= 0) {
		SD_DEBUG_PRINT("Failed to reset SD card: %x\n\r", ReadBuf[0]);
		return STA_NOINIT;
	}

	// Check for supported voltage range using CMD8.
	// 0x1AA is the magic number I found on the Internet, corresponding to the
	// voltage range 2.7 V - 3.6 V (we use 3.3v), and a checksum for the command.
	NumRespBytes = SendCmd(CMD8, 0x1AA, ReadBuf);
	if ((NumRespBytes > 4) && (ReadBuf[0] == SD_R1_IDLE)) {

		// Check that the voltage range is supported.
		if ((ReadBuf[3] != 0x01) || (ReadBuf[4] != 0xAA)) {
			SD_DEBUG_PRINT("SD card does not support 3.3V.\n\r");
			return STA_NOINIT;
		}

		// We call ACMD41 to check if the device is ready to use.
		RetriesRemaining = MAX_ACMD41_RETRIES;
		Idle = true;
		while (Idle && (RetriesRemaining > 0)) {
			NumRespBytes = SendCmd(ACMD41, 0x40100000, ReadBuf);
			if ((NumRespBytes > 0) && (ReadBuf[0] == 0)) {
				Idle = false;
			}
			RetriesRemaining--;
			MB_Sleep(10);
		}

		if (Idle) {
			SD_DEBUG_PRINT("Timed out waiting for SD to exit IDLE mode.\n\r");
			return STA_NOINIT;
		}

		// Check CCS (Card Capacity Status) bit in the OCR (Operating Conditions Register).
		// CCS of 1 indicates SDHC/SDXC, CCS of 0 indicates plain old SD.
		NumRespBytes = SendCmd(CMD58, 0, ReadBuf);
		if ((NumRespBytes > 1) && (ReadBuf[0] == 0)) {
			if (ReadBuf[1] & 0x40) {
				SD_DEBUG_PRINT("Card is running SDHX or SDXC (SD V2.0)\n\r");
				CardType = CT_SD2 | CT_BLOCK;
				SdStatus = 0;

			} else {
				SD_DEBUG_PRINT("Card is running regular SD V2.0\n\r");
				CardType = CT_SD2;
				SdStatus = 0;
			}
		} else {
			SD_DEBUG_PRINT("Error response to CMD58.\n\r");
			return STA_NOINIT;
		}

	} else {
		// An error response to CMD8 means the card only supports SDV1,
		// Which we do not currently support. Abort.
		SD_DEBUG_PRINT("SD card is running SD v1.\n\r");
		return STA_NOINIT;
	}

	return 0;
}


DRESULT disk_read (
	BYTE pdrv,		/* Physical drive number to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	u8 cmd;
	int NumRespBytes;

	//We only support the single SD card.
	if (pdrv) {
		SD_DEBUG_PRINT("Attempted to read from nonzero drive number.\n\r");
		return RES_ERROR;
	}

	if (disk_status(pdrv) & STA_NOINIT) {
		SD_DEBUG_PRINT("Attempted to write to a non-initialized disk.");
		return RES_NOTRDY;
	}

	// Convert the sector address to a byte address if necessary.
	if (!(CardType & CT_BLOCK)) {
		sector *= 512;
	}

	// We issue different commands depending on the count.
#ifdef SLOW_MODE
	cmd = CMD17;
	do {

		// Give multiple attempts at reading a sector.
		// This is needed because our SD card is junk.
		// Also, our SD card doesn't support multi-sector reads.
		int retriesRemaining = 100;
		while (retriesRemaining > 0) {
			NumRespBytes = SendCmd(cmd, sector, buff);
			if ((NumRespBytes < 1) || (buff[0] != 0)) {
				//SD_DEBUG_PRINT("Failed to send read command: %d, %02x\n\r", NumRespBytes, buff[0]);
				retriesRemaining--;
				continue;
			}

			if (ReceiveDatablock(buff, 512)) break;
			retriesRemaining--;
		}

		// Check if we ended up reading the sector or not.
		if (retriesRemaining <= 0) {
			SD_DEBUG_PRINT("Did not detect data block.\n\r");
			break;
		} else {
			buff += 512;
			sector++;
		}
	} while (--count);


#else
	cmd = (count > 1) ? CMD18 : CMD17;
	NumRespBytes = SendCmd(cmd, sector, buff);
	if ((NumRespBytes < 1) || (buff[0] != 0)) {
		SD_DEBUG_PRINT("Failed to send read command: %d, %02x\n\r", NumRespBytes, buff[0]);
		return RES_ERROR;
	}

	// Wait for the data block(s) that will appear later.
	do {
		if (!ReceiveDatablock(buff, 512)) break;
		buff += 512;
	} while (--count);
#endif

	// CMD18, used for multiple sector reads, requires a STOP command or it will perpetually read sectors.
	if (cmd == CMD18) {
		NumRespBytes = SendCmd(CMD12, 0, buff);
	}
	return count ? RES_ERROR : RES_OK;
}


DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{

	int NumRespBytes;
	u8 ReadBuf[MAX_RESP_SIZE];

	//We only support the single SD card.
	if (pdrv) {
		SD_DEBUG_PRINT("Attempted to read from nonzero drive number.\n\r");
		return RES_ERROR;
	}

	if (disk_status(pdrv) & STA_NOINIT) {
		SD_DEBUG_PRINT("Attempted to write to a non-initialized disk.");
		return RES_NOTRDY;
	}

	// Convert the sector address to a byte address if necessary.
	if (!(CardType & CT_BLOCK)) {
		sector *= 512;
	}

	if (count == 1) {
		// Single block write.
		NumRespBytes = SendCmd(CMD24, sector, ReadBuf);
		if ((NumRespBytes < 1) || (ReadBuf[0] != 0)) {
			SD_DEBUG_PRINT("Failed to send single block write (CMD24).\r\n");
			return RES_ERROR;
		}

		// 0xFE is the data token for single block writes.
		if (TransmitDatablock(buff, 512, 0xFE)) {
			count = 0;
		}
	} else {
		// Multiple block write.
		// SD devices support pre-erasing sectors for faster multiblock write.
		if (CardType & CT_SDC) {
			SendCmd(ACMD23, count, ReadBuf);
		}
		if (SendCmd(CMD25, sector, ReadBuf) == 0) {
			do {
				if (!TransmitDatablock(buff, 512, 0xFC)) {
					SD_DEBUG_PRINT("Failed to send block to SD card.\r\n");
					break;
				}
			} while (--count);

			// Send the stop token.
			SelectSdSlave();
			SendByte(0xFD);
			DeselectSdSlave();
		}
	}

	return count ? RES_ERROR : RES_OK;
}


DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT Result = RES_ERROR;
	int NumSectors;
	u8 Csd[16];
	u8 ReadBuf[MAX_RESP_SIZE];

	//We only support the single SD card.
	if (pdrv) {
		SD_DEBUG_PRINT("Attempted to ioctl a nonzero drive number.\n\r");
		return RES_ERROR;
	}

	if (disk_status(pdrv) & STA_NOINIT) {
		SD_DEBUG_PRINT("Attempted to ioctl a non-initialized disk.");
		return RES_NOTRDY;
	}

	switch (cmd) {
	case CTRL_SYNC:
		if (SelectSdSlave()) {
			DeselectSdSlave();
			Result = RES_OK;
		}
		break;

	case GET_SECTOR_COUNT:
		if ((SendCmd(CMD9, 0, ReadBuf) == 0) && ReceiveDatablock(Csd, 16)) {
			// NOTE: We assume SDV2 here. Handling is complicated for SDV1.
			NumSectors = Csd[9] + ((u32) Csd[8] << 8) + ((u32) (Csd[7] & 63) << 16) + 1;
			*(u32 *) buff = NumSectors << 10;

			Result = RES_OK;
		}
		break;

	case GET_BLOCK_SIZE:
		*(u32*)buff = 128;
		Result = RES_OK;
		break;

	default:
		Result = RES_PARERR;
		break;
	}

	return Result;
}


/******************************************************************************
 * Static Helper Functions
 *****************************************************************************/

/* Generate the CRC checksums for data send to the SD card.
 * @param Src The data to perform the CRC over.
 * @param Length The number of bytes the CRC is over.
 * @param Prev The previous checksum, used when doing a rolling checksum.
 *
 * @return The checksum.
 */
static u8 SdCrc(u8* Src, int Length, u8 Prev){
     u8 Data;

     for( int a = 0; a < Length; a++){
          Data = Src[a];
          for(int i = 0; i < 8; i++){
               Prev <<= 1;
               if( (Data & 0x80) ^ (Prev & 0x80) ) {
            	   Prev ^= 0x09;
               }
               Data <<= 1;
          }
     }
     return Prev & 0x7F;
}


/* Initialize the SPI peripheral connected to the SD card.
 * We reset the RX/TX, enable manual SS, enable Master mode, but
 * Keep the master in inhibited mode.
 */
static void InitializeSpi(void) {
	int CtrlReg;
	CtrlReg = XSpi_ReadReg(SPI_ADDR, XSP_CR_OFFSET);
	CtrlReg |= XSP_CR_TRANS_INHIBIT_MASK;
	CtrlReg |= XSP_CR_RXFIFO_RESET_MASK;
	CtrlReg |= XSP_CR_TXFIFO_RESET_MASK;
	CtrlReg |= XSP_CR_ENABLE_MASK;
	CtrlReg |= XSP_CR_MASTER_MODE_MASK;
	CtrlReg |= XSP_CR_MANUAL_SS_MASK;
	XSpi_WriteReg(SPI_ADDR, XSP_CR_OFFSET, CtrlReg);
}


/* Inhibit the SPI master, completing a transaction and preventing new ones.
 */
static inline void InhibitSpi(void) {
	int CtrlReg;
	CtrlReg = XSpi_ReadReg(SPI_ADDR, XSP_CR_OFFSET);
	CtrlReg |= XSP_CR_TRANS_INHIBIT_MASK;
	XSpi_WriteReg(SPI_ADDR, XSP_CR_OFFSET, CtrlReg);
}


/* Disinhibit the SPI master, allowing an SPI transmission to occur.
 */
static inline void DisInhibitSpi(void) {
	int CtrlReg;
	CtrlReg = XSpi_ReadReg(SPI_ADDR, XSP_CR_OFFSET);
	CtrlReg &= ~XSP_CR_TRANS_INHIBIT_MASK;
	XSpi_WriteReg(SPI_ADDR, XSP_CR_OFFSET, CtrlReg);
}


/* Transfer a single byte to the SD card over SPI, returning the byte on MISO.
 *
 * @param Byte The byte to send to the SD card.
 * @return The byte asserted on the MISO (i.e. Rx) line during transmission.
 */
static u8 SendByte(u8 Byte) {
	u8 Result;

	// Wait until the SPI peripheral is ready to perform a TX.
	while (XSpi_ReadReg(SPI_ADDR, XSP_SR_OFFSET) & XSP_SR_TX_FULL_MASK);


	XSpi_WriteReg(SPI_ADDR, XSP_DTR_OFFSET, (u32) Byte);
	DisInhibitSpi();

	// Wait until the SPI peripheral has received the response from the slave.
	while (XSpi_ReadReg(SPI_ADDR, XSP_SR_OFFSET) & XSP_SR_RX_EMPTY_MASK);

	InhibitSpi();
	Result = XSpi_ReadReg(SPI_ADDR, XSP_DRR_OFFSET);

	return Result;
}


/* Transfer bytes from a buffer to the SD card over SPI, ignoring it's response.
 *
 * @param Bytes The bytes to send to the SD card.
 * @param NumBytes The number of bytes to send.
 */
static void SendBytes(const u8 *Bytes, int NumBytes) {
	while (NumBytes > 0) {
		SendByte(*Bytes++);
		NumBytes--;
	}
}


/* Read a specified number of bytes from the SD card over the SPI interface.
 * @param Bytes The buffer to store the result in
 * @param NumBytes The number of bytes to get from the SD card
 */
static void ReadBytes(u8 *Bytes, int NumBytes){
	while (NumBytes--) {
		*Bytes++ = SendByte(0xFF);
	}
}


/* Clock the SPI interface until we are getting the NULL response back.
 */
static inline bool WaitReady(void) {
	for (int i = 0; i < 100; i++) {
		if (SendByte(0xFF) == 0xFF) return true;
	}
	return false;
}


/* Deselect the SD card as an SPI slave.
 */
inline void DeselectSdSlave(void)
{
	XSpi_WriteReg(SPI_ADDR, XSP_SSR_OFFSET, 0xFFFFFFFF);
}


/* Select the SD card as the SPI slave and wait until it is ready to receive commands.
 *
 * @return true if we succeed, false otherwise.
 */
inline bool SelectSdSlave(void)
{
	XSpi_WriteReg(SPI_ADDR, XSP_SSR_OFFSET, 0xFFFFFFFE);

	if (WaitReady()) {
		return true;
	} else {
		DeselectSdSlave();
		return false;
	}

	return true;
}


/* Send a single command to the SD card using the SDSPI protocol.
 *
 * NOTE: See the SD physical layer spec in the docs folder for details.
 *
 * @param Cmd The index of the command to send.
 * @param Arg The 32-bit argument to the command.
 * @param Resp A buffer to put the variable-length result in. We
 *     Write at most MAX_RESP_SIZE bytes to this buffer.
 *
 * @return The number of bytes in the response.
 */
static int SendCmd (u8 Cmd, u32 Arg, u8 *Resp) {
	int NumBytes;
	u8 Buf[6];
	u8 ReadByte;
	int NumRespBytes;
	int Timer;

	// ACMD<n> is the command sequense of CMD55-CMD<n>.
	if (Cmd & 0x80) {
		// Clear the 0x80 bit, which doesn't actually correspond to a command.
		Cmd &= 0x7F;

		NumBytes = SendCmd(CMD55, 0, Resp);
		if ((NumBytes < 1) || (Resp[0] > 1)) {
			return 0xFF;
		}
	}

	// Select the SD card as the SPI slave device.
	// TODO: determine if this is necessary each time...
	DeselectSdSlave();
	if (!SelectSdSlave()) {
		return 0;
	}

	// Start bytes, + command code
	Buf[0] = 0x40 | Cmd;

	// 4 Bytes for the command arguments
	Buf[1] = (u8) (Arg >> 24);
	Buf[2] = (u8) (Arg >> 16);
	Buf[3] = (u8) (Arg >> 8);
	Buf[4] = (u8) (Arg);

	// Last byte is a 7 bit CRC with a stop bit.
	Buf[5] = (SdCrc(Buf, 5, 0) << 1) | 1;

	// Some cards only work if you clock a few cycles after selecting them.
	SendByte(0xFF);

	SendBytes(Buf, 6);

	// The slave may take a couple of cycles to respond; When it does, it is guaranteed
	// To not be 0xFF.
	Timer = SPI_TIMEOUT;
	do {
		Resp[0] = SendByte(0xFF);
	} while ((Resp[0] == 0xFF) && (Timer-- > 0));

	if (Timer <= 0) {
		SD_DEBUG_PRINT("Timed out getting response for command %d arg %08x.\r\n", Cmd, Arg);
		return 0;
	}

	// Keep reading the response until we get the NULL 0xFF bytes.
	NumRespBytes = 1;
	while (NumRespBytes < MAX_RESP_SIZE) {
		ReadByte = SendByte(0xFF);
		if (ReadByte != 0xFF) {
			Resp[NumRespBytes++] = ReadByte;
		} else {
			break;
		}
	}

	DeselectSdSlave();

	return NumRespBytes;
}


/* Wait for a data sector to be sent by the SD card then save it to a buffer.
 *
 * @param Data The buffer to save the sector to.
 * @param ByteCount The number of bytes we are expecting.
 * 		NOTE: A sector is 512 bytes for SDHC cards.
 *
 * @return true if successful, false otherwise.
 */
static bool ReceiveDatablock(u8 *Data, int ByteCount) {
	u8 TestByte;
	int RetriesRemaining = 150;

	if (!SelectSdSlave()) return false;

	// Keep reading bytes until we see the start of a data block.
	while (RetriesRemaining > 0) {
		TestByte = SendByte(0xFF);
		if (TestByte != 0xFF) break;
		RetriesRemaining--;
	}

	if (RetriesRemaining == 0) {
		//SD_DEBUG_PRINT("Did not detect data block.\n\r");
		return false;
	}

	// The SD card indicates the start of a data block by 0xFE.
	if (TestByte != 0xFE) {
		SD_DEBUG_PRINT("Did not receive block response: %02x\n\r", TestByte);
		return false;
	}

	ReadBytes(Data, ByteCount);

	// We discard the two byte CRC.
	// TODO: actually use this CRC.
	SendByte(0xFF);
	SendByte(0xFF);

	DeselectSdSlave();
	return true;
}


static bool TransmitDatablock(const u8 *Data, int ByteCount, u8 Token) {
	u8 Result;
	int Timer;

	if (!SelectSdSlave()) return false;

	SendByte(Token);
	SendBytes(Data, ByteCount);

	// Dummy CRC:
	SendByte(0xFF);
	SendByte(0xFF);


	// The slave may take a couple of cycles to respond; When it does, it is guaranteed
	// To not be 0xFF.
	Timer = SPI_TIMEOUT;
	do {
		Result = SendByte(0xFF);
	} while ((Result == 0xFF) && (Timer-- > 0));

	if ((Result & 0x1F) != 0x05) {
		SD_DEBUG_PRINT("Error Transmitting block: %02x\r\n", Result);
		return false;
	}

	// SD will also send out busy bytes, wait them out.
	while(SendByte(0xFF) == 0);
	DeselectSdSlave();
	return true;
}
