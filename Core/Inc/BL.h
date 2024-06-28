/*
 * BL.h
 *
 *  Created on: Jun 23, 2024
 *      Author: Mohamed hamada
 */

#ifndef INC_BL_H_
#define INC_BL_H_

#include <stdint.h>

#define BL_ACK       0xA5
#define BL_NACK      0x7F

#define CRC_SUCCESS     1U
#define CRC_FAIL        0U

#define WRITING_SUCCESS     1U
#define WRITING_ERROR       0U

#define READING_SUCCESS     1U
#define READING_ERROR       0U

#define BL_VERISON       1


#define DBGMCU_IDCODE_REG        (*((volatile uint32_t*)0xE0042000))
#define RDP_USER_OPTION_WORD     (*((volatile uint32_t*)0x1FFFC000))


#define  BL_GET_VERSION        0x51
#define  BL_GET_HELP           0x52
#define  BL_GET_CID            0x53
#define  BL_GET_RDP_STATUS     0x54
#define  BL_GO_TO_ADDRESS      0x55
#define  BL_FLASH_ERASE        0x56
#define  BL_MEM_WRITE          0x57
#define  BL_EN_RW_PROTECT      0x58
#define  BL_MEM_READ           0x59
#define  BL_READ_SECTOR_STATUS 0x5A
#define  BL_OTP_READ           0x5B
#define  BL_DIS_RW_PROTECT     0x5C


#define VALID_ADRESS           1U
#define INVALID_ADRESS         0U

void BL_voidHandlerGetVersionCmd(uint8_t* copy_pu8cmdPacket);
void BL_voidHandlerGetHelpCmd(uint8_t* copy_pu8cmdPacket);
void BL_voidHandlerGetCIDCmd(uint8_t* copy_pu8cmdPacket);
void BL_voidHandlerGetRDPStatusCmd(uint8_t* copy_pu8cmdPacket);
void BL_voidHandlerGoToAdressCmd(uint8_t* copy_pu8cmdPacket);
void BL_voidHandlerFlashEraseCmd(uint8_t* copy_pu8cmdPacket);
void BL_voidHandlerMemWriteCmd(uint8_t* copy_pu8cmdPacket);
void BL_voidHandlerEnRwProtectCmd(uint8_t* copy_pu8cmdPacket);
void BL_voidHandlerMemReadCmd(uint8_t* copy_pu8cmdPacket);
void BL_voidHandlerReadSectorStatusCmd(uint8_t* copy_pu8cmdPacket);
void BL_voidHandlerOTPReadCmd(uint8_t* copy_pu8cmdPacket);
void BL_voidHandlerDisRwProtectCmd(uint8_t* copy_pu8cmdPacket);




#endif /* INC_BL_H_ */
