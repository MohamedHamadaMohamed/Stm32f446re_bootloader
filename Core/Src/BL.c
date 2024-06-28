/*
 * BL.c
 *
 *  Created on: Jun 23, 2024
 *      Author: Mohamed hamada
 *      Embedded software Engineer
 */
#include <stdint.h>

#include "main.h"
#include "BL.h"
extern CRC_HandleTypeDef hcrc;
extern UART_HandleTypeDef huart2;

static uint8_t u8VerifyCRC(uint8_t* copy_pu8DataArr,uint8_t copy_u8Length,uint32_t copy_u32HostCRC);
static void voidSendAck(uint8_t local_u8ReplyLength);
static void voidSendNack(void);
static uint8_t u8ValidateAdress(uint32_t copy_u32Address);
static uint8_t u8ExcuteFlashErase(uint8_t u8SectorNumber,uint8_t u8NumberofSectors);
static uint8_t u8ExcuteMemWrite(uint8_t* copy_pu8Bbuffer,uint32_t copy_u32Address,uint32_t copy_u8Length);
static uint8_t u8ExcuteMemRead(uint32_t copy_u32Addess,uint8_t copy_u8payloadLength,uint32_t* copy_32Data);

void BL_voidHandlerGetVersionCmd(uint8_t* copy_pu8cmdPacket)
{
	uint8_t local_u8Version    =0;
	uint8_t local_u8CRCStatus  =0;
	uint8_t local_u8CmdLength  =0;
	uint32_t local_u32HostCRC  =0;

	local_u8CmdLength = copy_pu8cmdPacket[0]+1;
	local_u32HostCRC  = *((uint32_t*)(copy_pu8cmdPacket + local_u8CmdLength -4));

	local_u8CRCStatus = u8VerifyCRC(copy_pu8cmdPacket,local_u8CmdLength-4,local_u32HostCRC);



	if(local_u8CRCStatus == CRC_SUCCESS)
	{
		voidSendAck(1u);
		local_u8Version = BL_VERISON;

		HAL_UART_Transmit(&huart2, &local_u8Version, 1, HAL_MAX_DELAY);
	}
	else
	{
		voidSendNack();

	}

}
void BL_voidHandlerGetHelpCmd(uint8_t* copy_pu8cmdPacket)
{
	uint8_t local_u8CRCStatus  =0;
	uint8_t local_u8CmdLength  =0;
	uint32_t local_u32HostCRC  =0;

	local_u8CmdLength = copy_pu8cmdPacket[0]+1;
	local_u32HostCRC  = *((uint32_t*)(copy_pu8cmdPacket + local_u8CmdLength -4));

	local_u8CRCStatus = u8VerifyCRC(copy_pu8cmdPacket,local_u8CmdLength-4,local_u32HostCRC);



	if(local_u8CRCStatus == CRC_SUCCESS)
	{
		uint8_t local_u8CMDs[] =
		{
				BL_GET_VERSION        ,
				BL_GET_HELP           ,
				BL_GET_CID            ,
				BL_GET_RDP_STATUS     ,
				BL_GO_TO_ADDRESS      ,
				BL_FLASH_ERASE        ,
				BL_MEM_WRITE          ,
				BL_EN_RW_PROTECT      ,
				BL_MEM_READ           ,
				BL_READ_SECTOR_STATUS ,
				BL_OTP_READ           ,
				BL_DIS_RW_PROTECT
		};
		voidSendAck(sizeof(local_u8CMDs));
		HAL_UART_Transmit(&huart2, local_u8CMDs, sizeof(local_u8CMDs), HAL_MAX_DELAY);

	}
	else
	{
		voidSendNack();

	}


}
void BL_voidHandlerGetCIDCmd(uint8_t* copy_pu8cmdPacket)
{
	uint8_t local_u8CRCStatus  =0;
	uint8_t local_u8CmdLength  =0;
	uint32_t local_u32HostCRC  =0;

	local_u8CmdLength = copy_pu8cmdPacket[0]+1;
	local_u32HostCRC  = *((uint32_t*)(copy_pu8cmdPacket + local_u8CmdLength -4));

	local_u8CRCStatus = u8VerifyCRC(copy_pu8cmdPacket,local_u8CmdLength-4,local_u32HostCRC);




	if(local_u8CRCStatus == CRC_SUCCESS)
	{
		uint16_t local_u16DeviceID ;
		local_u16DeviceID=((DBGMCU_IDCODE_REG) & (0x0FFF));

		voidSendAck(2u);

		HAL_UART_Transmit(&huart2, (uint8_t*)&local_u16DeviceID, 2, HAL_MAX_DELAY);
	}
	else
	{
		voidSendNack();

	}

}
void BL_voidHandlerGetRDPStatusCmd(uint8_t* copy_pu8cmdPacket)
{
	uint8_t local_u8CRCStatus  =0;
	uint8_t local_u8CmdLength  =0;
	uint32_t local_u32HostCRC  =0;

	local_u8CmdLength = copy_pu8cmdPacket[0]+1;
	local_u32HostCRC  = *((uint32_t*)(copy_pu8cmdPacket + local_u8CmdLength -4));

	local_u8CRCStatus = u8VerifyCRC(copy_pu8cmdPacket,local_u8CmdLength-4,local_u32HostCRC);




	if(local_u8CRCStatus == CRC_SUCCESS)
	{
		uint8_t local_u8RDPStatus = (uint8_t)((RDP_USER_OPTION_WORD>>8)&(0xFF));

		voidSendAck(1u);

		HAL_UART_Transmit(&huart2, &local_u8RDPStatus, 1, HAL_MAX_DELAY);
	}
	else
	{
		voidSendNack();

	}
}
void BL_voidHandlerGoToAdressCmd(uint8_t* copy_pu8cmdPacket)
{
	uint8_t local_u8CRCStatus  =0;
	uint8_t local_u8CmdLength  =0;
	uint32_t local_u32HostCRC  =0;

	local_u8CmdLength = copy_pu8cmdPacket[0]+1;
	local_u32HostCRC  = *((uint32_t*)(copy_pu8cmdPacket + local_u8CmdLength -4));

	local_u8CRCStatus = u8VerifyCRC(copy_pu8cmdPacket,local_u8CmdLength-4,local_u32HostCRC);




	if(local_u8CRCStatus == CRC_SUCCESS)
	{

		uint32_t local_u32Adress = *((uint32_t*)&copy_pu8cmdPacket[2]);
		uint8_t local_u8AdressStatus = u8ValidateAdress( local_u32Adress);

		voidSendAck(1u);

		if (local_u8AdressStatus == VALID_ADRESS) {

			void(*local_pvFunPtr)(void) = NULL;
			local_u32Adress++;//increment address by 1 to make t bit
			local_pvFunPtr = (void*)local_u32Adress;
			HAL_UART_Transmit(&huart2, &local_u8AdressStatus, 1, HAL_MAX_DELAY);

			local_pvFunPtr();
		}
		else
		{
			HAL_UART_Transmit(&huart2, &local_u8AdressStatus, 1, HAL_MAX_DELAY);

		}


	}
	else
	{
		voidSendNack();

	}
}
void BL_voidHandlerFlashEraseCmd(uint8_t* copy_pu8cmdPacket)
{
	uint8_t local_u8CRCStatus  =0;
	uint8_t local_u8CmdLength  =0;
	uint32_t local_u32HostCRC  =0;

	local_u8CmdLength = copy_pu8cmdPacket[0]+1;
	local_u32HostCRC  = *((uint32_t*)(copy_pu8cmdPacket + local_u8CmdLength -4));

	local_u8CRCStatus = u8VerifyCRC(copy_pu8cmdPacket,local_u8CmdLength-4,local_u32HostCRC);




	if(local_u8CRCStatus == CRC_SUCCESS)
	{

		uint8_t local_u8EraseStatus =0;
		voidSendAck(1u);

		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		local_u8EraseStatus = u8ExcuteFlashErase(copy_pu8cmdPacket[2],copy_pu8cmdPacket[3]);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

		HAL_UART_Transmit(&huart2, &local_u8EraseStatus, 1, HAL_MAX_DELAY);



	}
	else
	{
		voidSendNack();

	}

}
void BL_voidHandlerMemWriteCmd(uint8_t* copy_pu8cmdPacket)
{
	uint8_t local_u8CRCStatus  =0;
	uint8_t local_u8CmdLength  =0;
	uint32_t local_u32HostCRC  =0;

	local_u8CmdLength = copy_pu8cmdPacket[0]+1;
	local_u32HostCRC  = *((uint32_t*)(copy_pu8cmdPacket + local_u8CmdLength -4));

	local_u8CRCStatus = u8VerifyCRC(copy_pu8cmdPacket,local_u8CmdLength-4,local_u32HostCRC);



	if(local_u8CRCStatus == CRC_SUCCESS)
	{
		voidSendAck(1u);

		uint8_t local_u8WritingStatus ;
		uint32_t local_u32Addess = *((uint32_t*)&copy_pu8cmdPacket[2]);

		uint8_t lacal_u8AddressStatus =u8ValidateAdress(local_u32Addess) ;
		if (lacal_u8AddressStatus == VALID_ADRESS)
		{
			uint8_t local_u8payloadLength = copy_pu8cmdPacket[6];
			local_u8WritingStatus = u8ExcuteMemWrite(&copy_pu8cmdPacket[7],local_u32Addess,local_u8payloadLength);

		}
		else
		{
			local_u8WritingStatus = WRITING_ERROR;
		}
		HAL_UART_Transmit(&huart2, &local_u8WritingStatus, 1, HAL_MAX_DELAY);

	}
	else
	{
		voidSendNack();

	}

}
void BL_voidHandlerEnRwProtectCmd(uint8_t* copy_pu8cmdPacket)
{

}
void BL_voidHandlerMemReadCmd(uint8_t* copy_pu8cmdPacket)
{
	uint8_t local_u8CRCStatus  =0;
	uint8_t local_u8CmdLength  =0;
	uint32_t local_u32HostCRC  =0;

	local_u8CmdLength = copy_pu8cmdPacket[0]+1;
	local_u32HostCRC  = *((uint32_t*)(copy_pu8cmdPacket + local_u8CmdLength -4));

	local_u8CRCStatus = u8VerifyCRC(copy_pu8cmdPacket,local_u8CmdLength-4,local_u32HostCRC);



	if(local_u8CRCStatus == CRC_SUCCESS)
	{
		voidSendAck(1u);

		uint8_t local_u8ReadStatus ;
		uint32_t local_u32Addess = *((uint32_t*)&copy_pu8cmdPacket[2]);
		uint32_t local_u32Data ;

		uint8_t lacal_u8AddressStatus =u8ValidateAdress(local_u32Addess) ;
		if (lacal_u8AddressStatus == VALID_ADRESS)
		{
			uint8_t local_u8payloadLength = copy_pu8cmdPacket[6];
			local_u8ReadStatus = u8ExcuteMemRead(local_u32Addess,local_u8payloadLength,&local_u32Data);

		}
		else
		{
			local_u8ReadStatus = READING_ERROR;
		}
		HAL_UART_Transmit(&huart2, &local_u8ReadStatus, 1, HAL_MAX_DELAY);

	}
	else
	{
		voidSendNack();

	}

}
void BL_voidHandlerReadSectorStatusCmd(uint8_t* copy_pu8cmdPacket)
{

}
void BL_voidHandlerOTPReadCmd(uint8_t* copy_pu8cmdPacket)
{

}
void BL_voidHandlerDisRwProtectCmd(uint8_t* copy_pu8cmdPacket)
{

}
static uint8_t u8VerifyCRC(uint8_t* copy_pu8DataArr,uint8_t copy_u8Length,uint32_t copy_u32HostCRC)
{
	uint8_t status = 0;
	uint8_t i =0;
	uint32_t local_temp =0;
	uint32_t local_u32AccCRC =0 ;
	for(i=0;i<copy_u8Length;i++)
	{
		local_temp = copy_pu8DataArr[i];
		local_u32AccCRC= HAL_CRC_Accumulate(&hcrc, &local_temp, 1);

	}
	__HAL_CRC_DR_RESET(&hcrc);
	if (local_u32AccCRC == copy_u32HostCRC) {
		status =CRC_SUCCESS;
	}
	else
	{
		status =CRC_FAIL;
	}




	return status;
}
static void voidSendAck(uint8_t local_u8ReplyLength)
{


	uint8_t local_u8AckBuffer[2] = {BL_ACK,local_u8ReplyLength};
	HAL_UART_Transmit(&huart2, local_u8AckBuffer, 2, HAL_MAX_DELAY);

}
static void voidSendNack(void)
{
	uint8_t local_u8Nack = BL_NACK;
	HAL_UART_Transmit(&huart2,&local_u8Nack,1,HAL_MAX_DELAY);
}
static uint8_t u8ValidateAdress(uint32_t copy_u32Address)
{


	uint8_t local_u8AddressStatus = INVALID_ADRESS;

	if(((copy_u32Address >= FLASH_BASE) && (copy_u32Address < FLASH_END)) )
	{

		local_u8AddressStatus =VALID_ADRESS;

	}
	else if(((copy_u32Address >= SRAM1_BASE) && (copy_u32Address < (SRAM1_BASE+(128*1024)))) )
	{

		local_u8AddressStatus =VALID_ADRESS;

	}
	else
	{
		local_u8AddressStatus =INVALID_ADRESS;

	}
	return local_u8AddressStatus ;
}
static uint8_t u8ExcuteFlashErase(uint8_t u8SectorNumber,uint8_t u8NumberofSectors)
{
	HAL_StatusTypeDef local_ErrorState = HAL_OK;

	if((u8NumberofSectors>8)&&(u8NumberofSectors !=0xff))
	{
		local_ErrorState = HAL_ERROR;
	}
	else if((u8SectorNumber>7)&&(u8SectorNumber !=0xff))
	{
		local_ErrorState = HAL_ERROR;
	}
	else
	{
		FLASH_EraseInitTypeDef local_myErase ;
		uint32_t local_u32SectorError;
		if(u8SectorNumber ==0xff)
		{
			local_myErase.TypeErase=FLASH_TYPEERASE_MASSERASE;
		}
		else
		{
			uint8_t local_u8RemainingSector = 8 - u8SectorNumber;
			if(u8NumberofSectors > local_u8RemainingSector )
			{
				u8NumberofSectors = local_u8RemainingSector;
			}
			local_myErase.TypeErase = FLASH_TYPEERASE_SECTORS;
			local_myErase.NbSectors = u8NumberofSectors;
			local_myErase.Sector    = u8SectorNumber;
		}
		local_myErase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
		local_myErase.Banks = FLASH_BANK_1;
		HAL_FLASH_Unlock();
		local_ErrorState = HAL_FLASHEx_Erase(&local_myErase, &local_u32SectorError) ;
		HAL_FLASH_Lock();
	}


	return local_ErrorState;
}
static uint8_t u8ExcuteMemWrite(uint8_t* copy_pu8Bbuffer,uint32_t copy_u32Address,uint32_t copy_u8Length)
{
	uint8_t local_u8ErrorStatus = HAL_OK;

	if ((copy_u32Address >= FLASH_BASE )&&(copy_u32Address <= FLASH_END))
	{
		uint8_t local_u8Iterator =0 ;
		/*unloch the flash */
		HAL_FLASH_Unlock();
		for(local_u8Iterator =0 ;local_u8Iterator <copy_u8Length;local_u8Iterator++)
		{
			local_u8ErrorStatus=HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, copy_u32Address+local_u8Iterator,(uint64_t)copy_pu8Bbuffer[local_u8Iterator]);
		}
		/*unloch the flash */
		HAL_FLASH_Lock();

	}
	else
	{
		uint8_t  local_u8Iterator =0 ;
		uint8_t* local_pu8Dest = (uint8_t*)copy_u32Address;
		for(local_u8Iterator =0 ;local_u8Iterator <copy_u8Length;local_u8Iterator++)
		{
			local_pu8Dest[local_u8Iterator] = copy_pu8Bbuffer[local_u8Iterator];
		}
	}

	return local_u8ErrorStatus;
}
static uint8_t u8ExcuteMemRead(uint32_t copy_u32Addess,uint8_t copy_u8payloadLength,uint32_t* copy_32Data)
{
	uint8_t local_u8ErrorStatus = HAL_OK;
	uint8_t local_u8iterator =0;

	(*copy_32Data) = (*((uint32_t*)copy_u32Addess));

	return local_u8ErrorStatus;
}










