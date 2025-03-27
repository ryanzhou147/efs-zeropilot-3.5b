/*
* canard_stm32_driver.c
*
*  Created on: Jul 8, 2024
*      Author: Roni Kant
*/

#include <canard.h>
#include "stm32l5xx_hal.h"
#include "can_driver.h"
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

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
	// Receiving
	CanardCANFrame rx_frame;

	const uint64_t timestamp = HAL_GetTick() * 1000ULL;
	const int16_t rx_res = canardSTM32Recieve(hfdcan, FDCAN_RX_FIFO0, &rx_frame);

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


// NOTE: All canard handlers and senders are based on this reference: https://dronecan.github.io/Specification/7._List_of_standard_data_types/
// Alternatively, you can look at the corresponding generated header file in the dsdlc_generated folder

// Canard Handlers ( Many have code copied from libcanard esc_node example: https://github.com/dronecan/libcanard/blob/master/examples/ESCNode/esc_node.c )

void handle_NodeStatus(CanardInstance *ins, CanardRxTransfer *transfer) {
	struct uavcan_protocol_NodeStatus nodeStatus;

	if (uavcan_protocol_NodeStatus_decode(transfer, &nodeStatus)) {
		return;
	}

	printf("Node health: %ud Node Mode: %ud\n", nodeStatus.health, nodeStatus.mode);

	printf("Node Health ");

	switch (nodeStatus.health) {
	case UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK:
		printf("OK\n");
		break;
	case UAVCAN_PROTOCOL_NODESTATUS_HEALTH_WARNING:
		printf("WARNING\n");
		break;
	case UAVCAN_PROTOCOL_NODESTATUS_HEALTH_ERROR:
		printf("ERROR\n");
		break;
	case UAVCAN_PROTOCOL_NODESTATUS_HEALTH_CRITICAL:
		printf("CRITICAL\n");
		break;
	default:
		printf("UNKNOWN?\n");
		break;
	}

	printf("Node Mode ");

	switch(nodeStatus.mode) {
	case UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL:
		printf("OPERATIONAL\n");
		break;
	case UAVCAN_PROTOCOL_NODESTATUS_MODE_INITIALIZATION:
		printf("INITIALIZATION\n");
		break;
	case UAVCAN_PROTOCOL_NODESTATUS_MODE_MAINTENANCE:
		printf("MAINTENANCE\n");
		break;
	case UAVCAN_PROTOCOL_NODESTATUS_MODE_SOFTWARE_UPDATE:
		printf("SOFTWARE UPDATE\n");
		break;
	case UAVCAN_PROTOCOL_NODESTATUS_MODE_OFFLINE:
		printf("OFFLINE\n");
		break;
	default:
		printf("UNKNOWN?\n");
		break;
	}
}

void handle_NotifyState(CanardInstance *ins, CanardRxTransfer *transfer) {
	struct ardupilot_indication_NotifyState notifyState;

	if (ardupilot_indication_NotifyState_decode(transfer, &notifyState)) {
		return;
	}

	uint32_t nl = notifyState.vehicle_state & 0xFFFFFFFF;  // ignoring the last 32 bits for printing since the highest vehicle_state value right now is 23 even though they're allowed to be up to 64bit unsigned integer

	printf("Vehicle State: %lu ", nl);

	if (notifyState.aux_data.len > 0) {
		printf("Aux Data: 0x");

		for (int i = 0; i < notifyState.aux_data.len; i++) {
			printf("%02x", notifyState.aux_data.data[i]);
		}
	}

	printf("\n");

}

/*
  handle a ESC RawCommand request
*/
void handle_RawCommand(CanardInstance *ins, CanardRxTransfer *transfer)
{
    struct uavcan_equipment_esc_RawCommand rawCommand;
    if (uavcan_equipment_esc_RawCommand_decode(transfer, &rawCommand)) {
        return;
    }
    // see if it is for us
    if (rawCommand.cmd.len <= ESC_INDEX) {
        return;
    }
    // convert throttle to -1.0 to 1.0 range
//    printf("Throttle: %f \n", rawCommand.cmd.data[ESC_INDEX]/8192.0);
}

