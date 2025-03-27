/*
 * canard_stm32_driver.c
 *
 *  Created on: Jul 8, 2024
 *      Author: Roni Kant
 */

 #include <canard.h>
 #include "stm32l5xx_hal.h"
 #include <string.h>
 
 /**
   * @brief  Process CAN message from RxLocation FIFO into rx_frame
   * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
   *         the configuration information for the specified FDCAN.
   * @param  RxLocation Location of the received message to be read.
   *         This parameter can be a value of @arg FDCAN_Rx_location.
   * @param  rx_frame pointer to a CanardCANFrame structure where the received CAN message will be
   * 		stored.
   * @retval ret == 1: OK, ret < 0: CANARD_ERROR, ret == 0: Check hfdcan->ErrorCode
   */
 int16_t canardSTM32Recieve(FDCAN_HandleTypeDef *hfdcan, uint32_t RxLocation, CanardCANFrame *const rx_frame) {
 	if (rx_frame == NULL) {
 		return -CANARD_ERROR_INVALID_ARGUMENT;
 	}
 
 	FDCAN_RxHeaderTypeDef RxHeader;
 	uint8_t RxData[8];
 
 	if (HAL_FDCAN_GetRxMessage(hfdcan, RxLocation, &RxHeader, RxData) == HAL_OK) {
 
 		//	printf("Received message: ID=%lu, DLC=%lu\n", RxHeader.Identifier, RxHeader.DataLength);
 		//
 		//	printf("0x");
 		//	for (int i = 0; i < RxHeader.DataLength; i++) {
 		//		printf("%02x", RxData[i]);
 		//	}
 		//	printf("\n");
 
 		// Process ID to canard format
 		rx_frame->id = RxHeader.Identifier;
 
 		if (RxHeader.IdType == FDCAN_EXTENDED_ID) { // canard will only process the message if it is extended ID
 			rx_frame->id |= CANARD_CAN_FRAME_EFF;
 		}
 
 		if (RxHeader.RxFrameType == FDCAN_REMOTE_FRAME) { // canard won't process the message if it is a remote frame
 			rx_frame->id |= CANARD_CAN_FRAME_RTR;
 		}
 
 		rx_frame->data_len = RxHeader.DataLength;
 		memcpy(rx_frame->data, RxData, RxHeader.DataLength);
 
 		// assume a single interface
 		rx_frame->iface_id = 0;
 
 		return 1;
 	}
 
 	// Either no CAN msg to be read, or an error that can be read from hfdcan->ErrorCode
 	return 0;
 }
 
 /**
   * @brief  Process tx_frame CAN message into Tx FIFO/Queue and transmit it
   * @param  hfdcan pointer to an FDCAN_HandleTypeDef structure that contains
   *         the configuration information for the specified FDCAN.
   * @param  tx_frame pointer to a CanardCANFrame structure that contains the CAN message to
   * 		transmit.
   * @retval ret == 1: OK, ret < 0: CANARD_ERROR, ret == 0: Check hfdcan->ErrorCode
   */
 int16_t canardSTM32Transmit(FDCAN_HandleTypeDef *hfdcan, const CanardCANFrame* const tx_frame) {
     if (tx_frame == NULL) {
         return -CANARD_ERROR_INVALID_ARGUMENT;
     }
 
     if (tx_frame->id & CANARD_CAN_FRAME_ERR) {
         return -CANARD_ERROR_INVALID_ARGUMENT; // unsupported frame format
     }
 
     FDCAN_TxHeaderTypeDef TxHeader;
     uint8_t TxData[8];
 
     // Process canard id to STM FDCAN header format
     if (tx_frame->id & CANARD_CAN_FRAME_EFF) {
         TxHeader.IdType = FDCAN_EXTENDED_ID;
         TxHeader.Identifier = tx_frame->id & CANARD_CAN_EXT_ID_MASK;
     } else {
         TxHeader.IdType = FDCAN_STANDARD_ID;
         TxHeader.Identifier = tx_frame->id & CANARD_CAN_STD_ID_MASK;
     }
 
     TxHeader.DataLength = tx_frame->data_len;
 
     if (tx_frame->id & CANARD_CAN_FRAME_RTR) {
         TxHeader.TxFrameType = FDCAN_REMOTE_FRAME;
     } else {
         TxHeader.TxFrameType = FDCAN_DATA_FRAME;
     }
 
     TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE; // unsure about this one
     TxHeader.BitRateSwitch = FDCAN_BRS_OFF; // Disabling FDCAN (using CAN 2.0)
     TxHeader.FDFormat = FDCAN_CLASSIC_CAN; // Disabling FDCAN (using CAN 2.0)
     TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // unsure about this one
     TxHeader.MessageMarker = 0; // unsure about this one
     memcpy(TxData, tx_frame->data, TxHeader.DataLength);
 
     if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeader, TxData) == HAL_OK) {
 //		printf("Successfully sent message with id: %lu \n", TxHeader.Identifier);
         return 1;
     }
 
 //	printf("Failed at adding message with id: %lu to Tx Fifo", TxHeader.Identifier);
     // This might be for many reasons including the Tx Fifo being full, the error can be read from hfdcan->ErrorCode
     return 0;
 }
 
 
 
 /*
   get a 16 byte unique ID for this node, this should be based on the CPU unique ID or other unique ID
  */
 void getUniqueID(uint8_t id[16]){
     uint32_t HALUniqueIDs[4];
 // Make Unique ID out of the 96-bit STM32 UID
     memset(id, 0, 16);
     HALUniqueIDs[0] = HAL_GetUIDw0();
     HALUniqueIDs[1] = HAL_GetUIDw1();
     HALUniqueIDs[2] = HAL_GetUIDw2();
     HALUniqueIDs[3] = HAL_GetUIDw1(); // repeating UIDw1 for this, no specific reason I chose this..
     memcpy(id, HALUniqueIDs, 16);
 }
 
 // TODO: Change to FD_CAN: HAL_FDCAN_RxFifo0Callback
 // The actual ISR, modify this to your needs
 void HAL_CAN_RxFifo0MsgPendingCallback(FDCAN_HandleTypeDef *hfdcan) {
     // Receiving
     CanardCANFrame rx_frame;
 
     const uint64_t timestamp = HAL_GetTick() * 1000ULL;
     const int16_t rx_res = canardSTM32Recieve(hfdcan, CAN_RX_FIFO0, &rx_frame);
 
     if (rx_res < 0) {
         printf("Receive error %d\n", rx_res);
     }
     else if (rx_res > 0)        // Success - process the frame
     {
         canardHandleRxFrame(&canard, &rx_frame, timestamp);
     }
 }
 
 // Call this function very often to check if there are any Tx to process
 void processCanardTxQueue(FDCAN_HandleTypeDef *hfdcan) {
     // Transmitting
 
     for (const CanardCANFrame *tx_frame ; (tx_frame = canardPeekTxQueue(&canard)) != NULL;) {
         const int16_t tx_res = canardSTM32Transmit(hfdcan, tx_frame);
 
         if (tx_res < 0) {
             printf("Transmit error %d\n", tx_res);
         } else if (tx_res > 0) {
             printf("Successfully transmitted message\n");
         }
 
         // Pop canardTxQueue either way
         canardPopTxQueue(&canard);
     }
 }
 