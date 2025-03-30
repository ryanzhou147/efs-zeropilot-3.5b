#include "can.hpp"

CAN::CAN(FDCAN_HandleTypeDef hfdcan) : hfdcan(hfdcan) {
//	this->canInst.on_reception = &this->CanardOnTransferReception;
//	this->canInst.should_accept = &this->shouldAcceptTransfer;
	static uint8_t canardMemoryPool[1024];

	canardInit(&canInst,
			canardMemoryPool,
			sizeof(canardMemoryPool),
			&CAN::CanardOnTransferReception,
			&CAN::CanardShouldAcceptTransfer,
			this
	);
}

CAN::CAN() {}

CAN::~CAN() {

}

bool CAN::CanardShouldAcceptTransfer(
	const CanardInstance *ins,
	uint64_t *out_data_type_signature,
	uint16_t data_type_id,
	CanardTransferType transfer_type,
	uint8_t source_node_id)
{
	if (transfer_type == CanardTransferTypeRequest) {
		// check if we want to handle a specific service request
		switch (data_type_id) {
			case UAVCAN_PROTOCOL_GETNODEINFO_ID: {
				*out_data_type_signature = UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_SIGNATURE;
				return true;
			}
		}
	}
	return false;
}

void CAN::CanardOnTransferReception(
	CanardInstance *ins,
	CanardRxTransfer *transfer) 
{
    if (transfer->transfer_type == CanardTransferTypeRequest) {
        // check if we want to handle a specific service request
        switch (transfer->data_type_id) {
            case UAVCAN_PROTOCOL_GETNODEINFO_ID: {
                handle_ReceiveNodeInfo(transfer); // TODO need to implement this function
                break;
			}
        }
    }
}


/*
Function to convert all canard CAN frames and send them through HAL

Consider removing for loop
*/
void CAN::sendCANTx() {
	bool success = true;
	for (const CanardCANFrame *frame; frame != nullptr; frame = canardPeekTxQueue(&canInst)) {
		if (HAL_FDCAN_GetTxFifoFreeLevel(hfdcan) > 0) {
			FDCAN_TxHeaderTypeDef txHeader;

			txHeader.Identifier = frame->id;
			txHeader.IdType = FDCAN_EXTENDED_ID;
			txHeader.TxFrameType = FDCAN_DATA_FRAME;
			txHeader.DataLength = frame->data_len; // Must be betweeon 0-8
			txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
			txHeader.BitRateSwitch = FDCAN_BRS_OFF;
			txHeader.FDFormat = FDCAN_CLASSIC_CAN;
			
			const uint8_t *txData = frame->data;

			bool success = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan, &txHeader, txData) == HAL_OK;

			if (success) {
				canardPopTxQueue(&canInst);
			}
		}
	}
}

bool CAN::routineTasks() {
	sendCANTx();
}


/*
Wrapper function with mutex
*/
int16_t CAN::canardBroadcastObj(
	CanardTxTransfer* transfer
) {

	int16_t res = canardBroadcastObj(&canInst, transfer);


	return res;
}

int16_t CAN::CanardBroadcast(CanardInstance* ins,            ///< Library instance
	uint64_t data_type_signature,   ///< See above
	uint16_t data_type_id,          ///< Refer to the specification
	uint8_t* inout_transfer_id,     ///< Pointer to a persistent variable containing the transfer ID
	uint8_t priority,               ///< Refer to definitions CANARD_TRANSFER_PRIORITY_*
	const void* payload,            ///< Transfer payload
	uint16_t payload_len
)
	{
		CanardTxTransfer transfer_object = {
			.data_type_signature = data_type_signature,
			.data_type_id = data_type_id,
			.inout_transfer_id = inout_transfer_id,
			.priority = priority,
			.payload = (uint8_t*)payload,
			.payload_len = payload_len,
			#if CANARD_ENABLE_DEADLINE
					.deadline_usec = tx_deadline,
			#endif
			#if CANARD_MULTI_IFACE
					.iface_mask = iface_mask,
			#endif
			#if CANARD_ENABLE_CANFD
					.canfd = canfd,
			#endif
		};

		return canardBroadcastObj(&transfer_object);
	}