/*
  get a 16 byte unique ID for this node, this should be based on the CPU unique ID or other unique ID
 */
void getUniqueID(uint8_t id[16]) {
    uint32_t HALUniqueIDs[3];
    // Make Unique ID out of the 96-bit STM32 UID and fill the rest with 0s
    memset(id, 0, 16);
    HALUniqueIDs[0] = HAL_GetUIDw0();
    HALUniqueIDs[1] = HAL_GetUIDw1();
    HALUniqueIDs[2] = HAL_GetUIDw2();
    memcpy(id, HALUniqueIDs, 12);
}

/*
  handle a GetNodeInfo request
*/
// TODO: All the data in here is temporary for testing. If actually need to send valid data, edit accordingly.
void handle_GetNodeInfo(CanardInstance *ins, CanardRxTransfer *transfer) {
	printf("GetNodeInfo request from %d\n", transfer->source_node_id);

	uint8_t buffer[UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE];
	struct uavcan_protocol_GetNodeInfoResponse pkt;

	memset(&pkt, 0, sizeof(pkt));

	node_status.uptime_sec = HAL_GetTick() / 1000ULL;
	pkt.status = node_status;

	// fill in your major and minor firmware version
	pkt.software_version.major = 1;
	pkt.software_version.minor = 0;
	pkt.software_version.optional_field_flags = 0;
	pkt.software_version.vcs_commit = 0; // should put git hash in here

	// should fill in hardware version
	pkt.hardware_version.major = 1;
	pkt.hardware_version.minor = 0;

	// just setting all 16 bytes to 1 for testing
	getUniqueID(pkt.hardware_version.unique_id);

	strncpy((char*)pkt.name.data, "ESCNode", sizeof(pkt.name.data));
	pkt.name.len = strnlen((char*)pkt.name.data, sizeof(pkt.name.data));

	uint16_t total_size = uavcan_protocol_GetNodeInfoResponse_encode(&pkt, buffer);

	canardRequestOrRespond(ins,
						   transfer->source_node_id,
						   UAVCAN_PROTOCOL_GETNODEINFO_SIGNATURE,
						   UAVCAN_PROTOCOL_GETNODEINFO_ID,
						   &transfer->transfer_id,
						   transfer->priority,
						   CanardResponse,
						   &buffer[0],
						   total_size);
}

// Canard Senders

/*
  send the 1Hz NodeStatus message. This is what allows a node to show
  up in the DroneCAN GUI tool and in the flight controller logs
 */
void send_NodeStatus(void) {
    uint8_t buffer[UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE];

    node_status.uptime_sec = HAL_GetTick() / 1000UL;
    node_status.health = UAVCAN_PROTOCOL_NODESTATUS_HEALTH_OK;
    node_status.mode = UAVCAN_PROTOCOL_NODESTATUS_MODE_OPERATIONAL;
    node_status.sub_mode = 0;

    // put whatever you like in here for display in GUI
    node_status.vendor_specific_status_code = 1234;

    uint32_t len = uavcan_protocol_NodeStatus_encode(&node_status, buffer);

    // we need a static variable for the transfer ID. This is
    // incremeneted on each transfer, allowing for detection of packet
    // loss
    static uint8_t transfer_id;

    canardBroadcast(&canard,
                    UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE,
                    UAVCAN_PROTOCOL_NODESTATUS_ID,
                    &transfer_id,
                    CANARD_TRANSFER_PRIORITY_LOW,
                    buffer,
                    len);
}

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

/*
  This function is called at 1 Hz rate from the main loop.
*/
void process1HzTasks(uint64_t timestamp_usec) {
    /*
      Purge transfers that are no longer transmitted. This can free up some memory
    */
    canardCleanupStaleTransfers(&canard, timestamp_usec);

    /*
      Transmit the node status message
    */
    send_NodeStatus();
}